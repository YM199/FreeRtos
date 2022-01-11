
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


void led_switch( const u8 sta )
{
    u32 val = 0;
    if( sta == LED_ON )
    {
        val = readl(GPIO1_DR);
        val &= ~(1 << 3);
        writel(val, GPIO1_DR);
    }
    else if( sta == LED_OFF )
    {
        val = readl(GPIO1_DR);
        val|= (1 << 3);
        writel(val, GPIO1_DR);
    }
}

static int led_open( struct inode *inode, struct file *filp )
{
    filp->private_data = &chrled; /*设置私有数据*/
    return 0;
}

static int led_release( struct inode *inode, struct file *filp )
{
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
        led_switch( LED_ON );
    }
    else if(ledstat == LED_OFF)
    {
        led_switch( LED_OFF );
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
    u32 val = 0;

    /*地址映射*/
    IMX6U_CCM_CCGR1 = ioremap( CCM_CCGR1_BASE, 4 );
    SW_MUX_GPIO1_IO03 = ioremap( SW_MUX_GPIO1_IO03_BASE, 4 );
    SW_PAD_GPIO1_IO03 = ioremap( SW_PAD_GPIO1_IO03_BASE, 4 );
    GPIO1_DR = ioremap( GPIO1_DR_BASE, 4 );
    GPIO1_GDIR = ioremap( GPIO1_GDIR_BASE, 4 );

    /* 使能 GPIO1 时钟 */
    val = readl( IMX6U_CCM_CCGR1 );
    val &= ~(3 << 26); /* 清除以前的设置 */
    val |= (3 << 26); /* 设置新值 */
    writel(val, IMX6U_CCM_CCGR1);

    /* 复用为GPIO1_IO03 */
    writel(5, SW_MUX_GPIO1_IO03);

    /* 设置 IO 属性 */
    writel(0x10B0, SW_PAD_GPIO1_IO03);

    /* 设置 GPIO1_IO03 为输出功能 */
    val = readl(GPIO1_GDIR);
    val &= ~(1 << 3); /* 清除以前的设置 */
    val |= (1 << 3); /* 设置为输出 */
    writel(val, GPIO1_GDIR);

    /* 默认关闭 LED */
    val = readl(GPIO1_DR);
    val |= (1 << 3);
    writel(val, GPIO1_DR);


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