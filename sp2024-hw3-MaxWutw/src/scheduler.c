#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "routine.h"
#include "thread_tool.h"

struct tcb *sleeping_set[THREAD_MAX + 1];
// TODO::
// Prints out the signal you received.
// This function should not return. Instead, jumps to the scheduler.
void sighandler(int signum) {
    // Your code here
	if(signum == SIGTSTP){
		fprintf(stdout, "caught SIGTSTP\n");
		longjmp(sched_buf, 1);
	}
	else if(signum == SIGALRM){
		fprintf(stdout, "caught SIGALRM\n");
		longjmp(sched_buf, 1);
	}
}

// TODO::
// Perfectly setting up your scheduler.
void scheduler() {
    // Your code here
	int status = setjmp(sched_buf);
	if(first_time){
		int *thread_arg = NULL;
		thread_create(idle, 0, thread_arg);
		first_time = 0;
	}
	alarm(time_slice);
	struct sigaction sa_ignore, sa_tstp, sa_alrm;
	sa_ignore.sa_handler = SIG_IGN;
	sigemptyset(&sa_ignore.sa_mask);
	sa_ignore.sa_flags = 0;

	// set to ignore
	sigaction(SIGTSTP, &sa_ignore, &sa_tstp);
	sigaction(SIGALRM, &sa_ignore, &sa_alrm);

	// set to origin disposition
	sigaction(SIGTSTP, &sa_tstp, NULL);
	sigaction(SIGALRM, &sa_alrm, NULL);

	for(int i = 0;i < THREAD_MAX;i++){
		struct tcb *tcb = sleeping_set[i];
		if(tcb == NULL) continue;
		// printf("In sleeping set: %d\n", tcb->id);
		tcb->sleeping_time -= time_slice;
		// printf("id: %d, sleeping: %d\n", tcb->id, tcb->sleeping_time);
		if(tcb->sleeping_time <= 0){
			// printf("sleeping_time to ready: %d\n", tcb->id);
			ready_queue.arr[ready_queue.size] = tcb;
			ready_queue.size = (ready_queue.size + 1) % THREAD_MAX;
			tcb->keepgo = 1;
			// printf("free: %d\n", tcb->id);
			sleeping_set[i] = NULL;
		}
	}

	for(int i = waiting_queue.head;i < waiting_queue.size;i++){
		struct tcb *tcb = waiting_queue.arr[i];
		if(tcb->waiting_for == 4){
			if(rwlock.write_count == 0){
				ready_queue.arr[ready_queue.size] = tcb;
				ready_queue.size = (ready_queue.size + 1) % THREAD_MAX;
				waiting_queue.head = (waiting_queue.head + 1) % THREAD_MAX;
			}
		}
		else if(tcb->waiting_for == 5){
			if(rwlock.write_count == 0 && rwlock.read_count == 0){
				ready_queue.arr[ready_queue.size] = tcb;
				ready_queue.size = (ready_queue.size + 1) % THREAD_MAX;
				waiting_queue.head = (waiting_queue.head + 1) % THREAD_MAX;
			}
		}
	}

	if(status == 1){ // from sighandler
		if(current_thread->id != 0){
			ready_queue.arr[ready_queue.size] = current_thread;
			ready_queue.size = (ready_queue.size + 1) % THREAD_MAX;
		}
	}
	else if(status == 2){ // from sleep
		// pass
	}
	else if(status == 3){ // from exit
		free(current_thread);
	}
	else if(status == 4){ // from read lock

	}
	else if(status == 5){ // from write lock
		
	}
	else{
		// pass
	}
	if(ready_queue.head == ready_queue.size){
		int judge = 0;
		for(int i = 0;i < THREAD_MAX;i++){
			if(sleeping_set[i] != NULL){
				judge = 1;
				break;
			}
		}
		if(judge){
			current_thread = idle_thread;
		}
		else{
			current_thread = idle_thread;
			free(current_thread);
			return;
		}
	}
	else{
		while(ready_queue.head != ready_queue.size){
			int leave = 1;
			for(int i = 0;i < THREAD_MAX;i++){
				if(sleeping_set[i] != NULL && sleeping_set[i]->id == ready_queue.arr[ready_queue.head]->id){
					ready_queue.head = (ready_queue.head + 1) % THREAD_MAX;
					leave = 0;
					break;
				}
			}
			if(leave) break;
		}
		if(ready_queue.head == ready_queue.size){
			current_thread = idle_thread;
		}
		else{
			current_thread = ready_queue.arr[ready_queue.head];
			ready_queue.head = (ready_queue.head + 1) % THREAD_MAX;
		}
	}
	// printf("current id: %d\n", current_thread->id);
	longjmp(current_thread->env, 1);
}
