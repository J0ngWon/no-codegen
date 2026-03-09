/*
 * clock.c
 *
 *  Created on: Feb 26, 2026
 *      Author: night
 */
#include "clock.h"

#define HSE_HZ 8000000U
#define LSE_HZ 32768U
#define HSI_HZ 16000000U


const uint8_t apb_tbl[8] = {
	    1,  // 000 -> /1
	    1,  // 001 -> /1
	    1,  // 010 -> /1
	    1,  // 011 -> /1
	    2,  // 100 -> /2
	    4,  // 101 -> /4
	    8,  // 110 -> /8
	    16  // 111 -> /16
	};
const uint16_t ahb_tbl[16] = {
	    1,   // 0000 -> /1
	    1,   // 0001 -> /1
	    1,   // 0010 -> /1
	    1,   // 0011 -> /1
	    1,   // 0100 -> /1
	    1,   // 0101 -> /1
	    1,   // 0110 -> /1
	    1,   // 0111 -> /1
	    2,   // 1000 -> /2
	    4,   // 1001 -> /4
	    8,   // 1010 -> /8
	    16,  // 1011 -> /16
	    64,  // 1100 -> /64
	    128, // 1101 -> /128
	    256, // 1110 -> /256
	    512  // 1111 -> /512
	};
	static const uint16_t pllp_tbl[4] = {
	    2, 4, 6, 8   // RM0090 166p
	};
static uint32_t j_hclk  = 0U;
static uint32_t j_pclk1 = 0U;
static uint32_t j_pclk2 = 0U;

uint32_t get_hclk(void){return j_hclk;}

uint32_t get_pclk1(void){return j_pclk1;}

uint32_t get_pclk2(void){return j_pclk2;}

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

int clock_update(void) {
	uint32_t num,pll_m = 0, pll_n = 0, pll_p = 0, source_clk = 0,vco=0,pll_output=0,sys_clk=0;
	num=(*RCC_CFGR &(3U << 2))>>2; //00:HSI 10:HSE 10:PLL
	switch (num) {
	case 0:
		sys_clk= HSI_HZ
		;
		j_hclk = sys_clk / ahb_tbl[(*RCC_CFGR & (0xfU << 4U)) >> 4U];

		j_pclk1 = j_hclk / apb_tbl[(*RCC_CFGR & (7U << 10U)) >> 10U];

		j_pclk2 = j_hclk / apb_tbl[(*RCC_CFGR & (7U << 13U)) >> 13U];
		return 0;
	case 1:
		sys_clk = HSE_HZ
		;
		j_hclk = sys_clk / ahb_tbl[(*RCC_CFGR & (0xfU << 4U)) >> 4U];

		j_pclk1 = j_hclk / apb_tbl[(*RCC_CFGR & (7U << 10U)) >> 10U];

		j_pclk2 = j_hclk / apb_tbl[(*RCC_CFGR & (7U << 13U)) >> 13U];

		return 0;
	case 2: //PLL
		pll_m = (*RCC_PLLCFGR & 0x3fU);
		pll_n = (*RCC_PLLCFGR & (0x1ffU << 6U))>>6U;
		pll_p = pllp_tbl[(*RCC_PLLCFGR & (0x3U << 16U))>>16U];
		if (*RCC_PLLCFGR & (1U << 22U)){
			source_clk = HSE_HZ;
		}
		else{
			source_clk = HSI_HZ;
		}
		vco=source_clk*pll_n/pll_m;
		pll_output=vco/pll_p;

		j_hclk=pll_output/ahb_tbl[(*RCC_CFGR&(0xfU<<4U))>>4U];

		j_pclk1=j_hclk/apb_tbl[(*RCC_CFGR&(7U<<10U))>>10U];

		j_pclk2=j_hclk/apb_tbl[(*RCC_CFGR&(7U<<13U))>>13U];

		return 0;
	default:
		return -1;
	}
}

int hse_enable(void){
	*RCC_CR&=~(1U<<18U);
	*RCC_CR|=(1U<<16U);

	if (wait_set(RCC_CR, (1U<<17U), 3000000U) < 0) return -2;

	return 0;
}
int clock_hse_pll_168(void){
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
	*RCC_CFGR |=(2U)|(5U<<10U)|(4U<<13U); //set pll ,APB1:/4 APB2:/2

	if (wait_clear(RCC_CFGR, (1U << 2), 3000000U) < 0) return -2;
	if (wait_set(RCC_CFGR, (1U << 3), 3000000U) < 0) return -2;

	clock_update();
	return 0;

}


