/**
 *  timer.h
 *  @Author: CJQ2805
 *  2023/6/5  13:37
 */
#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

#define DELAY_10MS(X)  X

extern volatile u32TimDly_t g_Tim0Period10ms;
en_result_t TMR_Delay(bool judgement, u32TimDly_t *p_dly, uint32_t timecnt);
void Timer_Delay10ms(uint32_t time_count);

#endif

