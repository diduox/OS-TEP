#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main() {
	int pipe_fd[2];
	int rc1,rc2;
	// 创建管道
	int rp = pipe(pipe_fd);

	// 创建第一个子进程
	rc1 = fork();
	
	
	if (rc1 == 0) {
		// 在第一个子进程中
		close(pipe_fd[0]);  // 关闭读端
		
		// 将标准输出重定向到管道写端
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);  // 关闭原始的管道写端
		
		// 在子进程中输出数据
		printf("Hello from Child 1\n");
		
		exit(EXIT_SUCCESS);
	} else {
		// 创建第二个子进程
		rc2 = fork();
		
		if (rc2 == 0) {
			// 在第二个子进程中
			close(pipe_fd[1]);  // 关闭写端
			
			// 将标准输入重定向到管道读端
			dup2(pipe_fd[0], STDIN_FILENO);
			close(pipe_fd[0]);  // 关闭原始的管道读端
			
			// 在子进程中读取并处理数据
			char buffer[256];
			fgets(buffer, sizeof(buffer), stdin);
			printf("Child 2 received: %s", buffer);
			exit(EXIT_SUCCESS);
		} else {
			// 在父进程中
			close(pipe_fd[0]);  // 关闭父进程中未使用的读端
			close(pipe_fd[1]);  // 关闭父进程中未使用的写端
			// 等待两个子进程终止
			waitpid(rc1, NULL, 0);
			waitpid(rc2, NULL, 0);
		}
	}
	
	return 0;
}
