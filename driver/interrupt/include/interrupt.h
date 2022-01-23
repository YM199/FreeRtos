#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/errno.h> 
#include <linux/gpio.h> 
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/of_irq.h>
#include <linux/irq.h>

#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>


#define IMX6UIRQ_CNT   1           /*设备号个数*/   
#define IMX6UIRQ_NAME  "imx6uirq"  /*名字*/
#define KEY0VALUE      0x01        /*KEY0按键值*/
#define INVAKEY        0xFF        /*无效的按键值*/
#define KEY_NUM        1           /*按键数量*/

#define DEBUG

#ifdef DEBUG 
    #define debug(...) printk(__VA_ARGS__)
#else 
    #define debug(...)
#endif

/*中断IO描述结构体*/
struct irq_keydesc {
    int gpio;                            /*gpio*/
    int irqnum;                          /*中断号*/
    unsigned char value;                 /*按键对应的键值*/
    char name[10];                       /*名字*/
    irqreturn_t (*handler)(int, void *); /*中断服务函数*/
};

struct imx6uirq_dev
{
    dev_t devid;            /*设备号*/
    struct cdev cdev;   
    struct class *class;
    struct device *device;
    int major;              /*主设备号*/
    int minor;              /*次设备号*/
    struct device_node *nd; /*设备节点*/
    atomic_t keyvalue;     /*有效的按键值*/
    atomic_t releasekey;    /*标记是否完成一次完成的按键*/
    
    struct timer_list timer;/*定时器*/
    struct irq_keydesc irqkeydesc[KEY_NUM];/*按键描述数组*/
    unsigned char curkeynum;/*当前的按键号*/
};

#endif