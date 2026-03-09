/*
 * main.c
 *
 *  Created on: Mar 4, 2026
 *      Author: night
 */

#include <stdint.h>
#include "stm32f4xx.h"   // NVIC, __set_MSP, __disable_irq
#include "diy.h"
#include <string.h>

#define APP_BASE   (0x08010000U) // FLASH.app.ld
#define FLASH_END  (0x08200000U) // RM0090 77p

#define FAIL_LIMIT 3

static int app_valid(void);
static void jump_to_app(void);


//__asm volatile("BKPT #0");

int main(void) {


	if(clock_hse_pll_168()){__asm volatile("BKPT #0");};
	sys_init(1000);
	USART6_INIT();
	i2c1_init(0);

	uart_puts("BOOT start\n");

	// check crash
	int crash = crashlog_valid();
	uint32_t crash_seq = crash ? g_crashlog.seq : 0;

	// crashlog -> eeprom
	boot_eeprom();

	// will be recovery?
	int enter_recovery = 0;
	eestate_boot_step(FAIL_LIMIT, crash, crash_seq, &enter_recovery);

	if (enter_recovery) {
		uart_puts("recovery\n");
		//TODO
	}
	if (app_valid()) {
		jump_to_app();
	}

}

static int app_valid(void)
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

    //jump Reset_Handler

    app_entry();

    while (1) { }
}





