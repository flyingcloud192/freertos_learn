// 本文件用于定义FreeRTOS的一些配置宏

#ifndef _FREERTOSCONFIG_H__
#define _FREERTOSCONFIG_H__

#define configUSE_16_BIT_TICKS      0                      // 定义TickType_t是16位(0)还是32位(1)
#define configMAX_TASK_NAME_LEN     128                    // 定义任务名最大字符串长度
#define configMAX_PRIORITIES        31                     // 定义最大任务优先级默认是5,每一个优先级对应一个任务链表,相同优先级的任务挂载到同一个任务链表
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 191           // 定义系统调用中断屏蔽(所有优先级高于或等于11的中断被屏蔽)
#define configCPU_CLOCK_HZ     ((unsigned long) 72000000)  // 定义CPU时钟频率,单位是Hz,这里是72MHz
#define configTICK_RATE_HZ     ((TickType_t) 100)          // 定义系统滴答时钟频率,单位是Hz,这里是100Hz,每秒触发100次SysTick中断
#define configUSE_PORT_OPTIMISED_TASK_SELECTION   1        // 定义是否使用优化的任务选择算法(1表示使用,0表示不使用)

#define configKERNEL_INTERRUPT_PRIORITY    255        // 定义外部中断优先级0-15(数值越大优先级越低,0是最高优先级,每个中断的优先级由寄存器的八位bit表示,其中高四位代表优先级数值)
#define xPortPendSVHandler           PendSV_Handler   // 重新声明PendSV中断服务函数
#define xPortSysTickHandler          SysTick_Handler  // 重新声明SysTick中断服务函数
#define vPortSVCHandler              SVC_Handler      // 重新声明SVC中断服务函数

#endif