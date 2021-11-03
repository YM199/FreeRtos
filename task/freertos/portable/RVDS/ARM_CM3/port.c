#include "FreeRTOS.h"
#include "task.h"
#include "ARMCM3.h"
#define portINITIAL_XPSR          (0x01000000)
#define portSTART_ADDRESS_MASK    ((StackType_t)0xfffffffeUL)

#define portNVIC_SYSPRI2_REG      (*(( volatile uint32_t *) 0xe000ed20))
#define portNVIC_PENDSV_PRI       (((uint32_t) configKERNEL_INTERRUPT_PRIORITY ) << 16UL)
#define portNVIC_SYSTICK_PRI      (((uint32_t) configKERNEL_INTERRUPT_PRIORITY ) << 24UL )

void prvStartFirstTask(void);

static void prvTaskExitError(void)
{
	/*函数在这里停止*/
	for(;;);
}

/**
 * @brief   初始化任务的栈
 * @param   pxTopOfStack
 *          pxCode
 *          pvParameters    任务形参 
 * @return  栈顶指针
*/
StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack,
                                   TaskFunction_t pxCode,
								   void *pvParameters
								  )
{
	/*这个栈里面的寄存器是虚拟的，到时候要放到实际寄存器里面*/
	pxTopOfStack--;
	*pxTopOfStack = portINITIAL_XPSR;//??? 为什么是0x1000 0000L
	pxTopOfStack--;
	*pxTopOfStack = ((StackType_t)pxCode) & portSTART_ADDRESS_MASK;//R15 ??? 为什么任务的入口地址是这么多
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t)prvTaskExitError;//R14 任务通常是不会返回的
	pxTopOfStack -= 5;/*R12 R3 R2 R1默认初始化为0*/
	*pxTopOfStack = (StackType_t) pvParameters;//R0
	
	pxTopOfStack -= 8;/*R11~R4的寄存器默认为0*/
	
	/*返回栈顶指针，此时pxTopOfStack指向空闲栈*/
	return pxTopOfStack;//pxTopOfStack指向R4
}


BaseType_t xPortStarSheduler(void)
{
	/*配置PendSV和SysTick的中断优先级为最低*/
	portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
	portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;
	
	/*启动第一个任务，不再返回*/
	prvStartFirstTask();
	return 0;
}



/**
 * @brief   更新MSP、产生SVC系统调用
*/
__asm void prvStartFirstTask( void )
{
	PRESERVE8             /*8字节对齐*/
	 
	ldr r0,=0xE00ED08
	ldr r0, [r0] 	
	ldr r0, [r0]
	msr msp, r0/*MSP=0x2000_08DB,主堆栈的栈顶指针*/
	
	/*使能全局中断*/
	cpsie i
	cpsie f
	dsb
	isb
	
	/*调用SVC去启动第一个任务*/
	svc 0
	nop
	nop
}

__asm void vPortSVCHandler(void)
{
	extern pxCurrentTCB;/*正在运行或者即将运行的任务的任务控制块*/
	
	PRESERVE8
	
	ldr r3, = pxCurrentTCB;
	ldr r1, [r3]
	ldr r0, [r1]
	/*r0此时等于栈顶指针，任务栈在pxPortInitialiseStack已经被初始化好了*/
	ldmia r0!, {r4-r11}    /*将r4和r11弹出堆栈*/
	msr psp, r0
	isb
	mov r0, #0
	msr basepri, r0
	orr r14, #0xd
	
	bx r14
}


/**
 * @brief  PendSV中断服务函数
*/
__asm void xPortPendSVHandler(void)
{
	extern pxCurrentTCB;
	extern vTaskSwitchContext;
	
	PRESERVE8;
	
	mrs r0, psp;
	isb;
	
	ldr r3, = pxCurrentTCB
	ldr r2, [r3]
	
	stmdb r0!, {r4-r11}
	str r0, [r2]
	
	stmdb sp!, {r3,r14}
	mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY
	msr basepri, r0
	dsb
	isb
    bl vTaskSwitchContext
	mov r0, #0
	msr basepri, r0
	ldmia sp!, {r3,r14}
    	
	ldr r1, [r3]
	ldr r0, [r1]
	ldmia r0!, {r4-r11}
	msr psp, r0
	isb
	bx r14
	nop
}
