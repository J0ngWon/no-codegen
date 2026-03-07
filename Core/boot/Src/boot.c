/*
 * main.c
 *
 *  Created on: Mar 4, 2026
 *      Author: night
 */

#include <stdint.h>
#include "stm32f4xx.h"   // NVIC, __set_MSP, __disable_irq
#include "diy.h"
#include "eeplog.h"

#define APP_BASE   (0x08010000U) // FLASH.app.ld
#define FLASH_END  (0x08200000U) // RM0090 77p

static void crashlog_flush_eeprom(void);
static int boot_crashlog_service(void);
static void boot_dump_crashlog(const crashlog_t* log);

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

    //jump Reset_Handler
    __enable_irq();
    app_entry();

    while (1) { }
}

int main(void) {


	if(clock_hse_pll_168()){__asm volatile("BKPT #0");};
	sys_init(1000);
	USART6_INIT();
	i2c1_init(0);

	uart_put_str("BOOT start\r\n");

	int had_crash = boot_crashlog_service();

	if (had_crash) {
		uart_puts((const uint8_t*) "\r\nBOOT: crash handled. (pause)\r\n");
		while (1) {}
	}


	if (app_is_valid()) {
		jump_to_app();
	}


	while (1) {
	}
}

static int boot_crashlog_service(void)
{
    if (!crashlog_valid()) {
        return 0;
    }

    uart_puts((const uint8_t*)"\r\nBOOT: crashlog detected in SRAM\r\n");
    boot_dump_crashlog((const crashlog_t*)&g_crashlog);


    int r = at24c256_write(0x0000, (const uint8_t*)&g_crashlog, sizeof(g_crashlog));
    if (r != 0) {
        uart_puts((const uint8_t*)"BOOT: EEPROM write FAIL\r\n");
        return 1;
    }

    crashlog_t rb;
    r = at24c256_read(0x0000, (uint8_t*)&rb, sizeof(rb));
    if (r != 0) {
        uart_puts((const uint8_t*)"BOOT: EEPROM read FAIL\r\n");
        return 1;
    }

    if (memcmp(&rb, (const void*)&g_crashlog, sizeof(rb)) != 0) {
        uart_puts((const uint8_t*)"BOOT: EEPROM verify FAIL\r\n");
        return 1;
    }

    uart_puts((const uint8_t*)"BOOT: EEPROM verify OK\r\n");


    crashlog_clear();

    return 1;
}

static void boot_dump_crashlog(const crashlog_t* log)
{

    uart_puts((const uint8_t*)"  VTOR/HFSR/CFSR dump...\r\n");

}



