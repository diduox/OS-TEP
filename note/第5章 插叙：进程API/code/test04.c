#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(){

int rc = fork();

if(rc > 0){
printf("ASOUL!\n");
}else{
close(STDOUT_FILENO);
open("./test04.output",O_CREAT|O_WRONLY|O_TRUNC,S_IRWXU);
char *myargs[3];
myargs[0] = "ls";
myargs[1] = "-l";
myargs[2] = NULL;//一定要写NULL
execvp(myargs[0],myargs);

}


return 0;
}
