#ifndef __APP_H__
#define __APP_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <linux/input.h>

/* 用于描述MT多点触摸每一个触摸点的信息 */
struct ts_mt {
    int x;      	//X坐标
    int y;      	//Y坐标
    int id;     	//对应ABS_MT_TRACKING_ID
    int valid;		//数据有效标志位(=1表示触摸点信息发生更新)
};

#endif