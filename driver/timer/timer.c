#include "include/timer.h"

struct timer_dev timerdev;

static int timer_open( struct inode *inode, struct file *filp);
static long timer_unlocked_ioctl( struct file *filp, unsigned int cmd, unsigned long arg);
static int timer_release( struct inode *inode, struct file *filp );

static struct file_operations timer_fops = {
    .owner = THIS_MODULE,
    .open = timer_open,
    .release = timer_release,
    .unlocked_ioctl = timer_unlocked_ioctl,
};

/*================================================================
 * 函数名：led_init
 * 功能描述：led初始化函数
 * 参数：
 *      void
 * 返回值：
 *      成功: 0
 *      失败: -1
 * 作者：Yang Mou 2022/1/21
================================================================*/
static int led_init( void )
{
    int ret;
  
    timerdev.nd = of_find_node_by_path( "/led" ); /*获取设备树节点*/
    if( NULL == timerdev.nd )
    {
        debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
        return -1;
    }

    /*获取GPIO编号*/
    timerdev.led_gpio = of_get_named_gpio( timerdev.nd, "led-gpio", 0 );
    if( timerdev.led_gpio < 0 )
    {
        debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );

        return -1;        
    }

    gpio_request( timerdev.led_gpio, "led");
    ret = gpio_direction_output(  timerdev.led_gpio, 1 );
    if( ret < 0 )
    {
        debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
        return -1;
    }
    
    return 0;
}

/*================================================================
 * 函数名：timer_open
 * 功能描述：打开设备文件
 * 参数：
 *      inode[IN]: 设备节点
 *      filp [IN]: 要打开的设备文件
 * 返回值：
 *      成功: 0
 *      失败: -1
 * 作者：Yang Mou 2022/1/21
================================================================*/
static int timer_open( struct inode *inode, struct file *filp)
{
    int ret = 0;
    filp->private_data = &timerdev; /*设置私有数据*/

    timerdev.timeperiod = 1000;

    ret = led_init();
    if( ret < 0 )
    {
        debug( " __FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__);
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
static int timer_release( struct inode *inode, struct file *filp )
{
    struct timer_dev *dev = ( struct timer_dev * )filp->private_data;
    gpio_free( dev->led_gpio ); /*释放GPIO*/
    return 0;
}

static long timer_unlocked_ioctl( struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct timer_dev *dev = ( struct timer_dev * )filp->private_data;
    int timerperiod;
    unsigned long flags;

    switch( cmd )
    {
        case CLOSE_CMD: /*关闭定时器*/
            del_timer_sync( &dev->timer );
            break;
        case OPEN_CMD: /*打开定时器*/
            spin_lock_irqsave( &dev->lock, flags );
            timerperiod = dev->timeperiod;
            spin_unlock_irqrestore(&dev->lock, flags);
            mod_timer( &dev->timer, jiffies + msecs_to_jiffies( timerperiod ) );
            break;
        case SETPERIOD_CMD : /* 设置定时器周期 */
            spin_lock_irqsave( &dev->lock, flags);
            dev->timeperiod = arg;
            spin_unlock_irqrestore(&dev->lock, flags);
            mod_timer( &dev->timer, jiffies + msecs_to_jiffies( arg ) );
            break;
        default:
            break;
    }

    return 0;
}

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
    struct timer_dev *dev = ( struct timer_dev * )arg;
    static int sta = 1;
    int timerperiod;
    unsigned long flags;

    sta = !sta;
    gpio_set_value( dev->led_gpio, sta );

    spin_lock_irqsave( &dev->lock, flags ); /* 获取自旋锁 */
    timerperiod = dev->timeperiod;
    spin_unlock_irqrestore( &dev->lock, flags );/* 释放自旋锁 */
    mod_timer( &dev->timer, jiffies + msecs_to_jiffies(dev->timeperiod) );

    return; 
}

/*================================================================ 
 * 函数名：timer_init
 * 功能描述：加载驱动模块时会调用该函数
 * 参数：
 *      void
 * 返回值：
 *      成功: 0
 *      失败: 
 * 作者：Yang Mou 2022/1/21
================================================================*/
static int __init timer_init( void )
{
    spin_lock_init( &timerdev.lock ); /*初始化自旋锁*/

    /*申请设备号*/
    if( timerdev.major )
    {
        /*指定主设备号*/
        timerdev.devid = MKDEV( timerdev.major, 0 );
        register_chrdev_region( timerdev.devid, TIMER_CNT, TIMER_NAME );
    }
    else
    {
        /*动态分配主设备号*/
        alloc_chrdev_region( &timerdev.devid, 0, TIMER_CNT, TIMER_NAME );
        timerdev.major = MAJOR( timerdev.devid );
        timerdev.minor = MINOR( timerdev.devid );
    }

    /*创建字符设备*/
    timerdev.cdev.owner = THIS_MODULE;
    cdev_init( &timerdev.cdev, &timer_fops );
    cdev_add( &timerdev.cdev, timerdev.devid, TIMER_CNT );

    /*自动创建设备节点*/
    timerdev.class =  class_create( THIS_MODULE, TIMER_NAME );
    if( IS_ERR( timerdev.class ) )
    {
        debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
        return PTR_ERR( timerdev.class );
    }
    timerdev.device = device_create( timerdev.class, NULL, timerdev.devid, NULL, TIMER_NAME );
    if( IS_ERR( timerdev.device ) )
    {
        debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
        return PTR_ERR( timerdev.device );        
    }

    /*初始化定时器timer*/
    init_timer( &timerdev.timer );
    timerdev.timer.function = timer_function;
    timerdev.timer.data = ( unsigned long )&timerdev; /* 要传递给回调函数的参数 */


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
static void __exit timer_exit( void )
{
    gpio_free( timerdev.led_gpio ); /*释放GPIO*/

    del_timer_sync(&timerdev.timer); /*删除定时器timer*/

    cdev_del( &timerdev.cdev ); /*删除字符设备*/
    unregister_chrdev_region( timerdev.devid, TIMER_CNT ); /*注销设备号*/


    device_destroy( timerdev.class, timerdev.devid );
    class_destroy( timerdev.class );

    return;
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YangMou");

