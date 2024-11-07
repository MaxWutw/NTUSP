#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int glob = 6;
int main(void){
	int var;
	pid_t pid;
	var = 88;
	if ((pid = vfork()) < 0) {
        printf("vfork error");
        exit(-1);
    } else if (pid == 0) { /* 子进程 */
        var++;
		glob++;
        return 0;
    }
    printf("pid=%d, glob=%d, var=%d\n", getpid(), glob, var);
    return 0;
}
