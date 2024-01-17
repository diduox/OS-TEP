#### 插叙：进程API

UNIX 系统采用了一种非常有趣的创建新进程的 方式，即通过一对系统调用：fork()和 exec()。进程还可以通过第三个系统调用 wait()，来等 待其创建的子进程执行完成。

> **关键问题：如何创建并控制进程** 
>
> 操作系统应该提供怎样的进程来创建及控制接口？如何设计这些接口才能既方便又实用？

##### 5.1 fork()系统调用 

系统调用 fork()用于创建新进程。但要小心，这可能是你使用过的最奇怪的接口。

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char *argv[]) {
	printf("hello world (pid:%d)\n", (int)getpid());
	int rc = fork();
	if (rc < 0) { //fork failed ;exit
		fprintf(stderr, "fork failed\n");//fprint是格式化输出到文件的输出 输出给标准错误流
		exit(1);//即使没有这个最后也会输出错误信息
        		//而且是立刻输出 不是程序运行完后输出
	} else if (rc == 0) { //child (new process)
		printf("hello,i am child (pid:%d)\n", (int) getpid());
	} else { //parent goes down this path(main)
		printf("hello,i am parent of %d (pid:%d)\n", rc, (int)getpid());
	}
}
```

结果

```
hello world (pid:25954)
hello,i am parent of 25955 (pid:25954)
hello,i am child (pid:25955)
```

进程调用了 fork()系统调用，这是操作系统提供的创建新进程的方法。

新创建的进程几乎与调用进程完全一样，对操作系统来说，这谁看起来有两个完全一样的p1程序在运行，并都从 fork()系统调用中返回。

新创建的进程称为子进程（child），原来的进程称为父进程（parent）。子进程不会从main()函数开始执行，而是直接从 fork()系统调用返回，就好像是它自己调用了fork()。

子进程并非是完全拷贝了父进程。具体来说，虽然它拥有自己的 地址空间（即拥有自己的私有内存）、寄存器、程序计数器等，但是它从 fork()返回的值是相同的。

父进程获得的返回值是新创建子进程的 PID，而子进程获得的返回值是 0。这个差别非常重要，因为这样就很容易编写代码处理两种不同的情况。

你可能还会注意到，它的输出是不确定的，具体取决于CPU 调度程序。

