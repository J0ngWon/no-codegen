/*
 * main.c
 *
 *  Created on: Mar 4, 2026
 *      Author: night
 */

#include <stdint.h>
#include "stm32f4xx.h"   // SCB, SysTick, NVIC, __set_MSP, __disable_irq 등

#define APP_BASE   (0x08010000U)
#define FLASH_END  (0x08200000U) // 2MB 기준. 칩에 맞게 조정

//__asm volatile("BKPT #0");

static int app_is_valid(void)
{
    uint32_t msp   = *(volatile uint32_t*)(APP_BASE + 0U);
    uint32_t reset = *(volatile uint32_t*)(APP_BASE + 4U);

    // MSP가 SRAM 범위인지(대충 큰 범위로 0x2000_0000~0x2004_0000 같은 식)
    if (msp < 0x20000000U || msp > 0x20040000U) return 0;

    // Reset_Handler 주소는 Thumb bit(LSB=1)가 켜져 있어야 함
    if ((reset & 1U) == 0U) return 0;

    uint32_t pc = reset & ~1U;
    if (pc < APP_BASE || pc >= FLASH_END) return 0;

    return 1;
}

static void jump_to_app(void)
{
    uint32_t app_msp   = *(volatile uint32_t*)(APP_BASE + 0U);
    uint32_t app_reset = *(volatile uint32_t*)(APP_BASE + 4U);
    void (*app_entry)(void) = (void(*)(void))app_reset;

    __disable_irq();

    // SysTick 정지(boot에서 쓰고 있었다면 꼭)
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    // NVIC: enable/pend 정리 (최대한 reset 직후처럼)
    for (int i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFFU;
        NVIC->ICPR[i] = 0xFFFFFFFFU;
    }
    __DSB();
    __ISB();

    // 벡터테이블을 app으로
    SCB->VTOR = APP_BASE;
    __DSB();
    __ISB();

    // 스택을 app 것으로
    __set_MSP(app_msp);
    __DSB();
    __ISB();

    // Reset_Handler로 점프
    app_entry();

    while (1) { }
}

int main(void)
{
    // (여기서 boot UART 초기화 후 "BOOT" 출력해도 좋음)

    if (app_is_valid()) {

    	jump_to_app();
    }

    // app 없으면 여기서 업데이트 모드로 대기
    while (1) {}
}
