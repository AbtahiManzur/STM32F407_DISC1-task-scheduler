/*
 * led.c
 *
 *  Created on: Mar 20, 2021
 *      Author: AbMan
 */

#include <stdint.h>
#include "led.h"

void led_init_all(void) {
	//First turn on the clocks for port D
	uint32_t *pRCC_AHB1ENR = (uint32_t*)0x40023830;

	*pRCC_AHB1ENR |= (1 << 3);

	//Next set pins 12-15 of GPIOD to output mode
	uint32_t *pGPIOD_MODER = (uint32_t*)0x40020C00;
	*pGPIOD_MODER &= ~(0xFF000000);
	*pGPIOD_MODER |= (0x55 << 24);

	led_off(LED_BLUE);
	led_off(LED_GREEN);
	led_off(LED_ORANGE);
	led_off(LED_RED);
}

void led_on(uint8_t led_no) {
	uint32_t *pGPIOD_ODR = (uint32_t*)0x40020C14;
	*pGPIOD_ODR |= (1 << led_no);
}

void led_off(uint8_t led_no) {
	uint32_t *pGPIOD_ODR = (uint32_t*)0x40020C14;
	*pGPIOD_ODR &= ~(1 << led_no);
}

void delay(uint32_t count) {
	for(uint32_t i = 0; i < count; i++);
}
