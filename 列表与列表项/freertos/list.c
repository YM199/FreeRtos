#include "list.h"

/**
 * @brief 链表节点初始化
*/
void vListInitialiseItem(ListItem_t * const pxItem)
{
	  /*初始化该节点所在的链表为空，表示节点还没有插入任何链表*/
    pxItem->pvContainer = NULL;
}


/**
 * @brief 链表根节点初始化
 * @param pxList 链表根节点
*/
void vListInitialise( List_t * const pxList )
{
	  /*将链表索引指针指向最后一个节点*/
    pxList->pxIndex = ( ListItem_t * ) &(pxList->xListEnd);
	
	  /*将最后一个节点的辅助排序的值设置为最大，确保该节点是链表的最后节点*/
	  pxList->xListEnd.xItemValue = portMAX_DELAY;
	
	 /*将最后一个节点的的pxNext和pxPrevious指针均指向节点自身，表示链表为空*/
	  pxList->xListEnd.pxNext = ( ListItem_t * ) &( pxList->xListEnd );
	  pxList->xListEnd.pxPrevious = ( ListItem_t * ) &(pxList->xListEnd);
	
	 /*初始化链表节点计数器的值为0*/
	 pxList->uxNumber0fItems = (UBaseType_t) 0U;
}


/**
 * @brief 将节点插入链表尾部(将一个新的节点插入到一个空的链表)
 * @param pxList 被插入的链表（根节点）
 *        pxNewListItem 新插入的节点
*/
void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem)
{
	  /*将链表最后一个节点(根节点)取出来*/
	  ListItem_t * const pxIndex = pxList->pxIndex;
	

	  pxNewListItem->pxNext = pxIndex;//尾节点的下一个节点指向根节点(形成一个环)
	  pxNewListItem->pxPrevious =pxIndex->pxPrevious;//尾节点的上一个节点是倒数第二个节点(之前的尾节点)
	  pxIndex->pxPrevious = pxNewListItem;//更新尾节点(根节点的上一个节点是尾节点)
	
	  pxNewListItem->pvContainer = ( void * ) pxList;//指向该节点所在的链表
	
	  ( pxList->uxNumber0fItems )++;//链表节点计数器+1
}


/**
 * @brief 将节点按照升序排列插入到链表
 * @param pxList 根节点
          pxNewListItem 新插入的节点
*/
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem)
{
	  ListItem_t *pxIterator;//新节点会插入到pxIterator后面
	
	  /*获取排序辅助值*/
	  const TickType_t xValue0fInsertion = pxNewListItem->xItemValue;
	  
	  if ( xValue0fInsertion == portMAX_DELAY)//如果为最大值
	  {
			  pxIterator = pxList->xListEnd.pxPrevious;//pxIterator为尾节点
		}
		else
		{
			  for ( pxIterator = ( ListItem_t * ) &(pxList)-> xListEnd;
			        pxIterator->pxNext->xItemValue <= xValue0fInsertion;
			        pxIterator = pxIterator->pxNext)
				{
					  //不断迭代找到节点要插入的位置
				}
		}
		
		pxNewListItem->pxNext = pxIterator->pxNext;
		pxNewListItem->pxNext->pxPrevious =pxNewListItem;
		pxNewListItem->pxPrevious = pxIterator;
		pxIterator->pxNext = pxNewListItem;
		
		/*记住该节点所在的链表*/
		pxNewListItem->pvContainer = ( void * ) pxList;
		
		/*链表计数器++*/
		( pxList->uxNumber0fItems )++;
}

/**
 * @brief 将节点从链表删除
 * @param 待删除的节点
*/
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove)
{
	   /*获取节点所在的链表*/
	   List_t * const pxList = ( List_t * ) pxItemToRemove->pvContainer;
	   /*将指定节点从链表里面删除*/
	   pxItemToRemove->pxNext->pxPrevious  = pxItemToRemove->pxPrevious;
	   pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;
	
	   /*调整链表的节点索引*/
	   if ( pxList->pxIndex == pxItemToRemove )
		 {
			     pxList->pxIndex = pxItemToRemove->pxPrevious;
		 }
		 
		 /*初始化该节点所在的链表为空，表示该节点没有插入任何链表*/
		 pxItemToRemove->pvContainer = NULL;
		 
		 /*链表计数器--*/
		 ( pxList->uxNumber0fItems )--;
		 
		 /*返回链表中剩余节点的个数*/
		 return pxList->uxNumber0fItems;
}

