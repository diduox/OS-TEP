#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/wait.h>

int main() {

    int rc = fork();

    if (rc > 0) {
        printf("ASOUL!\n");
    } else {
		char *envp[] = {"HOME=/home/user", "USER=user", NULL};
        execle("/bin/ls","ls","-l",NULL,envp);
    }
    return 0;
}

