/**
 *  can.h
 *  @Author: CJQ2805
 *  2023/6/5  10:37
 */
 
#ifndef __CAN_H
#define __CAN_H
#include "sys.h"

u8 can_tx(u32 ID, u8* pu8send_buff, u8 u8data_Length);
void CAN_CheckError(void);

#endif

