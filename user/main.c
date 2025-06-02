#include "portmarco.h"
#include "list.h"
#include "task.h"
#pragma pack(4)

#define TASK_1_STACK_SIZE      128
#define TASK_2_STACK_SIZE      128
portCHAR flag1;
portCHAR flag2;
void delay(UBaseType_t count){
	for(;count!= 0;count--){}
}	

// 定义任务1的执行函数
void task_1_entry(void* arg_c){
	while(1){
		flag1 = 1;
		delay(100);
		flag1 = 0;
		delay(100);
		portYIELD();
	}
} 


// 定义任务2的执行函数
void task_2_entry(void* arg_c){
	while(1){
		flag2 = 1;
		delay(100);
		flag2 = 0;
		delay(100);
		portYIELD();
	}
} 

TCB_t TASK1TCB;
StackType_t Task1Stack[TASK_1_STACK_SIZE];
TCB_t TASK2TCB;
StackType_t Task2Stack[TASK_2_STACK_SIZE];
extern List_t pxReadyTasksLists[configMAX_PRIORITIES]; 

int main(void){
	// 初始化任务链表
	prvInitialiseTaskLists();

	// 初始化任务1控制块
	TCB_t*  Task_1_Handler = 
									xTaskCreateStatic((TaskFunction_t)task_1_entry,
													(char*) "Task_1",
													(StackType_t)TASK_1_STACK_SIZE,
													(void*)NULL,
													(StackType_t*)(&Task1Stack),
													(TCB_t*)&TASK1TCB);

	// 初始化任务2控制块
	TCB_t*  Task_2_Handler = 
									xTaskCreateStatic((TaskFunction_t)task_2_entry,
													(char*) "Task_2",
													(StackType_t)TASK_2_STACK_SIZE,
													(void*)NULL,
													(StackType_t*)(&Task2Stack),
													(TCB_t*)&TASK2TCB);


	/* 挂载任务1-2至链表 */
	vListInsertEnd((List_t*)(&pxReadyTasksLists[0]), 
	               (ListItem_t*)(&(Task_1_Handler->xStateListItem)));
				   
	vListInsertEnd((List_t*)(&pxReadyTasksLists[1]), 
	               (ListItem_t*)(&(Task_2_Handler->xStateListItem)));
													
	vTaskStartScheduler(&TASK1TCB);  // 开启任务调度,传入第一个任务的TCB

	while(1){
	}
	return 0;
}
