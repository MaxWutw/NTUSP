#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "hw2.h"
#include "utility.h"

#define ERR_EXIT(s) perror(s), exit(errno);

/*
If you need help from TAs,
please remember :
0. Show your efforts
    0.1 Fully understand course materials
    0.2 Read the spec thoroughly, including frequently updated FAQ section
    0.3 Use online resources
    0.4 Ask your friends while avoiding plagiarism, they might be able to understand you better, since the TAs know the solution, 
        they might not understand what you're trying to do as quickly as someone who is also doing this homework.
1. be respectful
2. the quality of your question will directly impact the value of the response you get.
3. think about what you want from your question, what is the response you expect to get
4. what do you want the TA to help you with. 
    4.0 Unrelated to Homework (wsl, workstation, systems configuration)
    4.1 Debug
    4.2 Logic evaluation (we may answer doable yes or no, but not always correct or incorrect, as it might be giving out the solution)
    4.3 Spec details inquiry
    4.4 Testcase possibility
5. If the solution to answering your question requires the TA to look at your code, you probably shouldn't ask it.
6. We CANNOT tell you the answer, but we can tell you how your current effort may approach it.
7. If you come with nothing, we cannot help you with anything.
*/

// somethings I recommend leaving here, but you may delete as you please
static char root[MAX_FRIEND_INFO_LEN] = "Not_Tako";     // root of tree
static char friend_info[MAX_FRIEND_INFO_LEN];   // current process info
static char friend_name[MAX_FRIEND_NAME_LEN];   // current process name
static int friend_value;    // current process value
FILE* read_fp = NULL;

// Is Root of tree
static inline bool is_Not_Tako() {
    return (strcmp(friend_name, root) == 0);
}

// a bunch of prints for you
void print_direct_meet(char *friend_name) {
    fprintf(stdout, "Not_Tako has met %s by himself\n", friend_name);
}

void print_indirect_meet(char *parent_friend_name, char *child_friend_name) {
    fprintf(stdout, "Not_Tako has met %s through %s\n", child_friend_name, parent_friend_name);
}

void print_fail_meet(char *parent_friend_name, char *child_friend_name) {
    fprintf(stdout, "Not_Tako does not know %s to meet %s\n", parent_friend_name, child_friend_name);
}

void print_fail_check(char *parent_friend_name){
    fprintf(stdout, "Not_Tako has checked, he doesn't know %s\n", parent_friend_name);
}

void print_success_adopt(char *parent_friend_name, char *child_friend_name) {
    fprintf(stdout, "%s has adopted %s\n", parent_friend_name, child_friend_name);
}

void print_fail_adopt(char *parent_friend_name, char *child_friend_name) {
    fprintf(stdout, "%s is a descendant of %s\n", parent_friend_name, child_friend_name);
}

void print_compare_gtr(char *friend_name){
    fprintf(stdout, "Not_Tako is still friends with %s\n", friend_name);
}

void print_compare_leq(char *friend_name){
    fprintf(stdout, "%s is dead to Not_Tako\n", friend_name);
}

void print_final_graduate(){
    fprintf(stdout, "Congratulations! You've finished Not_Tako's annoying tasks!\n");
}

/* terminate child pseudo code
void clean_child(){
    close(child read_fd);
    close(child write_fd);
    call wait() or waitpid() to reap child; // this is blocking
}

*/

/* remember read and write may not be fully transmitted in HW1?
void fully_write(int write_fd, void *write_buf, int write_len);

void fully_read(int read_fd, void *read_buf, int read_len);

please do above 2 functions to save some time
*/


void fully_read(int read_fd, void *read_buf, int read_len){
	int32_t readin = 0;
	while(readin < read_len){
		int32_t result = read(read_fd, (char *)read_buf + readin, read_len - readin);
		if(result > 0){
			readin += result;
		}
		else if(result == 0){
			break;
		}
		else{
			ERR_EXIT("Error to read");
			break;
		}
	}
}

void fully_write(int write_fd, void *write_buf, int write_len){
	int32_t written = 0;
	while(written < write_len){
		int32_t result = write(write_fd, (char *)write_buf + written, write_len - written);
		if(result > 0){
			written += result;
		}
		else if(result == 0){
			break;
		}
		else{
			ERR_EXIT("Error to write");
			break;
		}
	}
}

int main(int argc, char *argv[]) {
    // Hi! Welcome to SP Homework 2, I hope you have fun
	sList *pList = (sList *)malloc(1 * sizeof(sList));
	initList(pList);
	regFreeCallback(pList, myfree);
	regPrintCallback(pList, myprint);
    pid_t process_pid = getpid(); // you might need this when using fork()
	// printf("Current process pid: %d\n", process_pid);
    if(argc != 2){
        fprintf(stderr, "Usage: ./friend [friend_info]\n");
        return 0;
    }
    setvbuf(stdout, NULL, _IONBF, 0); // prevent buffered I/O, equivalent to fflush() after each stdout, study this as you may need to do it for other friends against their parents
    
    // put argument one into friend_info
    strncpy(friend_info, argv[1], MAX_FRIEND_INFO_LEN);
    
    if(strcmp(argv[1], root) == 0){
        // is Not_Tako
        strncpy(friend_name, friend_info,MAX_FRIEND_NAME_LEN);      // put name into friend_nae
        friend_name[MAX_FRIEND_NAME_LEN - 1] = '\0';        // in case strcmp messes with you
        read_fp = stdin;        // takes commands from stdin
        friend_value = 100;     // Not_Tako adopting nodes will not mod their values
    }
    else{
        // is other friends
        // extract name and value from info
        // where do you read from?
        // anything else you have to do before you start taking commands?
		char outProcessID[128];
		char cmd[MAX_CMD_LEN];
		fscanf(stdin, "%[^\n]", cmd);
		getchar();
		// parse
		char *command;
		command = strtok(cmd, " ");
		char *parent, *child;
		int32_t childVal = 0;
		char childName[128];
		parent = strtok(NULL, " ");
		child = strtok(NULL, " ");
		sscanf(child, "%[^_]_%d", childName, &childVal);
		strncpy(friend_name, childName, MAX_FRIEND_NAME_LEN);
        friend_name[MAX_FRIEND_NAME_LEN - 1] = '\0';        // in case strcmp messes with you
		read_fp = stdin;
		friend_value = childVal;
		printf("PID: %d, Name: %s, Value: %d\n", process_pid, childName, childVal);
		//finish parse command
		snprintf(outProcessID, 128, "process id: %d\n", process_pid);
		// fully_write(STDOUT_FILENO, outProcessID, strlen(outProcessID));
		sscanf(friend_info, "%[^_]_%d", friend_name, &friend_value);
    	friend_name[MAX_FRIEND_NAME_LEN - 1] = '\0';
    }

    //TODO:
    /* you may follow SOP if you wish, but it is not guaranteed to consider every possible outcome

    1. read from parent/stdin
    2. determine what the command is (Meet, Check, Adopt, Graduate, Compare(bonus)), I recommend using strcmp() and/or char check
    3. find out who should execute the command (extract information received)
    4. execute the command or tell the requested friend to execute the command
        4.1 command passing may be required here
    5. after previous command is done, repeat step 1.
    */
	while(true){
		char cmd[MAX_CMD_LEN], toChildCmd[MAX_CMD_LEN];
	 	printf("Now is read from: %d\n", process_pid);
		fscanf(stdin, "%[^\n]", cmd);
		getchar();
		strcpy(toChildCmd, cmd);
		char *command;
		command = strtok(cmd, " ");
		// fprintf(stdout, "%s\n", command);

		// Hint: do not return before receiving the command "Graduate"
		// please keep in mind that every process runs this exact same program, so think of all the possible cases and implement them
		if(strcmp(command, "Meet") == 0){
			char *parent, *child;
			int32_t childVal = 0;
			char childName[128];
			parent = strtok(NULL, " ");
			child = strtok(NULL, " ");
			sscanf(child, "%[^_]_%d", childName, &childVal);
			// if(strcmp(parent, friend_name) != 0){

			// }
			// printf("%s\n%s\n", parent, child);
			// printf("%s %d\n", childName, childVal);
			int32_t pipefdpw[2], pipefdcw[2];
			if(pipe(pipefdpw) < 0){
				ERR_EXIT("Error create a pipe");
			}
			if(pipe(pipefdcw) < 0){
				ERR_EXIT("Error create a pipe");
			}
			pid_t pid;
			if((pid = fork()) == 0){ // child process
				close(pipefdpw[1]);
				close(pipefdcw[0]);
				dup2(pipefdpw[0], STDIN_FILENO);
				dup2(pipefdcw[1], STDOUT_FILENO);
				close(pipefdpw[0]);
				close(pipefdcw[1]);
				// printf("%s\n", childName);
				execl("./friend", "./friend", childName, NULL);
				ERR_EXIT("Exec Failed");
			}
			else{ // parent process
				close(pipefdpw[0]);
				close(pipefdcw[1]);
				char fromChild[100];
				if(pList->pParam->size == 0){
					sNode *newNode = (sNode *)malloc(1 * sizeof(sNode));
					newNode->next = NULL;
					newNode->prev = NULL;

					friend *newFriend = (friend *)malloc(1 * sizeof(friend));
					newFriend->pid = pid;
					strcpy(newFriend->name, childName);
					newFriend->value = childVal;
					newNode->data = (void *)newFriend;

					pList->pHead->next = newNode;
					pList->pTail->prev = newNode;
				}
				else{
					sNode *newNode = (sNode *)malloc(1 * sizeof(sNode));
					newNode->next = NULL;
					newNode->prev = NULL;

					friend *newFriend = (friend *)malloc(1 * sizeof(friend));
					newFriend->pid = pid;
					strcpy(newFriend->name, childName);
					newFriend->value = childVal;
					newNode->data = (void *)newFriend;

					pList->pTail->prev->next = newNode;
					pList->pTail->prev = newNode;
				}
				int32_t leng = strlen(toChildCmd);
				toChildCmd[leng] = '\n';
				leng++;
				toChildCmd[leng] = '\0';
				write(pipefdpw[1], toChildCmd, leng);
				read(pipefdcw[0], fromChild, 100);
				printf("from child sent: %s\n", fromChild);
				// waitpid(pid, NULL, 0);
			}
		}
	}

    /* pseudo code
    if(Meet){
        create array[2]
        make pipe
        use fork.
            Hint: remember to fully understand how fork works, what it copies or doesn't
        check if you are parent or child
        as parent or child, think about what you do next.
            Hint: child needs to run this program again
    }
    else if(Check){
        obtain the info of this subtree, what are their info?
        distribute the info into levels 1 to 7 (refer to Additional Specifications: subtree level <= 7)
        use above distribution to print out level by level
            Q: why do above? can you make each process print itself?
            Hint: we can only print line by line, is DFS or BFS better in this case?
    }
    else if(Graduate){
        perform Check
        terminate the entire subtree
            Q1: what resources have to be cleaned up and why?
            Hint: Check pseudo code above
            Q2: what commands needs to be executed? what are their orders to avoid deadlock or infinite blocking?
            A: (tell child to die, reap child, tell parent you're dead, return (die))
    }
    else if(Adopt){
        remember to make fifo
        obtain the info of child node subtree, what are their info?
            Q: look at the info you got, how do you know where they are in the subtree?
            Hint: Think about how to recreate the subtree to design your info format
        A. terminate the entire child node subtree
        B. send the info through FIFO to parent node
            Q: why FIFO? will usin pipe here work? why of why not?
            Hint: Think about time efficiency, and message length
        C. parent node recreate the child node subtree with the obtained info
            Q: which of A, B and C should be done first? does parent child position in the tree matter?
            Hint: when does blocking occur when using FIFO?(mkfifo, open, read, write, unlink)
        please remember to mod the values of the subtree, you may use bruteforce methods to do this part (I did)
        also remember to print the output
    }
    else if(full_cmd[1] == 'o'){
        Bonus has no hints :D
    }
    else{
        there's an error, we only have valid commmands in the test cases
        fprintf(stderr, "%s received error input : %s\n", friend_name, full_cmd); // use this to print out what you received
    }
    */

   // final print, please leave this in, it may bepart of the test case output
    if(is_Not_Tako()){
        print_final_graduate();
    }
    return 0;
}
