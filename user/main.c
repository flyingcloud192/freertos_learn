#include "portmarco.h"
#include "list.h"
#pragma pack(4)
int main(void){
	
	List_t my_list;
	ListItem_t list_item_1;
	ListItem_t list_item_2;
	ListItem_t list_item_3;
	
	vListInitialise(&my_list);
	
	list_item_1.xItemValue = 0x03;
	list_item_2.xItemValue = 0x02;
	list_item_3.xItemValue = 0x01;
	
	vListInsert(&my_list,&list_item_1);
	vListInsert(&my_list,&list_item_2);
	vListInsert(&my_list,&list_item_3);
	
	UBaseType_t item_nums = vListRemove(&list_item_1);
	item_nums = vListRemove(&list_item_2);
	item_nums = vListRemove(&list_item_3);
	
	while(1){
	}
	return 0;
}
