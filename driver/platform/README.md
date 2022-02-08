# platform 设备驱动

![Linux总线、设备、驱动模式](img/Linux总线、设备、驱动模式.png)
>从设备树当中获取到设备信息

向系统注册一个驱动时，总线会在右侧的设备中查找，看看有没有与之对应匹配的设备，有的话就将两者联系起来。
> 同样的，向系统注册一个设备时，总线会在左侧的驱动中查找匹配的驱动，将它们联系起来。

有些SOC中外设是没有总线这个概念，为此 Linux 提出了 platform 虚拟总线，相应的就有platform_driver 和 platform_device。

## platform总线

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

>```C
>static int platform_match(struct device *dev, struct device_driver *drv)
>{
>    struct platform_device *pdev = to_platform_device(dev);
>    struct platform_driver *pdrv = to_platform_driver(drv);
>
>    /* When driver_override is set, only bind to the matching driver */
>    if (pdev->driver_override)
>        return !strcmp(pdev->driver_override, drv->name);
>
>    /* OF类型匹配，也就是设备树匹配 */
>    if (of_driver_match_device(dev, drv))
>    return 1;
>
>    /* ACPI匹配 */
>    if (acpi_driver_match_device(dev, drv))
>        return 1;
>
>    /* id table匹配 */
>    if (pdrv->id_table)
>        return platform_match_id(pdrv->id_table, pdev) != NULL;
>
>    /* 直接比较驱动和设备的 name 字段 */
>    return (strcmp(pdev->name, drv->name) == 0);
>}
>```
>
>驱动和设备设备有4种匹配方式

## platform驱动

platform_driver 结构体表示 platform 驱动。

```C
struct platform_driver {
    int (*probe)(struct platform_device *);
    int (*remove)(struct platform_device *);
    void (*shutdown)(struct platform_device *);
    int (*suspend)(struct platform_device *, pm_message_t state);
    int (*resume)(struct platform_device *);
    struct device_driver driver;
    const struct platform_device_id *id_table;
    bool prevent_deferred_probe;
10 };
```

>probe 函数，当驱动与设备匹配成功后就会执行。
>
>id_table 表，当使用 id table 匹配时会用到。id_table 就是个数组，数组元素类型为 platform_device_id。
>
>device_driver 结构体中有个成员 of_match_table ,用于采用设备树的时候驱动使用的匹配表。同样是个数组，数组元素类型为 of_device_id。
>of_device_id 中有个 compatible 成员，对于设备树而言，就是通过设备节点的 compatible 成员和 of_match_table 中每个项目的 compatible 成员变量进行匹配。
>
>
