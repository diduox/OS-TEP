#include <stdio.h>
#include <assert.h>
#include <pthread.h>

//线程函数的参数类型被限制为 void *。这种设计是为了通用性和灵活性。
void *mythread(void *arg){
	printf("%s\n",(char *)arg);
	return NULL;
}

int
main(int argc,char *argv[]){
//一个抽象结构，具体取决于编译器	
	pthread_t p1,p2;
//return code
	int rc;
	printf("main:gebin\n");
//当创建成功create会返回0,如果assert内的值为false，	程序会中止，并打印错误信息。
	rc = pthread_create(&p1, NULL ,mythread,"A");assert(rc == 0);
	rc = pthread_create(&p2, NULL ,mythread,"B");assert(rc == 0);
//如果join一个已经完成的线程，其会立即退出
	rc = pthread_join(p1,NULL);assert(rc == 0);
	rc = pthread_join(p2,NULL);assert(rc == 0);
	printf("main:end\n");
	return 0;
}
