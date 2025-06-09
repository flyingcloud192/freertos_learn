#include "portmarco.h"
#include "list.h"
#include "task.h"
#pragma pack(4)

#define TASK_1_STACK_SIZE        128
#define TASK_2_STACK_SIZE        128
#define TASK_3_STACK_SIZE        128

portCHAR flag1;
portCHAR flag2;
portCHAR flag3;

// 定义任务1的执行函数
void task_1_entry(void* arg_c){
	while(1){
		flag1 = 1;
		vTaskDelay(2);
		flag1 = 0;
		vTaskDelay(2);
	}
} 

// 定义任务2的执行函数
void task_2_entry(void* arg_c){
	while(1){
		flag2 = 1;
		vTaskDelay(2);
		flag2 = 0;
		vTaskDelay(2);
	}
} 

void task_3_entry(void* arg_c){
	while(1){
		flag3 = 1;
		vTaskDelay(2);
		flag3 = 0;
		vTaskDelay(2);
	}
}
// 就绪任务链表
extern List_t pxReadyTasksLists[configMAX_PRIORITIES]; 
TCB_t TASK1TCB;
StackType_t Task1Stack[TASK_1_STACK_SIZE];

TCB_t TASK2TCB;
StackType_t Task2Stack[TASK_2_STACK_SIZE];

TCB_t TASK3TCB;
StackType_t Task3Stack[TASK_3_STACK_SIZE];

int main(void){

	// 初始化任务1控制块
	TCB_t*  Task_1_Handler = 
									xTaskCreateStatic((TaskFunction_t)task_1_entry,
													(char*) "Task_1",
													(StackType_t)TASK_1_STACK_SIZE,
													(void*)NULL,
													(UBaseType_t)1,
													(StackType_t*)(&Task1Stack),
													(TCB_t*)&TASK1TCB);
													
													


	// 初始化任务2控制块
	TCB_t*  Task_2_Handler = 
									xTaskCreateStatic((TaskFunction_t)task_2_entry,
													(char*) "Task_2",
													(StackType_t)TASK_2_STACK_SIZE,
													(void*)NULL,
													(UBaseType_t)2,
													(StackType_t*)(&Task2Stack),
													(TCB_t*)&TASK2TCB);

	TCB_t*  Task_3_Handler = 
									xTaskCreateStatic((TaskFunction_t)task_3_entry,
													(char*) "Task_3",
													(StackType_t)TASK_3_STACK_SIZE,
													(void*)NULL,
													(UBaseType_t)3,
													(StackType_t*)(&Task3Stack),
													(TCB_t*)&TASK3TCB);


													
	vTaskStartScheduler(&TASK1TCB);  // 开启任务调度,传入第一个任务的TCB

	while(1){
	}
	return 0;
}
