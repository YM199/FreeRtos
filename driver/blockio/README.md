# 阻塞IO实验

当设备不可用时，线程会进入休眠状态，

    fd = open( "/dev/xxx_dev", O_RDWR ); /*阻塞方式打开*/

一般在中断函数里面完成唤醒工作，使用等待队列(wait queue)完成阻塞进程的唤醒工作。

---

首先要定义一个等待队列头，`wait_queue_head_t r_wait;`,再调用`init_waitqueue_head(&r_wait);`进行初始化。

> 另外一种方法是使用宏`DECLARE_WAIT_QUEUE_HEAD(r_wait)`一次性完成定义和初始化操作。

方法一:

    wait_queue_head_t r_wait;
    init_waitqueue_head(&r_wait);

方法二:

    DECLARE_WAIT_QUEUE_HEAD(r_wait);

两种方法等价。

---

在驱动代码里面创建一个等待队列项，并设置属于那个进程:

    DECLARE_WAITQUEUE( wait, current ); /*创建一个等待队列项wait，current表示属于当前进程*/

然后将等待队列项添加到等待队列头:

    add_wait_queue( &r_wait, &wait ); /*将等待队列项添加到等待队列头*/
    __set_current_state( TASK_INTERRUPTIBLE ); /*设置当前进程状态*/
    schedule(); /*执行任务切换，相当于当前进程进入休眠状态，当进程唤醒时会从它的下一句代码开始执行*/

当进程从休眠状态被唤醒，那么就需要将等待队列项从等待队列头中移除 (先判断一下是否是信号引起的唤醒) :

    if( signal_pending( current ) ) /*信号引起的唤醒*/
    {
        set_current_state( TASK_RUNNING ); /*设置任务为运行态*/
        remove_wait_queue( &r_wait, &wait ); /*将等待队列移除*/
        return -ERESTARTSYS; /*TODO: 没搞懂为什么要返回这个错误码*/
    }
    __set_current_state( TASK_RUNNING ); /*设置为运行状态*/
    remove_wait_queue( &r_wait, &wait );/*将等待队列移除*/

---

唤醒进程有两个函数可以使用 (主动唤醒) :

    wake_up( &r_wait ); /*能唤醒处于 TASK_INTERRUPTIBLE 和 TASK_UNINTERRUPTIBLE 状态的进程*/
    wake_up_interruptible( &r_wait ); /*能唤醒处于 TASK_INTERRUPTIBLE 状态的进程*/

一般在中断里面调用。

> 除主动唤醒外，还可以设置为等待某个事件自动唤醒。
>
> 阻塞模式不需要poll函数。
