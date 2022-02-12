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
    __set_bit( KEY_0, inputdev->evbit );  /*设置产生哪些按键值*/

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
