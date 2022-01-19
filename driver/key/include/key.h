#ifndef __KEY_H__
#define __KEY_H__

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
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define DEBUG

#ifdef DEBUG 
    #define debug(...) printk(__VA_ARGS__)
#else 
    #define debug(...)
#endif

#define KEY_CNT   1
#define KEY_NAME  "key" /*设备名*/


#define KEY0VALUE 0XF0
#define INVAKEY   0X00




struct keydev
{
    dev_t devid;            /*设备号*/
    struct cdev cdev;   
    struct class *class;
    struct device *device;
    int major;              /*主设备号*/
    int minor;              /*次设备号*/
    struct device_node *nd; /*设备节点*/
    int key_gpio;           /*key使用的GPIO编号*/
    atomic_t keyvalue;          /*原子变量*/
};

#endif