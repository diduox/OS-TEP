#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
/*
  getppid() 获取当前进程的父进程号
  kill() 使一个进程执行某个信号量所代表的函数
  signal();将一个信号量和一个函数绑定在一起
  SIGUSR1 系统提供的专为自定义使用的信号量
  pause() 系统调用，使当前进程挂起
 */
void refresh(){//不执行任何操作，只用调用来中止pause
	
}

int main() {
	//将信号和函数绑定
	signal(SIGUSR1,refresh);
	
	int rc = fork();
	//为父进程
	if(rc > 0){
		//根据操作系统的调度机制，当一个进程被挂起时，系统就会切换另一个已经就绪的进程执行
		pause();
		printf("goodbye\n");
	}else{
		printf("hello\n");
		kill(getppid(),SIGUSR1);
	}
	
	return 0;
}

