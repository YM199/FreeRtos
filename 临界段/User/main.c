/*
* 注意事项：1、该工程使用软件仿真，debug需选择 Ude Simulator
*           2、在Target选项卡里面把晶振Xtal(Mhz)的值改为25，默认是12，
*              改成25是为了跟system_ARMCM3.c中定义的__SYSTEM_CLOCK相同，确保仿真的时候时钟一致
*/
#include "FreeRTOS.h"
#include "task.h"


/*定义任务栈*/
#define TASK1_STACK_SIZE   128
StackType_t Task1Stack[TASK1_STACK_SIZE];
#define TASK2_STACK_SIZE   128
StackType_t Task2Stack[TASK2_STACK_SIZE];

/*定义任务控制块*/
TaskHandle_t Task1_Handle;
TaskHandle_t Task2_Handle;



portCHAR flag1;
portCHAR flag2;

extern List_t pxReadyTasksLists[configMAX_PRIORITIES];

/*定义任务控制块*/
TCB_t Task1TCB;
TCB_t Task2TCB;


/**
 * @brief 软件延时
*/
static void delay (uint32_t count)
{
	for (; count != 0; count--);
}

/**
 * @brief 任务1
*/
static void Task1_Entry( void *p_arg )
{
	for ( ;; )
	{
		flag1 = 1;
		delay( 100 );
		flag1 = 0;
		delay( 100 );
		
		/*任务切换，这里是手动切换*/
		taskYIELD();
	}
}

/**
 * @brief 任务2
*/
static void Task2_Entry( void *p_arg )
{
	for ( ;; )
	{
		flag2 = 1;
		delay( 100 );
		flag2 = 0;
		delay( 100 );
		
		/*任务切换，这里是手动切换*/
		taskYIELD();
	}
}

int main(void)
{
	prvInitialiseTaskLists();
	/*任务句柄*/
	Task1_Handle = xTaskCreateStatic((TaskFunction_t)Task1_Entry,
	                                 (char *)"Task1",
									 (uint32_t) TASK1_STACK_SIZE,
									 (void *) NULL,
								     (StackType_t *)Task1Stack,
									 (TCB_t *)&Task1TCB);
	
	vListInsertEnd(&(pxReadyTasksLists[1]),&( ((TCB_t *)(&Task1TCB))->xStateListItem));
									 
	Task2_Handle = xTaskCreateStatic((TaskFunction_t)Task2_Entry,
									 (char *)"Task2",
									 (uint32_t)TASK2_STACK_SIZE,
									 (void *)NULL,
									 (StackType_t *) Task2Stack,
									 (TCB_t *)&Task2TCB);
									 
	vListInsertEnd(&(pxReadyTasksLists[2]),&( ((TCB_t *)(&Task2TCB))->xStateListItem));								 

	/*启动调度器，开始多任务调度，启动成功则不返回*/
	vTaskStartScheduler();
									 
	for(;;){}
}
