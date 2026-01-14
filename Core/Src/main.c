#include <stdint.h>
#include "f429zi_reg.h"

int GPIO_Enable(gpio_port_t port);
void led_on(void);
void led_off(void);
void delay(uint32_t ms);
int GPIO_Write(gpio_port_t port ,uint32_t pin, uint32_t level);
int uart_echo(uint8_t msg);

int main(void){
	uart_echo('a');
	while(1){
		led_off();
		GPIO_Write(GPIO_PORT_B,10,0);
		delay(1000);

		led_on();
		GPIO_Write(GPIO_PORT_B,10,1);
		delay(1000);
	}
}

int GPIO_Enable(gpio_port_t port){
	if (port > GPIO_PORT_K) return 1;
	*AHB1ENR |=(1U<<port);
	return 0;
}

void led_on(void){
	//PB7 PC13 PB14  LD1,2,3
	uint32_t const GPIO_MOD_Mask      = (0x1U << (2U * 7U));
	uint32_t const GPIO_MOD_ClearMask = ~(0x3U << (2U * 7U));

	GPIO_Enable(GPIO_PORT_B);

	*GPIOB_MODER =(*GPIOB_MODER & GPIO_MOD_ClearMask) | GPIO_MOD_Mask;

	*GPIOB_BSRR = (0x1U<<7);
};

void led_off(void){

	*GPIOB_BSRR = (0x1U<<23);
}

//HSI 16 MHz
void delay(uint32_t ms){
	uint32_t count=0;

	*STK_LOAD=0x3e7fU;
	*STK_VAL=0U;
	*STK_CTRL=0x5U;

	for (count=0; count<ms; count++) {
	    while (((*STK_CTRL >> 16) & 1U) != 1U) {
	    }
	}
}

int GPIO_Write(gpio_port_t port ,uint32_t pin, uint32_t level){
	volatile uint32_t*base,*GPIOx_MODER,*GPIOx_BSRR;
	uint32_t const GPIO_MOD_Mask = (0x1U << (2U * pin));
	uint32_t const GPIO_MOD_ClearMask = ~(0x3U << (2U * pin));

	if(pin>15) return 1 ;

	base =(volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (port * 0x400));
	GPIOx_MODER=(volatile uint32_t*)((uintptr_t)base + 0x00U);
	GPIOx_BSRR =(volatile uint32_t*)((uintptr_t)base + 0x18U);

	GPIO_Enable(port);
	*GPIOx_MODER= (*GPIOx_MODER & GPIO_MOD_ClearMask) | GPIO_MOD_Mask;

	if (level == 1)
		*GPIOx_BSRR = (1U << pin);
	else
		*GPIOx_BSRR = (1U << (pin+16U));
	return 0;
}

//USART6(APB2) TX:PC6 RX:PC7
int uart_echo(uint8_t msg){
	volatile uint32_t* GPIOC_MODER=
			(volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (GPIO_PORT_C * 0x400) + 0x00U);
	uint32_t const GPIO_MOD_Mask = (0xaU << (2U * 6U));
	uint32_t const GPIO_MOD_ClearMask = ~(0xfU << (2U * 6U));

	volatile uint32_t* GPIOC_AFRL=
				(volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (GPIO_PORT_C * 0x400) + 0x20U);
	uint32_t const GPIO_AFRL_Mask = (0x88U << 24U);
	uint32_t const GPIO_AFRL_ClearMask = ~(0xffU << 24U);

	volatile uint32_t* USART_CR1=
				(volatile uint32_t*)((uintptr_t)USART6_BASE + (0xCU));
	volatile uint32_t* USART_CR2=
					(volatile uint32_t*)((uintptr_t)USART6_BASE + (0x10));
	volatile uint32_t* USART_BRR=
						(volatile uint32_t*)((uintptr_t)USART6_BASE + (0x8));

	*APB2ENR |=(1U<<5);
	GPIO_Enable(GPIO_PORT_C);
	*GPIOC_MODER= (*GPIOC_MODER & GPIO_MOD_ClearMask) | GPIO_MOD_Mask;

	*GPIOC_AFRL= (*GPIOC_AFRL & GPIO_AFRL_ClearMask) | GPIO_AFRL_Mask;

	*USART_CR1 = (1U<<13); //set UE bit =1 Enable ,M bit =0 1S 8D nST  ;

	*USART_CR2 &= ~(3U<<12); // 1 STOP bits
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }

}
