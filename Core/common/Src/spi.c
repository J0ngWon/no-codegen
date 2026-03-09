/*
 * spi.c
 *
 *  Created on: Mar 9, 2026
 *      Author: night
 */

#include "f429zi_reg.h"
#include "spi.h"
#include "gpio.h"


//int GPIO_AF(gpio_port_t port ,uint32_t pin_num,uint32_t af_num)

void spi4_init(void){
	*APB2ENR |= (1U<<13U)
	GPIO_Enable(GPIO_PORT_E);
	GPIO_AF(GPIO_PORT_E,12,5); // MOSI
	GPIO_AF(GPIO_PORT_E,13,5); // MISO
	GPIO_AF(GPIO_PORT_E,14,5); // SCK


}
