# 一、修改顶层 Makefile

```makefile
ARCH ?= arm
CROSS_COMPILE ?= arm-linux-gnueabihf-
```

编译：

```
#!/bin/sh
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- distclean
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf imx_alientek_emmc_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- all -j5
```

- 会在 arch/arm/boot 目录下生成 zImage 镜像文件
- 会在 arch/arm/boot/dts 目录下开发板对应的.dtb(设备树)文件

# 二、在 Linux 中添加自己的开发板

## 2.1、添加开发板默认配置文件

```bash
cd arch/arm/configs
cp imx_v7_mfg_defconfig imx_alientek_emmc_defconfig
```

## 2.2、添加开发板对应的设备树文件

```bash
cd arch/arm/boot/dts
cp imx6ull-14x14-evk.dts imx6ull-alientek-emmc.dts
```

修改文件 arch/arm/boot/dts/Makefile，找 到 “ dtb-$(CONFIG_SOC_IMX6ULL)”配置项，在此配置项末尾中加入“imx6ull-alientek-emmc.dtb”

```makefile
dtb-$(CONFIG_SOC_IMX6ULL) += \
	......
	imx6ull-alientek-emmc.dtb
```

