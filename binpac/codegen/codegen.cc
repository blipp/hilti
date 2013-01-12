
#include <hilti.h>

#include "codegen.h"

#include "code-builder.h"
#include "parser-builder.h"
#include "type-builder.h"
#include "context.h"
#include "attribute.h"

#include "module.h"
#include "statement.h"
#include "expression.h"
#include "grammar.h"

using namespace binpac;

CodeGen::CodeGen()
{
}

CodeGen::~CodeGen()
{

}

shared_ptr<hilti::Module> CodeGen::compile(shared_ptr<Module> module, int debug, bool verify)
{
    _compiling = true;
    _debug = debug;
    _verify = verify;
    _imported_types.clear();

    _code_builder = unique_ptr<codegen::CodeBuilder>(new codegen::CodeBuilder(this));
    _parser_builder = unique_ptr<codegen::ParserBuilder>(new codegen::ParserBuilder(this));
    _type_builder = unique_ptr<codegen::TypeBuilder>(new codegen::TypeBuilder(this));
    _coercer = unique_ptr<Coercer>(new Coercer());

    try {
        hilti::path_list paths = module->context()->libraryPaths();
        auto id = hilti::builder::id::node(module->id()->name(), module->id()->location());
        auto ctx = module->context()->hiltiContext();
        _mbuilder = std::make_shared<hilti::builder::ModuleBuilder>(ctx, id, module->path(), module->location());
        _mbuilder->importModule("Hilti");
        _mbuilder->importModule("BinPACHilti");

        if ( util::strtolower(module->id()->name()) != "binpac" )
            _mbuilder->importModule("BinPAC");

        auto name = ::util::fmt("__entry");
        builder()->addInstruction(::hilti::instruction::flow::CallVoid, hilti::builder::id::create(name), hilti::builder::tuple::create({}));
        auto entry = _mbuilder->pushFunction(name);

        _module = module;
        _code_builder->processOne(module);

        _mbuilder->popFunction();

        // Add types that need to be imported from other modules. Note that
        // we may add further types to the list as we proceed, that's what
        // the outer loop is for.

        std::set<string> done;

        while ( _imported_types.size() != done.size() ) {
            for ( auto t : _imported_types ) {
                if ( done.find(t.first) != done.end() )
                    continue;

                auto id = std::make_shared<ID>(t.first);
                auto type = t.second;
                auto unit = ast::tryCast<type::Unit>(type);
                auto hlttype = unit ? hiltiTypeParseObject(unit) : hiltiType(type);

                auto hid = hiltiID(id);
                if ( ! moduleBuilder()->declared(hid) )
                    moduleBuilder()->addType(hid, hlttype);

                done.insert(t.first);
            }
        }

#if 0
        _mbuilder->module()->compilerContext()->dump(_mbuilder->module(), std::cerr);
        _mbuilder->module()->compilerContext()->print(_mbuilder->module(), std::cerr);
#endif
        _module = nullptr;

        auto m = _mbuilder->finalize();

        if ( ! m )
            return verify ? nullptr : _mbuilder->module();

        return m;
    }

    catch ( const ast::FatalLoggerError& err ) {
        // Message has already been printed.
        return nullptr;
    }
}

shared_ptr<binpac::Module> CodeGen::module() const
{
    return _module;
}

shared_ptr<hilti::builder::ModuleBuilder> CodeGen::moduleBuilder() const
{
    assert(_compiling);

    return _mbuilder;
}

shared_ptr<hilti::builder::BlockBuilder> CodeGen::builder() const
{
    assert(_compiling);

    return _mbuilder->builder();
}

int CodeGen::debugLevel() const
{
    return _debug;
}

shared_ptr<binpac::type::Unit> CodeGen::unit() const
{
    return _parser_builder->unit();
}

shared_ptr<hilti::Expression> CodeGen::hiltiExpression(shared_ptr<Expression> expr, shared_ptr<Type> coerce_to)
{
    assert(_compiling);
    return _code_builder->hiltiExpression(expr, coerce_to);
}

void CodeGen::hiltiStatement(shared_ptr<Statement> stmt)
{
    assert(_compiling);
    _code_builder->hiltiStatement(stmt);
}

shared_ptr<hilti::Type> CodeGen::hiltiType(shared_ptr<Type> type)
{
    assert(_compiling);
    return _type_builder->hiltiType(type);
}

shared_ptr<hilti::Expression> CodeGen::hiltiDefault(shared_ptr<Type> type, bool null_on_default, bool can_be_unset)
{
    assert(_compiling);
    return _type_builder->hiltiDefault(type, null_on_default, can_be_unset);
}

shared_ptr<hilti::Expression> CodeGen::hiltiCoerce(shared_ptr<hilti::Expression> hexpr, shared_ptr<Type> src, shared_ptr<Type> dst)
{
    assert(_compiling);

    auto expr = std::make_shared<expression::CodeGen>(src, hexpr);
    auto cexpr = _coercer->coerceTo(expr, dst);

    if ( ! cexpr )
        internalError(util::fmt("coercion failed in CodeGen::hiltiCoerce(): cannot coerce HILTI expression of BinPAC++ type %s to type %s (%s / %s)",
                               src->render().c_str(),
                               dst->render().c_str(),
                               hexpr->render(), string(hexpr->location())));

    return hiltiExpression(cexpr);
}

shared_ptr<hilti::ID> CodeGen::hiltiID(shared_ptr<ID> id, bool qualify)
{
    auto uid = hilti::builder::id::node(id->pathAsString(), id->location());

    if ( ! qualify )
        return  uid;

    auto mod = id->firstParent<Module>();
    assert(mod);

    string name;

    if ( ! uid->isScoped() ) {
        if ( mod->id()->name() != module()->id()->name() )
            name = util::fmt("%s::%s", mod->id()->name(), uid->name());
        else
            name = uid->name();
    }

    else
        name = uid->pathAsString(hiltiID(module()->id()));

    return hilti::builder::id::node(name, id->location());
}

shared_ptr<hilti::Expression> CodeGen::hiltiParseFunction(shared_ptr<type::Unit> u)
{
    auto grammar = u->grammar();
    assert(grammar);

    auto func = ast::tryCast<hilti::Expression>(_mbuilder->lookupNode("parse-func", grammar->name()));

    if ( func )
        return func;

    func = _parser_builder->hiltiCreateParseFunction(u);

    _mbuilder->cacheNode("parse-func", grammar->name(), func);
    return func;
}

shared_ptr<hilti::Type> CodeGen::hiltiTypeParseObject(shared_ptr<type::Unit> unit)
{
    auto t = ast::tryCast<hilti::Type>(_mbuilder->lookupNode("parse-obj", unit->id()->name()));

    if ( t )
        return t;

    t = _parser_builder->hiltiTypeParseObject(unit);

    _mbuilder->cacheNode("parse-obj", unit->id()->name(), t);
    return t;
}

shared_ptr<hilti::Type> CodeGen::hiltiTypeParseObjectRef(shared_ptr<type::Unit> u)
{
    return hilti::builder::reference::type(hiltiTypeParseObject(u));
}

void CodeGen::hiltiExportParser(shared_ptr<type::Unit> unit)
{
    _parser_builder->hiltiExportParser(unit);
}

void CodeGen::hiltiUnitHooks(shared_ptr<type::Unit> unit)
{
    _parser_builder->hiltiUnitHooks(unit);
}

void CodeGen::hiltiRunFieldHooks(shared_ptr<type::unit::Item> item)
{
    _parser_builder->hiltiRunFieldHooks(item);
}

void CodeGen::hiltiDefineHook(shared_ptr<ID> id, shared_ptr<Hook> hook)
{
   _parser_builder->hiltiDefineHook(id, hook);
}

shared_ptr<hilti::declaration::Function> CodeGen::hiltiDefineFunction(shared_ptr<Function> func)
{
#if 0
    // If it's scoped and we import the module, assume the module declares
    // the function.
    if ( func->id()->isScoped() && moduleBuilder()->idImported(func->id()->path().front()))
        return nullptr;
#endif

    auto name = hiltiFunctionName(func);
    auto ftype = func->type();
    auto rtype = ftype->result() ? hiltiType(ftype->result()->type()) : hilti::builder::void_::type();
    auto result = hilti::builder::function::result(rtype);

    hilti::builder::function::parameter_list params;

    for ( auto p : ftype->parameters() ) {
        shared_ptr<hilti::Expression> def = p->default_() ? hiltiExpression(p->default_()) : nullptr;
        auto hp = hilti::builder::function::parameter(hiltiID(p->id()), hiltiType(p->type()), p->constant(), def);
        params.push_back(hp);
    }

    auto cookie = hilti::builder::function::parameter("__cookie", hiltiTypeCookie(), false, nullptr);

    hilti::type::function::CallingConvention cc;

    switch ( ftype->callingConvention() ) {
     case type::function::BINPAC:
        cc = hilti::type::function::HILTI;
        params.push_back(cookie);
        break;

     case type::function::BINPAC_HILTI_C:
        cc = hilti::type::function::HILTI_C;
        params.push_back(cookie);
        break;

     case type::function::BINPAC_HILTI:
        cc = hilti::type::function::HILTI;
        params.push_back(cookie);
        break;

     case type::function::HILTI_C:
        cc = hilti::type::function::HILTI_C;
        break;

     case type::function::HILTI:
        cc = hilti::type::function::HILTI;
        break;

     case type::function::C:
        cc = hilti::type::function::C;
        break;

     default:
        internalError("unexpected calling convention in hiltiDefineFunction()");
    }

    // TODO: Do we always want to export these?
    moduleBuilder()->exportID(name);

    if ( func->body() ) {
        auto decl = moduleBuilder()->pushFunction(name, result, params, cc, nullptr, false, func->location());
        hiltiStatement(func->body());
        moduleBuilder()->popFunction();
        return decl;
    }

    else
        return moduleBuilder()->declareFunction(name, result, params, cc, func->location());
}

shared_ptr<hilti::ID> CodeGen::hiltiFunctionName(shared_ptr<binpac::Function> func)
{
    if ( func->type()->callingConvention() != type::function::BINPAC ) {
        auto id = func->id();
        auto mod = func->firstParent<Module>();

        if ( mod && ! id->isScoped() )
            return hilti::builder::id::node(::util::fmt("%s::%s", mod->id()->name(), id->name()));

        return hilti::builder::id::node(id->pathAsString());
    }

    // To make overloaded functions unique, we add a hash of the signature.
    auto type = func->type()->render();
    auto name = util::fmt("%s__%s", func->id()->name(), util::uitoa_n(util::hash(type), 64, 4));

    return hilti::builder::id::node(name, func->location());
}

shared_ptr<hilti::ID> CodeGen::hiltiFunctionName(shared_ptr<expression::Function> expr)
{
    auto func = expr->function();
    auto id = func->id()->pathAsString();

    if ( ! func->id()->isScoped() && expr->scope().size() )
        id = util::fmt("%s::%s", expr->scope(), id);

    if ( func->type()->callingConvention() != type::function::BINPAC )
        return hilti::builder::id::node(id, func->id()->location());

    // To make overloaded functions unique, we add a hash of the signature.
    auto type = func->type()->render();
    auto name = util::fmt("%s__%s", id, util::uitoa_n(util::hash(type), 64, 4));

    return hilti::builder::id::node(name, func->location());
}

shared_ptr<hilti::Expression> CodeGen::hiltiCall(shared_ptr<expression::Function> func, const expression_list& args, shared_ptr<hilti::Expression> cookie)
{
    auto ftype = func->function()->type();
    auto hilti_func = hiltiExpression(func);

    hilti::builder::tuple::element_list hilti_arg_list;

    for ( auto a : args )
        hilti_arg_list.push_back(hiltiExpression(a));

    if ( ftype->callingConvention() == type::function::BINPAC_HILTI_C ||
         ftype->callingConvention() == type::function::HILTI_C ||
         ftype->callingConvention() == type::function::HILTI ||
         ftype->callingConvention() == type::function::BINPAC_HILTI ) {
        if ( func->scope().size() )
            hiltiDefineFunction(func->function());
            // moduleBuilder()->importModule(hilti::builder::id::node(func->scope()));
    }

    switch ( ftype->callingConvention() ) {
     case type::function::BINPAC:
     case type::function::BINPAC_HILTI:
     case type::function::BINPAC_HILTI_C: {
        hilti_arg_list.push_back(cookie);
        break;
     }

     case type::function::HILTI:
     case type::function::HILTI_C:
     case type::function::C:
        break;

     default:
        internalError("unexpected calling convention in expression::operator_::function::Call()");
    }

    auto hilti_args = hilti::builder::tuple::create(hilti_arg_list);

    shared_ptr<hilti::Expression> result = nullptr;

    if ( ftype->result() && ! ast::isA<type::Void>(ftype->result()->type()) ) {
        result = builder()->addTmp("result", hiltiType(ftype->result()->type()));
        builder()->addInstruction(result, hilti::instruction::flow::CallResult, hilti_func, hilti_args);
    }

    else {
        result = hilti::builder::id::create("<no return value>"); // Dummy ID; this should never be accessed ...
        builder()->addInstruction(hilti::instruction::flow::CallVoid, hilti_func, hilti_args);
    }

    return result;
}

shared_ptr<hilti::Expression> CodeGen::hiltiSelf()
{
    return _parser_builder->hiltiSelf();
}

shared_ptr<hilti::Expression> CodeGen::hiltiCookie()
{
    return _parser_builder->hiltiCookie();
}

shared_ptr<hilti::Type> CodeGen::hiltiTypeCookie()
{
    return hilti::builder::type::byName("BinPACHilti::UserCookie");
}

void CodeGen::hiltiBindDollarDollar(shared_ptr<hilti::Expression> val)
{
    _code_builder->hiltiBindDollarDollar(val);
}

void CodeGen::hiltiUnbindDollarDollar()
{
    _code_builder->hiltiBindDollarDollar(0);
}

shared_ptr<hilti::Expression> CodeGen::hiltiFunctionNew(shared_ptr<type::Unit> unit)
{
    return _parser_builder->hiltiFunctionNew(unit);
}

shared_ptr<hilti::Expression> CodeGen::hiltiCastEnum(shared_ptr<hilti::Expression> val, shared_ptr<hilti::Type> dst)
{
    auto i = builder()->addTmp("eval", hilti::builder::integer::type(64));
    auto e = builder()->addTmp("enum", dst);
    builder()->addInstruction(i, hilti::instruction::enum_::ToInt, val);
    builder()->addInstruction(e, hilti::instruction::enum_::FromInt, i);
    return e;
}

void CodeGen::hiltiWriteToSinks(shared_ptr<type::unit::item::Field> field, shared_ptr<hilti::Expression> data)
{
    _parser_builder->hiltiWriteToSinks(field, data);
}

void CodeGen::hiltiWriteToSink(shared_ptr<hilti::Expression> sink, shared_ptr<hilti::Expression> data)
{
    _parser_builder->hiltiWriteToSink(sink, data);
}

void CodeGen::hiltiWriteToSink(shared_ptr<hilti::Expression> sink, shared_ptr<hilti::Expression> begin, shared_ptr<hilti::Expression> end)
{
    auto data = builder()->addTmp("data", hilti::builder::reference::type(hilti::builder::bytes::type()));
    builder()->addInstruction(data, hilti::instruction::bytes::Sub, begin, end);
    return hiltiWriteToSink(sink, data);
}

shared_ptr<hilti::Expression> CodeGen::hiltiApplyAttributesToValue(shared_ptr<hilti::Expression> val, shared_ptr<AttributeSet> attrs)
{
    auto convert = attrs->lookup("convert");

    if ( convert ) {
        hiltiBindDollarDollar(val);
        auto dd = builder()->addTmp("__dollardollar", val->type());
        builder()->addInstruction(dd, hilti::instruction::operator_::Assign, val);
        val = hiltiExpression(convert->value());
        hiltiUnbindDollarDollar();
    }

    return val;
}

void CodeGen::hiltiImportType(shared_ptr<ID> id, shared_ptr<Type> t)
{
    _imported_types.insert(std::make_pair(id->pathAsString(), t));
}