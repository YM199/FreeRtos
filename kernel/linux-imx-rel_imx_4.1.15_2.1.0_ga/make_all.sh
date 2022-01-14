#!/bin/sh
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- distclean
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- imx_alientek_emmc_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- all -j5
cp arch/arm/boot/zImage /home/yangmou/linux/tftpboot/
cp arch/arm/boot/dts/imx6ull-alientek-emmc.dtb /home/yangmou/linux/tftpboot/
