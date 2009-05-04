# $Id$

import inspect

import type
import constant
import id
import util

class Signature:
    """Defines an instruction's signature. The signature includes the
    instruction's name as well as number and types of operands and target. The
    class is not supposed to be instantiated directly; instead, instances are
    implicitly created when using the :func:`instruction` decorator.
    
    Operands and targets are defined with constraints describing what kind of
    arguments they can take, and optionally with type refinement functions
    which allow an operand's type to be implicitly adjusted to the constraint
    function where appropiate (e.g., to adjust the width of an integer
    constant). See :ref:`signature-constraints` for more information.
    
    Note that the most common constraint and refinement functions are
    predefined, see ~~constraints. 
    
    name: string - The name of the instruction, i.e., the HILTI mnemonic.
    
    op1: Function or None - Constraint function for the instruction's first operand, or None  if operand is unused.
    op2: Function or None - Constraint function for the instruction's second operand, or None  if operand is unused.
    op3: Function or None - Constraint function for the instruction's third operand, or None  if operand is unused.
    target: Function or None - Constraint function for the instruction's target operand, or None if target is unused.
    
    callback: function - The function will be called each time an
    ~~Instruction has been instantiated with this signature. The callback will
    receive the ~~Instruction as its only parameter and can modify it as needed.
    
    terminator: bool - True if the instruction is considered a block
    |terminator|.
    """
    def __init__(self, name, op1=None, op2=None, op3=None, target=None, callback=None, terminator=False):
        self._name = name
        self._op1 = op1
        self._op2 = op2
        self._op3 = op3
        self._target = target
        self._callback = callback
        self._terminator = terminator
        
        for op in [target, op1, op2, op3]:
            if op: 
                assert_constraint(op)
                
    def name(self):
        """Returns the name of the instruction defined by the signature.
        
        Returns: string - The instruction name.
        """
        return self._name
    
    def op1(self):
        """Returns the type of the instruction's first operand.
        
        Returns: ~~Type or callable - The constraints for the operand, or None if unused.
        If a type it's a *class*, not an instance of thereof.
        """        
        return self._op1
    
    def op2(self):
        """Returns the type of the instruction's second operand.
        
        Returns: ~~Type or callable - The constraints for the operand, or None if unused.
        If a type it's a *class*, not an instance of thereof.
        """        
        return self._op2
    
    def op3(self):
        """Returns the type of the instruction's third operand.
        
        Returns: ~~Type or callable - The constraints for the operand, or None if unused.
        If a type it's a *class*, not an instance of thereof.
        """        
        return self._op3

    def target(self):
        """Returns the type of the instruction's result.
        
        Returns: ~~Type or callable - The constraints for the result, or None if unused.
        If a type it's a *class*, not an instance of thereof.
        """        
        return self._target
    
    def callback(self):
        """Returns the signature's callback function.
        
        Returns: function - The callback function.
        """
        return self._callback
    
    def terminator(self):
        """Returns whether the signature's instruction is a |terminator|.
        
        Returns: bool - True if it is terminator instruction.
        """
        return self._terminator
        
    def __str__(self):
        target = "<%s> = " % self._target if self._target else ""
        op1 = " <%s>" % self._op1 if self._op1 else ""
        op2 = " <%s>" % self._op2 if self._op2 else ""
        op3 = " <%s>" % self._op3 if self._op3 else ""
        return "%s%s%s%s%s" % (target, self._name, op1, op2, op3)
    
    def matchWithInstruction(self, i):
        """Checks whether an instruction matches the signature's constraints.
        
        i: ~~Instruction - The instruction to check.
        
        Returns: (success, errormsg) - *success* is True if the instruction matches
        the signature, and False otherwise. If *success* is False, *errormsg*
        contains a string describing the mismatch in a way suitable to present to
        the user in an error message."""
    
        def checkOp(op, sig, tag):
            if not sig:
                return "superfluous %s" % tag if op else None
            
            if not op:
                # Hard-coding the optional here is not great. If we don't do
                # that, however we can't handle the op==None case here, which
                # means that all constraint functions would need to catch it
                # themselves.
                return "%s missing" % tag if not "_optional" in sig.__dict__ else None

            # FIXME: This will go away.
            if inspect.isclass(sig):
                isclass = issubclass(sig, type.Type)
            else:
                isclass = issubclass(sig.__class__, type.Type)
                
            if isclass:
                t = op.type()
                if isinstance(t, type.TypeDeclType):
                    t = op.value()
                    
                if sig == t or isinstance(t, sig):
                    return None
                else:
                    return "type of %s does not match signature (expected %s, found %s) " % \
			            (tag, str(type.fmtTypeClass(sig)), str(op.type()))
            else:
                # Call type refinement function first (but only once).
                if not op._refined and "_refine" in sig.__dict__:
                    sig._refine(op, i)
                    op._refined = True
                
                (success, msg) = sig(op.type(), op, i)
                return (tag + " " + msg) if not success else None
            
        msg = checkOp(i.op1(), self.op1(), "operand 1")
        if not msg:
            msg = checkOp(i.op2(), self.op2(), "operand 2")
        if not msg:
            msg = checkOp(i.op3(), self.op3(), "operand 3")
        if not msg:
            msg = checkOp(i.target(), self.target(),"target")
            
        if not msg:
            return (True, "")
        else:
            return (False, msg)

    def getOpDoc(self, constraint):
        """Returns the string that should be included at the place of
        operators having the given constraint function.
        
        *constraint*: Function - The constraint function to get the
        documentation string from. 
        
        Returns: string - The documentation string.
        """
        assert "_constraint" in constraint.__dict__
        opdoc = constraint._constraint
        return opdoc if not inspect.isfunction(opdoc) else opdoc(self)
        
def constraint(str):
    """Function decorator to specify how a constraint function's operator
    should be formatted in the instruction reference. See
    :ref:`signature-constraints` for more information.
    
    str: string - The string to use in place of this operand in the
    instruction reference.
    """
    
    def _constraint(f):
        f._constraint = str
        return f
        
    return _constraint

def refineType(func):
    """Function decorator to add a type refinement function to a signature
    constraint function .  See :ref:`signature-constraints` for more
    information.
        
    func: Function - The function to perform the type refinement. 
    """
    
    def _refineType(f):
        f._refine = func
        return f

    return _refineType
 
# Makes sure "op" is a constraint function.
def assert_constraint(c):
    """Helper function that asserts that *c* is a constraint function. Report
    and internal error if not."""
    if not inspect.isfunction(c) or not "_constraint" in c.__dict__:
        util.internal_error("not a constraint function but %s" % repr(c))