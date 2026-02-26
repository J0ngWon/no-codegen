/*
 * timbase.c
 *
 *  Created on: Feb 26, 2026
 *      Author: night
 */


#include <timebase.h>

static volatile uint32_t sys_ms=0;

void timebase_isr_tick(void){
	 sys_ms++;
}

void delay(uint32_t ms){
	uint32_t start = millis();
	while ((uint32_t)(millis() - start) < ms) { }
}


uint32_t millis(void){
	return sys_ms;
}
