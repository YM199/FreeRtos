/************************************
 *
 * 文件名：chrdevbase.c
 *
 * 文件描述：字符驱动模板
 * 
 * 创建人：Yang Mou，2022 年  1 月 6 日
 * 
 * *********************************/

#include "chrdev.h"


static char readbuf[100];                      /*读缓冲区*/
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
 * 功能描述：从内核空间拷贝数据到用户空间
 * 参数：
 *      filp [IN]：要打开的设备文件
 *      buf  [OUT]：返回给用户空间的数据缓冲区
 *      cnt  [IN]：要读取的数据长度
 *      offt [IN]：相对于文件首地址的偏移
 * 返回值：
 *      成功：读取的字节数 
 *      失败：-1
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

    
    memcpy( readbuf, kerneldata, sizeof( kerneldata ) );
    retvalue = copy_to_user( buf, readbuf, cnt );/*向用户空间发送数据*/
    if(retvalue != 0)
    {
        debug( "kernel send data error!\r\n" );
        return -1;
    }

    return cnt;
}


/*================================================================ 
 * 函数名：chrdevbase_read
 * 功能描述：从用户空间拷贝数据到内核空间
 * 参数：
 *      filp [IN]：要打开的设备文件
 *      buf  [OUT]：返回给内核空间的数据缓冲区
 *      cnt  [IN]：要读取的数据长度
 *      offt [IN]：相对于文件首地址的偏移
 * 返回值：
 *      成功：读取的字节数 
 *      失败：-1
 * 作者：Yang Mou 2022/1/9
================================================================*/
static ssize_t chrdevbase_write(  
                                 struct file *filp,
                                 const char __user *buf,
                                 size_t cnt,
                                 loff_t *offt
                               )
{
    int retvalue = 0;
    retvalue = copy_from_user( writebuf, buf, cnt );
    if( retvalue != 0 )
    {
        debug( "FILE: %s, LINE: %d: kernel recevdata failed!\r\n", __FILE__, __LINE__ );
        return -1;
    }

    debug( "kernel recevdata: %s\r\n", writebuf );


    return cnt;
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


/*================================================================ 
 * 函数名：chrdevbase_init
 * 功能描述：加载字符设备
 * 参数：
 *      void
 * 返回值：
 *      0
 * 作者：Yang Mou 2022/1/8
================================================================*/
static int __init chrdevbase_init( void )
{
    int retvalue = 0;

    retvalue = register_chrdev( CHRDEVBASE_MAJOR, CHRDEVBASE_NAME, &chrdevbase_fops );

    if( retvalue < 0 )
    {
        debug( "FILE: %s, LINE: %d: chrdevbase driver register failed!\r\n", __FILE__, __LINE__ );
    }
    debug( "chrdevbase_init!\r\n" );
    return 0;
}


/*================================================================ 
 * 函数名：chrdevbase_exit
 * 功能描述：卸载字符设备
 * 参数：
 *      void
 * 返回值：
 *      无
 * 作者：Yang Mou 2022/1/8
================================================================*/
static void __exit chrdevbase_exit( void )
{
    unregister_chrdev( CHRDEVBASE_MAJOR, CHRDEVBASE_NAME );
    debug( "chrdevbase_exit!\r\n" );
}

module_init( chrdevbase_init );
module_exit( chrdevbase_exit )

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR("YangMou");