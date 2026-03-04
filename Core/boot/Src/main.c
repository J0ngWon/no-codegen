/*
 * main.c
 *
 *  Created on: Mar 4, 2026
 *      Author: night
 */

#include <stdint.h>
#include "stm32f4xx.h"   // NVIC, __set_MSP, __disable_irq
#include "f429zi_reg.h"

#define APP_BASE   (0x08010000U) // FLASH.app.ld
#define FLASH_END  (0x08200000U) // RM0090 77p

//__asm volatile("BKPT #0");

static int app_is_valid(void)
{
    uint32_t msp   = *(volatile uint32_t*)(APP_BASE + 0U);
    uint32_t reset = *(volatile uint32_t*)(APP_BASE + 4U);

    // MSP in SRAM?
    if (msp < 0x20000000U || msp > 0x20040000U) return 0;

    // check Thumb bit
    if ((reset & 1U) == 0U) return 0;

    uint32_t pc = reset & ~1U;
    if (pc < APP_BASE || pc >= FLASH_END) return 0;

    return 1;
}

static void jump_to_app(void)
{
	volatile uint32_t* NVIC_ICER=((volatile uint32_t*)((uintptr_t)NVIC_BASE + 0x180U));
	volatile uint32_t* NVIC_ICPR=((volatile uint32_t*)((uintptr_t)NVIC_BASE + 0x280U));

    uint32_t msp   = *(volatile uint32_t*)(APP_BASE + 0U);
    uint32_t reset = *(volatile uint32_t*)(APP_BASE + 4U);
    void (*app_entry)(void) = (void(*)(void))reset;

    __disable_irq();

    //Stop SysTick
    *STK_CTRL = 0;
    *STK_LOAD = 0;
    *STK_VAL  = 0;

    // NVIC: enable/pend
    for (int i = 0; i < 8; i++) {
        *(NVIC_ICER + i) = 0xFFFFFFFFU;
        *(NVIC_ICPR + i) = 0xFFFFFFFFU;
    }
    __DSB();
    __ISB();

    // VTOR = APP
    *SCB_VTOR = (uint32_t)APP_BASE;
    __DSB();
    __ISB();

    // Stack Pointer = app
    __set_MSP(msp);
    __DSB();
    __ISB();

    __enable_irq();

    //jump Reset_Handler
    app_entry();

    while (1) { }
}

int main(void)
{


    if (app_is_valid()) {

    	jump_to_app();
    }

    while (1) {}
}
