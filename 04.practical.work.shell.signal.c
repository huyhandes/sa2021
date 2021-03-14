#include<stdio.h>
#include<signal.h>

void handler(int signal_num) {
    printf("Signal %d => ", signal_num);
    switch (signal_num) {
        case SIGTSTP:
            printf("pause\n");
            break;
        case SIGINT:
        case SIGTERM:
            printf("Terminated\n");
            exit(0);
            break;
    }
}
void random_run(){
    for(int i = 0; i < 1e10; i++);
}
int main(void) {
    // ctrl z
    signal(SIGTSTP, handler);
    
    while(1){
		printf("$ ");
		//int argc = getArg();
		int pid = fork();
		if(pid == 0) { // this is child who execute shell
        // ctrl c or killed
            signal(SIGTSTP, handler);
            random_run();
		} 
		else { // this is parent who wait for next comment
            kill(&pid,SIGTSTP);
			wait(&pid);
		}
	}
    return 0;
}