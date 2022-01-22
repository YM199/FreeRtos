#ifndef __TIMER_H__
#define __TIMER_H__
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
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define  TIMER_CNT  1 /*设备号个数*/
#define  TIMER_NAME "timer"

#define CLOSE_CMD (_IO(0XEF, 0X1)) /*关闭定时器*/
#define OPEN_CMD  (_IO(0XEF, 0X2)) /*打开定时器*/
#define SETPERIOD_CMD (_IO(0XEF, 0X3)) /*设置定时器周期命令*/
#define LEDON 1
#define LEDOF 0

#define DEBUG

#ifdef DEBUG 
    #define debug(...) printk(__VA_ARGS__)
#else 
    #define debug(...)
#endif


struct timer_dev
{
    dev_t devid;            /*设备号*/
    struct cdev cdev;   
    struct class *class;
    struct device *device;
    int major;              /*主设备号*/
    int minor;              /*次设备号*/
    struct device_node *nd; /*设备节点*/
    int led_gpio;           /*led使用的GPIO编号*/
    int timeperiod;         /*定时周期, ms*/
    struct timer_list timer;/*定时器*/
    spinlock_t lock;        /*定义自旋锁*/
};

#endif