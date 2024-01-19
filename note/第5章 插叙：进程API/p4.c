#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
int
main(int argc,char *argv[]){
	int rc = fork();
	if(rc < 0){//fork failed ;exit
		fprintf(stderr,"fork failed\n");
		exit(1);	
	}else if(rc == 0){//child (new process)
		close(STDOUT_FILENO);
		open("./p4.output",O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
		char *myargs[3];
		myargs[0] = strdup("wc");
		myargs[1] = strdup("p4.c");
		myargs[2] = NULL;
		execvp(myargs[0],myargs);
	}else{//parent goes down this path(main)
		int wc = wait(NULL);
	}
	
}
