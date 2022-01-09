#ifndef __CHRDEV_H__
#define __CHRDEV_H__

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>

#define DEBUG

#ifdef DEBUG 
    #define debug(...) printk(__VA_ARGS__)
#else 
    #define debug(...)
#endif


#define CHRDEVBASE_MAJOR 200
#define CHRDEVBASE_NAME  "chrdevbase"


#endif /*__CHRDEV_H__*/