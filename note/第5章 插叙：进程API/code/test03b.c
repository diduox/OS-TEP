#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	//创建一个无名管道，实现单向通信
	int pipe_fd[2];
	pipe(pipe_fd);

	int rc = fork();

	if (rc > 0) {
		close(pipe_fd[1]);  //关闭写端(这只是一种良好编程习惯，代表以后不需要写了)
		char buffer[1];
		//如果没有可读的信息，就会放置
		read(pipe_fd[0],buffer,1);
		printf("goodbye\n");
		close(pipe_fd[0]); //关闭读端
	} else if (rc == 0) {
		close(pipe_fd[0]);
		printf("hello\n");
		write(pipe_fd[1], "1", 1); //向管道写入一个字节
		close(pipe_fd[1]);//关闭写端
	}

}
