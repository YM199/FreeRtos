#ifndef __APP_H__
#define __APP_H__


#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"

#define KEY0VALUE  0XF0
#define INVAKEY    0X00


#define DEBUG

#ifdef DEBUG 
    #define debug(...) printf(__VA_ARGS__)
#else 
    #define debug(...)
#endif

#endif /*__APP_H__*/