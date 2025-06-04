#ifndef _LIST_H__
#define _LIST_H__
#include "portmarco.h"
#pragma pack(4)
/* 定义任务链表节点 */
struct xLIST_ITEM{
	TickType_t  xItemValue;          // 用于节点顺序排序
	struct xLIST_ITEM* pxNext;       // 用于指向下一个链表节点
	struct xLIST_ITEM* pxPrevious;   // 用于指向前一个链表节点
	void*  pvOwner;                  // 指向拥有该节点的内核对象,通常是TCB
	void*  pvContainer;              // 指向当前节点所属链表就是当前链表根节点
};
typedef struct xLIST_ITEM  ListItem_t;

/* 定义链表 */
struct xLIST{
	TickType_t  uxNumberOfItems;     // 定义链表中item个数
	ListItem_t* pxIndex;             // 定义链表item索引指针
	ListItem_t  xListEnd;            // 定义链表尾节点也是头节点
};


typedef struct xLIST       List_t;

// 声明链表末端插入函数
void vListInsertEnd(List_t* const pxlist, ListItem_t* const pxNewListItem);
// 声明链表排序插入函数
void vListInsert(List_t* const pxlist, ListItem_t* const pxNewListItem);
// 声明初始化链表函数
void vListInitialise(List_t* const pxlist);
// 声明初始化链表节点函数
void vListInitialiseItem(ListItem_t* const pxItem);

// 声明删除链表
UBaseType_t vListRemove(ListItem_t* pxItemToRemove);

/* 初始化节点owner */
void listSET_LIST_ITEM_OWNER(ListItem_t* const pxListItem,void* pxOwner);
			
/* 获取节点owner */
void* listGET_LIST_ITEM_OWNER(ListItem_t* const pxListItem);
		
/* 初始化节点排序辅助值 */
void listSET_LIST_ITEM_VALUE(ListItem_t* const pxListItem, TickType_t pxValue);

/* 获取节点排序辅助值 */
TickType_t listGET_LIST_ITEM_VALUE(ListItem_t* const pxListItem);

/* 获取链表元素个数 */
TickType_t listCURRENT_LIST_LENGTH(List_t const* pxList);
		
/* 判断链表是否为空 */
BaseType_t listLIST_IS_EMPTY(List_t const* pxList);
		
/* 获取链表的第一个节点 */
ListItem_t* listGET_HEAD_ENTRY(List_t* const pxList);

/* 获取链表的尾节点 */
ListItem_t* listGET_END_MARKER(List_t* const pxList);
			
/* 获取当前节点的下一个节点 */
ListItem_t* listGET_NEXT(ListItem_t* const pxListItem);
		
/* 获取链表第一个节点的内核对象,就是TCB */
void* listGET_OWNER_OF_NEXT_ENTRY(List_t* const pxList);

#endif