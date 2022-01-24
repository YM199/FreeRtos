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


#endif