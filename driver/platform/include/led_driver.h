#ifndef __LED_DRIVER_H__
#define __LED_DRIVER_H__

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
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/irq.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/platform_device.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

struct led_dev{
    dev_t              devid;  /*设备号*/
    struct cdev        cdev;   /*字符设备*/
    struct class       *class; /*类*/
    struct device      *device; /*设备*/
    int                major;  /*主设备号*/
    int                minor;  /*次设备号*/
    struct device_node *node;  /*LED设备节点*/
    int                led0;   /*LED灯的GPIO编号*/
};


#define DEV_CNT  1
#define DEV_NAME "led" /*字符设备的名字: /dev/led */
#define LED_OFF  0
#define LED_ON   1

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

#endif /*__LED_DRIVER_H__*/