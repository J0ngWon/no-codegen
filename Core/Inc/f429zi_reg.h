/*
 * f429zi_reg.h
 *
 *  Created on: Jan 8, 2026
 *      Author: night
 */

#ifndef INC_F429ZI_REG_H_
#define INC_F429ZI_REG_H_

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

/* ===== RCC ===== */
#define AHB1ENR         ((volatile uint32_t *)(0x40023800U + 0x30U))
#define RCC_CFGR=(volatile uint32_t *)(0x40023800+0x08U);

/* ===== GPIO ===== */
#define GPIO_BASE_VAL   ((volatile uint32_t *)(0x40020000U + 0x00U))

#define GPIOB_MODER     ((volatile uint32_t *)(0x40020400U + 0x00U))
#define GPIOB_BSRR      ((volatile uint32_t *)(0x40020400U + 0x18U))

/* ===== SysTick ===== */
#define STK_CTRL        ((volatile uint32_t *)(0xE000E010U + 0x00U))
#define STK_LOAD        ((volatile uint32_t *)(0xE000E010U + 0x04U))
#define STK_VAL         ((volatile uint32_t *)(0xE000E010U + 0x08U))


#endif /* INC_F429ZI_REG_H_ */
