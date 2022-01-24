#include "include/interrupt.h"

struct irq_dev irqdev;

static int irq_open( struct inode *inode, struct file *filp);
static long irq_unlocked_ioctl( struct file *filp, unsigned int cmd, unsigned long arg);
static int irq_release( struct inode *inode, struct file *filp );
static ssize_t irq_read( struct file *filp, char __user *buf, size_t cnt, loff_t *offt );

static struct file_operations irq_fops = {
    .owner = THIS_MODULE,
    .open = irq_open,
    .release = irq_release,
    .read = irq_read,
    .unlocked_ioctl = irq_unlocked_ioctl,
};

/*================================================================ 
 * 函数名：key0_handler
 * 功能描述：按键中断处理函数
 * 参数：
 *      irqreturn_t
 * 返回值：
 *      成功: 0
 *      失败: -1
 * 作者：Yang Mou 2022/1/19
================================================================*/
static irqreturn_t key0_handler(int irq, void *dev_id)
{
    struct irq_dev *dev = (struct irq_dev *)dev_id;

    dev->timer.data = (volatile long)dev_id;
    debug( "hello wprld \r\n");
    return IRQ_RETVAL(IRQ_HANDLED);
}

/*================================================================ 
 * 函数名：keyio_init
 * 功能描述：初始化GPIO
 * 参数：
 *      void
 * 返回值：
 *      成功: 0
 *      失败: -1
 * 作者：Yang Mou 2022/1/19
================================================================*/
static int keyio_init(void)
{
    int ret = 0;
    unsigned int i = 0;
    irqdev.nd = of_find_node_by_path( "/key" ); /* 获取/dev/key */
    if( NULL == irqdev.nd )
    {
        debug( "FILE: %s, LINE: %d:\r\n", __FILE__, __LINE__ );
        return -1;        
    }


    irqdev.irqkeydesc.gpio = of_get_named_gpio( irqdev.nd, "key-gpio", i );
    if( irqdev.irqkeydesc.gpio < 0 )
    {
        /*提取GPIO失败*/
        debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
    }
    
    memset( irqdev.irqkeydesc.name, 0, sizeof( irqdev.irqkeydesc.name ) );/*将数组清0*/
    sprintf( irqdev.irqkeydesc.name, "KEY%d", 0 );
    gpio_request( irqdev.irqkeydesc.gpio, irqdev.irqkeydesc.name );
    gpio_direction_input( irqdev.irqkeydesc.gpio );
    irqdev.irqkeydesc.irqnum = irq_of_parse_and_map( irqdev.nd, 0); /*获取中断号*/
    debug("irqnum: %d\r\n", irqdev.irqkeydesc.irqnum );


    /*中断*/
    irqdev.irqkeydesc.handler = key0_handler; /*设置中断回调函数*/
    irqdev.irqkeydesc.value = KEY0VALUE;
    
    
    /*申请中断*/
    ret = request_irq( irqdev.irqkeydesc.irqnum, 
                       irqdev.irqkeydesc.handler, 
                       IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, 
                       irqdev.irqkeydesc.name, &irqdev );/*irqdev 会传递给中断处理函数 irq_handler_t 的第二个参数*/
    if( ret < 0 )
    {
        debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
        return -1;
    }                
    
    return 0;
}


/*================================================================
 * 函数名：irq_open
 * 功能描述：打开设备文件
 * 参数：
 *      inode[IN]: 设备节点
 *      filp [IN]: 要打开的设备文件
 * 返回值：
 *      成功: 0
 *      失败: -1
 * 作者：Yang Mou 2022/1/21
================================================================*/
static int irq_open( struct inode *inode, struct file *filp)
{
    filp->private_data = &irqdev; /*设置私有数据*/
    return 0;
}


static ssize_t irq_read( struct file *filp, char __user *buf, size_t cnt, loff_t *offt )
{
    int ret = 0;
    unsigned char keyvalue = 0;
    unsigned char releasekey = 0;
    struct irq_dev *dev = ( struct irq_dev *)filp->private_data;

    keyvalue = atomic_read( &dev->keyvalue );
    releasekey = atomic_read( &dev->releasekey );

    if( releasekey )
    {
        if( keyvalue & 0x80 )
        {
            keyvalue &= ~0x80;
            ret = copy_to_user( buf, &keyvalue, sizeof( keyvalue ) );
        }
        else
        {
            return -1;
        }
        atomic_set( &dev->releasekey, 0 );
    }
    else
    {
        return -1;
    }

    return 0;
}

/*================================================================
 * 函数名：key_release
 * 功能描述：释放设备文件
 * 参数：
 *      inode[IN]: 设备节点
 *      filp [IN]: 要打开的设备文件
 * 返回值：
 *      成功: 0
 *      失败: -1
 * 作者：Yang Mou 2022/1/21
================================================================*/
static int irq_release( struct inode *inode, struct file *filp )
{
    unsigned char i = 0;
    struct irq_dev *dev = ( struct irq_dev * )filp->private_data;
    
    for ( i = 0; i < KEY_NUM; ++i )
    {
        gpio_free( dev->irqkeydesc.gpio );
        debug("irqnum: %d\r\n", dev->irqkeydesc.irqnum );
    }
    return 0;
}

static long irq_unlocked_ioctl( struct file *filp, unsigned int cmd, unsigned long arg)
{
    return 0;
}



/*================================================================ 
 * 函数名：Irq_init
 * 功能描述：加载驱动模块时会调用该函数
 * 参数：
 *      void
 * 返回值：
 *      成功: 0
 *      失败: 
 * 作者：Yang Mou 2022/1/21
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
    if( IS_ERR( irqdev.class ) )
    {
        debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
        return PTR_ERR( irqdev.class );
    }
    irqdev.device = device_create( irqdev.class, NULL, irqdev.devid, NULL, IRQ_NAME );
    if( IS_ERR( irqdev.device ) )
    {
        debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
        return PTR_ERR( irqdev.device );        
    }

    atomic_set( &irqdev.keyvalue, INVAKEY );
    atomic_set( &irqdev.releasekey, 0 );
    keyio_init();

    return 0;
}

/*================================================================ 
 * 函数名：Irq_exit
 * 功能描述：卸载驱动模块时会调用该函数
 * 参数：
 *      void
 * 返回值：
 *      成功: 0
 *      失败: 
 * 作者：Yang Mou 2022/1/21
================================================================*/
static void __exit Irq_exit( void )
{
    unsigned char i = 0;

    for( i = 0; i < KEY_NUM; ++i )
    {
        free_irq( irqdev.irqkeydesc.irqnum, &irqdev );

    }


    cdev_del( &irqdev.cdev ); /*删除字符设备*/
    unregister_chrdev_region( irqdev.devid, IRQ_CNT ); /*注销设备号*/


    device_destroy( irqdev.class, irqdev.devid );
    class_destroy( irqdev.class );

    return;
}

module_init(Irq_init);
module_exit(Irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YangMou");

