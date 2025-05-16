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
// 声明删除链表
UBaseType_t vListRemove(ListItem_t* pxItemToRemove);

/* 初始化节点owner */
#define listSET_LIST_ITEM_OWNER(pxListItem,pxOwner) \
        ( (pxListItem)->(pvOwner) = (void)* (pxOwner) )
			
/* 获取节点owner */
#define listGET_LIST_ITEM_OWNER(pxListItem) \
		( (pxListItem)->(pvOwner) )
		
/* 初始化节点排序辅助值 */
#define listSET_LIST_ITEM_VALUE(pxListItem,pxValue) \
		( (pxListItem)->(xItemValue) = pxValue )
		
/* 获取节点排序辅助值 */
#define listGET_LIST_ITEM_VALUE(pxListItem) \
		( (pxListItem)->(xItemValue) )
		
/* 获取链表元素个数 */
#define listCURRENT_LIST_LENGTH(pxList) \
		( (pxList)->uxNumberOfItems )
		
/* 判断链表是否为空 */
#define listLIST_IS_EMPTY(pxList) \
		( (BaseType_t) ((pxList)->(uxNumberOfItems) == (UBaseType_t)0 ) )
		
/* 获取链表的第一个节点 */
#define listGET_GET_HEAD_ENTRY(pxList) \
		( ((pxList)->(xListEnd)).pxNext )

/* 获取链表的尾节点 */
#define listGET_END_MARKER(pxList) \
		( (ListItem_t const *) &((pxList)->(xListEnd)) )
			
/* 获取当前节点的下一个节点 */
#define listGET_NEXT(pxListItem)\
		((pxListItem)->pxNext)

#define NAME  {123}
		
/* 获取链表第一个节点的内核对象,就是TCB */
#define listGET_OWNER_OF_NEXT_ENTRY(pxTCB,pxList)     \
{                                                     \
	ListItem_t* pxHeadItem = pxList->pxIndex->pxNext; \
	(pxTCB) = (pxHeadItem)->(pvOwner);                \
}
