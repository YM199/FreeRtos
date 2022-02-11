#include "include/misc.h"

struct miscbeep_dev miscbeep; /*beep设备*/

/*
* 函数名称: open
* 函数功能:
* 函数备注:应用层调用open时自动执行该函数
*/
static int open( struct inode *inode, struct file *filp )
{
    filp->private_data = &miscbeep; /* 设置私有数据 */
    return 0;
}

/*
* 函数名称: write
* 函数功能: 读取应用层数据来控制BEEP
* 函数备注:应用层调用write时自动执行该函数
*/
static ssize_t write( struct file *filp, const char __user *buf, size_t cnt, loff_t *offt )
{
    int retvalue;
    unsigned char databuf[1];
    unsigned char beepstat;
    struct miscbeep_dev *dev = filp->private_data;

    retvalue = copy_from_user( databuf, buf, cnt );
    debug( retvalue < 0, -EFAULT );

    beepstat = databuf[0];
    if( beepstat == BEEPON )
    {
        gpio_set_value( dev->beep_gpio, 0 );
    }
    else if( beepstat == BEEPOFF )
    {
        gpio_set_value( dev->beep_gpio, 1 );
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

static struct miscdevice beep_miscdev = {
    .minor = MISCBEEP_MINOR,
    .name = MISCBEEP_NAME,
    .fops = &fops,
};

/*
* 函数名称: probe
* 函数功能:
* 函数备注: 驱动和设备匹配成功以后此函数就会执行
*/
static int probe( struct platform_device *dev )
{
    int ret = 0;
    printk("led driver and device was matched!\r\n");


    miscbeep.nd = of_find_node_by_path( "/beep"); /*获取设备树节点*/
    debug( NULL == miscbeep.nd, -EINVAL );

    miscbeep.beep_gpio = of_get_named_gpio( miscbeep.nd, "beep-gpio", 0 ); /*获取GPIO编号*/
    debug( miscbeep.beep_gpio < 0, -EINVAL );

    ret = gpio_direction_output( miscbeep.beep_gpio, 1 ); /*设置GPIO为输出，高电平*/
    debug( ret < 0, -EINVAL );

    /*一般情况向需要自己注册字符设备，但MISC不需要*/
    ret = misc_register( &beep_miscdev );

    return 0;
}

/*
* 函数名称: remove
* 函数功能: 释放驱动相关资源
* 函数备注: 当卸载 platform 驱动的时候此函数就会执行
*/
static int remove( struct platform_device *dev )
{
    gpio_set_value( miscbeep.beep_gpio, 1 ); /*卸载驱动时关闭BEEP*/

    /*注销MISC驱动*/
    misc_deregister( &beep_miscdev );
    return 0;
}

/*
* 结构名称: of_match
* 结构功能: 驱动的兼容表
* 结构备注: 一个驱动可以和多个设备匹配
*/
static const struct of_device_id of_match[] = {
    { .compatible = "atkalpha-beep" },
    {},
};

/*
* 结构名称: driver
* 结构功能: platform 平台驱动结构体
*/
static struct platform_driver driver = {
    .driver  = {
        .name = "beep",  /*platform驱动名字。可用于和设备匹配  /sys/bus/platform/drivers/beep */
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