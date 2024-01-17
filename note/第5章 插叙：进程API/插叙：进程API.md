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

***灵魂小画手来喽！！！***

<img src="D:\OS-TEP\OS-TEP\note\第5章 插叙：进程API\屏幕截图 2024-01-17 231133.png" style="zoom: 50%;" />

##### 5.2 wait()系统调用

事实表明，有谁谁父进程需要等待子进程执行完毕，这很有用。这项任务由 wait()系统调用（或者更完整的兄弟接口 waitpid()）。

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int
main(int argc,char *argv[]){
	printf("hello world (pid:%d)\n", (int)getpid());
	int rc = fork();
	if(rc < 0){//fork failed ;exit
		fprintf(stderr,"fork failed\n");
		exit(1);	
	}else if(rc == 0){//child (new process)
		printf("hello,i am child (pid:%d)\n", (int) getpid());
	}else{//parent goes down this path(main)
		int wc = wait(NULL);
		printf("hello,i am parent of %d (pid:%d)\n",rc,(int) getpid());
	}
	
}
```

在 p2.c 的例子中，父进程调用 wait()，延迟自己的执行，直到子进程执行完毕。当子进 程结束谁，wait()才返回父进程.

##### 5.3 最后是 exec()系统调用

最后是 exec()系统调用，它也是创建进程 API 的一个重要部分。这个系统调用可以让子进程执行与父进程不同的程序。

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int
main(int argc, char *argv[]) {
	printf("hello world (pid:%d)\n", (int) getpid());
	int rc = fork();
	if (rc < 0) { // fork failed; exit
		fprintf(stderr, "fork failed\n");
		exit(1);
	} else if (rc == 0) { // child (new process)
		printf("hello, I am child (pid:%d)\n", (int) getpid());
		char *myargs[3];
		myargs[0] = strdup("wc"); // 执行程序的路径
		myargs[1] = strdup("p3.c"); // 程序的参数
		myargs[2] = NULL; // 结尾的标识符
		execvp(myargs[0], myargs); // runs word count
		printf("this shouldn't print out");
	} else { // parent goes down this path (main)
		int wc = wait(NULL);
		printf("hello, I am parent of %d (wc:%d) (pid:%d)\n",
		       rc, wc, (int) getpid());
	}
	return 0;
}
```

关于int execvp(const char *file, char *const argv[]); 

第一个参数是要执行程序的路径 ；

第二个参数是要执行的具体指令本身（所以调用哪个程序也要写上），最后以NULL做结尾代表字符串结束。（在NULL后面再填入别的参数，并不影响程序的执行，因为数组读到NULL就会停止了（真有0人这么闲会提出这种问题吗......））。

------

在这个例子中，子进程调用 execvp()来运行字符计数程序 wc。实实上，它针对源代 文件 p3.c 运行 wc，从而告诉我们该文件有多少行、多少单词，以及多少字节。

fork()系统调用很奇怪，它的伙伴 exec()也不一般。

给定可执行程序的名称（如 wc）及 需要的参数（如 p3.c）后，exec()会从可执行程序中加载代码和静态数据，并用它覆写自己的代码段（以及静态数据），堆、栈及其他内存空间也会被重新初始化。

然后操作系统就执 行该程序，将参数通过 argv 传递给该进程。

因此，它并没有创建新进程，而是直接将当前运行的程序（以前的 p3）替换为不同的运行程序（wc）。

子进程执行 exec()之后，几乎就像 p3.c 从未运行过一样。对 exec()的成功调用永远不会返回。

##### 5.4 为什么这样设计 API 

事实证明，这种分离 fork()及 exec()的做法在构建 UNIX shell的时候非常有用，因为这给了 shell 在 fork 之后 exec 之前运行代码的机会，这些代码可以在运行新程序前改变环境，从而让一系列有趣的功能很容易实现。

> 提示：重要的是做对事（LAMPSON 定律） 
>
> Lampson 在他的著名论文《Hints for Computer Systems Design》[L83]中曾经说过：“做对事（Get it  right）。抽象和简化都不能替代做对事。”有时你必须做正确的事，当你这样做时，总是好过其他方案。 有许多方式来设计创建进程的 API，但 fork()和 exec()的组合既简单又极其强大。因此 UNIX 的设计师们 做对了。因为 Lampson 经常“做对事”，所以我们就以他来命名这条定律。

shell 也是一个用户程序，它首先显示一个提示符（prompt），然后等待用户输入。

你可以向它输入一个命令（一个可执行程序的名称及需要的参数），大多数情况下，shell 可以在文件系统中找到这个可执行程序，调用 fork()创建新进程，并调用 exec()的某个变体来执行这个可执行程序，调用 wait()等待该命令完成。

子进程执行结束后，shell 从 wait()返回并 再次输出一个提示符，等待用户输入下一条命令

（这是不是说shell也是一个进程，通过调用fork()和exec()来执行我们输入的命令.....）

笔者深深的感受到了Linux哲学.....这就是Linux之禅吗.....orz

fork()和 exec()的分离，让 shell 可以方便地实现很多有用的功能。比如：

```shell
prompt> wc p3.c > newfile.txt 
```

在上面的例子中，wc的输出结果被重定向（redirect）到文件newfile.txt中（通过newfile.txt 之前的大于号来指明重定向）。shell 实现结果重定向的方式也很简单，当完成子进程的创建 后，shell 在调用 exec()之前先关闭了标准输出（standard output），打开了文件 newfile.txt。 这样，即将运行的程序 wc 的输出结果就被发送到该文件，而不是打印谁屏幕上。
