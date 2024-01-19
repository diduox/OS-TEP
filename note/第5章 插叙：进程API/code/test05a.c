#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
int main(){

    int rc = fork();

    if(rc > 0){
        printf("父进程的进程号%d\n",getpid());
		printf("父进程执行完成\n");
    }else{
		int rw = wait(NULL);
		printf("子进程的wait()返回%d\n",rw);
        printf("子进程的进程号%d\n",getpid());

    }


    return 0;
}


