/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include "main.h"
#include "led.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

uint32_t current_task = 1;

uint32_t g_tick_count;

TCB_t user_tasks[MAX_TASKS];

int main(void)
{
	enable_processor_faults();

	init_sched_stack(SCHED_STACK_START);

	init_tasks_stack();

	led_init_all();

	init_systick_timer(TICK_HZ);

	switch_sp_to_psp();

	task1_handler();

	while(1);
}

void task1_handler(void) {
	while(1) {
		led_on(LED_BLUE);
		//delay(DELAY_COUNT_250MS);
		task_delay(250);
		led_off(LED_BLUE);
		//delay(DELAY_COUNT_250MS);
		task_delay(250);
	}
}
void task2_handler(void) {
	while(1) {
		led_on(LED_GREEN);
		//delay(DELAY_COUNT_1S);
		task_delay(1000);
		led_off(LED_GREEN);
		//delay(DELAY_COUNT_1S);
		task_delay(1000);
	}
}
void task3_handler(void) {
	while(1) {
		led_on(LED_ORANGE);
		//delay(DELAY_COUNT_500MS);
		task_delay(500);
		led_off(LED_ORANGE);
		//delay(DELAY_COUNT_500MS);
		task_delay(500);
	}
}
void task4_handler(void) {
	while(1) {
		led_on(LED_RED);
		//delay(DELAY_COUNT_125MS);
		task_delay(125);
		led_off(LED_RED);
		//delay(DELAY_COUNT_125MS);
		task_delay(125);
	}
}

void idle_task_handler(void) {
	while(1);
}

void init_systick_timer(uint32_t tick_hz) {
	uint32_t *pSRVR = (uint32_t*)0xE000E014;
	uint32_t *pSCSR = (uint32_t*)0xE000E010;

	uint32_t count_value = SYSTICK_TIM_CLK/tick_hz - 1;

	//Clear SRVR
	*pSRVR &= ~(0x00FFFFFF);

	//load count into SRVR
	*pSRVR |= count_value;

	//initializations
	*pSCSR |= (1 << 1); //Enables Systick exception
	*pSCSR |= (1 << 2); //Uses processor clock as source

	//enable the timer
	*pSCSR |= (1 << 0);
}

__attribute__((naked)) void init_sched_stack(uint32_t sched_stack_start) {
	__asm volatile("MSR MSP,%0": : "r" (sched_stack_start) : );
	__asm volatile("BX LR");
}

void init_tasks_stack(void) {
	uint32_t *pPSP;

	user_tasks[0].psp_value = IDLE_STACK_START;
	user_tasks[1].psp_value = T1_STACK_START;
	user_tasks[2].psp_value = T2_STACK_START;
	user_tasks[3].psp_value = T3_STACK_START;
	user_tasks[4].psp_value = T4_STACK_START;

	user_tasks[0].task_handler = idle_task_handler;
	user_tasks[1].task_handler = task1_handler;
	user_tasks[2].task_handler = task2_handler;
	user_tasks[3].task_handler = task3_handler;
	user_tasks[4].task_handler = task4_handler;

	user_tasks[0].current_state = TASK_READY_STATE;
	user_tasks[1].current_state = TASK_READY_STATE;
	user_tasks[2].current_state = TASK_READY_STATE;
	user_tasks[3].current_state = TASK_READY_STATE;
	user_tasks[4].current_state = TASK_READY_STATE;

	user_tasks[0].sleep_count = 0;
	user_tasks[1].sleep_count = 0;
	user_tasks[2].sleep_count = 0;
	user_tasks[3].sleep_count = 0;
	user_tasks[4].sleep_count = 0;

	for(int i = 0; i < MAX_TASKS; i++) {
		pPSP = (uint32_t*) user_tasks[i].psp_value;

		pPSP--; //XPSR
		*pPSP = DUMMY_XPSR; //0x01000000 to have thumb state enabled.

		pPSP--; //PC
		*pPSP = (uint32_t)user_tasks[i].task_handler;

		pPSP--; //LR
		//*pPSP = 0xFFFFFFFD; //This LR is to tell the processor to unstack using the PSP and use the PSP for thread mode.
		*pPSP = 0; //Actually it doesn't matter what the LR is since its already EXC_RETURN on exception entry.

		for(int j = 0; j < 13; j++) {
			pPSP--;
			//*pPSP = 0;
		}

		user_tasks[i].psp_value = (uint32_t)pPSP;
	}
}

uint32_t get_current_psp(void) {
	return user_tasks[current_task].psp_value;
}

void update_current_psp(uint32_t current_psp) {
	user_tasks[current_task].psp_value = current_psp;
}

void update_current_task(void) {
	for(uint32_t i = 0; i < MAX_TASKS; i++) {
		current_task++;
		current_task %= MAX_TASKS;
		if((user_tasks[current_task].current_state == TASK_READY_STATE) && (current_task != 0)){
			return;
		}
	}
	current_task = 0;
}

__attribute__((naked)) void switch_sp_to_psp(void) {
	//First save the current LR to go back to main
	__asm volatile("PUSH {LR}");

	//Get the current task's PSP and initialize the PSP register with it
	__asm volatile("BL get_current_psp");
	__asm volatile("MSR PSP, R0");

	//Restore LR
	__asm volatile("POP {LR}");

	//Next modify bit 1 in the CONTROL register to use PSP for tasks instead of the default MSP
	__asm volatile("MOV R0, #0x000002");
	__asm volatile("MSR CONTROL, R0");
	__asm volatile("BX LR");
}

void enable_processor_faults(void) {
	uint32_t *pSHCSR = (uint32_t*)0xE000ED24;

	*pSHCSR |= ( 1 << 16 ); //mem manage
	*pSHCSR |= ( 1 << 17 ); //bus fault
	*pSHCSR |= ( 1 << 18 ); //usage fault
}

void SysTick_Handler(void) {
	update_global_tick();
	unblock_tasks();
	task_yield();
}

__attribute__((naked)) void PendSV_Handler(void) {
	//Save the EXC_RETURN since we will be branching to multiple sub-routines
	__asm volatile("PUSH {LR}");
	//First obtain current PSP, since we can't use PUSH/POP as handler mode only uses MSP
	__asm volatile("MRS R0, PSP");
	//Then stack the rest of the context, R4-R11 using PSP which is located in R0
	__asm volatile("STMDB R0!, {R4-R11}");
	//Save this PSP
	__asm volatile("BL update_current_psp");
	//update current task
	__asm volatile("BL update_current_task");
	//get the PSP of this task
	__asm volatile("BL get_current_psp");
	//load R4-R11 of the next task
	__asm volatile("LDMIA R0!, {R4-R11}");
	//move R0 into PSP
	__asm volatile("MSR PSP, R0");
	//pop back EXC_RETURN
	__asm volatile("POP {LR}");
	//branch to EXC_RETURN
	__asm volatile("BX LR");
}

void task_delay(uint32_t tick_count) {
	user_tasks[current_task].sleep_count = g_tick_count + tick_count;
	user_tasks[current_task].current_state = TASK_BLOCKED_STATE;
	task_yield();
}

void task_yield(void) {
	//Pend the PendSV Handler where the context switching occurs
	uint32_t *pICSR = (uint32_t*)0xE000ED04;
	*pICSR |= (1 << 28);
}

void unblock_tasks(void) {
	for(uint32_t i = 0; i < MAX_TASKS; i++) {
		if(user_tasks[i].current_state == TASK_BLOCKED_STATE) {
			if(user_tasks[i].sleep_count == g_tick_count) {
				user_tasks[i].current_state = TASK_READY_STATE;
			}
		}
	}
}

void update_global_tick(void) {
	g_tick_count++;
}

void HardFault_Handler(void) {
	printf("Exception: Hard fault\n");
	while(1);
}

void MemManage_Handler(void) {
	printf("Exception: Memory manage fault\n");
	while(1);
}

void BusFault_Handler(void) {
	printf("Exception: Bus fault\n");
	while(1);
}

void UsageFault_Handler(void) {
	printf("Exception: Usage fault\n");
	while(1);
}
