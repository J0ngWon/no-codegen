/*
 * uart6.c
 *
 *  Created on: Feb 27, 2026
 *      Author: night
 */
#include "uart6.h"
#include "f429zi_reg.h"
#include "gpio.h"

//USART6(APB2) TX:PC6 RX:PC7 baud 115200
void USART6_INIT(void){
	volatile uint32_t* GPIOC_MODER=
			(volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (GPIO_PORT_C * 0x400) + 0x00U);
	uint32_t const GPIO_MOD_Mask = (0xaU << (2U * 6U));
	uint32_t const GPIO_MOD_ClearMask = ~(0xfU << (2U * 6U));

	volatile uint32_t* GPIOC_AFRL=
				(volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (GPIO_PORT_C * 0x400) + 0x20U);
	uint32_t const GPIO_AFRL_Mask = (0x88U << 24U);
	uint32_t const GPIO_AFRL_ClearMask = ~(0xffU << 24U);

	*APB2ENR |=(1U<<5);
	GPIO_Enable(GPIO_PORT_C);
	*GPIOC_MODER= (*GPIOC_MODER & GPIO_MOD_ClearMask) | GPIO_MOD_Mask;

	*GPIOC_AFRL= (*GPIOC_AFRL & GPIO_AFRL_ClearMask) | GPIO_AFRL_Mask;

	*USART6_CR1 = (1U<<13); //set UE bit =1 Enable ,M bit =0 1S 8D nST  ;

	*USART6_CR2&= ~(3U<<12); // 1 STOP bits

	uart_set_baud(USART6_BRR, USART6_CR1, 115200);

	*USART6_CR1 |=(1U<<3); //SET TE bit == TX ON

	*USART6_CR1 |=(1U<<2); //SET RE bit == RX ON
}

int uart_putc(uint8_t msg){
	while((*USART6_SR & (1U << 7)) == 0U); //TXE

	*USART6_DR=msg;

	while((*USART6_SR & (1U << 6)) == 0U); //TC

	return 0;
}

int uart_puts(const char *msg){

	while (*msg != '\0') {
		if (*msg == '\n') {
			while ((*USART6_SR & (1U << 7)) == 0U);
			*USART6_DR = '\r';
		}
		while ((*USART6_SR & (1U << 7)) == 0U);
		*USART6_DR = *msg;

		msg++;
	}

	while((*USART6_SR & (1U << 6)) == 0U);

	return 0;

}

int uart_getc(void){
	int msg;

	while((*USART6_SR & (1U << 5)) == 0U); //RXNE
	if((*USART6_SR & (0xf)) !=0)
		{
		msg=(int)(*USART6_DR & 0xFFU);
		return -1;
		}

	msg=(int)(*USART6_DR & 0xFFU);

	return msg;
}

int uart_fgets(char *msg ,uint32_t max_len){
	for(int i=0 ;i<(max_len-1);i++){
		int data=uart_getc();
		if(data <0) return -1;
		*msg=data;
		if(*msg=='\n'){
			msg++;
			*msg='\0';
			return 0;
		}
		msg++;
	}
	*msg='\0';
	return 0;
}

// USART1,6 APB2 else APB1
int uart_set_baud(volatile uint32_t *brr_reg,
                  volatile uint32_t *cr1_reg,
                  uint32_t baud){

	uint32_t fck=get_pclk2();

	uint32_t over8 = ((*cr1_reg >> 15U) & 0x1U);

	    if (over8 == 0U) {
	        // oversampling by 16
	        uint32_t usartdiv16 = (fck + (baud / 2U)) / baud;  // USARTDIV*16+rounded
	        uint32_t mantissa   = usartdiv16 / 16U;
	        uint32_t fraction   = usartdiv16 % 16U;
	        *brr_reg = (mantissa << 4U) | (fraction & 0xFU);
	    } else {
	        // oversampling by 8
	        uint32_t usartdiv8 = (fck + (baud / 2U)) / baud;   // USARTDIV*8+rounded
	        uint32_t mantissa  = usartdiv8 / 8U;
	        uint32_t fraction  = usartdiv8 % 8U;

	        *brr_reg = (mantissa << 4U) | (fraction & 0x7U);
	    }


	return 0;
}

void uart_put_hex32(uint32_t value)
{
    for (int i = 28; i >= 0; i -= 4) {
        uint32_t n = (value >> i) & 0xFU;
        uint8_t output = (n < 10) ? (uint8_t)('0' + n) : (uint8_t)('A' + (n - 10));
        uart_putc(output);
    }
}



