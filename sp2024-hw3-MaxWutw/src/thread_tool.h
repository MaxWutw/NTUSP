#ifndef THREAD_TOOL_H
#define THREAD_TOOL_H

#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

// The maximum number of threads.
#define THREAD_MAX 100


void sighandler(int signum);
void scheduler();

// The thread control block structure.
struct tcb {
    int id;
    int *args;
    // Reveals what resource the thread is waiting for. The values are:
    //  - 0: no resource.
    //  - 1: read lock.
    //  - 2: write lock.
    int waiting_for;
    int sleeping_time;
    jmp_buf env;  // Where the scheduler should jump to.
    int n, i, f_cur, f_prev; // TODO: Add some variables you wish to keep between switches.
};

// The only one thread in the RUNNING state.
extern struct tcb *current_thread;
extern struct tcb *idle_thread;

struct tcb_queue {
    struct tcb *arr[THREAD_MAX];  // The circular array.
    int head;                     // The index of the head of the queue
    int size;
};

extern struct tcb_queue ready_queue, waiting_queue;
extern struct tcb_queue sleeping_set;

// The rwlock structure.
//
// When a thread acquires a type of lock, it should increment the corresponding count.
struct rwlock {
    int read_count;
    int write_count;
};

extern struct rwlock rwlock;

// The remaining spots in classes.
extern int q_p, q_s;

// The maximum running time for each thread.
extern int time_slice;

// The long jump buffer for the scheduler.
extern jmp_buf sched_buf;

// First time step into scheduler.
extern int first_time;

// TODO::
// You should setup your own sleeping set as well as finish the marcos below
#define thread_create(func, t_id, t_args)                                              \
    ({                                                                                 \
	 func(t_id, t_args);\
    })

#define thread_setup(t_id, t_args)                                                     \
    ({                                                                                 \
	 fprintf(stdout, "thread %d: set up routine %s\n", t_id, __func__);\
	 struct tcb *new_tcb = (struct tcb*)malloc(1 * sizeof(struct tcb));\
	 new_tcb->id = t_id;\
	 new_tcb->args = t_args;\
	 new_tcb->waiting_for = 0;\
	 new_tcb->i = 0;\
	 new_tcb->f_cur = 0;\
	 new_tcb->f_prev = 0;\
	 if(strcmp(__func__, "idle") == 0){\
	 	idle_thread = new_tcb;\
		if(setjmp(idle_thread->env) == 0)\
			return;\
	 }\
	 else{\
		ready_queue.arr[ready_queue.size] = new_tcb;\
		if(setjmp(ready_queue.arr[ready_queue.size++]->env) == 0)\
			return;\
	 }\
    })

#define thread_yield()                                  \
    ({                                                  \
	 if(setjmp(current_thread->env) == 0){\
		 sigset_t mask;\
		 sigemptyset(&mask);\
		 sigaddset(&mask, SIGTSTP);\
		 sigprocmask(SIG_UNBLOCK, &mask, NULL);\
		 sigprocmask(SIG_BLOCK, &mask, NULL);\
		 \
		 sigemptyset(&mask);\
		 sigaddset(&mask, SIGTSTP);\
		 sigprocmask(SIG_UNBLOCK, &mask, NULL);\
		 sigprocmask(SIG_BLOCK, &mask, NULL);\
		 \
		 sigemptyset(&mask);\
		 sigaddset(&mask, SIGALRM);\
		 sigprocmask(SIG_UNBLOCK, &mask, NULL);\
		 sigprocmask(SIG_BLOCK, &mask, NULL);\
		 \
		 sigemptyset(&mask);\
		 sigaddset(&mask, SIGALRM);\
		 sigprocmask(SIG_UNBLOCK, &mask, NULL);\
		 sigprocmask(SIG_BLOCK, &mask, NULL);\
	 }\
    })

#define read_lock()                                                      \
    ({                                                                   \
	 if(rwlock.write_count != 0){\
		 if(setjmp(current_thread->env) == 0){\
		 	waiting_queue.arr[waiting_queue.size] = current_thread;\
			waiting_queue.size = (waiting_queue.size + 1) % THREAD_MAX;\
		 }\
	 }\
	 else{\
	 	rwlock.read_count++;\
	 }\
    })

#define write_lock()                                                     \
    ({                                                                   \
	 if(rwlock.write_count != 0 || rwlock.read_count != 0){\
		 if(setjmp(current_thread->env) == 0){\
		 	waiting_queue.arr[waiting_queue.size] = current_thread;\
			waiting_queue.size = (waiting_queue.size + 1) % THREAD_MAX;\
		 }\
	 }\
	 else{\
	 	rwlock.write_count = 1;\
	 }\
    })

#define read_unlock()                                                                 \
    ({                                                                                \
	 if(rwlock.read_lock > 0) rwlock.read_lock--;\
    })

#define write_unlock()                                                                \
    ({                                                                                \
	 if(rwlock.write_lock == 1) rwlock.write_lock = 0;\
    })

#define thread_sleep(sec)                                            \
    ({                                                               \
	 int head = sleeping.head;\
	 sleeping_set.arr[head]->sleeping_time = sec;\
    })

#define thread_awake(t_id)                                                        \
    ({                                                                            \
    })

#define thread_exit()                                    \
    ({                                                   \
	 fprintf(stdout, "thread %d: exit\n", current_thread->id);\
	 longjmp(sched_buf, 3);\
    })

#endif  // THREAD_TOOL_H
