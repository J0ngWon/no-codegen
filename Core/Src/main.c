#include <stdint.h>

#include "diy.h"

#define MEGA_ADDR_8BIT (0x12U << 1U)
#define timeout_ms 5000



char msg[3]={'O','K','\n'};

//__asm volatile("BKPT #0");

int main(void){


	if(clock_hse_pll_168()){__asm volatile("BKPT #0");};

	sys_init(1000);
	USART6_INIT();
	uart_puts(&msg);

	i2c1_init(0);
	lcd_init();
	lcd_set_cursor(0, 0);
	lcd_puts(msg);

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






//PB9 SDA PB6 SCL

/*The following is the required sequence in master mode.
• Program the peripheral input clock in I2C_CR2 register in order to generate correct
timings
• Configure the clock control registers
• Configure the rise time register
• Program the I2C_CR1 register to enable the peripheral
• Set the START bit in the I2C_CR1 register to generate a Start condition*/

//MOD 0: Master , 1:Slave
void i2c1_init(int mod) {

	/*uint32_t const GPIO_MOD_Mask = (0x82U << (2U * 6U));
	uint32_t const GPIO_MOD_ClearMask = ~(0xc3U << (2U * 6U));

	//PB6

	uint32_t const GPIO_AFRL_Mask = (0x4U << 24U);
	uint32_t const GPIO_AFRL_ClearMask = ~(0xfU << 24U);

	//PB9
	uint32_t const GPIO_AFRH_Mask = (0x4U << 4U);
	uint32_t const GPIO_AFRH_ClearMask = ~(0xfU << 4U);

	*GPIOB_MODER= (*GPIOB_MODER & GPIO_MOD_ClearMask) | GPIO_MOD_Mask;
	*GPIOB_AFRL= (*GPIOB_AFRL & GPIO_AFRL_ClearMask) | GPIO_AFRL_Mask;
	*GPIOB_AFRH= (*GPIOB_AFRH & GPIO_AFRH_ClearMask) | GPIO_AFRH_Mask;

	*/

	// PB6(12~13), PB9(18~19)
	uint32_t const OSPEED_MASK = (0x3U << (12U)) | (0x3U << (18U));
	uint32_t const OSPEED_VERYHIGH = (0x3U << (12U)) | (0x3U << (18U));

	uint32_t const PUPD_MASK = (0x3U << 12U) | (0x3U << 18U);
	uint32_t const PUPD_VAL = (0x1U << 12U) | (0x1U << 18U);  //Pull Up!

	GPIO_Enable(GPIO_PORT_B);
	*APB1ENR|=(0x1U<<21U); //I2C1 Enable

	GPIO_AF(GPIO_PORT_B,6,4);
	GPIO_AF(GPIO_PORT_B,9,4);

	*GPIOB_OTYPER|=(0x1U << 6U) | (0x1U << 9U);
	*GPIOB_PUPDR = (*GPIOB_PUPDR & ~PUPD_MASK) | PUPD_VAL;



	*GPIOB_OSPEEDR = (*GPIOB_OSPEEDR & ~OSPEED_MASK) | OSPEED_VERYHIGH;

	*I2C1_CR1&=~(0x1U);

/*  *I2C1_CR2=(0x10U); ////16Mhz ,Event+ERROR interrupt OFF
	*I2C1_CCR=(0x50U);
	*I2C1_TRISE=(0x11U); */

	*I2C1_CR2   = (get_pclk1()/1000000U);
	*I2C1_CCR   = (get_pclk1()/200000U); //Sm: CCR = fpclk1 / (2*fscl) (Thigh=Tlow=CCR*T_pclk1)
	*I2C1_TRISE = (get_pclk1()/1000000U)+1U;

	if(mod){
		*I2C1_OAR1 = (1U<<14U) | (0x20<<1U); //ADDR = Ox20

	}
	*I2C1_CR1|=(0x1U);
	if (mod) {
		*I2C1_CR1 |= (1U << 10U);
	}
}

int i2c1_master_tx(uint8_t addr_8bit ,uint8_t *data, int len){

	uint32_t t0;

	*I2C1_CR1|=(0x1U << 8U); //start bit
	t0=millis();
	while(((*I2C1_SR1)&0x1U)!=0x1U) //EV5  RF:852p
	{
	 if(i2c_SR1_ERR()) return -1;
	 if ((uint32_t)(millis() - t0) > timeout_ms) return -2;
	}
	(void)*I2C1_SR1; //clear RF:871p SB

	*I2C1_DR=addr_8bit;

	t0 = millis();
	while(((*I2C1_SR1)&0x2U)!=0x2U) //EV6....
	{
	 if(i2c_SR1_ERR()) return -1;
	 if ((uint32_t)(millis() - t0) > timeout_ms) return -2;
	}
	(void)*I2C1_SR1;
	(void)*I2C1_SR2; //clear RF:871p ADDR

	for (int i = 0; i < len; i++) {

		if (((*I2C1_SR1) & (0x1U << 10U)) == (0x1U << 10U)) { //AF(NACK)
			*I2C1_CR1 |= (1U << 9); //STOP
			*I2C1_SR1 &= ~(1U << 10); // AF clear
			return 1;
		}
		t0=millis();
		while (((*I2C1_SR1) & (0x1U << 7U)) != (0x1U << 7U)) //EV8_1
		{
		 if(i2c_SR1_ERR()) return -1;
		 if ((uint32_t)(millis() - t0) > timeout_ms) return -2;
		}
		*I2C1_DR = *data;
		data++;
	}
	t0=millis();
	//EV8_2: TXE != 1 or BTF !=1 --> wait
	while( (((*I2C1_SR1)&(0x1U<<7U))!= (0x1U<<7U)) || (((*I2C1_SR1)&(0x1U<<2U))!= (0x1U<<2U)) )
	{
	 if(i2c_SR1_ERR()) return -1;
	 if ((uint32_t)(millis() - t0) > timeout_ms) return -2;
	}
	*I2C1_CR1|=(0x1U << 9U); //stop bit
	return 0;
}

int i2c1_master_rx(uint8_t addr_8bit,uint8_t* msg,int len){

	uint32_t t0;

	*I2C1_CR1|=(0x1U << 10U); //SET CR1 ACK
	*I2C1_CR1|=(0x1U << 8U); //start bit

	t0 = millis();
	while(((*I2C1_SR1)&0x1U)!=0x1U)//EV5  RF:853p
	{
		 if(i2c_SR1_ERR()) return -1;
		 if ((uint32_t)(millis() - t0) > timeout_ms) return -2;
	}
	(void)*I2C1_SR1; //clear RF:871p SB
	*I2C1_DR=addr_8bit | 1U;

	t0 = millis();
	while(((*I2C1_SR1)&0x2U)!=0x2U) //EV6....
	{
	  if(i2c_SR1_ERR()) return -1;
	  if ((uint32_t)(millis() - t0) > timeout_ms) return -2;
	}
	switch (len) {
	case 2:
		*I2C1_CR1 &= ~(0x1U << 10U); //RESET CR1 ACK
		*I2C1_CR1 |= (0x1U << 11U); //POS HIGH
		(void) *I2C1_SR1;
		(void) *I2C1_SR2; //clear RF:871p ADDR
		t0=millis();
		while (((*I2C1_SR1) & (0x1U << 2U)) == 0)
		{
					 if(i2c_SR1_ERR()) return -1;
					 if ((uint32_t)(millis() - t0) > timeout_ms) return -2;
		}
		*I2C1_CR1 |= (0x1U << 9U); //stop bit
		*msg = *I2C1_DR;
		msg++;
		*msg = *I2C1_DR;

		*I2C1_CR1 &= ~(0x1U << 11U); //POS RESET
		break;
	default :
		 (void)*I2C1_SR1;
		 (void)*I2C1_SR2; //clear ADDR
		while(len>3) {
			t0 = millis();
			while (((*I2C1_SR1) & (0x1U << 6U)) != (0x1U << 6U)) //EV7
			{
			 if(i2c_SR1_ERR()) return -1;
			 if ((uint32_t)(millis() - t0) > timeout_ms) return -2;
			}
			*msg = *I2C1_DR;
			msg++;
			len--;
		}
		t0 = millis();
		while (((*I2C1_SR1) & (0x1U << 2U)) == 0) //BTF == 1 -> break
		{
			 if(i2c_SR1_ERR()) return -1;
			 if ((uint32_t)(millis() - t0) > timeout_ms) return -2;
		}
		*I2C1_CR1 &= ~(0x1U << 10U); //RESET CR1 ACK
		*msg = *I2C1_DR; // READ len-2
		msg++;

		t0 = millis();
		while (((*I2C1_SR1) & (0x1U << 2U)) == 0) //BTF == 1 -> break
		{
			 if(i2c_SR1_ERR()) return -1;
			 if ((uint32_t)(millis() - t0) > timeout_ms) return -2;
		}
		*I2C1_CR1 |= (0x1U << 9U); //stop bit
		*msg = *I2C1_DR; // READ len-1
		msg++;
		*msg = *I2C1_DR; // READ len
	}
	*I2C1_CR1|=(0x1U << 10U); //SET CR1 ACK
	return 0;
}

int i2c1_slave_tx(uint8_t *data, int len){

	uint32_t t0 = millis();

	while (((*I2C1_SR1) & (0x1U << 1U)) == 0) { //EV1
		if (i2c_SR1_ERR())
			return -1;
		if ((uint32_t) (millis() - t0) > timeout_ms)
			return -2;
	}
	(void)*I2C1_SR1;
	(void)*I2C1_SR2; //ADDR Clear

	if ((*I2C1_SR2 & (1U<<2)) == 0) {   // TRA==0 => Receiver
		t0 = millis();
		while (((*I2C1_SR1) & (1U << 4)) == 0) {   // STOPF

			if ((*I2C1_SR1) & (1U << 6)) {
				(void) *I2C1_DR;
			}
			if (i2c_SR1_ERR())
				return -1;
			if ((uint32_t) (millis() - t0) > timeout_ms)
				return -2;
		}

		// STOPF clear sequence
		(void) *I2C1_SR1;
		*I2C1_CR1 |= (1U << 0);
		return -3;
	}

	for(int i=0;i<len;i++){
	t0 = millis();
	while (((*I2C1_SR1) & (0x1U << 7U)) == 0) { //EV3
		if (i2c_SR1_ERR())
			return -1;
		if ((uint32_t) (millis() - t0) > timeout_ms)
			return -2;
	}
	*I2C1_DR=*data;
	data++;
	}
	t0 = millis();
		while (((*I2C1_SR1) & (0x1U << 10U)) == 0) { //EV3-2
			if (i2c_SR1_ERR())
				return -1;
			if ((uint32_t) (millis() - t0) > timeout_ms)
				return -2;
		}
	*I2C1_SR1&=~(0x1U << 10U); //AF Clear
	while (((*I2C1_SR1) & (0x1U << 4U)) == 0) {
		if (i2c_SR1_ERR())
			return -1;
		if ((uint32_t) (millis() - t0) > timeout_ms)
			return -2;
	}

	//STOPF clear
	(void)*I2C1_SR1;
	*I2C1_CR1 |= (1U<<0);

	return 0;

}

int i2c_SR1_ERR(void) {


	if ((*I2C1_SR1 & (1U << 10)) || (*I2C1_SR1 & (1U << 9)) || (*I2C1_SR1 & (1U << 8))) {  // AF,ARLO,BERR
		*I2C1_SR1 &= ~(1U << 10); // AF clear
		*I2C1_SR1 &= ~(1U << 9); //  ARLO
		*I2C1_SR1 &= ~(1U << 8); //  BERR
		*I2C1_CR1 |= (1U << 9);
	 return -1;
	}
	return 0;
}






void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }

}
