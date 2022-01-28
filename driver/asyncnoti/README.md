# 异步通知实验

## 驱动

阻塞和非阻塞方式需要应用程序主动去读取数据，对于非阻塞还需要应用程序通过poll函数不断轮询。异步通知能主动向应用程序发出通知，报告自己可访问( 类似于硬件中断 )。

异步通知的和核心就是信号：`arch/xtensa/include/uapi/asm/signal.h`中定义了所有的信号。
> 信号就相当于中断号，不同的中断号代表了不同的中断，不同的中断所做的处理不同。

使用异步通知，需要在设备驱动中实现`file_operations`操作集中的`fasync`函数

参考示例:

```c
struct xxx_dev
{
    ...
    struct fasync_struct *async_queue; /*异步相关结构体*/
};

static int xxx_fasync( int fd, struct file *filp, int on )
{
    struct xxx_dev *dev = ( struct xxx_dev * )filp->private_data;

    if( fasync_helper( fd, filp, on, &dev->async_queue ) < 0 )
    {
        return -EIO;
    }

    return 0;
}

static int xxx_release( struct inode *inode, struct file *filp )
{
    return xxx_fasync( -1, filp, 0 ); /*删除异步通知*/
}

static struct file_operation xxx_ops = {
    ...
    .fasync = xxx_fasync,
    .release = xxx_release,
    ...
};
```

当设备可以访问时，驱动程序需要向应用程序发出信号，`kill_fasync( struct fasync_struct **fp, int sig, int band )`函数负责发出指定信号。

> fp：要操作的 fasync_struct。
> sig：要发送的信号。
> band：可读时设置为 POLL_IN，可写时设置为 POLL_OUT。

```C
if(完成一次按键动作)
{
    kill_fasync( &dev->async_queue, SIGIO, POLL_IN );
}
```

## 应用

应用程序对异步通知的处理有三步:

1. 注册信号以及信号处理函数。
2. 将本应用程序告诉给内核。
3. 开启异步通知

```c
static void xxx_signal_func( int signum )
{

}

static int fd = 0;
int flags = 0;

fd = open( filename, O_RDWR );
signal( SIGIO, sigio_signal_func ); /*注册信号以及信号处理函数*/

fcntl( fd, F_SETOWN,  getpid() ); /*将本应用程序告诉给内核*/

/*开启异步通知*/
flags = fcntl( fd, F_GETFL ); /*获取当前进程状态*/
fcntl( fd, F_SETFL, flags | FASYNC ); /*开启当前进程的异步通知功能*/
```

> `fcntl`函数能够改变已经打开文件的属性，使用`man fcntl`查看手册。
