/*
 * uart.h
 *
 *  Created on: Feb 26, 2026
 *      Author: night
 */

#ifndef INC_UART6_H_
#define INC_UART6_H_

#include <stdint.h>

void USART6_INIT(void);
int uart_putc(uint8_t msg);
int uart_puts(const uint8_t* msg);
int uart_getc(void);
int uart_fgets(char *msg ,uint32_t max_len);

int uart_set_baud(volatile uint32_t *brr_reg,
                  volatile uint32_t *cr1_reg,
                  uint32_t baud);
#endif /* INC_UART6_H_ */
