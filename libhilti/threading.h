
#ifndef HLT_THREADING_H
#define HLT_THREADING_H

#include <pthread.h>

#include "types.h"
#include "tqueue.h"
#include "fiber.h"
#include "time_.h"

struct __kh_blocked_jobs_t;

/// Returns whether the HILTI runtime environment is configured for running
/// multiple threads.
///
/// Returns: True if configured for threaded execution.
extern int8_t hlt_is_multi_threaded();

/// The enumeration lists the possible states for a thread manager.
///
/// Note: One could imagine another option such as STOP_WHEN_READY in which
/// new jobs could still be scheduled, but the worker thread would terminate
/// once its job queue becomes empty. This is not currently implemented, but
/// might be useful or even preferable depending on the application.
typedef enum {
    /// NEW means a freshly initialized manager whose worker threads haven't
    /// started yet.
    HLT_THREAD_MGR_NEW,

    /// RUN means that the worker threads should keep running, even if they run
    /// out of jobs.
    HLT_THREAD_MGR_RUN,

    /// FINISH menas that workers should terminate once they are all idle.
    HLT_THREAD_MGR_FINISH,

    /// STOP means that the worker threads should disallow any new job
    /// scheduling, and terminate once their job queue is empty.
    HLT_THREAD_MGR_STOP,

    /// KILL means that the worker threads should immediately terminate,
    /// regardless of the contents of their jobs queues.
    HLT_THREAD_MGR_KILL,

    /// DEAD means that the worker threads are dead.
    HLT_THREAD_MGR_DEAD,

} hlt_thread_mgr_state;

/// Type for representing the ID of a virtual thread.
#define HLT_VID_MAIN -1
#define HLT_VID_CMDQUEUE -2

struct __hlt_thread_mgr_blockable {
    uint64_t num_blocked;	// Number of jobs waiting for this resource.
};

// A job queued for execution.
typedef struct __hlt_job {
    hlt_fiber* fiber;         // The fiber for running this job.
    hlt_vthread_id vid;       // The virtual thread the job is scheduled to.
    hlt_type_info* tcontext_type; // The type of the thread context.
    void* tcontext;           // The jobs thread context to use when executing.
    __hlt_thread_mgr_blockable* blockable; // For moving into the blocked queue.
#ifdef DEBUG
    uint64_t id;            // For debugging, we assign numerical IDs for easier identification.
#endif
} hlt_job;


// A struct that encapsulates data related to a single worker thread.
typedef struct __hlt_worker_thread {
    // Accesses to these must only be made from the worker thread itself.
    hlt_thread_mgr* mgr;          // The manager this thread is part of.
    hlt_execution_context** ctxs; // Execution contexts indexed by virtual thread id.
    hlt_vthread_id max_vid;       // Largest vid allocated space for in ctxs.
    hlt_time global_time;         // Last global time all virtual threads have been advanced to.
    __hlt_fiber_pool* fiber_pool; // The pool of available fiber objects for this worker.

    // This can be *read* from different threads without further locking.
    int id;                       // ID of this worker thread in the range 1..*num_workers*.
    char* name;                   // A string identifying the worker.
    int idle;                     // When in state FINISH, the worker will set this when idle.
    pthread_t handle;             // The pthread handle for this thread.

    // Write accesses to the main jobs queue can be made from all worker
    // threads and the main thread, while read accesses come only from the
    // worker thread itself. If another thread needs to schedule a job
    // already blocked, it can write here and it will be moved over to the
    // blocked queue by the worker later.
    hlt_thread_queue* jobs;  // Jobs queued for this worker and ready to run.

    // Write accesses to the blocked queue may be done only from the writer.
    // This is in fact a hash table indexed by the corresponding
    // hlt_thread_mgr_blockable address.
    struct __kh_blocked_jobs_t* jobs_blocked;
} hlt_worker_thread;

// A thread manager encapsulates the global state that all threads share.
struct __hlt_thread_mgr {
    hlt_thread_mgr_state state;    // The manager's current state.
    int num_workers;               // The number of worker threads.
    int num_excpts;                // The number of worker's that have raised exceptions.
    hlt_worker_thread** workers;   // The worker threads.
    pthread_key_t id;              // A per-thread key storing a string identifying the string.
};

/// Returns whether the HILTI runtime environment is configured for running
/// multiple threads.
///
/// Returns: True if configured for threaded execution.
extern int8_t hlt_is_multi_threaded();

/// Starts the threading system. This must be called after ~~hilti_init but
/// before any other HILTI function is run.
///
/// If the run-time system is not configured for threading, the function is a
/// no-op.
///
/// Returns: The thread manager created for the threading.
///
/// Note: This sets the global ~~__hlt_global_thread_mgr.
extern void __hlt_threading_init();

/// Stop the threading system. After this has been called, no further HILTI
/// function may be called.
///
/// When this function returns, all threads will have been shutdown. They
/// will be shutdown gracefully if no unhandled exceptions have be raised,
/// either by any of the workers or passed in via ``excp``. Otherwise, they
/// will be killed immediately.
///
/// If no excpeption is passed into the function but one of the threads has
/// raised an unhandled execption, the function returns an
/// UnhandledThreadException.
///
/// If the run-time system is not configured for threading, the function is a
/// no-op.
///
/// mgr: The mangager returned by ~~hilti_threading_start.
///
/// excpt: The raised UnhandledThreadException, if appropiate.
///
/// Note: This deletes the manager stored in ~~__hlt_global_thread_mgr.
extern void __hlt_threading_done(hlt_exception** excpt);

/// Returns an estimate of the current scheduler load. The load is normalized
/// into the range from zero (low) to one (high). If on the higher end, it
/// should consider reducing new load if possible.
///
/// excpt: &
extern double hlt_threading_load(hlt_exception** excpt);

/// Creates a new thread manager. A thread manager coordinates a set of
/// worker threads and encapsulates all the state that they share. The new
/// manager will be initialized to state ~~NEW.
///
/// This function must only be called if the runtime system has been
/// configured to run with at least one worker thread (see ~~hlt_config_get).
///
/// Returns: The new thread manager.
extern hlt_thread_mgr* hlt_thread_mgr_new();

/// Starts the thread manager's worker thread processing. The manager must be
/// in state ~~NEW and will be switched to ~~RUN.
///
/// This function must only be called if the runtime system has been
/// configured to run with at least one worker thread (see ~~hlt_config_get).
///
/// mgr: The manager to start.
extern void hlt_thread_mgr_start(hlt_thread_mgr*);

/// Deletes a thread manager. Deleting a manager terminates all still running
/// worker threads and releases all resources associated with them. The
/// manager to be deleted must be in state ~~DEAD.
///
/// mgr: The manager to delete.
extern void hlt_thread_mgr_delete(hlt_thread_mgr* mgr);

/// Transitions a thread manager into a new state.
///
/// Currently, the only transitions allowed are from ~~RUN to ~~STOP, and
/// from ~~RUN to ~~KILL. In both cases, the manager will then automatically
/// transition to ~~KILL before the function returns.
///
/// mgr: The manager to transition to a new state.
///
/// new_state: The new state.
///
/// Note: One could imagine adding the ability to transition back to RUN from
/// the DEAD state in the future, so that a thread context could be reused.
extern void hlt_thread_mgr_set_state(hlt_thread_mgr* mgr, const hlt_thread_mgr_state new_state);

/// Returns a thread manager's current state.
///
/// Returns: The current state.
extern hlt_thread_mgr_state hlt_thread_mgr_get_state(const hlt_thread_mgr* mgr);

/// Schedules a job to a virtual thread.
///
/// This function is safe to call from all threads.
///
/// mgr: The thread manager to use.
///
/// vid: The ID of the virtual target thread.
///
/// cont: The continuation representing a bound function, at +1. (!)
///
/// ctx: The caller's execution context.
///
/// excpt: &
extern void __hlt_thread_mgr_schedule(hlt_thread_mgr* mgr, hlt_vthread_id vid, hlt_callable* func, hlt_exception** excpt, hlt_execution_context* ctx);

/// Schedules a job to a virtual thread determined by an object's hash.
///
/// This function determined the target virtual thread by hashing *obj* into
/// the range determined by ``config.vid_schedule_max -
/// config.vid_schedule_min``.
/// 
/// This function is safe to call from all threads.
///
/// mgr: The thread manager to use.
///
/// type: The type of the thread context to be hashed (i.e., *tcontext*).
///
/// tcontext: The thread context for the job, per its module's ``context`` definition.
///
/// cont: The continuation representing a bound function, at +1. (!)
///
/// ctx: The caller's execution context.
///
/// excpt: &
extern void __hlt_thread_mgr_schedule_tcontext(hlt_thread_mgr* mgr, hlt_type_info* type, void* tcontext, hlt_callable* func, hlt_exception** excpt, hlt_execution_context* ctx);

/// Checks whether any worker thread has raised an uncaught exception. In
/// that case, all worker threads will have been terminated, and this
/// function willl raise an hlt_exception_uncaught_thread_exception.
///
/// mgr: The thread manager to use.
///
/// excpt: &
extern void hlt_thread_mgr_check_exceptions(hlt_thread_mgr* mgr, hlt_exception** excpt, hlt_execution_context* ctx);

/// Returns a string identifying the currently running native thread.
///
/// Calling this function is potentially expensive and should be restricted
/// for debugging purposes.
///
/// mgr: The thread manager to use. If NULL, the function will return a
/// string indicating that threading is not available.
///
/// Returns: A string with a readable identification of the current native thread.
extern const char* hlt_thread_mgr_current_native_thread();

/// Initialized a new native thread. Must be called once at the startup of
/// any new native thread *from that thread*. It sets a name identifying the
/// native thread, and also pins it to a core if specified so in the HILTI
/// configuration.
///
/// mgr: The thread manager to use. If NULL, the function will return without doing anything.
///
/// name: A string with a readable identification of the current native thread.
///
/// default_affinity: A core to pin the thread to if the HILTI is configured
/// to use its default affinity scheme (see config.h). Can be -1 to set no
/// affinity by default for this thread.
extern void __hlt_thread_mgr_init_native_thread(hlt_thread_mgr* mgr, const char* name, int default_affinity);

/// Returns true if worker threads should be terminating as soon as possible.
/// This is the main mechanism for flagging to threads that we want to
/// terminate and they need to check this regularly. If indicated so, they
/// must terminate immediately without doing any furhter work.
extern int8_t __hlt_thread_mgr_terminating();

inline static void hlt_thread_mgr_blockable_init(__hlt_thread_mgr_blockable* resource)
{
    resource->num_blocked = 0;
}

extern void __hlt_thread_mgr_unblock(__hlt_thread_mgr_blockable *resource, hlt_execution_context* ctx);

inline static void hlt_thread_mgr_unblock(__hlt_thread_mgr_blockable *resource, hlt_execution_context* ctx)
{
    if ( ! resource->num_blocked )
        return;

    __hlt_thread_mgr_unblock(resource, ctx);
}


#endif
