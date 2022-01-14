> 将 NXP 官方的 uboot 移植到正点原子的 I.MX6ULL 开发板上

------

# 一、添加开发板默认配置文件

```shell
cd configs
cp mx6ull_14x14_evk_emmc_defconfig mx6ull_alientek_emmc_defconfig
```

然后将文件 mx6ull_alientek_emmc_defconfig 中的内容改成下面的：

> 直接全部删除，然后复制

```
CONFIG_SYS_EXTRA_OPTIONS="IMX_CONFIG=board/freescale/mx6ull_alientek_emmc/imximage.cfg,MX6ULL_EVK_EMMC_REWORK"
CONFIG_ARM=y
CONFIG_ARCH_MX6=y
CONFIG_TARGET_MX6ULL_ALIENTEK_EMMC=y
CONFIG_CMD_GPIO=y
```

------

# 二、添加开发板对应的头文件

```shell
cd include/configs/
cp mx6ullevk.h mx6ull_alientek_emmc.h
```



将：

```c
#ifndef __MX6ULLEVK_CONFIG_H
#define __MX6ULLEVK_CONFIG_H
```

改为：

```c
#ifndef __MX6ULL_ALIENTEK_EMMC_CONFIG_H
#define __MX6ULL_ALIENTEK_EMMC_CONFIG_H
```

> mx6ull_alientek_emmc.h 里面有很多宏定义，这些宏定义基本用于配置 uboot，也有一些I.MX6ULL 的配置项目。如果我们自己要想使能或者禁止 uboot 的某些功能，那就在mx6ull_alientek_emmc.h 里面做修改即可。

------

# 三、添加开发板对应的板级文件夹

```shell
cd board/freescale/
cp mx6ullevk/ -r mx6ull_alientek_emmc
```

进 入 mx6ull_alientek_emmc 目 录 中 ， 将 其 中 的 mx6ullevk.c 文 件 重 命 名 为mx6ull_alientek_emmc.c，命令如下：

```bash
cd mx6ull_alientek_emmc
mv mx6ullevk.c mx6ull_alientek_emmc.c
```

**修改 mx6ull_alientek_emmc 目录下的 Makefile 文件**

> 直接全部删除，然后复制

```makefile
# (C) Copyright 2015 Freescale Semiconductor, Inc.
#
# SPDX-License-Identifier: GPL-2.0+
#

obj-y := mx6ull_alientek_emmc.o

extra-$(CONFIG_USE_PLUGIN) := plugin.bin

$(obj)/plugin.bin: $(obj)/plugin.o
	$(OBJCOPY) -O binary --gap-fill 0xff $< $@
```

**修改 mx6ull_alientek_emmc 目录下的 imximage.cfg 文件**

将 imximage.cfg 中的下面一句：

```
PLUGIN board/freescale/mx6ullevk/plugin.bin 0x00907000
```

改为：

```
PLUGIN board/freescale/mx6ull_alientek_emmc /plugin.bin 0x00907000
```

**修改 mx6ull_alientek_emmc 目录下的 Kconfig 文件**

> 直接全部删除，然后复制粘贴

```
if TARGET_MX6ULL_ALIENTEK_EMMC

config SYS_BOARD
	default "mx6ull_alientek_emmc"

config SYS_VENDOR
	default "freescale"

config SYS_SOC
	default "mx6"

config SYS_CONFIG_NAME
	default "mx6ull_alientek_emmc"

endif
```

**修改 mx6ull_alientek_emmc 目录下的 MAINTAINERS 文件**

```
MX6ULL_ALIENTEK_EMMC BOARD
M: Peng Fan <peng.fan@nxp.com>
S: Maintained
F: board/freescale/mx6ull_alientek_emmc/
F: include/configs/mx6ull_alientek_emmc.h
```

# 四、修改 U-Boot 图形界面配置文件

修改文件arch/arm/cpu/armv7/mx6/Kconfig，在 207 行加入如下内容：

```
config TARGET_MX6ULL_ALIENTEK_EMMC
	bool "Support mx6ull_alientek_emmc"
	select MX6ULL
	select DM
	select DM_THERMAL
```

在最后一行的 endif 的前一行添加如下内容：

```
source "board/freescale/mx6ull_alientek_emmc/Kconfig"
```

> 到此为止，I.MX6U-ALPHA 开发板就已经添加到 uboot 中了

# 五、使用新添加的板子配置编译 uboot

在 uboot 根目录下新建一个名为 mx6ull_alientek_emmc.sh 的 shell 脚本，在这个 shell 脚本里面输入如下内容：

```shell
#!/bin/bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- distclean
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- mx6ull_alientek_emmc_defconfig
make V=1 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j5
```

## 5.1、LCD 驱动修改

打开文件 mx6ull_alientek_emmc.c，将原始屏幕参数修改为如下内容：

```c
struct display_info_t const displays[] = {{
	.bus = MX6UL_LCDIF1_BASE_ADDR,
	.addr = 0,
	.pixfmt = 24,
	.detect = NULL,
	.enable = do_enable_parallel_lcd,
	.mode   = {
		.name           = "TFT4384",
		.xres           = 800,
		.yres           = 480,
		.pixclock       = 32258,
		.left_margin    = 88,
 		.right_margin   = 40,
		.upper_margin   = 32,
		.lower_margin   = 13,
		.hsync_len      = 48,
		.vsync_len      = 3,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} } };
```

打开 mx6ull_alientek_emmc.h，找到所有如下语句：

```
panel=TFT43AB
```

将其改为：

```
panel=TFT4384
```



## 5.2、网络驱动修改

**网络 PHY 地址修改**

打开 mx6ull_alientek_emmc.h修改了 334 和 344行

```c
332 #if (CONFIG_FEC_ENET_DEV == 0)
333 #define IMX_FEC_BASE            ENET_BASE_ADDR
334 #define CONFIG_FEC_MXC_PHYADDR          0x0
335 #define CONFIG_FEC_XCV_TYPE             RMII
336 #elif (CONFIG_FEC_ENET_DEV == 1)
337 #define IMX_FEC_BASE            ENET2_BASE_ADDR
338 #define CONFIG_FEC_MXC_PHYADDR      0x1
339 #define CONFIG_FEC_XCV_TYPE     RMII
340 #endif
341 #define CONFIG_ETHPRIME         "FEC"
342
343 #define CONFIG_PHYLIB
344 #define CONFIG_PHY_SMSC
345 #endif
```

**删除 uboot 中 74LV595 的驱动代码**

> mx6ull_alientek_emmc.c

将：

```c
 91 #define IOX_SDI IMX_GPIO_NR(5, 10)
 92 #define IOX_STCP IMX_GPIO_NR(5, 7)
 93 #define IOX_SHCP IMX_GPIO_NR(5, 11)
 94 #define IOX_OE IMX_GPIO_NR(5, 8)
```

修改为：

```c
#define ENET1_RESET IMX_GPIO_NR(5, 7)
#define ENET2_RESET IMX_GPIO_NR(5, 8)
```

------

删除掉如下内容：

```c
  94 static iomux_v3_cfg_t const iox_pads[] = {
  95     /* IOX_SDI */
  96     MX6_PAD_BOOT_MODE0__GPIO5_IO10 | MUX_PAD_CTRL(NO_PAD_CTRL),
  97     /* IOX_SHCP */
  98     MX6_PAD_BOOT_MODE1__GPIO5_IO11 | MUX_PAD_CTRL(NO_PAD_CTRL),
  99     /* IOX_STCP */
 100     MX6_PAD_SNVS_TAMPER7__GPIO5_IO07 | MUX_PAD_CTRL(NO_PAD_CTRL),
 101     /* IOX_nOE */
 102     MX6_PAD_SNVS_TAMPER8__GPIO5_IO08 | MUX_PAD_CTRL(NO_PAD_CTRL),
 103 };
```

删除掉static void iox74lv_init(void)函数和void iox74lv_set(int index)函数

找到 board_init 函数删除掉如下内容：

```c
imx_iomux_v3_setup_multiple_pads(iox_pads, ARRAY_SIZE(iox_pads));
iox74lv_init();
```

**添加 I.MX6U-ALPHA 开发板网络复位引脚驱动**

> mx6ull_alientek_emmc.c

找到数组并在末尾添加如下内容

```c
static iomux_v3_cfg_t const fec1_pads[] = {
	......
	MX6_PAD_SNVS_TAMPER7__GPIO5_IO07 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static iomux_v3_cfg_t const fec2_pads[] = {
	......
	MX6_PAD_SNVS_TAMPER8__GPIO5_IO08 | MUX_PAD_CTRL(NO_PAD_CTRL),
};
```

找到函数 setup_iomux_fec修改为如下内容：

```c
static void setup_iomux_fec(int fec_id)
{
	if (fec_id == 0)
	{
		imx_iomux_v3_setup_multiple_pads(fec1_pads,ARRAY_SIZE(fec1_pads));

		gpio_direction_output(ENET1_RESET, 1);
		gpio_set_value(ENET1_RESET, 0);
		mdelay(20);
		gpio_set_value(ENET1_RESET, 1);
	}
	else
	{
		imx_iomux_v3_setup_multiple_pads(fec2_pads,ARRAY_SIZE(fec2_pads));
        
		gpio_direction_output(ENET2_RESET, 1);
		gpio_set_value(ENET2_RESET, 0);
		mdelay(20);
		gpio_set_value(ENET2_RESET, 1);
	}
}
```

**修改 drivers/net/phy/phy.c 文件中的函数 genphy_update_link**

```c
int genphy_update_link(struct phy_device *phydev)
{
    unsigned int mii_reg;
	
    #ifdef CONFIG_PHY_SMSC
	static int lan8720_flag = 0;
	int bmcr_reg = 0;
	if (lan8720_flag == 0) {
		bmcr_reg = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR); 
		phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR, BMCR_RESET); 
		while(phy_read(phydev, MDIO_DEVAD_NONE, MII_BMCR) & 0X8000) {
		udelay(100); 
	}
		phy_write(phydev, MDIO_DEVAD_NONE, MII_BMCR, bmcr_reg); 
		lan8720_flag = 1;
	}
	#endif
	......
	return 0
}
```



------

**其他需要修改的地方**

> mx6ull_alientek_emmc.c

找到函数checkboard，将其改为如下所示内容：

```c
int checkboard(void) {
	if (is_mx6ull_9x9_evk())
		puts("Board: MX6ULL 9x9 EVK\n");
	else
 		puts("Board: MX6ULL ALIENTEK EMMC\n");
 	return 0; 
}
```

在 uboot 中使用网络之前要先设置几个环境变量

```
setenv ipaddr 192.168.101.50     //开发板 IP 地址
setenv ethaddr b8:ae:1d:01:00:00 //开发板网卡 MAC 地址
setenv gatewayip 192.168.101.1   //开发板默认网关
setenv netmask 255.255.255.0     //开发板子网掩码
setenv serverip 192.168.101.6    //服务器地址，也就是 Ubuntu 地址
saveenv //保存环境变量
```

# 六、启动Linux系统

**从 EMMC 启动 Linux 系统**

先检查一下 EMMC 的分区 1 中有没有zImage 文件和设备树文件，输入命令“ls mmc 1:1”

```
setenv bootargs 'console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw'
setenv bootcmd 'mmc dev 1; fatload mmc 1:1 80800000 zImage; fatload mmc 1:1 83000000 imx6ull-alientek-emmc.dtb; bootz 80800000 - 83000000;
saveenv
```

**从网络启动 Linux 系统**

```
setenv bootargs 'console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw'
setenv bootcmd 'tftp 80800000 zImage; tftp 83000000 imx6ull-alientek-emmc.dtb; bootz 80800000 - 83000000'
saveenv
```

如果是挂载nfs的根文件系统：

```
setenv bootargs 'console=ttymxc0,115200 root=/dev/nfs nfsroot=服务器IP:/home/yangmou/linux/nfs/rootfs,proto=tcp rw ip=客户端IP:服务端IP:192.168.101.1:255.255.255.0::eth0:off'
```



```
setenv bootargs 'console=ttymxc0,115200 root=/dev/nfs nfsroot=192.168.101.8:/home/yangmou/linux/nfs/rootfs,proto=tcp rw ip=192.168.101.50:192.168.101.8:192.168.101.1:255.255.255.0::eth0:off'
```

