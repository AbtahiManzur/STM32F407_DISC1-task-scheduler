/*
 * main.h
 *
 *  Created on: Mar 19, 2021
 *      Author: AbMan
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>

#define MAX_TASKS						5

/*Defining macros that describe the sizes and locations of the stacks*/
#define SIZE_TASK_STACK 				1024U
#define SIZE_SCHED_STACK 				1024U

#define SRAM_START 						0x20000000U
#define SIZE_SRAM 						((128) * (1024))
#define SRAM_END 						((SRAM_START) + (SIZE_SRAM))

#define T1_STACK_START 					SRAM_END
#define T2_STACK_START 					((SRAM_END) - ((1) * (SIZE_TASK_STACK)))
#define T3_STACK_START 					((SRAM_END) - ((2) * (SIZE_TASK_STACK)))
#define T4_STACK_START 					((SRAM_END) - ((3) * (SIZE_TASK_STACK)))
#define IDLE_STACK_START				((SRAM_END) - ((4) * (SIZE_TASK_STACK)))
#define SCHED_STACK_START				((SRAM_END) - ((5) * (SIZE_SCHED_STACK)))

/*Time macros*/
#define TICK_HZ							1000U

#define HSI_CLK 						16000000U
#define SYSTICK_TIM_CLK 				HSI_CLK

/*Other macros*/
#define DUMMY_XPSR 						0x01000000U
#define TASK_READY_STATE 				0x00
#define TASK_BLOCKED_STATE 				0xFF

/*Task control block*/
typedef struct {
	uint32_t psp_value;
	uint32_t sleep_count;
	uint8_t current_state;
	void (*task_handler)(void);
}TCB_t;

/*Function prototypes*/
void init_systick_timer(uint32_t tick_hz); //Used to initialize the systick timer for the scheduler
__attribute__((naked)) void init_sched_stack(uint32_t sched_stack_start); //Initializing dummy stack for scheduler
void init_tasks_stack(void);//Initializing dummy stacks for all the tasks
__attribute__((naked)) void switch_sp_to_psp(void);//Switching from using MSP to PSP for tasks and initializes task 1 psp
void enable_processor_faults(void);//Enables fault handling of the processor
void task_delay(uint32_t tick_count);
void update_global_tick(void);
void task_yield(void);
void unblock_tasks(void);

/*Helper functions*/
uint32_t get_current_psp(void);
void update_current_psp(uint32_t current_psp);
void update_current_task(void);

/*Task prototypes*/
void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);
void idle_task_handler(void);

#endif /* MAIN_H_ */
