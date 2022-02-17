#ifndef __AP3216C_H__
#define __AP3216C_H__

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
#include <linux/i2c.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

/* AP3316C 寄存器 */
#define AP3216C_SYSTEMCONG 0x00 /* 配置寄存器 */
#define AP3216C_INTSTATUS 0X01 /* 中断状态寄存器 */
#define AP3216C_INTCLEAR 0X02 /* 中断清除寄存器 */
#define AP3216C_IRDATALOW 0x0A /* IR 数据低字节 */
#define AP3216C_IRDATAHIGH 0x0B /* IR 数据高字节 */
#define AP3216C_ALSDATALOW 0x0C /* ALS 数据低字节 */
#define AP3216C_ALSDATAHIGH 0X0D /* ALS 数据高字节 */
#define AP3216C_PSDATALOW 0X0E /* PS 数据低字节 */
#define AP3216C_PSDATAHIGH 0X0F /* PS 数据高字节 */

#define AP3216C_CNT 1
#define AP3216C_NAME "ap3216c"

struct ap3216c_dev {
    dev_t devid; /* 设备号 */
    struct cdev cdev; /* cdev */
    struct class *class; /* 类 */
    struct device *device; /* 设备 */
    struct device_node *nd; /* 设备节点 */
    int major; /* 主设备号 */
    void *private_data; /* 私有数据 */
    unsigned short ir, als, ps; /* 三个光传感器数据 */
};

#endif