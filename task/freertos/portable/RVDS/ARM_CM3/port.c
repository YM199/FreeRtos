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
	/*����������ֹͣ*/
	for(;;);
}

/**
 * @brief   ��ʼ�������ջ
 * @param   pxTopOfStack
 *          pxCode
 *          pvParameters    �����β� 
 * @return  ջ��ָ��
*/
StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack,
                                   TaskFunction_t pxCode,
								   void *pvParameters
								  )
{
	/*���ջ����ļĴ���������ģ���ʱ��Ҫ�ŵ�ʵ�ʼĴ�������*/
	pxTopOfStack--;
	*pxTopOfStack = portINITIAL_XPSR;//??? Ϊʲô��0x1000 0000L
	pxTopOfStack--;
	*pxTopOfStack = ((StackType_t)pxCode) & portSTART_ADDRESS_MASK;//R15 ??? Ϊʲô�������ڵ�ַ����ô��
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t)prvTaskExitError;//R14 ����ͨ���ǲ��᷵�ص�
	pxTopOfStack -= 5;/*R12 R3 R2 R1Ĭ�ϳ�ʼ��Ϊ0*/
	*pxTopOfStack = (StackType_t) pvParameters;//R0
	
	pxTopOfStack -= 8;/*R11~R4�ļĴ���Ĭ��Ϊ0*/
	
	/*����ջ��ָ�룬��ʱpxTopOfStackָ�����ջ*/
	return pxTopOfStack;//pxTopOfStackָ��R4
}


BaseType_t xPortStarSheduler(void)
{
	/*����PendSV��SysTick���ж����ȼ�Ϊ���*/
	portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
	portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;
	
	/*������һ�����񣬲��ٷ���*/
	prvStartFirstTask();
	return 0;
}



/**
 * @brief   ����MSP������SVCϵͳ����
*/
__asm void prvStartFirstTask( void )
{
	PRESERVE8             /*8�ֽڶ���*/
	 
	ldr r0,=0xE00ED08
	ldr r0, [r0] 	
	ldr r0, [r0]
	msr msp, r0/*MSP=0x2000_08DB,����ջ��ջ��ָ��*/
	
	/*ʹ��ȫ���ж�*/
	cpsie i
	cpsie f
	dsb
	isb
	
	/*����SVCȥ������һ������*/
	svc 0
	nop
	nop
}

__asm void vPortSVCHandler(void)
{
	extern pxCurrentTCB;/*�������л��߼������е������������ƿ�*/
	
	PRESERVE8
	
	ldr r3, = pxCurrentTCB;
	ldr r1, [r3]
	ldr r0, [r1]
	/*r0��ʱ����ջ��ָ�룬����ջ��pxPortInitialiseStack�Ѿ�����ʼ������*/
	ldmia r0!, {r4-r11}    /*��r4��r11������ջ*/
	msr psp, r0
	isb
	mov r0, #0
	msr basepri, r0
	orr r14, #0xd
	
	bx r14
}


/**
 * @brief  PendSV�жϷ�����
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
