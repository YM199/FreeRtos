#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

//该部分宏定义是自己添加的，默认没有
//#define configSUPPORT_STATIC_ALLOCATION  1//静态创建任务
#define configSUPPORT_DYNAMIC_ALLOCATION 1//动态创建任务
#define configUSE_TIMERS 1//打开软件定时器

#define configUSE_PREEMPTION		1//置 1：FreeRTOS 使用抢占式调度器；置 0：FreeRTOS 使用协作式调度器（时间片）
#define configUSE_IDLE_HOOK			0//配置空闲钩子函数
#define configUSE_TICK_HOOK			0//配置时间片钩子函数
#define configCPU_CLOCK_HZ			( ( unsigned long ) 72000000 )//设置系统时钟	
#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )//SysTick 每秒中断多少次
#define configMAX_PRIORITIES		( 5 )//最大任务优先级的宏
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 128 )//栈的大小
#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 17 * 1024 ) )//堆内存的大小，用于动态分配
#define configMAX_TASK_NAME_LEN		( 16 )//TASK_NAME的字符串长度
#define configUSE_TRACE_FACILITY	0//可视化跟踪调试
#define configUSE_16_BIT_TICKS		0//定义变量的位数
#define configIDLE_SHOULD_YIELD		1//控制任务在空闲优先级中的行为

/* 协程定义，FreeRTOS已经不对携程做支持了 */
#define configUSE_CO_ROUTINES 		0//启用携程，默认关闭（FreeRTOS已经不对携程做支持了）
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* 将以下定义设置为 1 以包含 API 函数，或设置为零排除 API 函数 */

#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1

/* 这是根据 Cortex-M3 NVIC 的原始值  值可以是 255（最低）到 0（1？）（最高） */
#define configKERNEL_INTERRUPT_PRIORITY 		255//内核中断优先级
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	191 /* equivalent to 0xb0, or priority 11. */


/* This is the value being used as per the ST library which permits 16
priority values, 0 to 15.  This must correspond to the
configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
NVIC value of 255. */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY	15

#endif /* FREERTOS_CONFIG_H */

