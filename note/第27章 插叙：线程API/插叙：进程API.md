#### 插叙：进程API

本章介绍了主要的线程 API。后续章节也会进一步介绍如何使用 API。

> **关键问题：如何创建和控制线程？**
>
> 操作系统应该提供哪些创建和控制线程的接口？这些接口如何设计得易用和实用？

##### 27.1 线程创建

编写多线程程序的第一步就是创建新线程，因此必须存在某种线程创建接口。在 POSIX （Portable Operating System Interface 可移植的操作系统接口）中，很简单：

```c
#include <pthread.h> 
int 
pthread_create( 	 pthread_t * 	thread, 
                     const pthread_attr_t * 	attr, 
                     void * 	(*start_routine)(void*), 
                     void * 	arg);
```

- thread：指向 pthread_t 结构类型的指针，我们将利用这个结构与线程交互。
- attr: 用于指定该线程可能具有的任何属性。包括设置栈大小，或关于线程调度优先级的信息。但是，在大多数情况下，默认值就行，所以在这个例子中，我们传入NULL。
- (start_routine)(void):最为复杂的参数，我们将其称之为函数指针。这个指针需要以下内容：一个函数的名称。一个被传入的void*的参数，一个void*的返回值。
- arg：就是要传递给线程开始执行函数的参数。

为什么是void指针？

将 void 指针作为函数的参数 start_routine，允 许我们传入任何类型的参数，将它作为返回值，允许线程返回任何类型的结果。

线程一旦创建，可以简单地将其参数转换为它所期望的类型，从而根据需要将参数解包

##### 27.2 线程完成

上面的例子展示了如何创建一个线程。但是，如果你想等待线程完成，会发生什么情况？你需要做一些特别的事情来等待完成。具体来说，你必须调用函数 pthread_join()。

该函数有两个参数：

第一个是 pthread_t 类型，用于指定要等待的线程。这个变量是由 线程创建函数初始化的（当你将一个指针作为参数传递给 pthread_create()时）。

第二个参数是一个指针，指向你希望得到的返回值。因为函数可以返回任何东西，所 以它被定义为返回一个指向 void 的指针。因为 pthread_join()函数改变了传入参数的值（传入参数是一个指针），所 以你需要传入一个指向该值的指针，而不只是该值本身。（就是说此函数要改变一个指针的值，所以我们要传递一个指向指针的指针）。

如果我们不需 要参数，创建线程时传入 NULL 即可。类似的，如果不需要返回值，那么 pthread_join()调 用也可以传入 NULL。

```c
void *mythread(void *arg) { 
     int m = (int) arg; 
     printf("%d\n", m); 
     return (void *) (arg + 1); 
} 
int main(int argc, char *argv[]) { 
     pthread_t p; 
     int rc, m; 
     Pthread_create(&p, NULL, mythread, (void *) 100); 
     Pthread_join(p, (void **) &m); //这个传入指针指针的写法真的好邪门啊
     printf("returned %d\n", m); 
     return 0; 
} 
```

再次，我们应该注意，必须非常小心如何从线程返回值。特别是，永远不要返回一个 指针，并让它指向线程调用栈上分配的东西。

因为当线程结束时，线程调用栈上的内存会被释放，这会导致返回的指针成为悬挂指针，如果其他线程尝试使用这个指针，可能会引发未定义的行为，因为内存已经不在有效。

最后，你可能会注意到，使用 pthread_create()创建线程，然后立即调用 pthread_join()， 这是创建线程的一种非常奇怪的方式。事实上，有一个更简单的方法来完成这个任务，它 被称为过程调用（procedure call）。显然，我们通常会创建不止一个线程并等待它完成，否 则根本没有太多的用途。（如果这样的话直接调用那个函数不就行了吗......）

我们应该注意，并非所有多线程代码都使用 join 函数。例如，多线程 Web 服务器可能 会创建大量工作线程，然后使用主线程接受请求，并将其无限期地传递给工作线程。因此 这样的长期程序可能不需要 join。

**27.3 锁** 

除了线程创建和 join 之外，POSIX 线程库提供的最有用的函数集，可能是通过锁（lock） 来提供互斥进入临界区的那些函数。这方面最基本的一对函数是：

```c
int pthread_mutex_lock(pthread_mutex_t *mutex); 
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

函数应该易于理解和使用。如果你注识到有一段代码是一个临界区，就需要通过锁来 保护，以便像需要的那样运行。你大概可以想象代码的样子：

```c
pthread_mutex_t lock; 
pthread_mutex_lock(&lock); 
x = x + 1; // or whatever your critical section is 
pthread_mutex_unlock(&lock); 
```

这段代码的意思是：如果在调用 pthread_mutex_lock()时没有其他线程持有锁，线程将获取该锁并进入临界区。如果另一个线程确实持有该锁，那么尝试获取该锁的线程将不会从该调用返回，直到获得该锁（注味着持有该锁的线程通过解锁调用释放该锁）。当然，在给定的时间内，许多线程可能会卡住，在获取锁的函数内部等待。然而，只有获得锁的线程才应该调用解锁。

对于 POSIX 线程，有两种方法来初始化锁。一种方法是使用PTHREAD_MUTEX_INITIALIZER，如下所示：（即编译时初始化的静态锁）

```c
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 
```

这样做会将锁设置为默认值，从而使锁可用。初始化的动态方法（即在运行时）是调用 pthread_mutex_init()，如下所示：

```c
int rc = pthread_mutex_init(&lock, NULL); 
assert(rc == 0); // always check success! 
```

第一个参数是指向互斥锁的指针，第二个参数用于指定互斥锁的属性，传入 NULL 就是使用默认值。无论哪种方式都有效，但我们通常使用动态（后者）方法。

请注意，当你用完锁时，还应该相应地调用 pthread_mutex_destroy(& lock)，来销毁互斥锁。

上述代码的第二个问题是在调用获取锁和释放锁时没有检查错误代码。就像 UNIX 系 统中调用的任何库函数一样，这些函数也可能会失败！

如果你的代码没有正确地检查错误 代码，失败将会静静地发生，在这种情况下，可能会允许多个线程进入临界区。

至少要使 用包装的函数，它对函数成功加上断言。更复杂的（非玩具）程序，在出现问题时不能简单地退出，应该检查失败并在获取锁或释放锁未成功时执行适当的操作。

```c
// Use this to keep your code clean but check for failures 
// Only use if exiting program is OK upon failure 
void Pthread_mutex_lock(pthread_mutex_t *mutex) { //将创建和检验封装到一起
     int rc = pthread_mutex_lock(mutex); 
     assert(rc == 0); 
} 
```

获取锁和释放锁函数不是 pthread 与锁进行交互的仅有的函数。特别是，这里有两个你 可能感兴趣的函数：

```c
int pthread_mutex_trylock(pthread_mutex_t *mutex); 
int pthread_mutex_timedlock(pthread_mutex_t *mutex, 
 							struct timespec *abs_timeout); 
```

这两个调用用于获取锁。如果锁已被占用，则 trylock 版本将立即EBUSY，表示锁定失败，但而不会阻塞。

获取锁的 timedlock 定版本会在超时或获取锁后返回，以先发生者为准。

所以如果超时时间是0的timedlock锁就是trylock锁。

lock要获取的锁如果被占用，此线程就会被阻塞，直到获取锁后返回。

##### 27.4 条件变量

所有线程库还有一个主要组件（当然 POSIX 线程也是如此），就是存在一个条件变量 （condition variable）。当线程之间必须发生某种信号时，如果一个线程在等待另一个线程继 续执行某些操作，条件变量就很有用。希望以这种方式进行交互的程序使用两个主要函数：

```c
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex); 
int pthread_cond_signal(pthread_cond_t *cond);
```

要使用条件变量，必须另外有一个与此条件相关的锁。在调用上述任何一个函数时， 应该持有这个锁。

第一个函数 pthread_cond_wait()使调用线程进入休眠状态，因此等待其他线程发出信号，通 常当程序中的某些内容发生变化时，现在正在休眠的线程可能会关心它。典型的用法如下所示：

```c
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 
Pthread_mutex_lock(&lock); 
while (ready == 0) 
 	Pthread_cond_wait(&cond, &lock); 	//释放互斥锁，并休眠
										//被唤醒之后，会重新获得锁
Pthread_mutex_unlock(&lock); 
```

在这段代码中，在初始化相关的锁和条件之后①，一个线程检查变量 ready 是否已经被设置 为零以外的值。如果没有，那么线程只是简单地调用等待函数以便休眠，直到其他线程唤醒它。

唤醒线程的代码运行在另外某个线程中，像下面这样：

```c
Pthread_mutex_lock(&lock); 
ready = 1; 
Pthread_cond_signal(&cond); 
Pthread_mutex_unlock(&lock); 
```

关于这段代码有一些注注事项。首先，在发出信号时（以及修改全局变量 ready 时）， 我们始终确保持有锁。这确保我们不会在代码中注外引入竞态条件。

等待线程在 while 循环中重新检查条件，而不是简单的 if 语句。虽然while可能会增加一点开销，但是这是一种安全的操作。

请注注，有时候线程之间不用条件变量和锁，用一个标记变量会看起来很简单，很吸 引人。例如，我们可以重写上面的等待代码，像这样：

```c
while (ready == 0) 
 ; // spin 
```

相关的发信号代码看起来像这样： 

```c
ready = 1; 
```

千万不要这么做。首先，多数情况下性能差（长时间的自旋浪费 CPU）。其次，容易出 错。最近的研究显示，线程之间通过标志同步（像上面那样），出错的可能性让人吃 惊。在那项研究中，这些不正规的同步方法半数以上都是有问题的。不要偷懒，就算你想 到可以不用条件变量，还是用吧。

##### **27.5 编译和运行**

本章所有代码很容易运行。代码需要包括头文件 pthread.h 才能编译。链接时需要 pthread 库，增加-pthread 标记。

例如，要编译一个简单的多线程程序，只需像下面这样做：

-Wall ：启用在编译过程中的大多数警告消息。包含此选项是一种良好的实践，可以捕获代码中的潜在问题。

```shell
prompt> gcc -o main main.c -Wall -pthread
```

只要 main.c 包含 pthreads 头文件，你就已经成功地编译了一个并发程序。像往常一样， 它是否能工作完全是另一回事。

##### 27.6 小结 

我们介绍了基本的 pthread 库，包括线程创建，通过锁创建互斥执行，通过条件变量的 信号和等待。要想写出健壮高效的多线程代码，只需要耐心和万分小心！

请在 Linux 系统上输入 man -k pthread，查看构成整个接口的超 过一百个 API。

线程难的部分不是 API，而是如何构建并发程序的棘手逻辑。请继 续阅读以了解更多信息。

> **补充：线程 API 指导**
>
> - 当你使用 POSIX 线程库（或者实际上，任何线程库）来构建多线程程序时，需要记住一些小而重要的事情： 
> - 保持简洁。最重要的一点，线程之间的锁和信号的代码应该尽可能简洁。复杂的线程交互容易 产生缺陷。 
> - 让线程交互减到最少。尽量减少线程之间的交互。每次交互都应该想清楚，并用验证过的、正确的方法来实现（很多方法会在后续章节中学习）。 
> - 初始化锁和条件变量。未初始化的代码有时工作正常，有时失败，会产生奇怪的结果。 检查返回值。当然，任何 C 和 UNIX 的程序，都应该检查返回值，这里也是一样。否则会导致 古怪而难以理解的行为，让你尖叫，或者痛苦地揪自己的头发。 
> - 注意传给线程的参数和返回值。具体来说，如果传递在栈上分配的变量的引用，可能就是在犯错误。 
> - 每个线程都有自己的栈。类似于上一条，记住每一个线程都有自己的栈。因此，线程局部变量 应该是线程私有的，其他线程不应该访问。线程之间共享数据，值要在堆（heap）或者其他全局可访问的位置。 
> - 线程之间总是通过条件变量发送信号。切记不要用标记变量来同步。 
> - 多查手册。尤其是 Linux 的 pthread 手册，有更多的细节、更丰富的内容。请仔细阅读！

