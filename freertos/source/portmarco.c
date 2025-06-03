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