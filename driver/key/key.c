/************************************
 *
 * 文件名：key.c
 *
 * 文件描述：key驱动
 * 
 * 创建人：Yang Mou，2022 年  1 月 19 日
 * 
 * *********************************/

#include "include/key.h"


struct keydev chrkey;

static int key_open( struct inode *inode, struct file *filp );
static int key_release( struct inode *inode, struct file *filp );
static ssize_t key_read( struct file *filp, char __user *buf, size_t cnt, loff_t *offt );

static struct file_operations key_fops =
{
    .owner = THIS_MODULE,
    .open = key_open,
    .release = key_release,
    .read = key_read,
};

/*================================================================ 
 * 函数名：key_gpio_init
 * 功能描述：初始化GPIO
 * 参数：
 *      void
 * 返回值：
 *      成功: 0
 *      失败: -1
 * 作者：Yang Mou 2022/1/19
================================================================*/
static int key_gpio_init(void)
{
    chrkey.nd = of_find_node_by_path( "/key" ); /* 获取/dev/key */
    if( NULL == chrkey.nd )
    {
        debug( "FILE: %s, LINE: %d:\r\n", __FILE__, __LINE__ );
        debug( "can not found /key!\r\n" );
        return -1;        
    }

    chrkey.key_gpio = of_get_named_gpio( chrkey.nd, "key-gpio", 0 ); /* 获取GPIO编号 */
    if( chrkey.key_gpio < 0 )
    {
        debug( "FILE: %s, LINE: %d:\r\n", __FILE__, __LINE__ );
        debug( "can't get key-gpio!\r\n" );
        return -1;        
    }

    gpio_request( chrkey.key_gpio, "key0" ); /*申请一个 GPIO 管脚*/
    gpio_direction_input( chrkey.key_gpio );  

    return 0;
}


/*================================================================
 * 函数名：key_open
 * 功能描述：打开设备文件
 * 参数：
 *      inode[IN]: 设备节点
 *      filp [IN]: 要打开的设备文件
 * 返回值：
 *      成功: 0
 *      失败: -1
 * 作者：Yang Mou 2022/1/18
================================================================*/
static int key_open( struct inode *inode, struct file *filp )
{
    int ret = 0;
    filp->private_data = &chrkey; /*设置私有数据*/

    ret = key_gpio_init();
    if( ret < 0 )
    {
        debug( "FILE: %s, LINE: %d:\r\n", __FILE__, __LINE__ );
        debug( "key init error!\r\n" );        
        return ret;
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
 * 作者：Yang Mou 2022/1/18
================================================================*/
static int key_release( struct inode *inode, struct file *filp )
{

    gpio_free( chrkey.key_gpio ); /*释放GPIO*/
    return 0;
}

/*================================================================
 * 函数名：key_read
 * 功能描述：读取设备文件
 * 参数：
 *      filp [IN]:  要打开的设备文件
 *      buf  [OUT]: 用户空间的buf
 * 返回值：
 *      成功: 0
 *      失败: -1
 * 作者：Yang Mou 2022/1/18
================================================================*/
static ssize_t key_read( struct file *filp, char __user *buf, size_t cnt, loff_t *offt )
{
    int ret = 0;
    unsigned char value;
    struct keydev *dev = filp->private_data; /*获取私有数据*/

    if( 0 == gpio_get_value( dev->key_gpio ) )
    {
        while( !gpio_get_value( dev->key_gpio ) )
        {
            atomic_set( &dev->keyvalue, KEY0VALUE );
        }
    }
    else
    {
        atomic_set( &dev->keyvalue, INVAKEY );
    }
    
    value = atomic_read( &dev->keyvalue );
    ret = copy_to_user( buf, &value, sizeof( value ) ); /* 将value的值拷贝到用户空间 */
    return ret;
}




/*================================================================ 
 * 函数名：key_init
 * 功能描述：加载字符设备
 * 参数：
 *      void
 * 返回值：
 *      成功：0 
 *      失败：-1
 * 作者：Yang Mou 2022/1/9
================================================================*/
static int __init mykey_init( void )
{
  
    /* 初始化原子变量 */
    atomic_set( &chrkey.keyvalue, INVAKEY );

    /*申请设备号*/
    if( chrkey.major )
    {
        chrkey.devid = MKDEV( chrkey.major, 0 );
        register_chrdev_region( chrkey.devid, KEY_CNT, KEY_NAME);
    }
    else
    {
        alloc_chrdev_region( &chrkey.devid, 0, KEY_CNT, KEY_NAME );
        chrkey.major = MAJOR( chrkey.devid );
        chrkey.minor = MINOR( chrkey.devid );
    }

    /*初始化cdev*/
    chrkey.cdev.owner = THIS_MODULE;
    cdev_init( &chrkey.cdev, &key_fops );
    cdev_add( &chrkey.cdev, chrkey.devid, KEY_CNT );

    /*创建类*/
    chrkey.class = class_create( THIS_MODULE, KEY_NAME );
    if( IS_ERR( chrkey.class ) )
    {
        return PTR_ERR( chrkey.class );
    }

    /*创建设备*/
    chrkey.device = device_create( chrkey.class, NULL, chrkey.devid, NULL, KEY_NAME );
    if( IS_ERR( chrkey.device ) )
    {
        return PTR_ERR( chrkey.device );
    }

    debug( "key init\r\n" );
    return 0;
}


/*================================================================ 
 * 函数名：key_exit
 * 功能描述：退出字符设备
 * 参数：
 *      void
 * 返回值：
 *      空
 * 作者：Yang Mou 2022/1/9
================================================================*/
static void __exit mykey_exit( void )
{
    cdev_del( &chrkey.cdev );
    unregister_chrdev_region( chrkey.devid, KEY_CNT );

    device_destroy( chrkey.class, chrkey.devid );
    class_destroy( chrkey.class );

    debug("key exit!\r\n");

    return;
}

module_init( mykey_init );
module_exit( mykey_exit );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR("YangMou");
