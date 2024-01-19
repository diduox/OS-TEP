#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
int main(){
	
	int rc = fork();
	
	if(rc > 0){
		int rw = wait(NULL);
		printf("父进程的wait()返回%d\n",rw);
		printf("父进程的进程号%d\n",getpid());
	}else{
		printf("子进程的进程号%d\n",getpid());
		printf("子进程执行完成\n");
		
	}
	
	
	return 0;
}

