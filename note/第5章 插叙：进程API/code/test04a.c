#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main() {

	int rc = fork();

	if (rc > 0) {
		printf("ASOUL!\n");
	} else {
//		close(STDOUT_FILENO);
//		open("./test04.output", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
		execl("/bin/ls","ls","-l",NULL);

	}


	return 0;
}

