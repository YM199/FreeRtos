#include "include/led_driver.h"

struct led_dev leddev; /*定义设备结构体变量*/

/*
* 函数名称: led0_switch
* 函数功能: 控制LED0的GPIO高低电平
*/
void led0_switch(u8 sta)
{
    if( LED_ON == sta )
        gpio_set_value( leddev.led0, 0);
    else if( LED_OFF == sta )
        gpio_set_value( leddev.led0, 1 );
}



/*
* 函数名称: open
* 函数功能:
* 函数备注:应用层调用open时自动执行该函数
*/
static int open( struct inode *inode, struct file *filp )
{
    filp->private_data = &leddev; /* 设置私有数据 */
    return 0;
}

/*
* 函数名称: write
* 函数功能: 读取应用层数据来控制LED0
* 函数备注:应用层调用write时自动执行该函数
*/
static ssize_t write( struct file *filp, const char __user *buf, size_t cnt, loff_t *offt )
{
    int retvalue;
    unsigned char databuf[2];
    unsigned char ledstat;

    retvalue = copy_from_user( databuf, buf, cnt ); /*将用户空间的数据拷贝到内核空间*/
    debug( retvalue < 0, -EFAULT );

    ledstat = databuf[0];
    if( LED_ON == ledstat )
    {
        led0_switch( LED_ON );
    }
    else if( LED_OFF == ledstat )
    {
        led0_switch( LED_OFF );
    }

    return 0;
}

/*
* 结构名称：fops
* 结构类型：file_operations
* 结构备注：每一个设备号对应一个file_operations类型的结构，存放该驱动程序的各种操作函数
*/
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = open,
    .write = write,
};

/*
* 函数名称: probe
* 函数功能:
* 函数备注: 驱动和设备匹配成功以后此函数就会执行
*/
static int probe( struct platform_device *dev )
{
    printk("led driver and device was matched!\r\n");
    /*申请设备号*/
    if( leddev.major )
    {
        /*指定主设备号*/
        leddev.devid = MKDEV( leddev.major, 0 );
        register_chrdev_region( leddev.devid, DEV_CNT, DEV_NAME );
    }
    else
    {
        /*动态分配主设备号*/
        alloc_chrdev_region( &leddev.devid, 0, DEV_CNT, DEV_NAME );
        leddev.major = MAJOR( leddev.devid );
        leddev.minor = MINOR( leddev.devid );
    }

    /*创建字符设备*/
    leddev.cdev.owner = THIS_MODULE;
    cdev_init( &leddev.cdev, &fops );
    cdev_add( &leddev.cdev, leddev.devid, DEV_CNT );

    /*自动创建设备节点*/
    leddev.class =  class_create( THIS_MODULE, DEV_NAME );
    debug( IS_ERR( leddev.class ), PTR_ERR(leddev.class) );
    leddev.device = device_create( leddev.class, NULL, leddev.devid, NULL, DEV_NAME );
    debug( IS_ERR( leddev.device ), PTR_ERR(leddev.device) );

    /*初始化GPIO*/
    leddev.node = of_find_node_by_path( "/led" );
    debug( leddev.node == NULL, -EINVAL );
    leddev.led0 = of_get_named_gpio( leddev.node, "led-gpio", 0 );
    debug(leddev.led0 < 0, -EINVAL );
    gpio_request( leddev.led0, "led0" );
    gpio_direction_output( leddev.led0, 1 );

    return 0;
}

/*
* 函数名称: remove
* 函数功能: 释放驱动相关资源
* 函数备注: 当卸载 platform 驱动的时候此函数就会执行
*/
static int remove( struct platform_device *dev )
{
    gpio_set_value( leddev.led0, 1 ); /*卸载驱动时关闭LED*/
    gpio_free( leddev.led0 ); /*释放GPIO*/

    cdev_del( &leddev.cdev ); /*删除 dev*/
    unregister_chrdev_region( leddev.devid, DEV_CNT ); /*注销设备号*/

    /*删除自动创建的设备节点*/
    device_destroy( leddev.class, leddev.devid );
    class_destroy( leddev.class );

    return 0;
}

/*
* 结构名称: of_match
* 结构功能: 驱动的兼容表
* 结构备注: 一个驱动可以和多个设备匹配
*/
static const struct of_device_id of_match[] = {
    { .compatible = "atkalpha-led" },
    {},
};

/*
* 结构名称: driver
* 结构功能: platform 平台驱动结构体
*/
static struct platform_driver driver = {
    .driver  = {
        .name = "led",  /*platform驱动名字。可用于和设备匹配  /sys/bus/platform/drivers/led */
        .of_match_table = of_match, /*设备树匹配表*/
    },

    .probe = probe,
    .remove = remove,
};

/*
 * 函数名：driver_init
 * 功能描述：完成驱动的注册和调用其他注册函数
 * 备忘：加载驱动时自动调用该函数
*/
static int __init Driver_init( void )
{
    return platform_driver_register( &driver ); /*数向 Linux 内核注册一个 platform 驱动*/
}

/*================================================================
 * 函数名：driver_exit
 * 功能描述：释放驱动相关资源
 * 备忘：驱动卸载时自动执行该函数
================================================================*/
static void __exit Driver_exit( void )
{
    platform_driver_unregister( &driver ); /*卸载 platform 驱动*/
}

module_init( Driver_init );
module_exit( Driver_exit );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "YangMou" );