#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/fb.h>
#include <linux/clk.h>
#include <video/display_timing.h>
#include <video/of_display_timing.h>
#include <linux/gpio/consumer.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>

#include "imx6ull_elcdif.h"

#define RED 	0
#define GREEN 	1
#define BLUE 	2
#define TRANSP 	3


static struct fb_info *lcdfb_info;

static struct clk *clk_pix;
static struct clk *clk_axi;

static u32 pseudo_palette[16];

static struct imx6ull_elcdif *elcdif;

static struct fb_ops lcdfb_ops = {
	.owner		= THIS_MODULE,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
};

/**
 * @brief 使能
 * 
 * @param elcdif 
 */
static void imx6ull_elcdif_enable(struct imx6ull_elcdif *elcdif)
{
	elcdif->CTRL |= (1<<0);
}

static void imx6ull_elcdif_disable(struct imx6ull_elcdif *elcdif)
{
	elcdif->CTRL &= ~(1<<0);
}

/**
 * @brief 初始化硬件
 * 
 * @param elcdif 
 * @param info 
 * @param dt 
 * @param bus_width 
 * @return int 
 */
static int imx6ull_elcdif_init(struct imx6ull_elcdif *elcdif, struct fb_info *info, struct display_timing  *dt, unsigned int bus_width)
{
	unsigned int input_data_format;
	unsigned int data_bus_width;
	unsigned int hsync_active = 0;
	unsigned int vsync_active = 0;
	unsigned int de_active = 0;
	unsigned int pixelclk_active = 0;

	
	/* elcdif正常运行bit31、bit30  必须设置为0         */
	elcdif->CTRL &= ~((1 << 31) | (1 << 30)); 

	if(info->var.bits_per_pixel == 16)
		input_data_format = 0x0;
	else if (info->var.bits_per_pixel == 8)
		input_data_format = 0x1;
	else if (info->var.bits_per_pixel == 18)
		input_data_format = 0x2;
	else if (info->var.bits_per_pixel == 24)
		input_data_format = 0x3;
	else
	{
		printk(KERN_EMERG"Don't support %d bpp\n", info->var.bits_per_pixel);
		return -1;
	}

	if (bus_width == 16)
		data_bus_width = 0x0;
	else if (bus_width == 8)
		data_bus_width = 0x01;
	else if (bus_width == 18)
		data_bus_width = 0x02;
	else if (bus_width == 24)
		data_bus_width = 0x03;
	else
	{
		printk(KERN_EMERG"Don't support %d bit data bus mode\n", info->var.bits_per_pixel);
		return -1;		
	}

	/* 设置RGB格式和数据宽度 */
	elcdif->CTRL &= ~((0x03 << 8) | (0x03 << 10));
	elcdif->CTRL |= ((input_data_format << 8) | (data_bus_width << 10));

    elcdif->CTRL |= (1 << 17); /* 选择 RGB 模式 */
    elcdif->CTRL |= (1 << 19); /* 选择 RGB 模式 开启显示 */
    elcdif->CTRL |= (1 << 5);  /* 设置elcdf接口为主模式 */

	if (info->var.bits_per_pixel == 24 || info->var.bits_per_pixel == 32)
	{
		/* 设置32位有效位的低24位有效 */
		elcdif->CTRL1 &= ~(0xf << 16);
		elcdif->CTRL1 |= (0x7 << 16);
	}
	else
	{
		elcdif->CTRL1 |= (0xf << 16);		
	}

	/* 设置LCD分辨率 */
	elcdif->TRANSFER_COUNT &= ~0xffffffff;
	elcdif->TRANSFER_COUNT |= (dt->vactive.typ << 16); /* 设置LCD垂直分辨率 */
	elcdif->TRANSFER_COUNT |= (dt->hactive.typ << 0); /* 设置LCD垂直分辨率 */

    elcdif->VDCTRL0 |= (1 << 28);  /* 生成使能信号 */
    elcdif->VDCTRL0 |= (1 << 21);  /* 设置VSYNC周期 的单位为显示时钟的时钟周期 */
    elcdif->VDCTRL0 |= (1 << 20);  /* 设置VSYNC 脉冲宽度的单位为显示时钟的时钟周期 */

	if (dt->flags & DISPLAY_FLAGS_DE_HIGH)
		de_active = 1;
	if (dt->flags & DISPLAY_FLAGS_PIXDATA_POSEDGE)
		pixelclk_active = 1;
	if (dt->flags & DISPLAY_FLAGS_HSYNC_HIGH)
		hsync_active = 1;
	if (dt->flags & DISPLAY_FLAGS_VSYNC_HIGH)
		vsync_active = 1;

	/* 设置信号极性 */
	elcdif->VDCTRL0 &= ~(0xf << 24);            /* bit24~bit27清零 */
	elcdif->VDCTRL0 |= (de_active << 24);       /* 设置数据使能信号的有效电平 */
	elcdif->VDCTRL0 |= (pixelclk_active << 25); /* 设置时钟信号极性 */
	elcdif->VDCTRL0 |= (hsync_active << 26);    /* 设置HSYNC有效电平 */
	elcdif->VDCTRL0 |= (vsync_active << 27);    /* 设置VSYNC有效电平 */

	elcdif->VDCTRL0 |= (dt->vsync_len.typ << 0);/* 设置vysnc脉冲宽度 */

	/* 设置VSYNC信号周期 */
	elcdif->VDCTRL1 = dt->vsync_len.typ + dt->vactive.typ + dt->vfront_porch.typ + dt->vback_porch.typ;

	elcdif->VDCTRL2 |= (dt->hsync_len.typ << 18); /* 设置hysnc脉冲宽度 */
	/*设置HSYNC信号周期 */
	elcdif->VDCTRL2 |= (dt->hfront_porch.typ + dt->hback_porch.typ + dt->hactive.typ + dt->hsync_len.typ); 

    elcdif->VDCTRL3 |= ((dt->hback_porch.typ + dt->hsync_len.typ) << 16);
    elcdif->VDCTRL3 |= (dt->vback_porch.typ + dt->vsync_len.typ);


	elcdif->VDCTRL4 |= (1 << 18);
	elcdif->VDCTRL4 |= (dt->hactive.typ << 0);
	
    elcdif->CUR_BUF  = info->fix.smem_start;
    elcdif->NEXT_BUF = info->fix.smem_start;

	return 0;
}

/**
 * @brief device和driver匹配成功调用该函数
 * 
 * @param pdev 平台设备 
 * @return int 
 */
static int lcd_driver_probe(struct platform_device *pdev)
{
	struct device_node *display_np;
	struct display_timings *timings = NULL;
	struct display_timing  *dt = NULL;
	struct resource *res = NULL;
	unsigned int bits_per_pixel; /*像素位深*/
	unsigned int bus_width; /*总线宽度*/

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0); /*获取到寄存器的基地址*/
	elcdif = devm_ioremap_resource(&pdev->dev, res); /*将基地址转换为虚拟地址*/
	printk(KERN_EMERG"res：%x\n",res);
	printk(KERN_EMERG"elcdif：%x\n",elcdif);

	display_np = of_parse_phandle(pdev->dev.of_node, "display", 0); /*获取display0节点*/
	timings = of_get_display_timings(display_np); /*解析来自 device_node 的所有 display_timing 条目*/
	dt = timings->timings[timings->native_mode];

	of_property_read_u32(display_np, "bits-per-pixel", &bits_per_pixel); /*读取节点中的 32 位整数的值*/
	if (bits_per_pixel != 16)
	{
		printk(KERN_EMERG"not support %d bpp!\n", bits_per_pixel);
		return -1;
	}
	
	of_property_read_u32(display_np, "bus-width", &bus_width); /*读取节点中的 32 位整数的值*/
	
	clk_pix = devm_clk_get(&pdev->dev, "pix"); /*获取pxi时钟*/
	clk_axi = devm_clk_get(&pdev->dev, "axi"); /*获取axi时钟*/

	clk_set_rate(clk_pix, dt->pixelclock.typ); /*设置时钟的速率*/
	
	/*使能时钟*/
	clk_prepare_enable(clk_pix);
	clk_prepare_enable(clk_axi);

	lcdfb_info = framebuffer_alloc(0, &pdev->dev);/* 分配一个fb_info结构体 */

	/* LCD屏幕参数设置 */
	/********可见解析度（实际屏幕）********/
	lcdfb_info->var.xres   = dt->hactive.typ; /*x轴像素点数量*/
	lcdfb_info->var.yres   = dt->vactive.typ; /*y轴像素点数量*/
	/********虚拟解析度（虚拟屏幕）********/
	lcdfb_info->var.xres_virtual = dt->hactive.typ;/*x轴像素点数量*/
	lcdfb_info->var.yres_virtual = dt->vactive.typ;/*y轴像素点数量*/

	lcdfb_info->var.width  = dt->hactive.typ; /*宽度*/
	lcdfb_info->var.height = dt->vactive.typ; /*高度*/
	lcdfb_info->var.bits_per_pixel = bits_per_pixel;/*像素点位深*/

    /* LCD信号时序设置 */
	lcdfb_info->var.pixclock  = dt->pixelclock.typ; /*像素时钟*/
	lcdfb_info->var.left_margin	 = dt->hback_porch.typ;
	lcdfb_info->var.right_margin = dt->hfront_porch.typ;
	lcdfb_info->var.upper_margin = dt->vback_porch.typ;
	lcdfb_info->var.lower_margin = dt->vfront_porch.typ;
	lcdfb_info->var.vsync_len = dt->vsync_len.typ;
	lcdfb_info->var.hsync_len = dt->hsync_len.typ;

	/* LCD RGB格式设置, 这里使用的是RGB565 */		
	lcdfb_info->var.red.offset   = 11;
	lcdfb_info->var.red.length   = 5;
	lcdfb_info->var.green.offset = 5;
	lcdfb_info->var.green.length = 6;
	lcdfb_info->var.blue.offset  = 0;
	lcdfb_info->var.blue.length  = 5;		
	
	
	/* 设置固定参数 */
	strcpy(lcdfb_info->fix.id, "fire,lcd");
	lcdfb_info->fix.type   = FB_TYPE_PACKED_PIXELS;
	lcdfb_info->fix.visual = FB_VISUAL_TRUECOLOR;
	lcdfb_info->fix.line_length = dt->hactive.typ * bits_per_pixel / 8;                   
	lcdfb_info->fix.smem_len	= dt->hactive.typ * dt->vactive.typ * bits_per_pixel / 8;  /*存放一帧图片所需要的内存大小（单位：字节）*/

	/* 其他参数设置 */
	lcdfb_info->screen_size = dt->hactive.typ * dt->vactive.typ * bits_per_pixel / 8;

	/* dma_alloc_writecombine：分配smem_len大小的内存，返回screen_base虚拟地址，对应的物理地址保存在smem_start */
	lcdfb_info->screen_base = dma_alloc_writecombine(&pdev->dev, lcdfb_info->fix.smem_len, (dma_addr_t*)&lcdfb_info->fix.smem_start, GFP_KERNEL);
	lcdfb_info->pseudo_palette = pseudo_palette;
	lcdfb_info->fbops = &lcdfb_ops;

    /* elcdif控制器硬件初始化 */	
	imx6ull_elcdif_init(elcdif, lcdfb_info, dt, bus_width);
	imx6ull_elcdif_enable(elcdif);

	/* 注册fb_info结构体 */
	// register_framebuffer(lcdfb_info);

	printk(KERN_EMERG"match success!\n");
	return 0;
}

/**
 * @brief 注销fb_info结构体
 * 
 * @param pdev 平台设备
 * @return int 0
 */
static int lcd_driver_remove(struct platform_device *pdev)
{
	// if (unregister_framebuffer(lcdfb_info))
	// {
	// 	printk(KERN_EMERG"Framebuffer is in use!!!!!!!!\n");
	// }
	imx6ull_elcdif_disable(elcdif);
	framebuffer_release(lcdfb_info); /*释放lcdfb_info结构体*/
	dma_free_writecombine(&pdev->dev, lcdfb_info->fix.smem_len, lcdfb_info->screen_base, (dma_addr_t)&lcdfb_info->fix.smem_start);/*这个一定要最后释放，否者会引用到无效的内存空间*/
	printk(KERN_EMERG"module exit!\n");
	return 0;
}


/**
 * @brief 设备树匹配列表，对于一个驱动匹配多个设备的情况，我们使用struct of_device_id tbl[]来表示
 * 
 */
static struct of_device_id	lcd_of_match[] = {
	{.compatible = "fire,lcd_drv",},
	{},
};

/**
 * @brief 平台驱动结构体
 * 
 */
static struct platform_driver lcd_driver = {
	.probe  = lcd_driver_probe,
	.remove = lcd_driver_remove,
	.driver = {
		.name = "lcd_drv", /*设备驱动程序的名称*/
		.of_match_table = lcd_of_match, /*对于使用设备树编码的设备信息，我们使用device_driver中的of_match_table来匹配*/
	},
};

/**
 * @brief 驱动入口函数
 * 
 * @return int 成功：0 失败：负数
 * 
 */
static int __init lcd_driver_init(void)
{
	return platform_driver_register(&lcd_driver);
}

/**
 * @brief 驱动出口函数
 * 
 */
static void __exit lcd_driver_exit(void)
{
	platform_driver_unregister(&lcd_driver);
}

module_init(lcd_driver_init);
module_exit(lcd_driver_exit);
MODULE_LICENSE("GPL");

