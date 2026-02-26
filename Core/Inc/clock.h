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

int hse_enable(void);

static int wait_set(volatile uint32_t *reg, uint32_t mask, uint32_t loop_max);
static int wait_clear(volatile uint32_t *reg, uint32_t mask, uint32_t loop_max);

#endif /* INC_CLOCK_H_ */
