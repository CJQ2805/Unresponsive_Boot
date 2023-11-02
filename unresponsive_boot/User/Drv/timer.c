/**
 *  timer.c
 *  @Author: CJQ2805
 *  2023/6/5  13:37
 */
#include "timer.h"
volatile u32TimDly_t g_Tim0Period10ms = 0UL;
/**
@brief  TIM_Delay()
@param	
*/
en_result_t TMR_Delay(bool judgement, u32TimDly_t *p_dly, uint32_t timecnt)
{
	if(false == (judgement) || (NULL == p_dly))
	{
		if(NULL != p_dly)
		{
			*p_dly = g_Tim0Period10ms;
			return ErrorInvalidParameter;
		}
	}
	
	if(NULL == *p_dly)
	{
		*p_dly = g_Tim0Period10ms;
		return ErrorInvalidParameter;		
	}
	
	if((g_Tim0Period10ms - *p_dly) >= timecnt)
	{
		*p_dly = g_Tim0Period10ms;
		return Ok;
	}
	
	return OperationInProgress;
}


/**
@brief TMR_ResetCount
@param
*/
u32TimDly_t TMR_ResetCount(void)
{
	return g_Tim0Period10ms;
}

void Timer_Delay10ms(uint32_t time_count)
{
	uint32_t time_tmp = TMR_ResetCount();
	while((TMR_ResetCount() - time_tmp) <= time_count)
	{
		
	}
	
}
