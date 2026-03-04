/*
 * timbase.h
 *
 *  Created on: Feb 26, 2026
 *      Author: night
 */

#ifndef INC_TIMBASE_H_
#define INC_TIMBASE_H_

#include <stdint.h>

void sys_init(uint32_t tick_hz);
void timebase_isr_tick(void);
void delay(uint32_t ms);
uint32_t millis(void);

int tim2_pwm_output(void);
int tim2_input_capture_init(void);
int tim5_capture_get(uint32_t *ccr1,int ch);
void tim5_freq_duty(uint32_t *freq,uint32_t *duty,int ch);

uint32_t get_tim_clk(void);


void old_sys_init(void);



#endif /* INC_TIMBASE_H_ */
