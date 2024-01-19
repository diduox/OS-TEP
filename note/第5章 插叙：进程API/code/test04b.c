#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main() {

	int rc = fork();

	if (rc > 0) {
		printf("ASOUL!\n");
	} else {
		execlp("ls","ls","-l",NULL);
	}
	return 0;
}

