#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("test02.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);

    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // 子进程
        
	for(int i = 1 ; i <= 100 ; i++)write(fd, "Child Process\n", 15);
    } else {
        // 父进程
     for(int i = 1 ; i <= 100 ; i++)write(fd, "Parent Process\n", 16);
    }

    close(fd);

    return 0;
}

