#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "routine.h"
#include "thread_tool.h"

struct tcb_queue sleeping_set;
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

	// for(int i = 0;i <= sleeping_set.size;i++){
	// 	struct tcb *tcb = sleeping_set.arr[i];
	// 	tcb->sleeping_time -= time_slice;
	// 	if(tcb->sleeping_time <= 0){
	// 		ready_queue.head++;
	// 		ready_queue.arr[ready_queue.head] = tcb;
	// 		for(int j = i + 1;j <= sleeping_set.size;j++){
	// 			sleeping_set[j - 1] = sleeping_set[j];
	// 		}
	// 		sleeping_set.size--;
	// 	}
	// }

	// for(int i = 0;i < waiting_queue.size;i++){

	// }

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

	}
	if(ready_queue.head == ready_queue.size){
		int judge = 1;
		if(sleeping_set.head == sleeping_set.size) judge = 0;
		if(judge){
			current_thread = idle_thread;
		}
		else{
			free(current_thread);
			return;
		}
	}
	else{
		current_thread = ready_queue.arr[ready_queue.head];
		ready_queue.head = (ready_queue.head + 1) % THREAD_MAX;
	}
	longjmp(current_thread->env, 1);
}
