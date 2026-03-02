/*
 * timbase.c
 *
 *  Created on: Feb 26, 2026
 *      Author: night
 */
#include "f429zi_reg.h"

#include <timebase.h>
#include <gpio.h>

static volatile uint32_t sys_ms=0;

void timebase_isr_tick(void){
	 sys_ms++;
}

void sys_init(uint32_t tick_hz)
{
    uint32_t hclk = get_hclk();
    uint32_t reload = (hclk / tick_hz) - 1U;

    *STK_LOAD = reload;
    *STK_VAL  = 0U;
    *STK_CTRL = (1U<<2) | (1U<<1) | (1U<<0);
}

int tim2_init(void){
	GPIO_Enable(GPIO_PORT_B);

	return 0;
}


void old_sys_init(void){
	*STK_LOAD = 0x3e7fU;  //hard coding!!!
	*STK_VAL  = 0U;
	*STK_CTRL = 0x7U;
}


void delay(uint32_t ms){
	uint32_t start = millis();
	while ((uint32_t)(millis() - start) < ms) { }
}


uint32_t millis(void){
	return sys_ms;
}
