/*
 * main.c
 *
 *  Created on: Mar 4, 2026
 *      Author: night
 */


#include "diy.h"

char msg[2]={'o','k'};

int main(void)
{
    clock_hse_pll_168();
    sys_init(1000);
	USART6_INIT();
	uart_puts(msg);
}
