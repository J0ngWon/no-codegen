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
	extern const uint16_t apb_tbl[8];

	uint32_t tim2_clk=0;

	//RM0090 155p
	if(*RCC_DCKCFGR&(1U<<24)){
		if(apb_tbl[(*RCC_CFGR & (7U << 10U)) >> 10U]<=4){
					tim2_clk=get_hclk();
				}
				else{
					tim2_clk=(get_pclk1())*4;
				}
	}
	else{
		if(apb_tbl[(*RCC_CFGR & (7U << 10U)) >> 10U]==1){
			tim2_clk=get_pclk1();
		}
		else{
			tim2_clk=(get_pclk1())*2;
		}
	}
	GPIO_AF(GPIO_PORT_B,11,1); //PB11

	*TIM2_CR1=1U | (1U<<7U); //enable auto-reload buffered

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
