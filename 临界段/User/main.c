/*
* ע�����1���ù���ʹ��������棬debug��ѡ�� Ude Simulator
*           2����Targetѡ�����Ѿ���Xtal(Mhz)��ֵ��Ϊ25��Ĭ����12��
*              �ĳ�25��Ϊ�˸�system_ARMCM3.c�ж����__SYSTEM_CLOCK��ͬ��ȷ�������ʱ��ʱ��һ��
*/
#include "FreeRTOS.h"
#include "task.h"


/*��������ջ*/
#define TASK1_STACK_SIZE   128
StackType_t Task1Stack[TASK1_STACK_SIZE];
#define TASK2_STACK_SIZE   128
StackType_t Task2Stack[TASK2_STACK_SIZE];

/*����������ƿ�*/
TaskHandle_t Task1_Handle;
TaskHandle_t Task2_Handle;



portCHAR flag1;
portCHAR flag2;

extern List_t pxReadyTasksLists[configMAX_PRIORITIES];

/*����������ƿ�*/
TCB_t Task1TCB;
TCB_t Task2TCB;


/**
 * @brief �����ʱ
*/
static void delay (uint32_t count)
{
	for (; count != 0; count--);
}

/**
 * @brief ����1
*/
static void Task1_Entry( void *p_arg )
{
	for ( ;; )
	{
		flag1 = 1;
		delay( 100 );
		flag1 = 0;
		delay( 100 );
		
		/*�����л����������ֶ��л�*/
		taskYIELD();
	}
}

/**
 * @brief ����2
*/
static void Task2_Entry( void *p_arg )
{
	for ( ;; )
	{
		flag2 = 1;
		delay( 100 );
		flag2 = 0;
		delay( 100 );
		
		/*�����л����������ֶ��л�*/
		taskYIELD();
	}
}

int main(void)
{
	prvInitialiseTaskLists();
	/*������*/
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

	/*��������������ʼ��������ȣ������ɹ��򲻷���*/
	vTaskStartScheduler();
									 
	for(;;){}
}
