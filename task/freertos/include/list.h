#ifndef LIST_H
#define LIST_H

#include "FreeRTOS.h"

/**
 * @brief ����ڵ����ݽṹ
*/
struct xLIST_ITEM
{
	  TickType_t xItemValue;         /*����ֵ�����������ڵ���˳������*/
	  struct xLIST_ITEM * pxNext;    /*ָ��������һ���ڵ�*/
	  struct xLIST_ITEM * pxPrevious;/*ָ������ǰһ���ڵ�*/
	  void * pvOwner;                /*ָ��ӵ�иýڵ���ں˶���ͨ����TCB*/
	  void * pvContainer;            /*ָ��ýڵ����ڵ�����*/
};
typedef struct xLIST_ITEM ListItem_t;/*�ڵ����������ض���*/


/**
 * @brief ������ڵ�
*/
struct xMINI_LIST_ITEM
{
	  TickType_t xItemValue;         /*����ֵ�����������ڵ���˳������*/
	  struct xLIST_ITEM * pxNext;    /*ָ��������һ���ڵ�*/
	  struct xLIST_ITEM * pxPrevious;/*ָ������ǰһ���ڵ�*/
};
typedef struct xMINI_LIST_ITEM MiniListItem_t;/*����ڵ����������ض���*/


/**
 * @brief ������ڵ�
*/
typedef struct xLIST
{
    UBaseType_t uxNumber0fItems;/*����ڵ������*/
	  ListItem_t * pxIndex;       /*����ڵ�����ָ��*/
	  MiniListItem_t xListEnd;    /*�������һ���ڵ�*/
}List_t;


/*��ʼ���ڵ��ӵ����*/
#define listSET_LIST_ITEM_OWNER( pxListItem, pxOwner )\
        ((pxListItem)->pvOwner = (void *)(pxOwner))
/*��ȡ�ڵ�ӵ����*/
#define listGET_LIST_ITEM_OWNER( pxListItem )\
				( ( pxListItem )-> pvOwner )
/* ��ʼ���ڵ�������ֵ */
#define listSET_LIST_ITEM_VALUE( pxListItem, xValue )\
        (  ( pxListItem )-> xItemValue = ( xValue ) )
/* ��ȡ�ڵ�������ֵ */
#define listGET_LIST_ITEM_VALUE( pxListItem )\
				( ( pxListItem )->xItemValue )
/* ��ȡ������ڵ�Ľڵ��������ֵ */
#define listGET_ITEM_VALUE_OF_HEAD_ENTRY( pxList )\
				( ( ( pxList )->xListEnd ).pxNext->xItemValue )
/* ��ȡ�������ڽڵ� */
#define listGET_HEAD_ENTRY( pxList )\
				( ( ( pxList )->xListEnd ).pxNext )
/* ��ȡ�ڵ����һ���ڵ� */
#define listGET_NEXT( pxListItem )\
        ( ( pxListItem )->pxNext )
/* ��ȡ��������һ���ڵ� */
#define listGET_END_MARKER( pxList )\
        ( (ListItem_t const * ) ( &( ( pxList )->xListEnd ) ) )
/* �ж������Ƿ�Ϊ�� */
#define listLIST_TS_EMPTY( pxList )\
				( (BaseType_t) ( ( pxList )->uxNumber0fItems == ( UBaseType_t ) )
/* ��ȡ����Ľڵ��� */
#define listCURRENT_LIST_LENGTH( pxList )\
				( ( pxList )-> uxNumber0fItems )
/* ��ȡ�����һ���ڵ��OWNER, ��TCB */
#define listGET_OWNER_OF_NEXT_ENTRY( pxTCB, pxList )										             \
{																							                                       \
	  List_t * const pxConstList = ( pxList );											                   \
	  /* �ڵ�����ָ�������һ���ڵ�����ڵ�����ָ�룬ָ����һ���ڵ㣬
    �����ǰ������N���ڵ㣬����N�ε��øú���ʱ��pxInedex��ָ���N���ڵ� */           \
	  ( pxConstList )->pxIndex = ( pxConstList )->pxIndex->pxNext;							       \
    /* �������Ϊ�� */                                                               \
	  if ( (void *) (pxConsList)->pxIndex == ( void *) &( ( pxConstList )->xListEnd ) )\
		{                                                                                \
		    ( pxConstList )->pxIndex = ( pxConstList )->pxIndex->pxNext;                 \
		}                                                                                \
		/*��ȡ�ڵ��OWNER����TCB*/                                                       \
		( pxTCB ) = ( pxConstList )->pxIndex->pvOwner;                                   \
}

				
void vListInitialiseItem(ListItem_t * const pxItem);
void vListInitialise( List_t * const pxList );
void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem);
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem);
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove);
#endif/*LIST_H*/

