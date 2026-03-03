/*
 * timbase.c
 *
 *  Created on: Feb 26, 2026
 *      Author: night
 */
#include "f429zi_reg.h"

#include <timebase.h>
#include <gpio.h>
#include <clock.h>

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

//PB11 AF:TIM2 CH4
int tim2_pwm_output(void){
	uint32_t tim2_clk=0;
	 tim2_clk=get_tim_clk();

	*APB1ENR |= (1U << 0U);

	GPIO_AF(GPIO_PORT_B,11,1); //PB11
	*TIM2_CR1&=~(1U);

	*TIM2_CR1|=(1U<<7U); //auto-reload buffered

	*TIM2_PSC=(tim2_clk/1000000)- 1U; // 1MHz

	*TIM2_ARR=0x3e8U- 1U;  // 1kHz

	*TIM2_CCMR2|=(6U<<12U); // 0C4M 110 ->Pwm 1

	*TIM2_CCER|=(1U<<12U);

	*TIM2_CCR4=0x1f4;

	*TIM2_EGR |= 1U;

	*TIM2_CR1|=1U; //enable

	return 0;
}

//PA0 AF:TIM5_CH1
int tim5_pwm_capture(void){
	uint32_t tim_clk=0;
	tim_clk=get_tim_clk();

	*APB1ENR |= (1U << 3U);

	GPIO_AF(GPIO_PORT_A,0,2);

	*TIM5_CR1&=~(1U);

	*TIM5_PSC=(tim_clk/1000000)- 1U; // 1MHz

	*TIM5_ARR=0xffffffffU;  // 2147.483648[s]

	*TIM5_CCER &= ~((1U<<0) | (1U<<4));   // CC1E=0, CC2E=0

	// CCMR1:
	// CC1S=01 (IC1 <- TI1)
	// CC2S=10 (IC2 <- TI1)
	// IC1F/IC2F=0, IC1PSC/IC2PSC=0
	*TIM5_CCMR1 &= ~((3U << 0) | (3U << 2) | (0xFU << 4) |	// CC1S, IC1PSC, IC1F
			(3U << 8) | (3U << 10) | (0xFU << 12));	// CC2S, IC2PSC, IC2F

	*TIM5_CCMR1 |= ((1U << 0) | (2U << 8));	// CC1S=01  CC2S=10

	// CCER: CH1=rising, CH2=falling
	*TIM5_CCER &= ~((1U << 1) | (1U << 3) | (1U << 5) | (1U << 7)); // CC1P/CC1NP/CC2P/CC2NP
	*TIM5_CCER |= ((1U << 0) | (1U << 4) | (1U << 5)); // CC1E=1 // CC2E=1// CC2P=1

	// SMCR: Reset mode + Trigger = TI1FP1
	*TIM5_SMCR &= ~((7U << 4) | (7U << 0)); // TS:101(TI1FP1), SMS:100(Reset mode)
	*TIM5_SMCR |= (5U << 4) | (4U << 0);


	*TIM5_SR &= ~((1U << 1) | (1U << 2)); //CC1IF, CC2IF

	*TIM5_EGR |= 1U;

	*TIM5_CR1|=1U; //enable

	return 0;
}

int tim5_capture_get(uint32_t *ccr1 ,int ch){
	if (ch < 1 || ch > 4) return -1;
	volatile uint32_t *TIM5_CCR;
	uint32_t start = millis();

	    while (((*TIM5_SR) & (1U << ch)) == 0U) {
	        if ((uint32_t)(millis() - start) >= 1000) {
	            return -2;
	        }
	    }
	    TIM5_CCR=((volatile uint32_t *)((uintptr_t)0x40000C00 + 0x30U + (4*ch)));
	    *ccr1 = *TIM5_CCR;
	    return 0;
}

void tim5_freq_duty(uint32_t *freq,uint32_t *duty,int ch){
	uint32_t period;
	volatile uint32_t *Other_CCR;
	Other_CCR=((volatile uint32_t *)((uintptr_t)0x40000C00 + 0x30U + (4*(ch+1))));
	if(tim5_capture_get(&period,ch)){__asm volatile("BKPT #0");}
	*freq=1000000/period;
	*duty=((*Other_CCR)*100)/(period);
}

uint32_t get_tim_clk(void){
	uint32_t tim_clk=0;
		uint32_t pclk=0;
		//RM0090 155p
		if(*RCC_DCKCFGR&(1U<<24)){
			if(apb_tbl[(*RCC_CFGR & (7U << 10U)) >> 10U]<=4){
						tim_clk=get_hclk();
					}
					else{
						pclk=get_pclk1();
						tim_clk=pclk*4;
					}
		}
		else{
			if(apb_tbl[(*RCC_CFGR & (7U << 10U)) >> 10U] ==1){
				tim_clk=get_pclk1();

			}
			else{
				pclk=get_pclk1();
				tim_clk=pclk*2;

			}
		}
		return tim_clk;
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
