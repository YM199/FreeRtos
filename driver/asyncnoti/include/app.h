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
#include "signal.h"

#define DEBUG

#ifdef DEBUG
    #define debug( condition, ret ) \
    if( condition ) \
    {\
        printf( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );\
        return ret;\
    }
#else
    #define debug(...)
#endif


#endif