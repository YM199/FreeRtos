#include "include/interrupt.h"

struct irq_dev irqdev;

static struct file_operations irq_fops = {
    .owner = THIS_MODULE,
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
    debug( "hello world \r\n");
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

    /*中断*/
    irqdev.irqkeydesc.handler = key0_handler; /*设置中断回调函数*/

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
    free_irq( irqdev.irqkeydesc.irqnum, &irqdev );
    gpio_free( irqdev.irqkeydesc.gpio );
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

