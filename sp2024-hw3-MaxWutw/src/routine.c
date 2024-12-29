#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "thread_tool.h"

void idle(int id, int *args) {
    // TODO:: IDLE ^-^
	thread_setup(id, args);

	sleep(1);
	thread_yield();
}

void fibonacci(int id, int *args) {
    thread_setup(id, args);

    current_thread->n = current_thread->args[0];
    for (current_thread->i = 1;; current_thread->i++) {
        if (current_thread->i <= 2) {
            current_thread->f_cur = 1;
            current_thread->f_prev = 1;
        } else {
            int f_next = current_thread->f_cur + current_thread->f_prev;
            current_thread->f_prev = current_thread->f_cur;
            current_thread->f_cur = f_next;
        }
        printf("thread %d: F_%d = %d\n", current_thread->id, current_thread->i,
               current_thread->f_cur);

        sleep(1);

        if (current_thread->i == current_thread->n) {
            thread_exit();
        } else {
            thread_yield();
        }
    }
}

void pm(int id, int *args) {
    // TODO:: pm ^^--^^
	thread_setup(id, args);

	current_thread->n = current_thread->args[0];
	for(current_thread->i = 1;;current_thread->i++){
		if(current_thread->i == 1){
			current_thread->f_cur = 1;
			current_thread->f_prev = 1;
		}
		else{
			current_thread->f_prev = current_thread->f_cur;
			int f_next = ((current_thread->i & 1 ? 1 : -1) * current_thread->i) + current_thread->f_prev;
			current_thread->f_cur = f_next;
		}
        printf("thread %d: pm(%d) = %d\n", current_thread->id, current_thread->i,
               current_thread->f_cur);

		sleep(1);

		if(current_thread->i == current_thread->n){
			thread_exit();
		}
		else{
			thread_yield();
		}
	}
}

void enroll(int id, int *args) {
    // TODO:: enroll !! -^-
	thread_setup(id, args);
	
	int dp = current_thread->args[0];
	int ds = current_thread->args[1];
	int s = current_thread->args[2];
	int b = current_thread->args[3];

	fprintf(stdout, "thread %d: sleep %d\n", current_thread->id, s);
	thread_sleep(s);
	thread_awake(b);

	read_lock();
	fprintf(stdout, "thread %d: acquire read lock\n", current_thread->id);
	sleep(1);
	if(current_thread->i == current_thread->n){
		thread_exit();
	}
	else{
		thread_yield();
	}

	read_unlock();
	int pp = dp * q_p, ps = ds * q_s;
	fprintf(stdout, "thread %d: release read lock, p_p = %d, p_s = %d\n", current_thread->id, pp, ps);
	sleep(1);
	if(current_thread->i == current_thread->n){
		thread_exit();
	}
	else{
		thread_yield();
	}

	write_lock();
	fprintf(stdout, "thread %d: acquire write lock, enroll in %s\n", current_thread->id, (pp > ps ? "pj_class" : "sw_class"));
	sleep(1);
	if(current_thread->i == current_thread->n){
		thread_exit();
	}
	else{
		thread_yield();
	}

	write_unlock();
	fprintf(stdout, "thread %d: release write lock\n", current_thread->id);
	sleep(1);
	if(current_thread->i == current_thread->n){
		thread_exit();
	}
	else{
		thread_yield();
	}
}

