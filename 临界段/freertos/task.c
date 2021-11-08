#include "FreeRTOS.h"
#include "task.h"

/*��ǰ�������е������������ƿ�ָ�룬Ĭ�ϳ�ʼ��ΪNULL*/
TCB_t * volatile pxCurrentTCB = NULL;


/*��������б�*/
List_t pxReadyTasksLists[configMAX_PRIORITIES];


/**
 * @brief ��ʼ���µ�����
 * @param pxTaskCode      ����ĺ�������(�������)
 *        pcName          ���������
 *        ulStackDepth    ����ջ��С
 *        pvParameters    �����β�
 *        pxCreatedTask   ������
 *        pxNewTCB        ������ƿ�ָ��  
*/
static void prvInitialiseNewTask(TaskFunction_t pxTaskCode,
                                 const char * const pcName,
                                 const uint32_t ulStackDepth,
								 void * const pvParameters,
								 TaskHandle_t * const pxCreatedTask,
								 TCB_t *pxNewTCB
                                )
{
	StackType_t *pxTopOfStack;
	UBaseType_t x;
	
	/*��ȡջ����ַ*/
	pxTopOfStack = pxNewTCB->pxStack + (ulStackDepth - (uint32_t)1);
	/*������8�ֽڶ���*/
	pxTopOfStack = (StackType_t *) (((uint32_t)pxTopOfStack) & (~((uint32_t)0x0007)));
	/*����������ִ洢��TCB��*/
	for (x = (UBaseType_t)0; x < (UBaseType_t)configMAX_TASK_NAME_LEN; x++)
	{
		pxNewTCB->pcTaskName[x] = pcName[x];
		if (pcName[x] == 0x00)
		{
			break;
		}
	}
	/*��������ֵĳ��Ȳ��ܳ���configMAX_TASK_NAME_LEN*/
	pxNewTCB->pcTaskName[configMAX_TASK_NAME_LEN - 1] = '\0';
	/*��ʼ��TCB�е�xStateListItem�ڵ�*/
	vListInitialiseItem(&(pxNewTCB->xStateListItem));
	/*����xStateListItem�ڵ�ӵ����*/
	listSET_LIST_ITEM_OWNER( &( pxNewTCB->xStateListItem ), pxNewTCB );
	
	/*��ʼ������ջ*/
	pxNewTCB->pxTopofStack = pxPortInitialiseStack(pxTopOfStack,
												   pxTaskCode,
	                                               pvParameters
												  );
	
	/*��������ָ��������ƿ�*/
	if ((void *)pxCreatedTask != NULL)
	{
		*pxCreatedTask = (TaskHandle_t) pxNewTCB;
	}
	
}




#if( configSUPPORT_STATIC_ALLOCATION == 1 )
/**
 * @brief ��̬���񴴽�����
 * @param pxTaskCode      ����ĺ�������(�������)
 *        pcName          ���������
 *        ulStackDepth    ����ջ��С
 *        pvParameters    �����β�
 *        puxStackBuffer  ����ջ��ʼ��ַ
 *        pxTaskBuffer    ������ƿ�ָ��  
*/
TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,
                               const char * const pcName,
							   const uint32_t ulStackDepth,
							   void * const pvParameters,
							   StackType_t * const puxStackBuffer,
							   TCB_t * const pxTaskBuffer
							  )
{
	TCB_t *pxNewTCB;
	TaskHandle_t xReturn;
	
	if ((pxTaskBuffer != NULL) && (puxStackBuffer != NULL))
	{
		pxNewTCB = (TCB_t *)pxTaskBuffer;
		pxNewTCB->pxStack = (StackType_t *)puxStackBuffer;
		
		/*�����µ�����*/
		prvInitialiseNewTask( pxTaskCode,
		                      pcName,
		                      ulStackDepth,
		                      pvParameters,
		                      &xReturn,
		                      pxNewTCB
		                     );
	}
	else
	{
		xReturn = NULL;
	}
	
	/*����������ƾ����������񴴽��ɹ�����ʱxReturnӦ��ָ��������ƿ�*/
	return xReturn;
}
#endif /*configSUPPORT_STATIC_ALLOCATION*/
	
/**
 * @brief �����б��ʼ��
*/
void prvInitialiseTaskLists(void)
{
    UBaseType_t uxPriority;
	for (uxPriority = (UBaseType_t)0U; uxPriority < (UBaseType_t)configMAX_PRIORITIES; uxPriority++)
	{
		vListInitialise(&(pxReadyTasksLists[uxPriority]));
	}
}

/**
 * @brief ����������
*/
extern TCB_t Task1TCB;
extern TCB_t Task2TCB;
void vTaskStartScheduler(void)
{
	/*�ֶ�ָ����һ�����е�����*/
	pxCurrentTCB = &Task1TCB;
	/*����������*/
	if (xPortStarSheduler() != pdFALSE)
	{
		/*�����������ɹ����򲻻᷵�أ��Ȳ�����������*/
	}
	
}


void vTaskSwitchContext(void)
{
	/*�������������л�*/
	if (pxCurrentTCB == &Task1TCB)
	{
		pxCurrentTCB = &Task2TCB;
	}
	else
	{
		pxCurrentTCB = &Task1TCB;
	}
}

