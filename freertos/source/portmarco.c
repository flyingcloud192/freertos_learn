#include "portmarco.h"

// 不带返回值的关中断函数，不支持嵌套
void vportRaiseBASEPRI(void){
	uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
	__asm{
		msr basepri, ulNewBASEPRI  // 设置中断屏蔽寄存器BASEPRI
		isb
		dsb
	}
}

// 带返回值的关中断函数，返回当前中断屏蔽寄存器的值，支持嵌套
uint32_t ulPortRaiseBASEPRI(void){
	uint32_t ulReturn;
	__asm{
		mrs ulReturn, basepri     // 读取当前中断屏蔽寄存器BASEPRI的值
		msr basepri, configMAX_SYSCALL_INTERRUPT_PRIORITY  // 设置中断屏蔽寄存器BASEPRI
		isb
		dsb
	}
	return ulReturn;
}

// 开中断函数
void vPortSetBASEPRI(uint32_t ulNewBASEPRI){
	__asm{
		msr basepri, ulNewBASEPRI  // 设置中断屏蔽寄存器BASEPRI
		isb
		dsb
	}
}

/* SysTick初始化函数 */
void vPortSetupTimerInterrupt(void){
	// 设置SysTick重载寄存器的值
	portNVIC_SYSTICK_LOAD_REG = (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ) - 1; // 设置SysTick重载值
	/* 设置SysTick控制寄存器
	   1、设置系统定时器时钟为CPU内核时钟
	   2、使能SysTick中断
	   3、使能SysTick计数器
	*/
	portNVIC_SYSTICK_CTRL_REG = (portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT);
}