# platform 设备驱动

![Linux总线、设备、驱动模式](img/Linux总线、设备、驱动模式.png)
>从设备树当中获取到设备信息

向系统注册一个驱动时，总线会在右侧的设备中查找，看看有没有与之对应匹配的设备，有的话就将两者联系起来。
> 同样的，向系统注册一个设备时，总线会在左侧的驱动中查找匹配的驱动，将它们联系起来。

有些SOC中外设是没有总线这个概念，为此 Linux 提出了 platform 虚拟总线，相应的就有platform_driver 和 platform_device。

---

platform_bus_type 就是 platform 平台总线

```C
struct bus_type platform_bus_type = { 
    name = "platform",
    .dev_groups = platform_dev_groups,
    .match = platform_match, /*匹配驱动和设备*/
    .uevent = platform_uevent,
    .pm = &platform_dev_pm_ops,
};
```
