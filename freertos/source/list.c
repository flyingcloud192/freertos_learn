#include "list.h"
// 初始化当前链表
void vListInitialise(List_t* const pxlist){
	pxlist->uxNumberOfItems = (TickType_t) 0x00;                       // 链表初始化节点数为0
	pxlist->pxIndex     =   (ListItem_t* )&(pxlist->xListEnd);         // 将当前链表索引指针指向尾节点
	
	pxlist->xListEnd.xItemValue  = portMAX_DELAY;                       // 将当前链表尾节点元素的xItemValue值设置为最大
	pxlist->xListEnd.pxNext      = (ListItem_t*)&(pxlist->xListEnd);    // 将当前链表尾节点元素的next指针指向当前链表尾节点
	pxlist->xListEnd.pxPrevious  = (ListItem_t*)&(pxlist->xListEnd);    // 将当前链表尾节点元素的previous指针指向当前链表尾节点
	pxlist->xListEnd.pvOwner     = NULL;                                // 当前链表的pvOwner暂时为NULL
	pxlist->xListEnd.pvContainer = pxlist;                              // 当前链表尾节点的container为当前链表
}

void vListInitialiseItem(ListItem_t* const pxItem){
	// 初始化该节点所在链表为空,表示该节点尚未插入任何链表
	pxItem->pvContainer = NULL;   
}

// 向当前链表尾部插入一个新的节点
void vListInsertEnd(List_t* const pxlist, ListItem_t* const pxNewListItem){
	pxlist->uxNumberOfItems += 0x01;	
	ListItem_t* newpxIndex = pxlist->pxIndex;
	
	pxNewListItem->pxPrevious = newpxIndex->pxPrevious;
	pxNewListItem->pxNext = newpxIndex;
	newpxIndex->pxPrevious->pxNext = pxNewListItem;
	newpxIndex->pxPrevious = pxNewListItem;
	
	pxNewListItem->pvContainer = pxlist;
}

// 向当前链表插入一个新的节点并且链表按照xItemValue升序排序
void vListInsert(List_t* const pxlist, ListItem_t* const pxNewListItem){
	ListItem_t* newpxIndex = pxlist->pxIndex->pxNext;
	
	while(1){
		if(pxNewListItem->xItemValue == portMAX_DELAY){
			newpxIndex = pxlist->pxIndex;
			break;
		}
		
		if(newpxIndex->xItemValue > pxNewListItem->xItemValue){
			break;
		}
		newpxIndex = newpxIndex->pxNext;
	}
	
	// 插入到newpxIndex节点之前
	pxlist->uxNumberOfItems += 0x01;	
	pxNewListItem->pxPrevious = newpxIndex->pxPrevious;
	pxNewListItem->pxNext     = newpxIndex;
	newpxIndex->pxPrevious->pxNext = pxNewListItem;
	newpxIndex->pxPrevious = pxNewListItem;
	pxNewListItem->pvContainer = pxlist;
}

// 删除一个指定节点
UBaseType_t vListRemove(ListItem_t* pxItemToRemove){
	List_t* ToRemoveList = (List_t*)pxItemToRemove->pvContainer;
	ListItem_t* newpxIndex = ToRemoveList->pxIndex;
	
	// 不允许删除尾节点
	if (ToRemoveList->uxNumberOfItems == 0 || pxItemToRemove == ToRemoveList->pxIndex)
		return 0;
	
	while(1){
		if(newpxIndex == pxItemToRemove && newpxIndex != ToRemoveList->pxIndex){
			break;
		}
		newpxIndex = newpxIndex->pxNext;
	}
	
	newpxIndex->pxPrevious->pxNext = newpxIndex->pxNext;
	newpxIndex->pxNext->pxPrevious = newpxIndex->pxPrevious;
	(ToRemoveList->uxNumberOfItems)--;
	return ToRemoveList->uxNumberOfItems;
}

/* 设置链表当前节点owner */
void listSET_LIST_ITEM_OWNER(ListItem_t* const pxListItem,void* pxOwner){
	pxListItem->pvOwner = pxOwner;
}

/* 获取链表当前节点owner */
void* listGET_LIST_ITEM_OWNER(ListItem_t* const pxListItem){
	return (void*)(pxListItem->pvOwner);
}

/* 初始化链表节点排序辅助值 */
void listSET_LIST_ITEM_VALUE(ListItem_t* const pxListItem, TickType_t pxValue){
	pxListItem->xItemValue = pxValue;
}

/* 获取链表节点排序辅助值 */
TickType_t listGET_LIST_ITEM_VALUE(ListItem_t* const pxListItem){
	return (TickType_t)(pxListItem->xItemValue);
}

/* 获取链表元素个数 */
TickType_t listCURRENT_LIST_LENGTH(List_t const* pxList){
	return (TickType_t)(pxList->uxNumberOfItems);
}

/* 判断链表是否为空 */
BaseType_t listLIST_IS_EMPTY(List_t const* pxList){
	return (BaseType_t) ((pxList)->uxNumberOfItems == (UBaseType_t)0 );
}

/* 获取链表的第一个节点 */
ListItem_t* listGET_HEAD_ENTRY(List_t* const pxList){
	return (ListItem_t*) &((pxList->xListEnd).pxNext);
}

/* 获取当前链表的尾节点 */
ListItem_t* listGET_END_MARKER(List_t* const pxList){
	return (ListItem_t* )&(pxList->xListEnd); 
}

/* 获取当前节点的下一个节点 */
ListItem_t* listGET_NEXT(ListItem_t* const pxListItem){
	return pxListItem->pxNext;
}

/* 获取链表第一个节点的内核对象,就是TCB */
void* listGET_OWNER_OF_NEXT_ENTRY(List_t* const pxList){
	ListItem_t* pxHeadItem = pxList->pxIndex->pxNext;
	return (void*)(pxHeadItem->pvOwner);
}

