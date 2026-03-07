/*
 * i2c.c
 *
 *  Created on: Mar 7, 2026
 *      Author: night
 */


#include "f429zi_reg.h"
#include "i2c1.h"
#include "gpio.h"
#include "timebase.h"

#define AT24_ADDR_8BIT (0x50u << 1) // A0/A1/A2 = 0일 때 0x50(7-bit)
#define MEGA_ADDR_8BIT (0x12U << 1U)
#define timeout_ms 5000



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

//read: 2byte + stop + (read)
int at24c256_read(uint16_t mem, uint8_t* out, uint16_t len)
{
    uint8_t a[2] = { (uint8_t)(mem >> 8), (uint8_t)(mem & 0xFF) };
    int r = i2c1_master_tx(AT24_ADDR_8BIT, a, 2);
    if (r != 0) return r;
    return i2c1_master_rx(AT24_ADDR_8BIT, out, len);
}

int at24c256_write(uint16_t mem_addr, const uint8_t* data, uint16_t len)
{
    // AT24C256 : 2byte
    uint8_t buf[2 + 64]; // <=64B
    if (len > 64) return -1;

    buf[0] = (uint8_t)(mem_addr >> 8);
    buf[1] = (uint8_t)(mem_addr & 0xFF);
    for (uint16_t i = 0; i < len; i++) buf[2 + i] = data[i];

    int r = i2c1_master_tx(AT24_ADDR_8BIT, buf, 2 + len);

    // EEPROM write cycle
    delay(5);

    return r;
}
