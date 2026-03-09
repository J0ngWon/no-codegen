/*
 * gpio.h
 *
 *  Created on: Feb 26, 2026
 *      Author: night
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_

#include <stdint.h>

typedef enum
{
	GPIO_PORT_A = 0,
	GPIO_PORT_B,
	GPIO_PORT_C,
	GPIO_PORT_D,
	GPIO_PORT_E,
	GPIO_PORT_F,
	GPIO_PORT_G,
	GPIO_PORT_H,
	GPIO_PORT_I,
	GPIO_PORT_J,
	GPIO_PORT_K,
}gpio_port_t;

int GPIO_Enable(gpio_port_t port);
int GPIO_Write(gpio_port_t port ,uint32_t pin, uint32_t level);
int GPIO_AF(gpio_port_t port ,uint32_t pin_num,uint32_t af_num);

#endif /* INC_GPIO_H_ */
