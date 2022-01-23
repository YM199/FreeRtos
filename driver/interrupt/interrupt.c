#include "include/interrupt.h"

struct imx6uirq_dev imx6uirqdev;

static int imx6uirq_open( struct inode *inode, struct file *filp);
static long imx6uirq_unlocked_ioctl( struct file *filp, unsigned int cmd, unsigned long arg);
static int imx6uirq_release( struct inode *inode, struct file *filp );
static ssize_t imx6uirq_read( struct file *filp, char __user *buf, size_t cnt, loff_t *offt );

static struct file_operations timer_fops = {
    .owner = THIS_MODULE,
    .open = imx6uirq_open,
    .release = imx6uirq_release,
    .read = imx6uirq_read,
    .unlocked_ioctl = imx6uirq_unlocked_ioctl,
};


/*================================================================ 
 * 函数名：timer_function
 * 功能描述：定时器timer的回调函数
 * 参数：
 *      arg[IN]: 私有数据
 * 返回值：
 *      成功: 0
 *      失败: -1
 * 作者：Yang Mou 2022/1/21
================================================================*/
void timer_function( unsigned long arg )
{
    unsigned char value;
    unsigned char num;
    struct irq_keydesc *keydesc;
    struct imx6uirq_dev *dev = (struct imx6uirq_dev *)arg;
    num = dev->curkeynum;
    keydesc = &dev->irqkeydesc[num];
    value = gpio_get_value( keydesc->gpio ); /*读取GPIO*/
    if( 0 == value ) 
    {
        /*按下按键*/
        atomic_set( &dev->keyvalue, keydesc->value );
    }
    else
    {
        atomic_set(&dev->keyvalue, 0x80 | keydesc->value);
        atomic_set(&dev->releasekey, 1);
    }

    return; 
}


static irqreturn_t key0_handler(int irq, void *dev_id)
{
    struct imx6uirq_dev *dev = (struct imx6uirq_dev *)dev_id;

    dev->curkeynum = 0;
    dev->timer.data = (volatile long)dev_id;
    mod_timer(&dev->timer, jiffies + msecs_to_jiffies(10));
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
    imx6uirqdev.nd = of_find_node_by_path( "/key" ); /* 获取/dev/key */
    if( NULL == imx6uirqdev.nd )
    {
        debug( "FILE: %s, LINE: %d:\r\n", __FILE__, __LINE__ );
        return -1;        
    }

    for( i = 0; i < KEY_NUM; ++i )
    {
        imx6uirqdev.irqkeydesc[i].gpio = of_get_named_gpio( imx6uirqdev.nd, "key-gpio", i );
        if( imx6uirqdev.irqkeydesc[i].gpio < 0 )
        {
            /*提取GPIO失败*/
            debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
        }
    }


    for ( i = 0; i < KEY_NUM; ++i )
    {
        memset( imx6uirqdev.irqkeydesc[i].name, 0, sizeof( imx6uirqdev.irqkeydesc[i].name ) );/*将数组清0*/
        sprintf( imx6uirqdev.irqkeydesc[i].name, "KEY%d", i );
        gpio_request( imx6uirqdev.irqkeydesc[i].gpio, imx6uirqdev.irqkeydesc[i].name );
        gpio_direction_input( imx6uirqdev.irqkeydesc[i].gpio );
        imx6uirqdev.irqkeydesc[i].irqnum = irq_of_parse_and_map( imx6uirqdev.nd, i); /*获取中断号*/
        debug("irqnum: %d\r\n", imx6uirqdev.irqkeydesc[i].irqnum );
    }

    /*中断*/
    imx6uirqdev.irqkeydesc[0].handler = key0_handler; /*设置中断回调函数*/
    imx6uirqdev.irqkeydesc[0].value = KEY0VALUE;
    for( i = 0; i < KEY_NUM; ++i )
    {
        /*申请中断*/
        ret = request_irq( imx6uirqdev.irqkeydesc[i].irqnum, 
                           imx6uirqdev.irqkeydesc[i].handler, 
                           IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, 
                           imx6uirqdev.irqkeydesc[i].name, &imx6uirqdev );
        if( ret < 0 )
        {
            debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
            return -1;
        }                
    }

    /*创建定时器*/
    init_timer( &imx6uirqdev.timer );
    imx6uirqdev.timer.function = timer_function;

    return 0;
}


/*================================================================
 * 函数名：imx6uirq_open
 * 功能描述：打开设备文件
 * 参数：
 *      inode[IN]: 设备节点
 *      filp [IN]: 要打开的设备文件
 * 返回值：
 *      成功: 0
 *      失败: -1
 * 作者：Yang Mou 2022/1/21
================================================================*/
static int imx6uirq_open( struct inode *inode, struct file *filp)
{
    filp->private_data = &imx6uirqdev; /*设置私有数据*/
    return 0;
}


static ssize_t imx6uirq_read( struct file *filp, char __user *buf, size_t cnt, loff_t *offt )
{
    int ret = 0;
    unsigned char keyvalue = 0;
    unsigned char releasekey = 0;
    struct imx6uirq_dev *dev = ( struct imx6uirq_dev *)filp->private_data;

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
static int imx6uirq_release( struct inode *inode, struct file *filp )
{
    unsigned char i = 0;
    struct imx6uirq_dev *dev = ( struct imx6uirq_dev * )filp->private_data;
    
    for ( i = 0; i < KEY_NUM; ++i )
    {
        gpio_free( dev->irqkeydesc[i].gpio );
        debug("irqnum: %d\r\n", dev->irqkeydesc[i].irqnum );
    }
    return 0;
}

static long imx6uirq_unlocked_ioctl( struct file *filp, unsigned int cmd, unsigned long arg)
{
    return 0;
}



/*================================================================ 
 * 函数名：imx6uirq_init
 * 功能描述：加载驱动模块时会调用该函数
 * 参数：
 *      void
 * 返回值：
 *      成功: 0
 *      失败: 
 * 作者：Yang Mou 2022/1/21
================================================================*/
static int __init imx6uirq_init( void )
{
    /*申请设备号*/
    if( imx6uirqdev.major )
    {
        /*指定主设备号*/
        imx6uirqdev.devid = MKDEV( imx6uirqdev.major, 0 );
        register_chrdev_region( imx6uirqdev.devid, IMX6UIRQ_CNT, IMX6UIRQ_NAME );
    }
    else
    {
        /*动态分配主设备号*/
        alloc_chrdev_region( &imx6uirqdev.devid, 0, IMX6UIRQ_CNT, IMX6UIRQ_NAME );
        imx6uirqdev.major = MAJOR( imx6uirqdev.devid );
        imx6uirqdev.minor = MINOR( imx6uirqdev.devid );
    }

    /*创建字符设备*/
    imx6uirqdev.cdev.owner = THIS_MODULE;
    cdev_init( &imx6uirqdev.cdev, &timer_fops );
    cdev_add( &imx6uirqdev.cdev, imx6uirqdev.devid, IMX6UIRQ_CNT );

    /*自动创建设备节点*/
    imx6uirqdev.class =  class_create( THIS_MODULE, IMX6UIRQ_NAME );
    if( IS_ERR( imx6uirqdev.class ) )
    {
        debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
        return PTR_ERR( imx6uirqdev.class );
    }
    imx6uirqdev.device = device_create( imx6uirqdev.class, NULL, imx6uirqdev.devid, NULL, IMX6UIRQ_NAME );
    if( IS_ERR( imx6uirqdev.device ) )
    {
        debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
        return PTR_ERR( imx6uirqdev.device );        
    }

    atomic_set( &imx6uirqdev.keyvalue, INVAKEY );
    atomic_set( &imx6uirqdev.releasekey, 0 );
    keyio_init();

    return 0;
}

/*================================================================ 
 * 函数名：timer_exit
 * 功能描述：卸载驱动模块时会调用该函数
 * 参数：
 *      void
 * 返回值：
 *      成功: 0
 *      失败: 
 * 作者：Yang Mou 2022/1/21
================================================================*/
static void __exit imx6uirq_exit( void )
{
    unsigned char i = 0;
    del_timer_sync(&imx6uirqdev.timer); /*删除定时器timer*/

    for( i = 0; i < KEY_NUM; ++i )
    {
        free_irq( imx6uirqdev.irqkeydesc[i].irqnum, &imx6uirqdev );

    }


    cdev_del( &imx6uirqdev.cdev ); /*删除字符设备*/
    unregister_chrdev_region( imx6uirqdev.devid, IMX6UIRQ_CNT ); /*注销设备号*/


    device_destroy( imx6uirqdev.class, imx6uirqdev.devid );
    class_destroy( imx6uirqdev.class );

    return;
}

module_init(imx6uirq_init);
module_exit(imx6uirq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YangMou");

