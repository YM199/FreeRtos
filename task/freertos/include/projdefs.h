#ifndef PROJDEFS_H
#define PROJDEFS_H

typedef void (*TaskFunction_t)   (void *);/*TaskFunction是一个void *的函数指针*/

#define pdFALSE     ((BaseType_t)0)
#define pdTRUE      ((BaseType_t)1)

#define pdPASS       (pdTRUE)
#define pdFAIL       (pdFALSE)

#endif /*PROJDEFS_H*/
