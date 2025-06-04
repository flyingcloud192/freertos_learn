#include "task.h"
#include <stddef.h>

/* 任务就绪列表 */
List_t pxReadyTasksLists[configMAX_PRIORITIES];                     // 定义任务就绪链表数组

TCB_t* pxCurrentTCB = NULL;                                         // 定义调度器当前执行的任务TCB指针

TickType_t xTickCount = 0;                                          // 定义系统滴答计数器

StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE]; // 空闲任务栈
TCB_t IdleTaskTCB; // 空闲任务控制块

void delay(UBaseType_t count){
	for(;count!= 0;count--){}
}

/* 创建新任务函数
	@return 任务控制块句柄,其实就是pxTaskBuffer
*/
TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,          // 任务执行函数指针
								const char* const pxTaskName,       // 任务名字
								const StackType_t ulStackDepth,     // 任务栈大小
								void * const pvParameters,          // 任务执行函数形参
								StackType_t* const puxStackBuffer,  // 任务栈的起始地址
								TCB_t*  pxTaskBuffer)               // 任务控制块指针
{
	TCB_t* pxNewTCB;
	TaskHandle_t xReturn;       
	// 创建一个任务
	if ((puxStackBuffer != NULL) && (pxTaskBuffer != NULL)){
		pxNewTCB = (TCB_t*) pxTaskBuffer;
		pxNewTCB->pxStack = (StackType_t*)puxStackBuffer;
		prvInitialiseNewTask(pxTaskCode,pxTaskName,ulStackDepth,pvParameters,&xReturn,pxNewTCB);
	}
	else{
		xReturn = NULL;
	}
	return xReturn;
}

/* 初始化新任务函数 */
void prvInitialiseNewTask( TaskFunction_t pxTaskCode,          // 任务执行函数指针
						   const char* const pxTaskName,       // 任务名字
						   const StackType_t ulStackDepth,     // 任务栈大小 
			               void * const pvParameters,          // 任务执行函数形参
						   TaskHandle_t* const pxCreatedTask,  // 创建任务句柄
						   TCB_t* pxNewTCB)                    // 任务控制块指针,任务控制块在调用本函数前完成定义	
{
	
	/* 初始化任务栈
		这里是这样的:
			因为目前采用的是静态方式创建任务;栈的定义其实就是一个数组, stack[128]
	        因此,传过来的pxStack所谓的栈起始地址其实就是数组首地址,对应数组空间的最低地址
	        然而栈顶是最高地址因此需要 + 栈大小 - 1 */
	StackType_t* pxTopOfStack;         // 栈顶指针
	pxTopOfStack = pxNewTCB->pxStack + (ulStackDepth - (StackType_t)0x01);
	
	// 栈顶八字节对齐(因为后续可能有浮点数运算,需要八字节对齐)
	pxTopOfStack = (StackType_t*) ( ((uint32_t)pxTopOfStack ) & ( ~((uint32_t)0x0007 )));

	pxNewTCB->pxTopOfStack = pxPortInitialiseStack(pxTopOfStack,pxTaskCode,pvParameters);
	
	/* 初始化任务名字把任务名字pxTaskName添加到任务控制块TCB里的pcTaskName里面 */
	for (UBaseType_t i = 0;i<configMAX_TASK_NAME_LEN;i++){
		pxNewTCB->pcTaskName[i] = pxTaskName[i];
		if(pxTaskName[i] == 0x00)
			break;
	}
	pxNewTCB->pcTaskName[configMAX_TASK_NAME_LEN - 1] = '\0';

	
	
	/* 初始化TCB里的任务节点xStateListItem */
	vListInitialiseItem((ListItem_t *)&(pxNewTCB->xStateListItem));        // 当前任务节点未挂载任何链表
	listSET_LIST_ITEM_OWNER(&(pxNewTCB->xStateListItem), pxNewTCB);        // 设置当前节点拥有者是当前TCB
	
	
	
	/* 任务句柄指向任务控制块 */
	if((void *)pxCreatedTask != NULL)
		*pxCreatedTask = (TaskHandle_t)pxNewTCB;                           // 传进来的是二级指针pxCreatedTask
}

/* 任务异常函数,一旦任务发生异常就跳到此函数 */
static void prvTaskExitError(void){
	while(1);
}


/* 初始化任务栈函数
	@return 栈初始化后的栈顶指针
*/
StackType_t* pxPortInitialiseStack(StackType_t* pxTopOfStack,
						           TaskFunction_t pxTaskCode,
						           void * const pvParameters)
{
	// 设置自动加载到CPU寄存器的值,
	pxTopOfStack--;
	*pxTopOfStack = portINITIAL_XPSR;               // 加载XPSR
	pxTopOfStack--;
	*pxTopOfStack = ((StackType_t)pxTaskCode) & portSTART_ADDRESS_MASK;    // 加载R15(PC)寄存器,存储的是任务函数的入口地址pxTaskCode
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t)prvTaskExitError;                         // 加载任务返回地址LR寄存器
	pxTopOfStack -= 5;                                                     // 加载R12、R3、R2、R1;
	*pxTopOfStack = (StackType_t)pvParameters;                             // 加载函数形参地址至R0寄存器
	
	// 手动加载到CPU寄存器,R11-R4
	pxTopOfStack -= 8; 
	
	return pxTopOfStack;     // 返回当前栈顶
}

/* 初始化任务链表数组 */
void prvInitialiseTaskLists(void){
	for (UBaseType_t i = 0;i<configMAX_PRIORITIES;i++){
		 vListInitialise(&pxReadyTasksLists[i]);
	}
}


void prvIdleTask(void* pvParameters){
	/* 空闲任务,空闲任务会一直运行 */
	while(1){
	}
}

/* 开启任务调度 */
void vTaskStartScheduler(TCB_t* toSchedulerTCB){
	// 开启一个空闲任务
	TCB_t*  IdleTask_Handler =  xTaskCreateStatic((TaskFunction_t)prvIdleTask,
												(char*) "IDLE",
												(StackType_t)configMINIMAL_STACK_SIZE,
												(void*)NULL,
												(StackType_t*)(&IdleTaskStack),
												(TCB_t*)&IdleTaskTCB);

	// 挂载空闲任务至就绪列表
	vListInsertEnd((List_t*)(&pxReadyTasksLists[0]), 
	               (ListItem_t*)(&(IdleTask_Handler->xStateListItem)));

	
	// 暂时手动输入需要调度执行的任务
	pxCurrentTCB = toSchedulerTCB;

	/* 开启任务调度,xPortStartScheduler主要设置系统滴答中断()和任务切换中断的优先级 */
	if (xPortStartScheduler() != pdFALSE){    

	}
}

/* 设置系统上下文切换中断、滴答中断为最低优先级并且启动第一个任务 */
BaseType_t xPortStartScheduler(void){
	portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;        // 设置上下文切换中断为最低优先级
	portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;       // 设置系统滴答中断为最低优先级

	// 初始化SysTick
	vPortSetupTimerInterrupt();

	// 开启第一个任务
	prvStartFirstTask();
	return 0;
}

/* 开启第一个任务, */
__asm void prvStartFirstTask(void){
	PRESERVE8

	// 设置MSP栈指针,这里其实可以省略,因为再复位的时候已经做了MSP栈指针的初始化
	ldr r0, =0xE000ED08
	ldr r0, [r0]           // 读取SCB_VTOR寄存器的值,也就是向量表的起始地址,也就是MSP指针的地址
	ldr r0, [r0]           // 读取MSP指针的值也就是此时MSP栈顶指针的值
	msr msp, r0            // 设置栈指针

	// 使能全局中断
	cpsie i                // 开全局中断
	cpsie f                // 开异常
	dsb
	isb

	// 调用SVC去启动第一个函数
	svc 0
	nop
	nop
}

/* SVC中断服务函数
    其实就是把在pxPortInitialiseStack函数里存储到栈里的一些寄存器的值加载到CPU寄存器
	在执行完这个函数之后,已经把当前任务的入口地址添加到了CPU的PC寄存器里，因此函数返回之后就会自动去执行任务函数
	任务运行在PSP栈上，需要把PSP栈指针寄存器设置为当前任务的栈顶指针
*/
__asm void vPortSVCHandler(void){
	extern pxCurrentTCB;          // 这里传入的是pxCurrentTCB的地址
	PRESERVE8

	ldr r3, =pxCurrentTCB         // 获取pxCurrentTCB的地址
	ldr r1, [r3]                  // 读取pxCurrentTCB的值
	ldr r0, [r1]                  // 获取pxCurrentTCB指向pxTopOfStack的值,就是当前任务栈空间的首地址
	
	ldmia r0!,{r4-r11}            // 从任务栈中读取R4-R11寄存器的值并赋给R4-R11;
	msr psp,r0                    // 设置PSP栈指针,也就是当前任务的栈顶指针,之后CPU会从这里开始自动读取任务形参、LR寄存器、PC寄存器等
	isb
	mov r0, #0
	msr basepri, r0               // 设置中断屏蔽为0，相当于打开所有中断
	
	/*
		补充一下：在ARM Cortex-M3中LR寄存器的值为EXC_RETURN其决定了中断返回模式和栈指针
		当 Cortex-M3 发生中断（或异常）时，硬件自动完成以下操作：
			1、保存上下文：将 xPSR、PC、LR、R12、R3-R0 压入当前栈(MSP 或 PSP);
		    2、更新 LR：将 LR 设置为特殊值 EXC_RETURN，用于指示返回行为;
			3、跳转到中断服务程序（ISR）：从向量表加载 PC。
		EXC_RETURN 是存储在 LR 中的 32 位值，其高 28 位固定为 0xFFFFFFF，低 4 位定义返回行为：
			0xFFFFFFF1	返回 Handler 模式，使用 MSP（嵌套中断）
			0xFFFFFFF9	返回 Thread 模式，使用 MSP（特权级代码）
			0xFFFFFFFD	返回 Thread 模式，使用 PSP（用户级任务）
			0xFFFFFFBC	返回时启用浮点上下文（仅支持浮点的芯片，如 Cortex-M4）
		在这里选择Thread模式并使用PSP指针，中断返回之后其实就开始执行任务函数
	*/
	orr r14, #0xd                // 设置返回模式为线程模式
	bx r14                        // 退出SVC中断进入任务模式,此时返回Thread模式并使用PSP栈指针
}

/* 开启PendSV中断 */
void portYIELD(void)
{   
	/* 触发PendSV中断 */
	portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;  
	// 下面两条指令就是确保所有指令都执行完            
	__dsb(portSY_FULL_READ_ERITE);                  
	__isb(portSY_FULL_READ_ERITE);                              
}

#if 0
void vTaskSwitchContext(void){
	extern TCB_t TASK1TCB;  // 声明任务1的TCB
	extern TCB_t TASK2TCB;  // 声明任务2的TCB
	if(pxCurrentTCB == &TASK1TCB){
		pxCurrentTCB = &TASK2TCB;
	}
	else{
		pxCurrentTCB = &TASK1TCB;
	}
}
#else	
#endif

/* 任务切换上下文
	如果当前任务是空闲任务就尝试去执行任务1或任务2，主要判断任务1或任务2里的延时(xTicksToDelay)是否到期
	如果没有到期就继续执行空闲任务，反之去执行任务1或任务2
*/
void vTaskSwitchContext(void){
	extern TCB_t TASK1TCB;  // 声明任务1的TCB
	extern TCB_t TASK2TCB;  // 声明任务2的TCB
	if(pxCurrentTCB == &IdleTaskTCB){
		if(TASK1TCB.xTicksToDelay == 0){
			pxCurrentTCB = &TASK1TCB;
		}
		else if(TASK2TCB.xTicksToDelay == 0){
			pxCurrentTCB = &TASK2TCB;
		}
		else{
			return;
		}
	}
	else{
		/* 如果当前任务是任务1或任务2，判断下一个任务是否还在延时，如果还在延时就切换到空闲任务，反之则切换到另一个任务 */
		if(pxCurrentTCB == &TASK1TCB){
			if(TASK2TCB.xTicksToDelay == 0){
				pxCurrentTCB = &TASK2TCB;      // 切换到任务2
			}
			else if(pxCurrentTCB->xTicksToDelay != 0){
				pxCurrentTCB = &IdleTaskTCB;   // 切换到空闲任务
			}
			else{
				return;                        // 不切换
			}
		}
		else if(pxCurrentTCB == &TASK2TCB){
			if(TASK1TCB.xTicksToDelay == 0){
				pxCurrentTCB = &TASK1TCB;      // 切换到任务1
			}
			else if(pxCurrentTCB->xTicksToDelay != 0){
				pxCurrentTCB = &IdleTaskTCB;   // 切换到空闲任务
			}
			else{
				return;                        // 不切换
			}
		}
	}
}

/* 定义PendSV中断服务函数 */
__asm void xPortPendSVHandler(void){
	extern pxCurrentTCB;
	extern vTaskSwitchContext;
	PRESERVE8

	/* 保存当前任务的上下文
	在进入PendSV中断服务函数时,CPU会首先把当前任务的PSP栈指针指向任务栈顶
	然后CPU会把当前任务的xPSR、PC、LR、R12、R3-R0寄存器的值自动存储到当前任务栈空间;
	此时PSP指向R0，R14-R11寄存器的值需要手动保存到当前任务栈空间
	*/
	mrs r0, psp                    // 读取当前栈顶指针(CPU自动保存PC、LR等之后的PSP指针)
	isb
	
	ldr r3, =pxCurrentTCB          // 获取当前pxCurrentTCB的地址
	ldr r2,[r3]                    // r2存储当期pxCurrentTCB的值，也就是当前任务控制块TCB的地址
	
	stmdb r0!, {r4-r11}            // 将r4-r11寄存器的值存储到当前任务栈空间
	str r0, [r2]                   // 将当前顶地址存储到当前任务控制块pxTOPOfStack中

	// 接下来要调用vTaskSwitchContext函数,会修改R14的值因此需要先存储中断服务函数的r14寄存器
	// r3存储的是pxCurrentTCB的地址,为了避免vTaskSwitchConetx函数修改r3寄存器的值,这里先存储r3寄存器
	stmdb sp!, {r3,r14}     
	
	// 设置中断屏蔽寄存器BASEPRI的值,关中断因为需要修改pxCurrentTCB的值
	// 这里的configMAX_SYSCALL_INTERRUPT_PRIORITY是FreeRTOSConfig.h中定义的宏,表示系统调用中断屏蔽优先级
	// 这个值是191,表示优先级11,所有优先级高于11的中断都会被屏蔽
	// 这样可以确保在执行任务切换时不会被其他中断打断
	mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY
	msr basepri, r0                 
	dsb
	isb
	bl vTaskSwitchContext           // 调用上下文切换更新pxCurrentTCB的值

	// 完成任务切换后，开中断
	mov r0, #0                     // 设置中断屏蔽寄存器BASEPRI的值为0，打开所有中断
	msr basepri, r0                // 开启中断
	// 恢复中断服务函数的r14寄存器
	ldmia sp!, {r3,r14}
	
	
	ldr r1,[r3]                    // r2存储当期pxCurrentTCB的值，也就是下文任务控制块TCB的地址
	ldr r0,[r1]                    // 读取下文任务的pxTopOfStack的值,也就是下文任务栈空间的首地址
	ldmia r0!, {r4-r11}            // 从下文任务栈空间中读取R4-R11寄存器的值
	msr psp, r0                    // 设置psp指针，手动保存r4-r11寄存器的值到下文任务栈空间
	                               // 在中断退出后CPU会依据psp自动从下文栈空间中读取r0、pc寄存器等值
	isb
	bx r14                         // 中断返回,PendSV中断服务函数r14被设置为0xfffffffd（硬件设置的）
	nop
}


void vTaskDelay(const TickType_t xTicksToDelay){
	TCB_t* pxTCB = pxCurrentTCB;                  // 获取当前任务控制块句柄
	pxCurrentTCB->xTicksToDelay = xTicksToDelay;  // 设置当前任务的延时ticks周期数
	portYIELD();
}


/* SysTick中断服务函数 */
void xPortSysTickHandler(void){
	// 进入临界区
	uint32_t uxSavedInterruptStatus;
	uxSavedInterruptStatus = ulPortRaiseBASEPRI();       // 关中断，保存当前中断屏蔽寄存器的值
	xTaskIncrementTick();                                // 更新系统滴答计数器
	taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);  // 退出临界区，带中断返回值
	// 退出临界区
}

/* 系统更新时基函数 */
void xTaskIncrementTick(void){
	xTickCount++;
	TCB_t* pxTCB = NULL;

	// 遍历就绪链表数组并且更新所有任务的延时计数器xTicksToDelay
	for (UBaseType_t i = 0;i<configMAX_PRIORITIES;i++){
		List_t* pxReadyTasksList = &pxReadyTasksLists[i];  // 获取当前就绪链表
		pxTCB = (TCB_t*)listGET_OWNER_OF_NEXT_ENTRY(pxReadyTasksList);
		if (pxTCB->xTicksToDelay > 0){
			pxTCB->xTicksToDelay--;  // 延时计数器减1
		}
	}

	portYIELD();           // 触发PendSV中断,进行任务切换
}

