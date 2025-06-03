#ifndef __TASK_H__
#define __TASK_H__

#include "portmarco.h"
#include "list.h"
#include "FreeRTOSConfig.h"
#define portINITIAL_XPSR         (0x01000000)
#define portSTART_ADDRESS_MASK   ((StackType_t)0xfffffffeUL)

/*
* 参考资料《STM32F10xxx Cortex-M3 programming manual》4.4.7，百度搜索“PM0056”即可找到这个文档
* 在 Cortex-M 中，内核外设 SCB 中 SHPR3 寄存器用于设置 SysTick 和 PendSV 的异常优先级,需要把这两个中断设置为最低优先级
* System handler priority register 3 (SCB_SHPR3) SCB_SHPR3：0xE000 ED20
* Bits 31:24 PRI_15[7:0]: Priority of system handler 15, SysTick exception
* Bits 23:16 PRI_14[7:0]: Priority of system handler 14, PendSV
*/

#define portNVIC_SYSPRI2_REG    (*((volatile uint32_t*)0xe000ed20))                        // 系统滴答中断以及任务切换中断
#define portNVIC_PENDSV_PRI     (((uint32_t) configKERNEL_INTERRUPT_PRIORITY << 16UL))     // 系统任务切换中断最低优先级 
#define portNVIC_SYSTICK_PRI    (((uint32_t) configKERNEL_INTERRUPT_PRIORITY << 24UL))     // 系统滴答中断最低优先级

#define portNVIC_INT_CTRL_REG   (*((volatile uint32_t*)0xe000ed04))                        // 中断状态控制寄存器(控制PendSV、SVC、SysTick等特殊中断)
#define portNVIC_PENDSVSET_BIT  (1UL << 28UL)                                              // PendSV中断设置位，设置该位为1时会触发PendSV中断
#define portSY_FULL_READ_ERITE  (15)                                                       // 全系统范围同步

// 定义任务控制块,控制块里存储任务的栈指针、
struct tsTaskControlBlock{
	volatile StackType_t*         pxTopOfStack;          /* 栈顶指针 */
	ListItem_t                    xStateListItem;        /* 定义任务节点可将所有任务控制块挂载到指定链表 */
	StackType_t*                  pxStack;               /* 任务栈起始地址 */
	
	char        pcTaskName[configMAX_TASK_NAME_LEN];     /* 定义任务名称字符串 */
};

typedef struct tsTaskControlBlock TCB_t;

/* 静态方式创建一个任务 
   xTaskCreateStatic -->  prvInitialiseNewTask 创建新的任务
   prvInitialiseNewTask  -->  pxPortInitialiseStack  初始化任务栈空间
*/
TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,          // 任务执行函数指针
								const char* const pxTaskName,       // 任务名字
								const StackType_t ulStackDepth,     // 任务栈大小
								void * const pvParameters,          // 任务执行函数形参
								StackType_t* const puxStackBuffer,  // 任务栈的起始地址
								TCB_t*  pxTaskBuffer);              // 任务控制块指针
								
/* 初始化新任务 */
void prvInitialiseNewTask( TaskFunction_t pxTaskCode,          // 任务执行函数指针
						   const char* const pxTaskName,       // 任务名字
						   const StackType_t ulStackDepth,     // 任务栈大小 
			               void * const pvParameters,          // 任务执行函数形参
						   TaskHandle_t* const pxCreatedTask,  // 创建任务句柄
						   TCB_t* pxNewTCB);                   // 任务控制块指针,任务控制块在调用本函数前完成定义

/* 初始化栈主要是需要存储一些寄存器的值 */
static void prvTaskExitError(void);
StackType_t* pxPortInitialiseStack(StackType_t* pxTopOfStack,
						           TaskFunction_t pxTaskCode,
						           void * const pvParameters);
						   

/* 初始化任务链表数组 */
void prvInitialiseTaskLists(void);
		

/* 开始调度函数 */
void vTaskStartScheduler(TCB_t* toSchedulerTCB); 

/* 设置系统中断优先级,主要是系统滴答中断和任务切换中断 */
BaseType_t xPortStartScheduler(void);

/* 开启执行第一个任务 */
__asm void prvStartFirstTask(void);

/* SVC中断服务函数 */
__asm void vPortSVCHandler(void);


/* 中断上下文切换 */
void portYIELD(void);
/* task1和task2轮流切换,就是切换当前  */

/* PendSV中断服务函数 */
__asm void xPortPendSVHandler(void);


/******系统临界段相关，检验使用带中断返回值的开关中断操作
 * demo
 * {
 * 		uint32_t uxSavedInterruptStatus;
 * 		uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();  // 关中断，返回当前中断屏蔽寄存器的值
 *      TODO..........
 * 		taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);      // 退出临界段，带中断返回值
 * }
 * *******/

/* 进入临界段，带返回值的关中断函数 */
#define taskENTER_CRITICAL_FROM_ISR()   portSET_INTERRUPT_MASK_FROM_ISR()
// 退出临界段，带中断返回值
#define taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus)  portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus)


#endif