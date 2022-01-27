# 异步通知实验

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

    if( fasync_helper( fd, filp, on, &dev->async_queue ) < 0 ) //TODO：在哪里调用初始化的，没看懂？
    {
        return -EIO;
    }

    return Success;
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

当设备可以访问时，驱动程序需要向应用程序发出信号，`kill_fasync`函数负责发出指定信号。

