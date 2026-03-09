/*
 * clock.h
 *
 *  Created on: Feb 26, 2026
 *      Author: night
 */

#ifndef INC_CLOCK_H_
#define INC_CLOCK_H_

#include "f429zi_reg.h"

#define HSE_TIMEOUT_MS 1000U
extern const uint8_t apb_tbl[8];

int hse_enable(void);
int clock_hse_pll_168(void);

int clock_update(void);
uint32_t get_hclk(void);
uint32_t get_pclk1(void);
uint32_t get_pclk2(void);



static int wait_set(volatile uint32_t *reg, uint32_t mask, uint32_t loop_max);
static int wait_clear(volatile uint32_t *reg, uint32_t mask, uint32_t loop_max);

#endif /* INC_CLOCK_H_ */
