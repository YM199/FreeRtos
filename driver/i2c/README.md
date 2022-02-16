# I2C

## I2C总线(适配器)驱动

在讲 platform 的时候就说过，platform 是虚拟出来的一条总线，目的是为了实现总线、设备、驱动框架。**对于 I2C 而言，不需要虚拟出一条总线，直接使用 I2C总线即可**。

I2C 总线驱动重点是 I2C 适配器，Linux 内核将 SOC 的 I2C 适配器抽象成 i2c_adapter。

```C
struct i2c_adapter
{
    ......
    const struct i2c_algorithm *algo; /* 总线访问算法 */
    ......
}
```

对于一个 I2C 适配器，肯定要对外提供读写 API 函数，设备驱动程序可以使用这些 API 函数来完成读写操作。i2c_algorithm 就是 I2C 适配器与 IIC 设备进行通信的方法。

> 一般 SOC 的 I2C 总线驱动都是由半导体厂商编写的，比如 I.MX6U 的 I2C 适配器驱动 NXP 已经编写好了，这个不需要用户去编写。因此 I2C 总线驱动对我们这些 SOC 使用者来说是被屏蔽掉的，我们只要专注于 I2C 设备驱动即可。除非你是在半导体公司上班，工作内容就是写 I2C 适配器驱动。

## I2C设备驱动

i2c_client 就是描述设备信息的，i2c_driver 描述驱动内容，类似于 platform_driver。

### i2c_client 结构体

```C
struct i2c_client {
    unsigned short flags;        /*标志 */
    unsigned short addr;         /*芯片地址，7 位，存在低 7 位*/
    ......
    char name[I2C_NAME_SIZE];    /*名字*/
    struct i2c_adapter *adapter; /*对应的 I2C 适配器*/
    struct device dev;           /*设备结构体*/
    int irq;                     /*中断*/
    struct list_head detected;
    ......
    };
```

一个设备对应一个 i2c_client，每检测到一个 I2C 设备就会给这个 I2C 设备分配一个i2c_client。

### i2c_driver 结构体

i2c_driver 类似 platform_driver。

```C
struct i2c_driver {
    ......
    int (*probe)(struct i2c_client *, const struct i2c_device_id *);
    struct device_driver driver;
    const struct i2c_device_id *id_table;
    ......
};
```

1. 当 I2C 设备和驱动匹配成功以后 probe 函数就会执行，和 platform 驱动一样。
2. device_driver 驱动结构体，如果使用设备树的话，需要设置 device_driver 的of_match_table 成员变量，也就是驱动的兼容(compatible)属性。
3. id_table 是传统的、未使用设备树的设备匹配 ID 表。

---

构建完成以后需要向 Linux 内核注册这个 i2c_driver。

```C
int i2c_register_driver(struct module *owner, struct i2c_driver *driver);
```

另外 i2c_add_driver 也常常用于注册 i2c_driver,i2c_add_driver 就是对 i2c_register_driver 做了一个简单的封装，只有一个参数，就是要注册的 i2c_driver。

>注销 I2C 设备驱动的时候需要将前面注册的 i2c_driver 从 Linux 内核中注销掉，需要用到i2c_del_driver 函数。
>
>```C
>void i2c_del_driver(struct i2c_driver *driver);
>```
