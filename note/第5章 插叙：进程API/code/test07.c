#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	int rc = fork();
	
	if(rc > 0){
	int rw = wait(NULL);//等待子进程执行完毕
	printf("这是父进程的标准输出\n");
	}else{
	close(STDOUT_FILENO);
	printf("这是子进程的标准输出\n");
	}
	
	return 0;
}
