/*
* ע�����1���ù���ʹ��������棬debug��ѡ�� Ude Simulator
*           2����Targetѡ�����Ѿ���Xtal(Mhz)��ֵ��Ϊ25��Ĭ����12��
*              �ĳ�25��Ϊ�˸�system_ARMCM3.c�ж����__SYSTEM_CLOCK��ͬ��ȷ�������ʱ��ʱ��һ��
*/

#include "list.h"

/*������ڵ�*/
static struct xLIST List_Test;
/*����ڵ�*/
static struct xLIST_ITEM List_Item1;
static struct xLIST_ITEM List_Item2;
static struct xLIST_ITEM List_Item3;
int main(void)
{
	/* ������ڵ��ʼ�� */
    vListInitialise( &List_Test );
	
	/* �ڵ�1��ʼ�� */
	vListInitialiseItem( &List_Item1 );
	List_Item1.xItemValue = 1;
	
	/* �ڵ�2��ʼ�� */
	vListInitialiseItem( &List_Item2 );
	List_Item2.xItemValue = 2;	

	/* �ڵ�3��ʼ�� */
	vListInitialiseItem( &List_Item3 );
	List_Item3.xItemValue = 3;

	/*���ڵ��������������������*/
    vListInsert( &List_Test, &List_Item2 );
	vListInsert( &List_Test, &List_Item1 );
	vListInsert( &List_Test, &List_Item3 );
	
	for(;;){}
}

