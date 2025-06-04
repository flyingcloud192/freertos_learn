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

#define portNVIC_SYSTICK_CTRL_REG   (*((volatile uint32_t*)0xe000e010)) // SysTick控制寄存器
#define portNVIC_SYSTICK_LOAD_REG   (*((volatile uint32_t*)0xe000e014)) // SysTick重载寄存器
#ifndef configSYSTICK_CLOCK_HZ
	#define configSYSTICK_CLOCK_HZ     configCPU_CLOCK_HZ
	#define portNVIC_SYSTICK_CLK_BIT  (1UL << 2UL) // SysTick时钟源选择位
#else
	#define portNVIC_SYSTICK_CLK_BIT  (0)          // SysTick时钟源选择位
#endif

#define portNVIC_SYSTICK_INT_BIT    (1UL << 1UL)   // SysTick中断使能位
#define portNVIC_SYSTICK_ENABLE_BIT (1UL << 0UL)   // SysTick使能位



typedef portSTACK_TYPE   StackType_t;
typedef long             BaseType_t;
typedef unsigned long    UBaseType_t;
typedef void *          TaskHandle_t;       // 定义任务句柄
typedef void (*TaskFunction_t)(void*);      // 定义函数指针TaskFunction_t 其指向输入形参为void*,返回类型为void的函数

#if (configUSE_16_BIT_TICKS == 1)
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

/* SysTick的中断服务函数,已经在FreeRTOSConfig.h中重命名 */
void xPortSysTickHandler(void);
/* SysTick初始化函数 */
void vPortSetupTimerInterrupt(void);
#endif
