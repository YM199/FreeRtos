# Framebuffer

fb 是一种机制，将系统中所有跟显示有关的硬件以及软件集合起来，虚拟出一个 fb 设备，当我们编写好 LCD 驱动以后会生成一个名为/dev/fbX(X=0~n)的设备，应用程序通过访问/dev/fbX 这个设备就可以访问 LCD。

Linux 内核将所有的 Framebuffer 抽象为一个叫做 fb_info 的结构体，fb_info 结构体包含了 Framebuffer 设备的完整属性和操作集合。

Linux kenel 已经编写好了驱动，只要配置好设备树就可以直接使用。