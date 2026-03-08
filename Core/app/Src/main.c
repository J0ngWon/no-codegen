#include <stdint.h>
#include "stm32f4xx.h"
#include "diy.h"



static void fault_test_invstate(void)
{
    void (*fn)(void) = (void(*)(void))0x08010000U; // LSB=0 (Thumb x)
    fn(); // INVSTATE
}

char msg[4]={'A','p','p','\n'};

//__asm volatile("BKPT #0");

int main(void){
	__enable_irq();

	uint32_t ff,dd;

	/*if(clock_hse_pll_168()){__asm volatile("BKPT #0");};
	sys_init(1000);
	USART6_INIT();
	i2c1_init(0);*/

	uart_puts(msg);

	eestate_confirm_ok();

	fault_test_invstate();


    tim2_pwm_output();
    tim5_pwm_capture();

    tim5_freq_duty(&ff,&dd,1);

    delay(100);
    led_on();

	i2c1_init(0);
	lcd_init();
	lcd_set_cursor(0, 0);
	lcd_puts(&msg);

	while(1){

		led_off();
		GPIO_Write(GPIO_PORT_B,10,0);
		delay(100);

		led_on();
		GPIO_Write(GPIO_PORT_B,10,1);
		delay(100);
	}
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
void old_delay(uint32_t ms){
	uint32_t count=0;

	*STK_LOAD=0x3e7fU;
	*STK_VAL=0U;
	*STK_CTRL=0x5U;

	for (count=0; count<ms; count++) {
	    while (((*STK_CTRL >> 16) & 1U) != 1U) {
	    }
	}
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }

}
