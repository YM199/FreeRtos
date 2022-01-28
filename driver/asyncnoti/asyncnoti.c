#include "include/asyncnoti.h"

struct irq_dev irqdev;
/*
 *备注：函数声明区
*/
static int open(struct inode *inode, struct file *filp);
static ssize_t read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt);
static int fasync( int fd, struct file *filp,  int on );
static int release( struct inode *inode, struct file *filp );

/*===============================================================
* 结构名称：irq_fops
* 结构类型：file_operations
* 备    注：每一个设备号对应一个file_operations类型的结构，存放该驱动程序的各种操作函数
*===============================================================*/
static struct file_operations irq_fops = {
    .owner = THIS_MODULE,
    .open = open,
    .release = release,
    .fasync = fasync,
    .read = read,
};

/*================================================================
 * 函数名：open
 * 功 能：无
 * 备 注：应用层调用open时自动执行该函数
================================================================*/
static int open(struct inode *inode, struct file *filp)
{
	filp->private_data = &irqdev;	/* 设置私有数据 */
	return 0;
}

/*================================================================
 * 函数名：read
 * 功能：如果按键按下就向用户空间发送数据
 * 备注：应用层调用read时自动执行该函数
================================================================*/
static ssize_t read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
    int ret = 0;
    int data = 123;
    struct irq_dev *dev = ( struct irq_dev * )filp->private_data;

    unsigned char releasekey = atomic_read(&dev->releasekey);


    if( releasekey )
    {
        ret = copy_to_user( buf, &data, sizeof( data ));
        atomic_set( &dev->releasekey, 0 );
    }
    else
    {
        return -EINVAL;/*没有被按下直接返回错误码*/
    }

    return 0;
}

/*================================================================
 * 函数名：fasync
 * 功能：调用fasync_helper初始化fasync_struct结构体，主要是应用程序的PID等信息
 * 备注：应用层调用fcntl改变设备文件时自动执行该函数
================================================================*/
static int fasync( int fd, struct file *filp,  int on )
{
    struct irq_dev *dev = ( struct irq_dev * )filp->private_data;

    return fasync_helper( fd, filp, on, &dev->async_queue );
}

/*================================================================
 * 函数名：release
 * 功能：资源释放
 * 备注：应用层调用close改变设备文件时自动执行该函数
================================================================*/
static int release( struct inode *inode, struct file *filp )
{
    return fasync( -1, filp, 0); /*删除异步通知*/
}

/*================================================================
 * 函数名：key0_handler
 * 功能：打开定时器
 * 备注：按键中断处理函数
================================================================*/
static irqreturn_t key0_handler(int irq, void *dev_id)
{
    struct irq_dev *dev = ( struct irq_dev * )dev_id;

    dev->timer.data = (volatile long)dev_id; /*将结构体传递给timer定时器*/
    mod_timer( &dev->timer, jiffies + msecs_to_jiffies( 10 ) ); /*定时10ms，定时器开始运行*/
    return IRQ_RETVAL(IRQ_HANDLED);
}

/*================================================================
 * 函数名：timer_function
 * 功能：检查到按键松开置1标志位并发送信号
 * 备注：定时器回调函数
================================================================*/
void timer_function( unsigned long arg )
{
    struct irq_dev *dev = ( struct irq_dev * )arg;
    struct irq_keydesc *keydesc;
    unsigned char value;
    keydesc = &dev->irqkeydesc; /*获取irq_keydesc结构体*/

    value = gpio_get_value( keydesc->gpio ); /*读取按键*/
    if( value == 0 ) /*按键还没松开*/
    {
    }
    else /*按键已经松开*/
    {
        atomic_set( &dev->releasekey, 1 ); /*表示按键按下并且已经松开*/
    }

    if( atomic_read(&dev->releasekey) ) /*一次完整的按键过程*/
    {
        if( dev->async_queue ) /*TODO:为什么要判断一下？*/
        {
            kill_fasync( &dev->async_queue, SIGIO, POLL_IN ); /*应用程序注册的什么信号，这里就发送对应的信号*/
        }
    }

    return;
}

/*================================================================ 
 * 函数名：keyio_init
 * 功能描述：初始化GPIO和定时器timer
 * 备注: 初始化函数
================================================================*/
static int keyio_init(void)
{
    int ret = 0;

    irqdev.nd = of_find_node_by_path( "/key" ); /* 获取/dev/key */
    debug( NULL == irqdev.nd, -ENXIO );

    irqdev.irqkeydesc.gpio = of_get_named_gpio( irqdev.nd, "key-gpio", 0 );
    debug( irqdev.irqkeydesc.gpio < 0, -EINVAL );
    memset( irqdev.irqkeydesc.name, 0, sizeof( irqdev.irqkeydesc.name ) );/*将数组清0*/
    sprintf( irqdev.irqkeydesc.name, "KEY%d", 0 );/*中断名字*/
    gpio_request( irqdev.irqkeydesc.gpio, irqdev.irqkeydesc.name ); /*申请GPIO*/
    gpio_direction_input( irqdev.irqkeydesc.gpio ); /*设置GPIO为输入*/
    irqdev.irqkeydesc.irqnum = irq_of_parse_and_map( irqdev.nd, 0); /*获取中断号*/

    /*中断*/
    irqdev.irqkeydesc.handler = key0_handler; /*设置中断回调函数*/

    /*申请中断*/
    ret = request_irq( irqdev.irqkeydesc.irqnum,
                       irqdev.irqkeydesc.handler,
                       IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, /*上升沿和下降沿*/
                       irqdev.irqkeydesc.name, &irqdev );/*irqdev 会传递给中断处理函数 irq_handler_t 的第二个参数*/

    debug( ret < 0, -EAGAIN );

    init_timer( &irqdev.timer ); /*初始化定时器*/
    irqdev.timer.function = timer_function; /*设置定时器的回调函数，当前定时器并未开始运行*/

    return 0;
}

/*================================================================
 * 函数名：Irq_init
 * 功能描述：完成驱动的注册和调用其他注册函数
 * 备忘：加载驱动时自动调用该函数
================================================================*/
static int __init Irq_init( void )
{
    /*申请设备号*/
    if( irqdev.major )
    {
        /*指定主设备号*/
        irqdev.devid = MKDEV( irqdev.major, 0 );
        register_chrdev_region( irqdev.devid, IRQ_CNT, IRQ_NAME );
    }
    else
    {
        /*动态分配主设备号*/
        alloc_chrdev_region( &irqdev.devid, 0, IRQ_CNT, IRQ_NAME );
        irqdev.major = MAJOR( irqdev.devid );
        irqdev.minor = MINOR( irqdev.devid );
    }

    /*创建字符设备*/
    irqdev.cdev.owner = THIS_MODULE;
    cdev_init( &irqdev.cdev, &irq_fops );
    cdev_add( &irqdev.cdev, irqdev.devid, IRQ_CNT );

    /*自动创建设备节点*/
    irqdev.class =  class_create( THIS_MODULE, IRQ_NAME );
    debug( IS_ERR( irqdev.class ), PTR_ERR(irqdev.class) );
    irqdev.device = device_create( irqdev.class, NULL, irqdev.devid, NULL, IRQ_NAME );
    debug( IS_ERR( irqdev.device ), PTR_ERR(irqdev.device) );

    atomic_set( &irqdev.releasekey, 0 );
    keyio_init();

    return 0;
}

/*================================================================
 * 函数名：Irq_exit
 * 功能描述：释放驱动相关资源
 * 备忘：驱动卸载时自动执行该函数
================================================================*/
static void __exit Irq_exit( void )
{
    free_irq( irqdev.irqkeydesc.irqnum, &irqdev ); /*释放中断*/
    gpio_free( irqdev.irqkeydesc.gpio ); /*释放GPIO*/

    cdev_del( &irqdev.cdev ); /*删除字符设备*/
    unregister_chrdev_region( irqdev.devid, IRQ_CNT ); /*注销设备号*/

    /*删除自动创建的设备节点*/
    device_destroy( irqdev.class, irqdev.devid );
    class_destroy( irqdev.class );

    return;
}

module_init(Irq_init);
module_exit(Irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YangMou");

