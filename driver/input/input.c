#include "include/input.h"

struct keyinput_dev keyinputdev; /*key input设备*/

/*
* 函数名称: key_handler
* 函数功能: 打开定时器
* 函数备注: key按键中断的回调函数
*/
static irqreturn_t key_handler( int irq, void *dev_id )
{
    struct keyinput_dev *dev = ( struct keyinput_dev * )dev_id;
    dev->timer.data = ( volatile long )dev_id; /*传递给定时器回调函数的参数*/

    mod_timer( &dev->timer, jiffies + msecs_to_jiffies(10) ); /*定时10ms，打开定时器*/

    return IRQ_RETVAL( IRQ_HANDLED );
}

/*
* 函数名称: timer_function
* 函数功能: 定时到了以后再次读取按键值，如果按键还是处于按下状态就表示按键有效
* 函数备注: 定时器回调函数
*/
void timer_function(unsigned long arg)
{
    unsigned char  value;
    struct irq_keydesc *keydesc;

    struct keyinput_dev *dev = (struct keyinput_dev *)arg;
    keydesc = &dev->irqkeydesc;

    value = gpio_get_value( keydesc->gpio );
    if ( 0 == value )
    {
        input_report_key( dev->inputdev, keydesc->value, 1 ); /*1 按下*/
        input_sync(dev->inputdev);
    }
    else
    {
        input_report_key( dev->inputdev, keydesc->value, 0 ); /*0 松开*/
        input_sync(dev->inputdev);
    }
}

/*
* 函数名称: keyio_init
* 函数功能: 初始化GPIO、创建中断、创建定时器、创建输入设备
* 函数备注: 定时器回调函数
*/
static int keyio_init( void )
{
    char name[10];
    int ret = 0;

    keyinputdev.nd = of_find_node_by_path("/key"); /*获取设备树节点*/
    debug( NULL == keyinputdev.nd, -EINVAL );

    keyinputdev.irqkeydesc.gpio = of_get_named_gpio( keyinputdev.nd, "key-gpio", 0 ); /*获取GPIO编号*/
    debug( keyinputdev.irqkeydesc.gpio < 0, -EINVAL );

    /*设置GPIO的名字*/
    memset( keyinputdev.irqkeydesc.name, 0, sizeof( name ) );
    sprintf( keyinputdev.irqkeydesc.name, "KEY%d", 0 );

    gpio_request( keyinputdev.irqkeydesc.gpio, name );
    gpio_direction_input( keyinputdev.irqkeydesc.gpio );/*设置GPIO为输入*/

    keyinputdev.irqkeydesc.irqnum = irq_of_parse_and_map( keyinputdev.nd, 0 ); /*获取中断号*/
    keyinputdev.irqkeydesc.handler = key_handler; /*设置中断回调函数*/
    keyinputdev.irqkeydesc.value = KEY_0;

    /*申请中断*/
    ret = request_irq( keyinputdev.irqkeydesc.irqnum,
                       keyinputdev.irqkeydesc.handler,
                       IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,
                       keyinputdev.irqkeydesc.name, &keyinputdev );/*keyinputdev 会传递给中断处理函数 irq_handler_t 的第二个参数*/

    /*创建定时器*/
    init_timer( &keyinputdev.timer );
    keyinputdev.timer.function = timer_function; /*设置定时器回调函数*/

    /*input*/
    keyinputdev.inputdev = input_allocate_device();/*申请 input_dev*/
    keyinputdev.inputdev->name = KEYINPUT_NAME;    /*设置 input_dev 名字*/

    /*设置产生哪些事件*/
    __set_bit( EV_KEY, keyinputdev.inputdev->evbit ); /*按键事件*/
    __set_bit( EV_REP, keyinputdev.inputdev->evbit ); /*重复事件*/

    /*设置产生哪些按键值*/
    __set_bit( keyinputdev.irqkeydesc.value, keyinputdev.inputdev->keybit );

    ret = input_register_device( keyinputdev.inputdev );/*注册 input_dev*/
    debug( ret, ret );

    return 0;
}

/*
 * 函数名：driver_init
 * 功能描述：完成驱动的注册和调用其他注册函数
 * 备忘：加载驱动时自动调用该函数
*/
static int __init keyinput_init( void )
{
    keyio_init();
    return 0;
}

/*================================================================
 * 函数名：driver_exit
 * 功能描述：释放驱动相关资源
 * 备忘：驱动卸载时自动执行该函数
================================================================*/
static void __exit keyinput_exit( void )
{
    gpio_free( keyinputdev.irqkeydesc.gpio ); /*释放GPIO*/

    del_timer_sync( &keyinputdev.timer ); /*删除定时器*/

    free_irq( keyinputdev.irqkeydesc.irqnum, &keyinputdev ); /*释放中断*/

    /*释放 input_dev*/
    input_unregister_device( keyinputdev.inputdev );
    input_free_device( keyinputdev.inputdev );
}

module_init( keyinput_init );
module_exit( keyinput_exit );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "YangMou" );



