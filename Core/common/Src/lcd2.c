/*
 * lcd.c
 *
 *  Created on: Dec 23, 2025
 *      Author: night
 */

// lcd.c
#include <stdio.h>
#include "diy.h"
#include "stm32f4xx_it.h"

#define LCD_ADDR_8BIT   (0x27U << 1)
#define LCD_RS  (1U<<0)
#define LCD_RW  (1U<<1)
#define LCD_EN  (1U<<2)
#define LCD_BL  (1U<<3)


// 밑단 함수 타입
typedef void (*pcf_write_p)(uint8_t);

// 기본(blocking) 구현
static void pcf_write_blocking(uint8_t b)
{
    //(void)HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR_8BIT, &b, 1, 50);
	(void)i2c1_master_tx(LCD_ADDR_8BIT, &b, 1);
}

// 현재 사용 중인 구현 포인터 (처음엔 blocking)
static pcf_write_p pcf_write_impl = pcf_write_blocking;

// lcd.c 내부에서만 쓰는 wrap
static inline void pcf_write(uint8_t b)
{
    pcf_write_impl(b);
}

void lcd_set_pcf_write_impl(void (*fn)(uint8_t))
{
    if (fn) {
        pcf_write_impl = fn;
    } else {
        pcf_write_impl = pcf_write_blocking; // NULL이면 기본으로 복구
    }
}

/* --- DWT us delay --- */
/*static void dwt_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL = 0;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (16000000U / 1000000U);
    while ((uint32_t)(DWT->CYCCNT - start) < ticks) { }
}*/

/* --- low level write (현재는 blocking) --- */


/* --- 4bit 전송 --- */
static void lcd_send_nibble(uint8_t nibble, uint8_t rs)
{
    uint8_t data = (uint8_t)((nibble & 0x0F) << 4); // D4..D7 -> P4..P7

    data |= LCD_BL;          // backlight ON
    data &= (uint8_t)~LCD_RW;   // RW=0 (write only)
    if (rs) data |= LCD_RS;

    pcf_write((uint8_t)(data | LCD_EN));
    //delay_us(50);
    delay(1);
    pcf_write((uint8_t)(data & (uint8_t)~LCD_EN));
    //delay_us(50);
    delay(1);
}

/* --- command / data --- */
static void lcd_cmd(uint8_t cmd)
{
    lcd_send_nibble((uint8_t)(cmd >> 4), 0);
    lcd_send_nibble((uint8_t)(cmd & 0x0F), 0);
    if (cmd == 0x01U || cmd == 0x02U) delay(2);
    else delay(1);
}

static void lcd_data(uint8_t ch)
{
    lcd_send_nibble((uint8_t)(ch >> 4), 1);
    lcd_send_nibble((uint8_t)(ch & 0x0F), 1);
    delay(1);
}

/* --- public API --- */
void lcd_init(void)
{
    //dwt_init();
    delay(200);                 // power-up settle

    // HD44780 4-bit init sequence
    lcd_send_nibble(0x03, 0);
    delay(10);
    lcd_send_nibble(0x03, 0);
    delay(10);
    lcd_send_nibble(0x03, 0);
    delay(2);
    lcd_send_nibble(0x02, 0);
    delay(2);

    lcd_cmd(0x28); // 4-bit, 2-line, 5x8
    lcd_cmd(0x0C); // display ON, cursor OFF
    lcd_cmd(0x06); // entry mode
    lcd_cmd(0x01); // clear
    delay(5);
}

void lcd_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t addr = (row == 0U) ? 0x00U : 0x40U;
    lcd_cmd((uint8_t)(0x80U | (addr + col)));
}

void lcd_puts(const char *s)
{
    while (*s != '\0') {
        lcd_data((uint8_t)*s++);
    }
}

/* RPM 표시용 헬퍼 */
void lcd_print(float rpm)
{
    char buf[17];

    if (rpm < 0.0f) rpm = 0.0f;

    int rpm100 = (int)(rpm * 100.0f + 0.5f);  // 2자리 소수
    int whole  = rpm100 / 100;
    int frac   = rpm100 % 100;

    // 소수부 두 자리를 %02d로 0패딩
    snprintf(buf, sizeof(buf), "RPM:%3d.%02d ", whole, frac);

    lcd_set_cursor(0, 0);
    lcd_puts(buf);
}

/* I2C 스캐너 (디버그용) */
/*void I2C1_Scan(void)
{
    printf("\r\n=== I2C1 Scan Start ===\r\n");

    for (uint8_t addr7 = 0x03; addr7 <= 0x77; addr7++) {
        uint16_t addr8 = (uint16_t)(addr7 << 1);

        if (HAL_I2C_IsDeviceReady(&hi2c1, addr8, 2, 20) == HAL_OK) {
            printf("FOUND: 0x%02X (7-bit)\r\n", addr7);
        }
    }

    printf("=== I2C1 Scan End ===\r\n");
}*/

