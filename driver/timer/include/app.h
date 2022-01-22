#ifndef __APP_H__
#define __APP_H__

#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"
#include "linux/ioctl.h"

#define DEBUG

#ifdef DEBUG 
    #define debug(...) printf(__VA_ARGS__)
#else 
    #define debug(...)
#endif

#define CLOSE_CMD     (_IO(0XEF, 0X01)) /*关闭定时器*/
#define OPEN_CMD      (_IO(0xEF, 0X02)) /*打开定时器*/
#define SETPERIOD_CMD (_IO(0XEF, 0X03)) /*设置定时器周期命令*/



#endif