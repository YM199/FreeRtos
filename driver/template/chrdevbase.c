/************************************
 *
 * 文件名：chrdevbase.c
 *
 * 文件描述：字符驱动模板
 * 
 * 创建人：Yang Mou，2022 年  1 月 6 日
 * 
 * *********************************/
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>


#define CHRDEVBASE_MAJOR 200
#define CHRDEVBASE_NAME  "chrdevbase"


static char readbuf[1100];                      /*读缓冲区*/
static char writebuf[100];                      /*写缓冲区*/
static char kerneldata[100] = {"kernel data"};  /*读写的数据*/ 


/*================================================================ 
 * 函数名：chrdevbase_open
 * 功能描述：打开字符设备
 * 参数：
 *      inode [IN]：传递给驱动的inode
 *      filp  [IN]：设备文件，file结构体有个叫做private_data的成员变量，一般
 *                  在open的时候将private_data指向设备结构体
 * 返回值：
 *      成功：0 
 *      失败：其他
 * 作者：Yang Mou 2022/1/6
================================================================*/
static int chrdevbase_open( struct inode *inode, struct file *filp )
{
    return 0;
}

/*================================================================ 
 * 函数名：chrdevbase_read
 * 功能描述：从字符设备中读取数据
 * 参数：
 *      filp [IN]：要打开的设备文件
 *      buf  [OUT]：返回给用户空间的数据缓冲区
 *      cnt  [IN]：要读取的数据长度
 *      offt [IN]：相对于文件首地址的偏移
 * 返回值：
 *      成功：读取的字节数 
 *      失败：负数
 * 作者：Yang Mou 2022/1/6
================================================================*/
static ssize_t chrdevbase_read( 
                                struct file *filp, 
                                char __user *buf,
                                size_t cnt,
                                loff_t *offt  
                              )
{
    int retvalue = 0;

    /*向用户空间发送数据*/
    memcpy( readbuf, kerneldata, sizeof( kerneldata ) );
    retvalue = copy_to_user( buf, readbuf, cnt );
    if( 0 == retvalue )
    {
        printk( "kernel send data ok!\r\n" );
    }
    else
    {
        printk( "kernel send data failed!\r\n" );
    }

    return retvalue;
}



static ssize_t chrdevbase_write(  
                                 struct file *filp,
                                 const char __user *buf,
                                 size_t cnt,
                                 loff_t *offt
                               )
{
    int retvalue = 0;
    retvalue = copy_from_user( writebuf, buf, cnt );
    if( 0 == retvalue )
    {
        printk("kernel recevdata: %s\r\n", writebuf);
    }
    else
    {
        printk("kernel recevdata failed!\r\n");
    }

    return 0;
}

static int chrdevbase_release( 
                               struct inode *inode,
                               struct file *filp
                             )
{
    return 0;
}


static struct file_operations chrdevbase_fops = {
    .owner = THIS_MODULE,
    .open = chrdevbase_open,
    .read = chrdevbase_read,
    .write = chrdevbase_write,
    .release = chrdevbase_release,
};

static int __init chrdevbase_init( void )
{
    int retvalue = 0;

    retvalue = register_chrdev( CHRDEVBASE_MAJOR, CHRDEVBASE_NAME, &chrdevbase_fops );

    if( retvalue < 0 )
    {
        printk("chrdevbase driver register failed\r\n");
    }
    printk("chrdevbase_init()\r\n");
    return 0;
}


static void __exit chrdevbase_exit( void )
{
    unregister_chrdev( CHRDEVBASE_MAJOR, CHRDEVBASE_NAME );
    printk("chrdevbase_exit()\r\n");
}

module_init( chrdevbase_init );
module_exit( chrdevbase_exit )

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR("YangMou");