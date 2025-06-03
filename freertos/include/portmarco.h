#ifndef __PORTMARCO_H__
#define __PORTMARCO_H__
#include <stddef.h>
#include "FreeRTOSConfig.h"
/* 数据类型重定义 */
typedef unsigned int     uint32_t;
typedef unsigned short   uint16_t;

#define portCHAR         char
#define portFLOAT        float
#define portDOUBLE       double
#define portLONG         long
#define portSHORT        short
#define portSTACK_TYPE   uint32_t
#define portBASE_TYPE    long
#define pdFALSE          0x00


typedef portSTACK_TYPE   StackType_t;
typedef long             BaseType_t;
typedef unsigned long    UBaseType_t;
typedef void *          TaskHandle_t;       // 定义任务句柄
typedef void (*TaskFunction_t)(void*);      // 定义函数指针TaskFunction_t 其指向输入形参为void*,返回类型为void的函数

#if (configUSE_16_BIT_TICKS == 0)
	typedef uint16_t TickType_t;
	#define portMAX_DELAY    (TickType_t)  0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY    (TickType_t)  0xffffffff
#endif


/****************代码临界段保护*******************/
/* 不带返回值的关中断函数，（不管当前中断屏蔽寄存器BASEPRI的值，直接覆盖）
   所以不能再中断函数里面使用，即不支持中断嵌套 */
void vportRaiseBASEPRI(void);
#define portDISABLE_INTERRUPTS()   vportRaiseBASEPRI()

/* 带返回值的关中断函数,返回当前中断屏蔽寄存器的值 */
uint32_t ulPortRaiseBASEPRI(void);
#define portSET_INTERRUPT_MASK_FROM_ISR()  ulPortRaiseBASEPRI()

void vPortSetBASEPRI(uint32_t ulNewBASEPRI);
/* 不带中断保护的开中断 */
#define portENABLE_INTERRUPTS()    vPortSetBASEPRI(0)
/* 带中断保护的开中断 */
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus)  vPortSetBASEPRI(uxSavedInterruptStatus)

#endif
