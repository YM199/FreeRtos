
/************************************
 *
 * 文件名：led.c
 *
 * 文件描述：led驱动
 * 
 * 创建人：Yang Mou，2022 年  1 月 9 日
 * 
 * *********************************/

#include "include/led.h"

static void __iomem *IMX6U_CCM_CCGR1;
static void __iomem *SW_MUX_GPIO1_IO03;
static void __iomem *SW_PAD_GPIO1_IO03;
static void __iomem *GPIO1_DR;
static void __iomem *GPIO1_GDIR;

struct chrdev chrled;



/*================================================================ 
 * 函数名：led_open
 * 功能描述：应用层调用open打开设备时就会调用该函数
 * 参数：
 *      inode[IN]: 设备节点
 *      filp [IN]: 要打开的设备文件
 * 返回值：
 *      成功: 0
 *      失败: -1
 * 作者：Yang Mou 2022/1/18
================================================================*/
static int led_open( struct inode *inode, struct file *filp )
{
    /* 通过判断原子变量的值来检查 LED 有没有被别的应用使用 */
    if( !atomic_dec_and_test( &chrled.lock ) )
    {
        atomic_inc( &chrled.lock );
        debug ("FILE: %s, LINE: %d", __FILE__, __LINE__);
        debug("device is busy!\r\n");
        return -1;
    }

    filp->private_data = &chrled; /*设置私有数据*/
    return 0;
}

/*================================================================ 
 * 函数名：led_release
 * 功能描述：应用层调用close关闭设备时就会调用该函数
 * 参数：
 *      inode[IN]: 设备节点
 *      filp [IN]: 要关闭的设备文件
 * 返回值：
 *      成功: 0
 *      失败: -1
 * 作者：Yang Mou 2022/1/18
================================================================*/
static int led_release( struct inode *inode, struct file *filp )
{
    struct chrdev *dev = filp->private_data;
    /* 关闭驱动文件的时候释放原子变量 */
    atomic_inc(&dev->lock);
    return 0;
}

static ssize_t led_read( struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
    return 0;
}

/*================================================================ 
 * 函数名：led_write
 * 功能描述：控制LED的状态
 * 参数：
 *      filp [IN]： 要打开的设备文件
 *      buf  [OUT]：传递给内核空间的数据缓冲区
 *      cnt  [IN]： 要读取的数据长度
 *      offt [IN]： 相对于文件首地址的偏移
 * 返回值：
 *      成功：读取的字节数 
 *      失败：-1
 * 作者：Yang Mou 2022/1/10
================================================================*/
static ssize_t led_write( struct file *filp, const char __user *buf, size_t cnt, loff_t *offt )
{
    int retvalue;
    unsigned char databuf[1];
    unsigned char ledstat;
    struct chrdev *dev = filp->private_data;


    retvalue = copy_from_user( databuf, buf, cnt );
    if( retvalue < 0 )
    {
        debug ("FILE: %s, LINE: %d", __FILE__, __LINE__);
        debug("kernel write failed!\r\n");
        return -1;
    }

    ledstat = databuf[0];
    if( ledstat == LED_ON )
    {
        gpio_set_value( dev->led_gpio, 0 );
    }
    else if(ledstat == LED_OFF)
    {
        gpio_set_value( dev->led_gpio, 1 );
    }
    
    return 0;
} 

static struct file_operations led_fops =
{
    .owner = THIS_MODULE,
    .open = led_open,
    .read = led_read,
    .write = led_write,
    .release = led_release,
};


/*================================================================ 
 * 函数名：led_init
 * 功能描述：加载字符设备
 * 参数：
 *      void
 * 返回值：
 *      成功：0 
 *      失败：-1
 * 作者：Yang Mou 2022/1/9
================================================================*/
static int __init led_init( void )
{
    int ret;
  
    /* 初始化原子变量 */
    atomic_set( &chrled.lock, 1 );/* 原子变量初始值为 1 */

    /*获取设备节点*/
    chrled.nd = of_find_node_by_path( "/led" );
    if( NULL == chrled.nd )
    {
        debug( "FILE: %s, LINE: %d", __FILE__, __LINE__ );
        debug( "alphaled node can not found!\r\n" );
        return -1;
    }

    /*获取设备树中的 gpio 属性，得到 LED 所使用的 LED 编号*/
    chrled.led_gpio = of_get_named_gpio( chrled.nd, "led-gpio", 0 );
    if( chrled.led_gpio < 0 )
    {
        debug( "FILE: %s, LINE: %d", __FILE__, __LINE__ );
        debug( "can't get led-gpio!\r\n" );
        return -1;
    }

    /* 设置 GPIO1_IO03 为输出，并且输出高电平，默认关闭 LED 灯 */
    ret = gpio_direction_output( chrled.led_gpio, 1 );
    if( ret < 0 )
    {
        debug( "FILE: %s, LINE: %d", __FILE__, __LINE__ );
        debug( "can't set gpio!\r\n" );
        return -1;        
    }

    /*申请设备号*/
    if( chrled.major )
    {
        chrled.devid = MKDEV( chrled.major, 0 );
        register_chrdev_region( chrled.devid, LED_CNT, LED_NAME);
    }
    else
    {
        alloc_chrdev_region( &chrled.devid, 0, LED_CNT, LED_NAME );
        chrled.major = MAJOR( chrled.devid );
        chrled.minor = MINOR( chrled.devid );
    }

    /*初始化cdev*/
    chrled.cdev.owner = THIS_MODULE;
    cdev_init( &chrled.cdev, &led_fops );
    cdev_add( &chrled.cdev, chrled.devid, LED_CNT );

    /*创建类*/
    chrled.class = class_create( THIS_MODULE, LED_NAME );
    if( IS_ERR( chrled.class ) )
    {
        return PTR_ERR( chrled.class );
    }

    /*创建设备*/
    chrled.device = device_create( chrled.class, NULL, chrled.devid, NULL, LED_NAME );
    if( IS_ERR( chrled.device ) )
    {
        return PTR_ERR( chrled.device );
    }

    debug( "led init\r\n" );
    return 0;
}


/*================================================================ 
 * 函数名：led_exit
 * 功能描述：退出字符设备
 * 参数：
 *      void
 * 返回值：
 *      空
 * 作者：Yang Mou 2022/1/9
================================================================*/
static void __exit led_exit( void )
{
    iounmap( IMX6U_CCM_CCGR1 );
    iounmap( SW_MUX_GPIO1_IO03 );
    iounmap( SW_PAD_GPIO1_IO03 );
    iounmap( GPIO1_DR );
    iounmap( GPIO1_GDIR );

    //*unregister_chrdev( LED_MAJOR, LED_NAME );*/
    cdev_del( &chrled.cdev );
    unregister_chrdev_region( chrled.devid, LED_CNT );

    device_destroy( chrled.class, chrled.devid );
    class_destroy( chrled.class );

    debug("led exit!\r\n");

    return;
}

module_init( led_init );
module_exit( led_exit );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR("YangMou");