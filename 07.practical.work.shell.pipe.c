#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

int childpid;
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

int main() {
    // ctrl z
    signal(SIGTSTP, handler);
    signal(SIGINT, handler);
    signal(SIGTERM, handler);
	char cmd[1000];
	while(1){
		printf("$ ");
		fgets(cmd, sizeof(cmd), stdin);
		if (strcmp(cmd,"/q\n") == 0) break;
            char* args[100];
            memset(args,0,sizeof(args));
            int argc = 0;
            int prevArgPos = 0;
            int len = strlen(cmd);
            for( int i = 0 ; i < len; ++i){
                if(cmd[i] == ' ' || i == len - 1){
                    args[argc++] = &cmd[prevArgPos];
                    cmd[i] = 0;
                    prevArgPos = i+1;
                }
            }
            int writePos = -1;
            for (int i = 0; i < argc; i++) {
                if (args[i] && strcmp(args[i], ">") == 0) {
                    writePos = i;
                    args[i] = 0;
                    break;		// one write at the moment only
                }
            }
            int pipePos = -1;
            for (int i = 0; i < argc; i++) {
                if (args[i] && strcmp(args[i], "|") == 0) {
                    pipePos = i;
                    args[i] = 0;
                    break;		// one pipe at the moment only
                }
            }
            if (pipePos < 0){
                int outputFd = -1;
                if (writePos > 0) {
                    // create an output file for this one
                    outputFd = creat(args[writePos + 1], O_CREAT | O_TRUNC | O_WRONLY |
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                }

                int pid = fork();
                if (pid) {
                    // parent
                    childpid = pid;				
                    waitpid(&pid, NULL, 0);
                }
                else {
                    // child, exec
                    if (outputFd > 0) {
                        printf("child, redirecting to fd %d...\n", outputFd);
                        dup2(outputFd, 1);
                    }				
                    execvp(args[0], args);
                }
            }
            else {
                int mypipe[2];
                pipe(mypipe);

                // pipe, fork 2 times
                int pid1 = fork();
                if (pid1 == 0) {
                    // first child
                    dup2(mypipe[1], 1);
                    close(mypipe[0]);
                    close(mypipe[1]);
                    // for (int i = 0; i < pipePos - 1; i++) {
                    // 	printf("%d arg %s\n", i, args[i]);
                    // }
                    execvp(args[0], args);
                }

                int pid2 = fork();
                if (pid2 == 0) {
                    // second child
                    dup2(mypipe[0], 0);
                    close(mypipe[0]);
                    close(mypipe[1]);

                    // for (int i = pipePos + 1; i < argc; i++) {
                    // 	printf("%d arg %s\n", i, args[i]);
                    // }
                    execvp(args[pipePos+1], &args[pipePos + 1]);
                }
                close(mypipe[0]);
                close(mypipe[1]);

                waitpid(pid1, NULL, 0);
                waitpid(pid2, NULL, 0);
            }
	}
	return 0;
}