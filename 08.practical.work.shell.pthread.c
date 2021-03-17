#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>

int childpid;
char cmd[1000];
void handler(int signal_num) {
    printf("Signal %d => ", signal_num);
    switch (signal_num) {
        case SIGTSTP:
            printf("pause\n");
            kill(childpid,SIGTSTP);
            break;
        case SIGINT:
        case SIGTERM:
            printf("Terminated\n");
            exit(0);
            break;
    }
}

void *backgoundTask(){
    int pid = fork();
    if(pid == 0) { // this is child who execute shell
        char* args[] = {"/bin/bash", "-c", cmd, NULL};
        execvp(args[0], args);
    } 
    else { // this is parent who wait for next comment
        childpid = pid;
        wait(&pid);
    }
}

int main() {
    // ctrl z
    signal(SIGTSTP, handler);
    signal(SIGINT, handler);
    signal(SIGTERM, handler);
	while(1){
		printf("$ ");
        memset(cmd,0,sizeof(cmd));
		fgets(cmd, sizeof(cmd), stdin);
		if (strcmp(cmd,"/q\n") == 0) break;
		pthread_t tid;
        pthread_create(&tid, NULL, backgoundTask, NULL);
        pthread_join(tid, NULL);
	}
	return 0;
}