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