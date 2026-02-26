/*
 * clock.c
 *
 *  Created on: Feb 26, 2026
 *      Author: night
 */
#include "clock.h"

int hse_enable(void){
	*RCC_CR&=~(1U<<18U);
	*RCC_CR|=(1U<<16U);

	if (wait_set(RCC_CR, (1U<<17U), 3000000U) < 0) return -2;

	return 0;
}
int clock_hse_pll(void){
	if(hse_enable()){
		return -1;
	}
	*RCC_CR&=~(1U<<24U); //PLL OFF
	if (wait_clear(RCC_CR, (1U<<25), 3000000U) < 0) return -2;

	*RCC_PLLCFGR|=(1U<<22U); //hse

	*RCC_PLLCFGR&=~(0x3fU);
	*RCC_PLLCFGR|=(1U<<2U); //PLLM /4

	*RCC_PLLCFGR&=~(0x1ffU<<6U);
	*RCC_PLLCFGR|=(0xa8U<<6U); //PLLN: 168

	*RCC_PLLCFGR&=~(3U<<16U); //PLLP: /2

	*RCC_CR|=(1U<<24U); //PLL ON

	if (wait_set(RCC_CR, (1U << 25), 3000000U) < 0) return -2;

	*FLASH_ACR &= ~0x7U;  // LATENCY[2:0] clear
	*FLASH_ACR |=  5U;    // 5 wait states

	*RCC_CFGR &=~((3U)|(7U<<10U)|(7U<<13U));
	*RCC_CFGR |=(2U)|(5U<<10U)|(4U<<13U); //set hse ,APB1:/4 APB2:/2

	if (wait_set(RCC_CFGR, (1U << 2), 3000000U) < 0) return -2;
	if (wait_clear(RCC_CFGR, (1U << 3), 3000000U) < 0) return -2;

	return 0;

}

static int wait_set(volatile uint32_t *reg, uint32_t mask, uint32_t loop_max)
{
    for (volatile uint32_t i = 0; i < loop_max; i++) {
        if ((*reg & mask) != 0U) return 0;
    }
    return -1;
}

static int wait_clear(volatile uint32_t *reg, uint32_t mask, uint32_t loop_max)
{
    for (volatile uint32_t i = 0; i < loop_max; i++) {
        if ((*reg & mask) == 0U) return 0;
    }
    return -1;
}
