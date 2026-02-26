/*
 * ledlcd.h
 *
 *  Created on: Feb 26, 2026
 *      Author: night
 */

#ifndef INC_LEDLCD_H_
#define INC_LEDLCD_H_


#include <stdint.h>

void led_on(void);
void led_off(void);
void old_delay(uint32_t ms);

//lcd
void lcd_init(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_puts(const char *s);
void lcd_print(float rpm);
void I2C1_Scan(void);
void lcd_set_pcf_write_impl(void (*fn)(uint8_t));


#endif /* INC_LEDLCD_H_ */
