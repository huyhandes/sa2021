#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// int childpid;
int main() {
	char cmd[1000];
	while(1){
		printf("$ ");
		fgets(cmd, sizeof(cmd), stdin);
		if (strcmp(cmd,"/q\n") == 0) break;
		int pid = fork();
		if(pid == 0) { // this is child who execute shell
			char* args[] = {"/bin/bash", "-c", cmd, NULL};
			execvp(args[0], args);
		} 
		else { // this is parent who wait for next comment
			// childpid = pid;
			wait(&pid);
		}
	}
	return 0;
}