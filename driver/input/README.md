# INPUT子系统

input 子系统所有设备的主设备号都为13，在使用 input 子系统时不需要去注册字符设备，只需要向系统注册一个 input_dev 即可。
> input_dev结构体表示input设备。

申请一个 input_dev :
struct input_dev *input_allocate_device( void )

> void input_free_device(struct input_dev *dev)

然初始化这个 input_dev 的事件类型和事件值，初始化完成后向内核注册 input_dev。
int input_register_device( struct input_dev *dev )

> void input_unregister_device( struct input_dev *)

input_dev 注册过程如下：

1. 使用 input_allocate_device 函数申请一个 input_dev。
2. 初始化 input_dev 的事件类型以及事件值。
3. 使用 input_register_device 函数向 Linux 系统注册前面初始化好的 input_dev。
4. 卸载input驱动的时候需要先使用input_unregister_device函数注销掉注册的input_dev，
5. 然后使用 input_free_device 函数释放掉前面申请的 input_dev。

```C
struct input_dev *inputdev; /*input结构体变量*/

/*驱动入口函数*/
static int __init xxx_init( void )
{
    ......
    inputdev = input_allocate_device(); /*申请 input_dev*/
    inputdev->name = "test_inputdev";   /*设置 input_dev 名字*/

    /*第一种设置事件和事件值的方法*/
    __set_bit( EV_KEY, inputdev->evbit ); /*设置产生按键事件*/
    __set_bit( EV_REP, inputdev->evbit ); /*重复事件*/
    __set_bit( KEY_0, inputdev->keybit );  /*设置产生哪些按键值*/

    /*第二种设置事件和事件值的方法*/
    keyinputdev.inputdev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
    keyinputdev.inputdev->keybit[BIT_WORD(KEY_0)] |= BIT_MASK(KEY_0);

    /*第三种设置事件和事件值的方法*/
    keyinputdev.inputdev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
    input_set_capability( keyinputdev.inputdev, EV_KEY, KEY_0 );

    /*注册 input_dev*/
    input_register_device( inputdev );
    ......

    return 0;
}

/*驱动出口函数*/
static void __exit xxx_exit( void )
{
    input_unregister_device( inputdev ); /*注销 input_dev*/
    input_free_device( inputdev ); /*删除 input_dev*/
}
```

当我们向 Linux 内核成功注册 input_dev 设备以后，会在/dev/input 目录下生成一个名为“eventX(X=0….n)”的文件，这个/dev/input/eventX 就是对应的 input 设备文件。我们读取这个文件就可以获取到输入事件信息，比如按键值什么的。使用read函数读取输入设备文件，也就是/dev/input/eventX，读取到的数据按照 input_event 结构体组织起来。获取到输入事件以后(input_event 结构体类型)使用 switch case 语句来判断事件类型

## 上报输入事件

上报函数如下:

```C
static inline void input_report_key(struct input_dev *dev,unsigned int code, int value)
void input_report_rel(struct input_dev *dev, unsigned int code, int value)
void input_report_abs(struct input_dev *dev, unsigned int code, int value)
void input_report_ff_status(struct input_dev *dev, unsigned int code, int value)
void input_report_switch(struct input_dev *dev, unsigned int code, int value)
void input_mt_sync(struct input_dev *dev)
```

>这些函数底层都是调用void input_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)

在上报事件以后使用 input_sync 函数来告诉 Linux 内核 input 子系统上报结束

void input_sync(struct input_dev *dev)

```C
void timer_function( unsigned long arg )
{
    unsigned char value;

    value = gpio_get_value( keydesc->gpio );
    if( 0 == value )
    {
        /*上报按键值*/
        input_report_key( input, KEY_0, 1 ); /*最后一个参数1、按下*/
        input_sync( inputdev ); /*同步事件*/
    }
    else
    {
        input_report_key( input, KEY_0, 0 ); /*最后一个参数0、松开*/
        input_sync( inputdev ); /*同步事件*/
    }
}
```

---

在应用层定义一个 input_dev 类型的变量，使用 read 将数据读取到其中。

```C
static struct input_event inputevent;
read( fd, &inputevent, sizeof( inputevent ) );
inputevent.code  /*按键码*/
inputevent.value /*按键值*/
```

> input_report_key( dev->inputdev, 按键码, 按键值 );

按键码用来区分哪个按键
