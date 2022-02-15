#ifndef __INPUT_H__
#define __INPUT_H__

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
#include <linux/input.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/of_irq.h>
#include <linux/irq.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define KEYINPUT_CNT   1            /*设备号个数*/
#define KEYINPUT_NAME  "keyinput"   /*名字*/
#define KEY0VALUE      0X01         /*key0按键值*/
#define INVAKEY        0XFF         /*无效的按键值*/

/*中断IO描述结构体*/
struct irq_keydesc
{
    int gpio;                           /*gpio*/
    int irqnum;                         /*中断号*/
    unsigned char value;                /*按键对应的键值*/
    char name[10];                      /*名字*/
    irqreturn_t (*handler)(int, void *);/*中断服务函数*/
};

struct keyinput_dev
{
    dev_t devid;                            /*设备号*/
    struct cdev cdev;                       /*cdev*/
    struct class *class;                    /*类*/
    struct device *device;                  /*设备*/
    struct device_node *nd;                 /*设备节点*/
    struct timer_list timer;                /*定义一个定时器*/
    struct irq_keydesc irqkeydesc;          /*按键描述数组*/
    unsigned char curkeynum;                /*当前的按键号*/
    struct input_dev *inputdev;             /*input 结构体*/
};

#define DEBUG
#ifdef  DEBUG
    #define debug(condition, ret) \
    if( condition ) \
    {\
        printk( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );\
        return ret;\
    }
#else
    #define debug(...)
#endif

#endif /*__INPUT_H__*/