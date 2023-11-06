/**
 *  flash_update_handle.h
 *  @Author: CJQ2805
 *  2023/11/3  8:55
 */ 
#ifndef __FLASH_UPDATE_HANDLE_H
#define __FLASH_UPDATE_HANDLE_H
#include "sys.h"

void ChipFlashDownload_Process(void);
uint8_t AppUpdate_Process(void);
uint8_t BootFlag_Diag(void);

#endif
