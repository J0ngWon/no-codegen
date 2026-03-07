/*
 * f429zi_reg.h
 *
 *  Created on: Jan 8, 2026
 *      Author: night
 */

#ifndef INC_F429ZI_REG_H_
#define INC_F429ZI_REG_H_

#include <stdint.h>

/* ===== NVIC ===== */
#define NVIC_BASE        ((volatile uint32_t *)(0xE000E000))

/* ===== SCB ===== */
#define SCB_BASE      ((volatile uint32_t *)(0xE000ED00))
#define SCB_VTOR      ((volatile uint32_t*)((uintptr_t)SCB_BASE + 0x08U))
#define SCB_SHCSR     ((volatile uint32_t*)((uintptr_t)SCB_BASE + 0x24UL))
#define SCB_CFSR      ((volatile uint32_t*)((uintptr_t)SCB_BASE + 0x28U))
#define SCB_HFSR      ((volatile uint32_t*)((uintptr_t)SCB_BASE + 0x2CUL))
#define SCB_MMFAR     ((volatile uint32_t*)((uintptr_t)SCB_BASE + 0x34UL))
#define SCB_BFAR      ((volatile uint32_t*)((uintptr_t)SCB_BASE + 0x38UL))
#define SCB_AFSR      ((volatile uint32_t*)((uintptr_t)SCB_BASE + 0x3CUL))


/* ===== SysTick ===== */
#define STK_CTRL        ((volatile uint32_t *)(0xE000E010U + 0x00U))
#define STK_LOAD        ((volatile uint32_t *)(0xE000E010U + 0x04U))
#define STK_VAL         ((volatile uint32_t *)(0xE000E010U + 0x08U))

/* ===== FLASH ===== */
#define FLASH_ACR ((volatile uint32_t *)(0x40023C00U + 0x00U))

/* ===== RCC ===== */

#define RCC_BASE        (volatile uint32_t *)(0x40023800U)

#define RCC_CR          (volatile uint32_t *)(0x40023800U)
#define RCC_PLLCFGR        (volatile uint32_t *)(0x40023800U+0x04U)
#define RCC_CFGR        (volatile uint32_t *)(0x40023800U+0x08U)
#define RCC_DCKCFGR      (volatile uint32_t *)(0x40023800U+0x8CU)


#define AHB1ENR         ((volatile uint32_t *)(0x40023800U + 0x30U))
#define APB1ENR         ((volatile uint32_t *)(0x40023800U + 0x40U))
#define APB2ENR         ((volatile uint32_t *)(0x40023800U + 0x44U))

/* ===== GPIO ===== */
#define GPIO_BASE_VAL   ((volatile uint32_t *)(0x40020000U + 0x00U))

#define GPIOB_MODER ((volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (GPIO_PORT_B * 0x400U) + 0x00U))
#define GPIOB_OTYPER  ((volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (GPIO_PORT_B * 0x400U) + 0x04U))
#define GPIOB_OSPEEDR ((volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (GPIO_PORT_B * 0x400U) + 0x08U))
#define GPIOB_PUPDR  ((volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (GPIO_PORT_B * 0x400U) + 0x0CU))
#define GPIOB_MODER_LED     ((volatile uint32_t *)(0x40020400U + 0x00U))
#define GPIOB_BSRR_LED      ((volatile uint32_t *)(0x40020400U + 0x18U))

/* ===== USART6 ===== */
#define USART6_BASE     ((volatile uint32_t *)(0x40011400U + 0x00U))

#define USART6_CR1   ((volatile uint32_t *)((uintptr_t)USART6_BASE + 0x0CU))
#define USART6_CR2  ((volatile uint32_t *)((uintptr_t)USART6_BASE + 0x10U))
#define USART6_BRR   ((volatile uint32_t *)((uintptr_t)USART6_BASE + 0x08U))
#define USART6_DR    ((volatile uint32_t *)((uintptr_t)USART6_BASE + 0x04U))
#define USART6_SR    ((volatile uint32_t *)((uintptr_t)USART6_BASE + 0x00U))

/* ===== I2C ===== */
#define I2C1_BASE   (volatile uint32_t *)(0x40005400)
#define I2C1_CR1   ((volatile uint32_t*)((uintptr_t)I2C1_BASE + 0x00U))
#define I2C1_CR2     ((volatile uint32_t*)((uintptr_t)I2C1_BASE + 0x04U))
#define I2C1_OAR1    ((volatile uint32_t*)((uintptr_t)I2C1_BASE + 0x08U))
#define I2C1_DR      ((volatile uint32_t*)((uintptr_t)I2C1_BASE + 0x10U))
#define I2C1_SR1     ((volatile uint32_t*)((uintptr_t)I2C1_BASE + 0x14U))
#define I2C1_SR2     ((volatile uint32_t*)((uintptr_t)I2C1_BASE + 0x18U))
#define I2C1_CCR     ((volatile uint32_t*)((uintptr_t)I2C1_BASE + 0x1CU))
#define I2C1_TRISE  ((volatile uint32_t*)((uintptr_t)I2C1_BASE + 0x20U))
/* ===== TIM ===== */
#define TIM2_BASE    ((volatile uint32_t *)((uintptr_t)0x40000000 + 0x00U))
#define TIM2_CR1    ((volatile uint32_t *)((uintptr_t)TIM2_BASE  + 0x00U))
#define TIM2_CR2    ((volatile uint32_t *)((uintptr_t)TIM2_BASE  + 0x04U))
#define TIM2_SR    ((volatile uint32_t *)((uintptr_t)TIM2_BASE  + 0x10U))
#define TIM2_EGR    ((volatile uint32_t *)((uintptr_t)TIM2_BASE  + 0x14U))
#define TIM2_CCMR1    ((volatile uint32_t *)((uintptr_t)TIM2_BASE  + 0x18U))
#define TIM2_CCMR2   ((volatile uint32_t *)((uintptr_t)TIM2_BASE  + 0x1CU))
#define TIM2_CCER   ((volatile uint32_t *)((uintptr_t)TIM2_BASE  + 0x20U))
#define TIM2_CNT   ((volatile uint32_t *)((uintptr_t)TIM2_BASE  + 0x24U))
#define TIM2_PSC   ((volatile uint32_t *)((uintptr_t)TIM2_BASE  + 0x28U))
#define TIM2_ARR   ((volatile uint32_t *)((uintptr_t)TIM2_BASE  + 0x2CU))
#define TIM2_CCR1   ((volatile uint32_t *)((uintptr_t)TIM2_BASE  + 0x34U))
#define TIM2_CCR2   ((volatile uint32_t *)((uintptr_t)TIM2_BASE  + 0x38U))
#define TIM2_CCR3   ((volatile uint32_t *)((uintptr_t)TIM2_BASE  + 0x3CU))
#define TIM2_CCR4   ((volatile uint32_t *)((uintptr_t)TIM2_BASE  + 0x40U))

#define TIM5_BASE    ((volatile uint32_t *)((uintptr_t)0x40000C00 + 0x00U))
#define TIM5_CR1    ((volatile uint32_t *)((uintptr_t)TIM5_BASE  + 0x00U))
#define TIM5_CR2    ((volatile uint32_t *)((uintptr_t)TIM5_BASE  + 0x04U))
#define TIM5_SMCR   ((volatile uint32_t *)((uintptr_t)TIM5_BASE  + 0x08U))
#define TIM5_SR    ((volatile uint32_t *)((uintptr_t)TIM5_BASE  + 0x10U))
#define TIM5_EGR    ((volatile uint32_t *)((uintptr_t)TIM5_BASE  + 0x14U))
#define TIM5_CCMR1    ((volatile uint32_t *)((uintptr_t)TIM5_BASE  + 0x18U))
#define TIM5_CCMR2   ((volatile uint32_t *)((uintptr_t)TIM5_BASE  + 0x1CU))
#define TIM5_CCER   ((volatile uint32_t *)((uintptr_t)TIM5_BASE  + 0x20U))
#define TIM5_CNT   ((volatile uint32_t *)((uintptr_t)TIM5_BASE  + 0x24U))
#define TIM5_PSC   ((volatile uint32_t *)((uintptr_t)TIM5_BASE  + 0x28U))
#define TIM5_ARR   ((volatile uint32_t *)((uintptr_t)TIM5_BASE  + 0x2CU))
#define TIM5_CCR1   ((volatile uint32_t *)((uintptr_t)TIM5_BASE  + 0x34U))
#define TIM5_CCR2   ((volatile uint32_t *)((uintptr_t)TIM5_BASE  + 0x38U))
#define TIM5_CCR3   ((volatile uint32_t *)((uintptr_t)TIM5_BASE  + 0x3CU))
#define TIM5_CCR4   ((volatile uint32_t *)((uintptr_t)TIM5_BASE  + 0x40U))



#endif /* INC_F429ZI_REG_H_ */


