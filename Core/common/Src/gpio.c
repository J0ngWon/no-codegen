/*
 * gpio.c
 *
 *  Created on: Mar 2, 2026
 *      Author: night
 */


#include "f429zi_reg.h"
#include <gpio.h>


int GPIO_Enable(gpio_port_t port){
	if (port > GPIO_PORT_K) return 1;
	*AHB1ENR |=(1U<<port);
	return 0;
}

int GPIO_Write(gpio_port_t port ,uint32_t pin, uint32_t level){
	volatile uint32_t*base,*GPIOx_MODER,*GPIOx_BSRR;
	uint32_t const GPIO_MOD_Mask = (0x1U << (2U * pin));
	uint32_t const GPIO_MOD_ClearMask = ~(0x3U << (2U * pin));

	if(pin>15) return 1 ;

	base =(volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (port * 0x400));
	GPIOx_MODER=(volatile uint32_t*)((uintptr_t)base + 0x00U);
	GPIOx_BSRR =(volatile uint32_t*)((uintptr_t)base + 0x18U);

	GPIO_Enable(port);
	*GPIOx_MODER= (*GPIOx_MODER & GPIO_MOD_ClearMask) | GPIO_MOD_Mask;

	if (level == 1)
		*GPIOx_BSRR = (1U << pin);
	else
		*GPIOx_BSRR = (1U << (pin+16U));
	return 0;
}

int GPIO_AF(gpio_port_t port ,uint32_t pin_num,uint32_t af_num){
	volatile uint32_t* GPIOx_MODER= (volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (port * 0x400) + 0x00U);
	volatile uint32_t* GPIOx_AFRL= (volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (port * 0x400) + 0x20U);
	volatile uint32_t* GPIOx_AFRH= (volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (port * 0x400) + 0x24U);

	uint32_t const GPIO_MOD_Mask = (2U << (2U * pin_num));
	uint32_t const GPIO_MOD_ClearMask = ~(3U << (2U * pin_num));

	if(af_num>15 ||pin_num>15){
		return -1;
	}

	GPIO_Enable(port);
	*GPIOx_MODER= (*GPIOx_MODER & GPIO_MOD_ClearMask) | GPIO_MOD_Mask;


	if (pin_num > 7) {
		uint32_t const GPIO_AFRH_Mask = (af_num << ((pin_num - 8) * 4U));
		uint32_t const GPIO_AFRH_ClearMask = ~(0xfU << ((pin_num - 8) * 4U));
		*GPIOx_AFRH= (*GPIOx_AFRH & GPIO_AFRH_ClearMask) | GPIO_AFRH_Mask;
	} else {
		uint32_t const GPIO_AFRL_Mask = (af_num << (pin_num * 4U));
		uint32_t const GPIO_AFRL_ClearMask = ~(0xfU << (pin_num * 4U));
		*GPIOx_AFRL= (*GPIOx_AFRL & GPIO_AFRL_ClearMask) | GPIO_AFRL_Mask;
	}

	return 0;
}
