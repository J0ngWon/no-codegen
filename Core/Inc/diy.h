/*
 * diy.h
 *
 *  Created on: Feb 9, 2026
 *      Author: night
 */

#ifndef INC_DIY_H_
#define INC_DIY_H_

#include "f429zi_reg.h"

int GPIO_Enable(gpio_port_t port);
void led_on(void);
void led_off(void);
void delay(uint32_t ms);
int GPIO_Write(gpio_port_t port ,uint32_t pin, uint32_t level);
int uart_putc(uint8_t msg);
int uart_puts(const uint8_t* msg);
int uart_getc(void);
int uart_fgets(char *msg ,uint32_t max_len);
void i2c1_init(void);
int i2c1_master_tx(uint8_t addr_8bit ,uint8_t *data, int len);


//lcd
void lcd_init(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_puts(const char *s);
void lcd_print(float rpm);
void I2C1_Scan(void);
void lcd_set_pcf_write_impl(void (*fn)(uint8_t));

#endif /* INC_DIY_H_ */
