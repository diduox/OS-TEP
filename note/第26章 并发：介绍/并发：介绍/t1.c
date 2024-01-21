#include <stdio.h>
#include <pthread.h>
//#include "mythreads.h"
//代表这是个静态变量（相当于全局变量，但是只在此文件中可见），并且此变量不该被编译器进行优化
static volatile int counter = 0;

//
// mythread()
//

// Simply adds 1 to counter repeatedly, in a loop
// No, this is not how you would add 10,000,000 to
// a counter, but it shows the problem nicely.
//
void *
mythread(void *arg) {
	printf("%s: begin\n", (char *) arg);
	int i;
	for (i = 0; i < 1e7; i++) {
		counter ++;
	}
	printf("%s: done\n", (char *) arg);
	return NULL;
}

//
// main()
//
// Just launches two threads (pthread_create)
// and then waits for them (pthread_join)
//
int
main(int argc, char *argv[]) {
	pthread_t p1, p2;
	int rc;
	printf("main: begin (counter = %d)\n", counter);
	rc = pthread_create(&p1, NULL, mythread, "A");
	rc = pthread_create(&p2, NULL, mythread, "B");

	// join waits for the threads to finish
	rc = pthread_join(p1, NULL);
	rc = pthread_join(p2, NULL);
	printf("main: done with both (counter = %d)\n", counter);
	return 0;
}
