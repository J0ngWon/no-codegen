/*
 * i2c1.h
 *
 *  Created on: Feb 26, 2026
 *      Author: night
 */

#ifndef INC_I2C1_H_
#define INC_I2C1_H_


#include <stdint.h>

void i2c1_init(int mod);
int i2c1_master_tx(uint8_t addr_8bit ,uint8_t *data, int len);
int i2c1_master_rx(uint8_t addr_8bit,uint8_t* msg,int len);
int i2c_SR1_ERR(void);
int i2c1_slave_tx(uint8_t *data, int len);

int at24c256_write(uint16_t mem_addr, const uint8_t* data, uint16_t len);

#endif /* INC_I2C1_H_ */
