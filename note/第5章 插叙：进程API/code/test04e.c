#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
int main() {
    int rc = fork();
    if (rc > 0) {
        printf("ASOUL!\n");
    } else {
		char *envp[] = {"HOME/home/user", "USER = user", NULL};
        char *myargs[0];
        myargs[0] = "ls";
        myargs[1] = "-l";
        myargs[2] = NULL;//一定要写NULL
        execvpe(myargs[0], myargs,envp);
    }
    return 0;
}
