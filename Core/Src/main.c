#include <stdint.h>
#include "f429zi_reg.h"

void USART6_INIT(void);

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

int main(void){
	char test[]="";

	USART6_INIT();

	uart_putc('o');
	uart_putc('k');
	uart_putc('\n');



	while(1){
		/*uart_fgets(test,16);
		uart_puts(test);*/
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

	*GPIOB_MODER_LED =(*GPIOB_MODER_LED & GPIO_MOD_ClearMask) | GPIO_MOD_Mask;

	*GPIOB_BSRR_LED = (0x1U<<7);
}

void led_off(void){

	*GPIOB_BSRR_LED = (0x1U<<23);
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

	*USART_CR1 = (1U<<13); //set UE bit =1 Enable ,M bit =0 1S 8D nST  ;

	*USART_CR2 &= ~(3U<<12); // 1 STOP bits

	*USART_BRR =0x8bU;

	*USART_CR1 |=(1U<<3); //SET TE bit == TX ON

	*USART_CR1 |=(1U<<2); //SET RE bit == RX ON
}

int uart_putc(uint8_t msg){
	while((*USART_SR & (1U << 7)) == 0U); //TXE

	*USART_DR=msg;

	while((*USART_SR & (1U << 6)) == 0U); //TC

	return 0;
}

int uart_puts(const uint8_t* msg){

	while (*msg != '\0') {
		if (*msg == '\n') {
			while ((*USART_SR & (1U << 7)) == 0U);
			*USART_DR = '\r';
		}
		while ((*USART_SR & (1U << 7)) == 0U);
		*USART_DR = *msg;

		msg++;
	}

	while((*USART_SR & (1U << 6)) == 0U);

	return 0;

}

int uart_getc(void){
	int msg;

	while((*USART_SR & (1U << 5)) == 0U); //RXNE
	if((*USART_SR & (0xf)) !=0)
		{
		msg=(int)(*USART_DR & 0xFFU);
		return -1;
		}

	msg=(int)(*USART_DR & 0xFFU);

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


//PB9 SDA PB6 SCL

/*The following is the required sequence in master mode.
• Program the peripheral input clock in I2C_CR2 register in order to generate correct
timings ----> OK
• Configure the clock control registers
• Configure the rise time register
• Program the I2C_CR1 register to enable the peripheral
• Set the START bit in the I2C_CR1 register to generate a Start condition*/

void i2c1_init(void){
	volatile uint32_t* GPIOB_MODER=
				(volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (GPIO_PORT_B * 0x400) + 0x00U);
		uint32_t const GPIO_MOD_Mask = (0x82U << (2U * 6U));
		uint32_t const GPIO_MOD_ClearMask = ~(0xc3U << (2U * 6U));

		//PB6
		volatile uint32_t* GPIOB_AFRL=
				(volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (GPIO_PORT_B * 0x400) + 0x20U);
		uint32_t const GPIO_AFRL_Mask = (0x4U << 24U);
		uint32_t const GPIO_AFRL_ClearMask = ~(0xfU << 24U);

		//PB9
		volatile uint32_t* GPIOB_AFRH=
				(volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (GPIO_PORT_B * 0x400) + 0x24U);
				uint32_t const GPIO_AFRH_Mask = (0x4U << 4U);
				uint32_t const GPIO_AFRH_ClearMask = ~(0xfU << 4U);


	   volatile uint32_t* GPIOB_OTYPER=
				(volatile uint32_t*)((uintptr_t)GPIO_BASE_VAL + (GPIO_PORT_B * 0x400) + 0x04U);

	   volatile uint32_t* I2C1_CR2=(volatile uint32_t*)((uintptr_t)I2C1_BASE + 0x04U);
	   volatile uint32_t* I2C1_CCR=(volatile uint32_t*)((uintptr_t)I2C1_BASE + 0x1CU);
	   volatile uint32_t* I2C1_TRISE=(volatile uint32_t*)((uintptr_t)I2C1_BASE + 0x20U);
	   volatile uint32_t* I2C1_CR1=(volatile uint32_t*)((uintptr_t)I2C1_BASE + 0x00U);

	GPIO_Enable(GPIO_PORT_B);
	*APB1ENR|=(0x1U<<21U); //I2C1 Enable

	*GPIOB_MODER= (*GPIOB_MODER & GPIO_MOD_ClearMask) | GPIO_MOD_Mask;
	*GPIOB_OTYPER|=(0x1U << 6U) | (0x1U << 9U);
	//need pull up
	*GPIOB_AFRL= (*GPIOB_AFRL & GPIO_AFRL_ClearMask) | GPIO_AFRL_Mask;
	*GPIOB_AFRH= (*GPIOB_AFRH & GPIO_AFRH_ClearMask) | GPIO_AFRH_Mask;


	*I2C1_CR2=(0x10U); ////16Mhz ,Event+ERROR interrupt OFF
	//*I2C1_CR2=(0x310U); //16Mhz ,Event+ERROR interrupt ON
	*I2C1_CCR=(0x50U);
	*I2C1_TRISE=(0x11U);
	*I2C1_CR1|=(0x1U);
	//*I2C1_CR1|=(0x1U << 8U); //start bit
}



void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }

}
