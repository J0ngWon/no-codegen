/*
 * spi.c
 *
 *  Created on: Mar 9, 2026
 *      Author: night
 */

#include "f429zi_reg.h"
#include "spi.h"
#include "gpio.h"
#include "timebase.h"

#define SPI_TIMEOUT 5000

static int spi4_wait_rxne(void);
static int spi4_wait_txe(void);
static int spi4_wait_not_busy(void);
static uint8_t spi4_xfer8(uint8_t tx, int *err);

void spi4_init(void){
	*APB2ENR |= (1U<<13U);
	GPIO_Enable(GPIO_PORT_E);
	GPIO_AF(GPIO_PORT_E,12,5); // SCK
	GPIO_AF(GPIO_PORT_E,13,5); // MISO
	GPIO_AF(GPIO_PORT_E,14,5); // MOSI

	 GPIO_Write(GPIO_PORT_E, 11, 1); // CS idle high, PE11 output

	*SPI4_CR1&=~(7U<<3U); // BR Clear
	*SPI4_CR1|=(6U<<3U); // apb2/128 =baud

	*SPI4_CR1&=~(3U); // CPHA,CPOL :0

	*SPI4_CR1&=~(1U<<11U); // data:8bit
	*SPI4_CR1&=~(1U<<7U); // MSB transmitted first

	*SPI4_CR1|=(1U<<9U); // SSM Software slave management enabled
	*SPI4_CR1|=(1U<<8U); // SSI
	*SPI4_CR1&=~(1U<<10U); // Full duplex

	*SPI4_CR1&=~(1U<<13U); // CRC calculation disabled
	*SPI4_CR1&=~(1U<<15U); // 2-line unidirectional data mode selected

	*SPI4_CR2&=~(1U<<4U); // Motorola mode

	*SPI4_CR1|=(1U<<2U); // Master
	*SPI4_CR1|=(1U<<6U); // enable
}

int spi4_txrx(uint8_t *rx, const uint8_t *tx, uint32_t len){
    uint32_t i;
    int err = 0;

    GPIO_Write(GPIO_PORT_E, 11, 0);   // CS low

    for (i = 0; i < len; i++){
        uint8_t dout = (tx != 0) ? tx[i] : 0xFF;
        uint8_t din  = spi4_xfer8(dout, &err);

        if (err){
            GPIO_Write(GPIO_PORT_E, 11, 1);   // CS high on error
            return err;
        }

        if (rx != 0){
            rx[i] = din;
        }
    }

    if (spi4_wait_txe()){
        GPIO_Write(GPIO_PORT_E, 11, 1);
        return -5;
    }

    if (spi4_wait_not_busy()){
        GPIO_Write(GPIO_PORT_E, 11, 1);
        return -6;
    }

    GPIO_Write(GPIO_PORT_E, 11, 1);   // CS high
    return 0;
}

static int spi4_wait_txe(void){
	uint32_t t;
	t=millis();
	while(millis() - t < SPI_TIMEOUT){
		if((*SPI4_SR&(1U<<1U))){return 0;}
	}
	return -2 ;
}

static int spi4_wait_rxne(void){
	uint32_t t;
		t=millis();
		while(millis() - t < SPI_TIMEOUT){
			if((*SPI4_SR&(1U))){return 0;}
		}
		return -2 ;
}

static int spi4_wait_not_busy(void){
    uint32_t t = millis();
    while ((millis() - t) < SPI_TIMEOUT){
        if ((*SPI4_SR & (1U << 7U)) == 0U){   // BSY == 0
            return 0;
        }
    }
    return -2;
}

static uint8_t spi4_xfer8(uint8_t tx, int *err){
    if (spi4_wait_txe()){
        if (err) *err = -3;
        return 0xFF;
    }

    *((volatile uint8_t*)SPI4_DR) = tx;

    if (spi4_wait_rxne()){
        if (err) *err = -4;
        return 0xFF;
    }

    return *((volatile uint8_t*)SPI4_DR);
}
