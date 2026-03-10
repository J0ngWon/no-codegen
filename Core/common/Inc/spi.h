/*
 * spi.h
 *
 *  Created on: Mar 9, 2026
 *      Author: night
 */

#ifndef COMMON_INC_SPI_H_
#define COMMON_INC_SPI_H_

void spi4_init(void);
int spi4_txrx(uint8_t *rx, const uint8_t *tx, uint32_t len);

#endif /* COMMON_INC_SPI_H_ */
