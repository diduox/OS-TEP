#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main() {
	int rc = fork();
	if (rc > 0) {
		printf("ASOUL!\n");
	} else {
		char *myargs[0];
		myargs[0] = "ls";
		myargs[1] = "-l";
		myargs[2] = NULL;//一定要写NULL
		execv("/bin/ls", myargs);

	}
	return 0;
}
