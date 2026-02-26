/*
 * f429zi_reg.h
 *
 *  Created on: Jan 8, 2026
 *      Author: night
 */

#ifndef INC_F429ZI_REG_H_
#define INC_F429ZI_REG_H_

#include <stdint.h>

/* ===== FLASH ===== */
#define FLASH_ACR ((volatile uint32_t *)(0x40023C00U + 0x00U))

/* ===== RCC ===== */

#define RCC_BASE        (volatile uint32_t *)(0x40023800U)

#define RCC_CR          (volatile uint32_t *)(0x40023800U)
#define RCC_PLLCFGR        (volatile uint32_t *)(0x40023800U+0x04U)
#define RCC_CFGR        (volatile uint32_t *)(0x40023800U+0x08U)


#define AHB1ENR         ((volatile uint32_t *)(0x40023800U + 0x30U))
#define APB1ENR         ((volatile uint32_t *)(0x40023800U + 0x40U))
#define APB2ENR         ((volatile uint32_t *)(0x40023800U + 0x44U))

/* ===== GPIO ===== */
#define GPIO_BASE_VAL   ((volatile uint32_t *)(0x40020000U + 0x00U))

#define GPIOB_MODER_LED     ((volatile uint32_t *)(0x40020400U + 0x00U))
#define GPIOB_BSRR_LED      ((volatile uint32_t *)(0x40020400U + 0x18U))

/* ===== SysTick ===== */
#define STK_CTRL        ((volatile uint32_t *)(0xE000E010U + 0x00U))
#define STK_LOAD        ((volatile uint32_t *)(0xE000E010U + 0x04U))
#define STK_VAL         ((volatile uint32_t *)(0xE000E010U + 0x08U))

/* ===== USART6 ===== */
#define USART6_BASE     ((volatile uint32_t *)(0x40011400U + 0x00U))

#define USART_CR1   ((volatile uint32_t *)((uintptr_t)USART6_BASE + 0x0CU))
#define USART_CR2   ((volatile uint32_t *)((uintptr_t)USART6_BASE + 0x10U))
#define USART_BRR   ((volatile uint32_t *)((uintptr_t)USART6_BASE + 0x08U))
#define USART_DR    ((volatile uint32_t *)((uintptr_t)USART6_BASE + 0x04U))
#define USART_SR    ((volatile uint32_t *)((uintptr_t)USART6_BASE + 0x00U))

/* ===== I2C ===== */
#define I2C1_BASE   (volatile uint32_t *)(0x40005400)


#endif /* INC_F429ZI_REG_H_ */
