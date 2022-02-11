#ifndef __MISC_H__
#define __MISC_H__

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
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

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

#define MISCBEEP_NAME   "miscbeep" /*名字*/
#define MISCBEEP_MINOR  144        /*次设备号*/
#define BEEPOFF        0          /*关蜂鸣器*/
#define BEEPON         1          /*打开蜂鸣器*/

/*miscbeep设备结构体*/
struct miscbeep_dev
{
    dev_t devid;             /*设备号*/
    struct cdev cdev;        /*字符设备*/
    struct class *class;     /*类*/
    struct device *device;   /*设备*/
    struct device_node *nd;  /*设备节点*/
    int beep_gpio;           /*beep所使用的GPIO编号*/
};

#endif /*__MISC_H__*/