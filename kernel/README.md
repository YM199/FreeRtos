# 一、修改顶层 Makefile

```makefile
ARCH ?= arm
CROSS_COMPILE ?= arm-linux-gnueabihf-
```

编译：

```
#!/bin/sh
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- distclean
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf  imx_alientek_emmc_defconfig
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

## 2.3、CPU 主频修改

CPU频率默认就处于792MHz，不用更改

## 2.4、使能 8 线 EMMC 驱动

>  imx6ull-alientek-emmc.dts

直接替换掉usdhc2

```
&usdhc2 {
	pinctrl-names = "default", "state_100mhz", "state_200mhz";
	pinctrl-0 = <&pinctrl_usdhc2_8bit>;
	pinctrl-1 = <&pinctrl_usdhc2_8bit_100mhz>;
	pinctrl-2 = <&pinctrl_usdhc2_8bit_200mhz>;
	bus-width = <8>;
	non-removable;
	status = "okay";
};
```

## 2.5、修改网络驱动

> imx6ull-alientek-emmc.dts

**修改 LAN8720 的复位以及网络时钟引脚驱动**

找到：

```
pinctrl_spi4: spi4grp {
			fsl,pins = <
				MX6ULL_PAD_BOOT_MODE0__GPIO5_IO10        0x70a1
				MX6ULL_PAD_BOOT_MODE1__GPIO5_IO11        0x70a1
				MX6ULL_PAD_SNVS_TAMPER7__GPIO5_IO07      0x70a1
				MX6ULL_PAD_SNVS_TAMPER8__GPIO5_IO08      0x80000000
		>;
};

```

删除掉：

	MX6ULL_PAD_SNVS_TAMPER7__GPIO5_IO07      0x70a1
	MX6ULL_PAD_SNVS_TAMPER8__GPIO5_IO08      0x80000000

------

找到：

```
spi4 {
	......
	pinctrl-assert-gpios = <&gpio5 8 GPIO_ACTIVE_LOW>;
	......
	cs-gpios = <&gpio5 7 0>;
};
```

删除掉

	pinctrl-assert-gpios = <&gpio5 8 GPIO_ACTIVE_LOW>;
	cs-gpios = <&gpio5 7 0>;

------

找到：

```
&iomuxc_snvs { 
	pinctrl-names = "default_snvs";
		pinctrl-0 = <&pinctrl_hog_2>;
		imx6ul-evk {
			...... /*省略掉其他*/
			待插入
		};
};
```

插入：

```
&iomuxc_snvs { 
	pinctrl-names = "default_snvs";
		pinctrl-0 = <&pinctrl_hog_2>;
		imx6ul-evk {
			...... /*省略掉其他*/
			/*enet1 reset zuozhongkai*/
			pinctrl_enet1_reset: enet1resetgrp {
				fsl,pins = <
					/* used for enet1 reset */
					MX6ULL_PAD_SNVS_TAMPER7__GPIO5_IO07 0x10B0 
				>;
			};

			/*enet2 reset zuozhongkai*/
			pinctrl_enet2_reset: enet2resetgrp {
				fsl,pins = <
					/* used for enet2 reset */
					MX6ULL_PAD_SNVS_TAMPER8__GPIO5_IO08 0x10B0 
				>;
			};
		};
};
```

------

找到：

```
pinctrl_enet1: enet1grp {
	fsl,pins = <
		......
		MX6UL_PAD_ENET1_TX_CLK__ENET1_REF_CLK1  0x4001b031
	>;
};

pinctrl_enet2: enet2grp {
	fsl,pins = <
		......
		MX6UL_PAD_ENET2_TX_CLK__ENET2_REF_CLK2  0x4001b031
		>;
};
```

修改为：

```
pinctrl_enet1: enet1grp {
	fsl,pins = <
		......
		MX6UL_PAD_ENET1_TX_CLK__ENET1_REF_CLK1  0x4001b009
	>;
};

pinctrl_enet2: enet2grp {
	fsl,pins = <
		......
		MX6UL_PAD_ENET2_TX_CLK__ENET2_REF_CLK2  0x4001b009
		>;
};
```

> 只修改了配置参数

**修改 fec1 和 fec2 节点的 pinctrl-0 属性**

找到：

```
&fec1 {
    ......
    pinctrl-0 = <&pinctrl_enet1>;
	......
};

&fec2 {
    ......
    pinctrl-0 = <&pinctrl_enet2>;
	......
};
```

改为：

```
&fec1 {
    ......
    pinctrl-0 = <&pinctrl_enet1 &pinctrl_enet1_reset>;
	......
};

&fec2 {
    ......
    pinctrl-0 = <&pinctrl_enet2 &pinctrl_enet2_reset>;
	......
};
```

**修改 LAN8720A 的 PHY 地址**

找到：

```
&fec1 {
	......
    phy-mode = "rmii";
    phy-handle = <&ethphy0>;
    status = "okay";
};

&fec2 {
	......
    phy-mode = "rmii";
    phy-handle = <&ethphy1>;
    status = "okay";

    mdio {
 		......
        ethphy0: ethernet-phy@2 {
            compatible = "ethernet-phy-ieee802.3-c22";
            reg = <2>;
        };

        ethphy1: ethernet-phy@1 {
            compatible = "ethernet-phy-ieee802.3-c22";
            reg = <1>;
        };
    };
};
```

修改为：

> new为新添加的

```
&fec1 {
	......
    phy-mode = "rmii";
    phy-handle = <&ethphy0>;
    
new:   phy-reset-gpios = <&gpio5 7 GPIO_ACTIVE_LOW>;
new:   phy-reset-duration = <200>;
    
    status = "okay";
};

&fec2 {
	......
    phy-mode = "rmii";
    phy-handle = <&ethphy1>;
    
new:   phy-reset-gpios = <&gpio5 8 GPIO_ACTIVE_LOW>;
new:   phy-reset-duration = <200>;
    
    status = "okay";
    mdio {
 		......
new:    ethphy0: ethernet-phy@0 {
            compatible = "ethernet-phy-ieee802.3-c22";
new:        reg = <0>;
        };

        ethphy1: ethernet-phy@1 {
            compatible = "ethernet-phy-ieee802.3-c22";
            reg = <1>;
        };
    };
};
```

**修改 fec_main.c 文件**

> drivers/net/ethernet/freescale/fec_main.c

找到：

```c
static int
fec_probe(struct platform_device *pdev)
{
    struct fec_enet_private *fep;
    struct fec_platform_data *pdata;
    struct net_device *ndev;
    int i, irq, ret = 0;
    struct resource *r;
    const struct of_device_id *of_id;
    static int dev_id;
    struct device_node *np = pdev->dev.of_node, *phy_node;
    int num_tx_qs;
    int num_rx_qs;
    
    
	......
}
```

加入：

```c
static int
fec_probe(struct platform_device *pdev)
{
    struct fec_enet_private *fep;
    struct fec_platform_data *pdata;
    struct net_device *ndev;
    int i, irq, ret = 0;
    struct resource *r;
    const struct of_device_id *of_id;
    static int dev_id;
    struct device_node *np = pdev->dev.of_node, *phy_node;
    int num_tx_qs;
    int num_rx_qs;
    
 	/* 设置 MX6UL_PAD_ENET1_TX_CLK 和 MX6UL_PAD_ENET2_TX_CLK
	* 这两个 IO 的复用寄存器的 SION 位为 1。
	*/   
    void __iomem *IMX6U_ENET1_TX_CLK;
    void __iomem *IMX6U_ENET2_TX_CLK;
    
	IMX6U_ENET1_TX_CLK = ioremap(0X020E00DC, 4);
	writel(0X14, IMX6U_ENET1_TX_CLK);
 
	IMX6U_ENET2_TX_CLK = ioremap(0X020E00FC, 4);
	writel(0X14, IMX6U_ENET2_TX_CLK);   
    
	......
}
```

**配置 Linux 内核，使能 LAN8720 驱动**

```
-> Device Drivers 
	-> Network device support 
 		-> PHY Device support and infrastructure 
			-> Drivers for SMSC PHYs
```

选中 * 最后一个



**修改 smsc.c 文件**

> drivers/net/phy/smsc.c

修改完的内容如下：

```c
static int smsc_phy_reset(struct phy_device *phydev)
{
    int err, phy_reset;
    int msec = 1;
    struct device_node *np;
    int timeout = 50000;
    if(phydev->addr == 0) /* FEC1 */ {
		np = of_find_node_by_path("/soc/aips-bus@02100000/ethernet@02188000");
        if(np == NULL) {
            return -EINVAL;
        }
    }

    if(phydev->addr == 1) /* FEC2 */ {
        np = of_find_node_by_path("/soc/aips-bus@02000000/ethernet@020b4000");
        if(np == NULL) {
            return -EINVAL;
        }
    }

    err = of_property_read_u32(np, "phy-reset-duration", &msec);
    /* A sane reset duration should not be longer than 1s */
    if (!err && msec > 1000)
        msec = 1;
    phy_reset = of_get_named_gpio(np, "phy-reset-gpios", 0);
    if (!gpio_is_valid(phy_reset))
        return;

    gpio_direction_output(phy_reset, 0);
    gpio_set_value(phy_reset, 0);
    msleep(msec);
    gpio_set_value(phy_reset, 1);

    int rc = phy_read(phydev, MII_LAN83C185_SPECIAL_MODES);
    if (rc < 0)
        return rc;

    /* If the SMSC PHY is in power down mode, then set it
     * in all capable mode before using it.
     */
    if ((rc & MII_LAN83C185_MODE_MASK) == MII_LAN83C185_MODE_POWERDOWN) {
        int timeout = 50000;

        /* set "all capable" mode and reset the phy */
        rc |= MII_LAN83C185_MODE_ALL;
        phy_write(phydev, MII_LAN83C185_SPECIAL_MODES, rc);

        /* wait end of reset (max 500 ms) */
        do {
            udelay(10);
            if (timeout-- == 0)
                return -1;
            rc = phy_read(phydev, MII_BMCR);
        } while (rc & BMCR_RESET);
    }
    return 0;
}
```

添加两个头文件：

```c
#include <linux/of_gpio.h>
#include <linux/io.h>
```



**保存修改后的图形化配置文件**

在图形界面中会有“< Save >”选项，输入“arch/arm/configs/imx_alientek_emmc_defconfig”即可

> 这样就不怕make clean清除掉配置文件
