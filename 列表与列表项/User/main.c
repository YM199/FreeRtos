/*
* 注意事项：1、该工程使用软件仿真，debug需选择 Ude Simulator
*           2、在Target选项卡里面把晶振Xtal(Mhz)的值改为25，默认是12，
*              改成25是为了跟system_ARMCM3.c中定义的__SYSTEM_CLOCK相同，确保仿真的时候时钟一致
*/

#include "list.h"

/*定义根节点*/
static struct xLIST List_Test;
/*定义节点*/
static struct xLIST_ITEM List_Item1;
static struct xLIST_ITEM List_Item2;
static struct xLIST_ITEM List_Item3;
int main(void)
{
	/* 链表根节点初始化 */
    vListInitialise( &List_Test );
	
	/* 节点1初始化 */
	vListInitialiseItem( &List_Item1 );
	List_Item1.xItemValue = 1;
	
	/* 节点2初始化 */
	vListInitialiseItem( &List_Item2 );
	List_Item2.xItemValue = 2;	

	/* 节点3初始化 */
	vListInitialiseItem( &List_Item3 );
	List_Item3.xItemValue = 3;

	/*将节点插入链表，按照升序排列*/
    vListInsert( &List_Test, &List_Item2 );
	vListInsert( &List_Test, &List_Item1 );
	vListInsert( &List_Test, &List_Item3 );
	
	for(;;){}
}

