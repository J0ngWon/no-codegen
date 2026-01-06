#include <stdint.h>




int main(void){
	volatile uint32_t* const RCC_CFGR=(volatile uint32_t *)(0x40023800+0x08U);

	while(1){
		led_off();
		led_on();

	}
}

void led_on(void){
	//PB7 PC13 PB14  LD 1 2 3
	volatile uint32_t* const AHB1ENR=(volatile uint32_t *)(0x40023800+0x30U);
	volatile uint32_t const GPIOB_RCC_Mask = (0x1U<<1U);

	*AHB1ENR=*AHB1ENR|GPIOB_RCC_Mask; //GPIOB Enable

	volatile uint32_t* const GPIOB_MODER = (volatile uint32_t*)(0x40020400U+0x00U);

	volatile uint32_t const GPIOB_MOD_Mask      = (0x1U << (2U * 7U));
	volatile uint32_t const GPIOB_MOD_ClearMask = ~(0x3U << (2U * 7U));
	*GPIOB_MODER =(*GPIOB_MODER & GPIOB_MOD_ClearMask) | GPIOB_MOD_Mask;

	volatile uint32_t* const GPIOB_BSRR = (volatile uint32_t*)(0x40020400U+0x18U);
	*GPIOB_BSRR = (0x1U<<7);
};

void led_off(void){
	volatile uint32_t* const GPIOB_BSRR = (volatile uint32_t*)(0x40020400U+0x18U);
	*GPIOB_BSRR = (0x1U<<23);
}

//HSI 16 MHz
void millis(){

}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }

}
