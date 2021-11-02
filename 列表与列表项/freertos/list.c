#include "list.h"

/**
 * @brief ����ڵ��ʼ��
*/
void vListInitialiseItem(ListItem_t * const pxItem)
{
	  /*��ʼ���ýڵ����ڵ�����Ϊ�գ���ʾ�ڵ㻹û�в����κ�����*/
    pxItem->pvContainer = NULL;
}


/**
 * @brief ������ڵ��ʼ��
 * @param pxList ������ڵ�
*/
void vListInitialise( List_t * const pxList )
{
	  /*����������ָ��ָ�����һ���ڵ�*/
    pxList->pxIndex = ( ListItem_t * ) &(pxList->xListEnd);
	
	  /*�����һ���ڵ�ĸ��������ֵ����Ϊ���ȷ���ýڵ�����������ڵ�*/
	  pxList->xListEnd.xItemValue = portMAX_DELAY;
	
	 /*�����һ���ڵ�ĵ�pxNext��pxPreviousָ���ָ��ڵ�������ʾ����Ϊ��*/
	  pxList->xListEnd.pxNext = ( ListItem_t * ) &( pxList->xListEnd );
	  pxList->xListEnd.pxPrevious = ( ListItem_t * ) &(pxList->xListEnd);
	
	 /*��ʼ������ڵ��������ֵΪ0*/
	 pxList->uxNumber0fItems = (UBaseType_t) 0U;
}


/**
 * @brief ���ڵ��������β��(��һ���µĽڵ���뵽һ���յ�����)
 * @param pxList ��������������ڵ㣩
 *        pxNewListItem �²���Ľڵ�
*/
void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem)
{
	  /*���������һ���ڵ�(���ڵ�)ȡ����*/
	  ListItem_t * const pxIndex = pxList->pxIndex;
	

	  pxNewListItem->pxNext = pxIndex;//β�ڵ����һ���ڵ�ָ����ڵ�(�γ�һ����)
	  pxNewListItem->pxPrevious =pxIndex->pxPrevious;//β�ڵ����һ���ڵ��ǵ����ڶ����ڵ�(֮ǰ��β�ڵ�)
	  pxIndex->pxPrevious = pxNewListItem;//����β�ڵ�(���ڵ����һ���ڵ���β�ڵ�)
	
	  pxNewListItem->pvContainer = ( void * ) pxList;//ָ��ýڵ����ڵ�����
	
	  ( pxList->uxNumber0fItems )++;//����ڵ������+1
}


/**
 * @brief ���ڵ㰴���������в��뵽����
 * @param pxList ���ڵ�
          pxNewListItem �²���Ľڵ�
*/
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem)
{
	  ListItem_t *pxIterator;//�½ڵ����뵽pxIterator����
	
	  /*��ȡ������ֵ*/
	  const TickType_t xValue0fInsertion = pxNewListItem->xItemValue;
	  
	  if ( xValue0fInsertion == portMAX_DELAY)//���Ϊ���ֵ
	  {
			  pxIterator = pxList->xListEnd.pxPrevious;//pxIteratorΪβ�ڵ�
		}
		else
		{
			  for ( pxIterator = ( ListItem_t * ) &(pxList)-> xListEnd;
			        pxIterator->pxNext->xItemValue <= xValue0fInsertion;
			        pxIterator = pxIterator->pxNext)
				{
					  //���ϵ����ҵ��ڵ�Ҫ�����λ��
				}
		}
		
		pxNewListItem->pxNext = pxIterator->pxNext;
		pxNewListItem->pxNext->pxPrevious =pxNewListItem;
		pxNewListItem->pxPrevious = pxIterator;
		pxIterator->pxNext = pxNewListItem;
		
		/*��ס�ýڵ����ڵ�����*/
		pxNewListItem->pvContainer = ( void * ) pxList;
		
		/*���������++*/
		( pxList->uxNumber0fItems )++;
}

/**
 * @brief ���ڵ������ɾ��
 * @param ��ɾ���Ľڵ�
*/
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove)
{
	   /*��ȡ�ڵ����ڵ�����*/
	   List_t * const pxList = ( List_t * ) pxItemToRemove->pvContainer;
	   /*��ָ���ڵ����������ɾ��*/
	   pxItemToRemove->pxNext->pxPrevious  = pxItemToRemove->pxPrevious;
	   pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;
	
	   /*��������Ľڵ�����*/
	   if ( pxList->pxIndex == pxItemToRemove )
		 {
			     pxList->pxIndex = pxItemToRemove->pxPrevious;
		 }
		 
		 /*��ʼ���ýڵ����ڵ�����Ϊ�գ���ʾ�ýڵ�û�в����κ�����*/
		 pxItemToRemove->pvContainer = NULL;
		 
		 /*���������--*/
		 ( pxList->uxNumber0fItems )--;
		 
		 /*����������ʣ��ڵ�ĸ���*/
		 return pxList->uxNumber0fItems;
}

