/*
 * timbase.h
 *
 *  Created on: Feb 26, 2026
 *      Author: night
 */

#ifndef INC_TIMBASE_H_
#define INC_TIMBASE_H_

#include <stdint.h>

void timebase_isr_tick(void);
void delay(uint32_t ms);
uint32_t millis(void);

#endif /* INC_TIMBASE_H_ */
