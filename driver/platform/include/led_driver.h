#ifndef __LED_DRIVER_H__
#define __LED_DRIVER_H__


struct xxx_dev{
    struct cdev cdev;
};


#define DEBUG
#ifdef  DEBUG
    #define debug(condition, ret) \
    if( condition ) \
    {\
        printk( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );\
        return ret;\
    }
#else
    #define debug(...)
#endif

#endif /*__LED_DRIVER_H__*/