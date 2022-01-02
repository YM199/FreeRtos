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

**LCD 驱动修改**

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

**网络驱动修改**

待修改

**其他需要修改的地方**
