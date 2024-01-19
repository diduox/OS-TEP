#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(){
	int x = 100;
	int rc = fork();
	if(rc == 0){
	//	int wc = wait(NULL);
		x = 110;
		printf("我是子进程,x = %d",x);
	}else{
		int ec = wait(NULL);
	//	x = 110;
		printf("我是父进程,x = %d",x);
	}
	return 0;
}
