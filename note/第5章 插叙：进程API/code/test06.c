#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	int rc = fork();
	
	if(rc > 0){
	int status;
	int rw = waitpid(rc, &status, 0);
	printf("父进程\n");
	}else{
	printf("子进程\n");	
	}
	
	return 0;
}
