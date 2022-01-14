#ifndef __LED_H__
#define __LED_H__

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
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define DEBUG

#ifdef DEBUG 
    #define debug(...) printk(__VA_ARGS__)
#else 
    #define debug(...)
#endif

#define LED_CNT   1
#define LED_NAME  "led" /*设备名*/



#define LED_ON  1  /*开灯*/
#define LED_OFF 0  /*关灯*/


struct chrdev
{
    dev_t devid;            /*设备号*/
    struct cdev cdev;   
    struct class *class;
    struct device *device;
    int major;              /*主设备号*/
    int minor;              /*次设备号*/
    struct device_node *nd; /*设备节点*/
};



#define CCM_CCGR1_BASE          (0X020C406C)
#define SW_MUX_GPIO1_IO03_BASE  (0X020E0068)
#define SW_PAD_GPIO1_IO03_BASE  (0X020E02F4)
#define GPIO1_DR_BASE           (0X0209C000)
#define GPIO1_GDIR_BASE         (0X0209C004)

#endif