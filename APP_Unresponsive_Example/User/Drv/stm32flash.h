/**
 *  stm32flash.h
 *  @Author: CJQ2805
 *  2022/12/17  11:16
 */ 

#ifndef __STM32FLASH_H
#define __STM32FLASH_H
#include "sys.h"
 
#define FLASH_BASE_ADDR			0x08000000

/*BOOT FLASH SIZE 30K*/
#define FLASH_BOOT_START_ADDR   0x08000000
#define FLASH_BOOT_END_ADDR       0x08007800

/*BOOTFLAG AND BOOTDATA FLASH SIZE 6k */
#define FLASH_BOOTFLAG_DATA_START_ADDR 0x08007800

#define FLASH_BOOTFLAG_ADDRESS         0x08007800
#define FLASH_APPLEN_ADDRESS           0x08008000

#define FLASH_BOOTFLAG_DATA_END_ADDR 0x08009000

/*APP FLASH SIZE 100k */
#define FLASH_APP_START_ADDR   0x08009000
#define FLASH_APP_END_ADDR       0x08022000

/*APP BACKUPS FLASH SIZE 100k */
#define FLASH_APPBACKUPS_START_ADDR  0x08022000
#define FLASH_APPBACKUPS_END_ADDR  0x0803B000

/*APP DATA FLASH SIZE 20k */
#define FLASH_APP_DATA_START_ADDR  0x0803B000
#define FLASH_APP_DATA_END_ADDR   0x08040000

#define FLASH_END_ADDR        0x08040000

/*BOOT state*/
#define APP_OK          	0xAAAAAAAA
#define APP_UPDATA_JUMP     0xBBBBBBBB
#define APP_UPDATING   		0xCCCCCCCC
#define APP_INIT       		0xFFFFFFFF

/*falsh sector*/
#define FALSH_SECTOR_SIZE 0X800

uint8_t Flash_Write(uint32_t Address, uint8_t *buffer, uint32_t length);
void Flash_Read(uint32_t Address,uint8_t * Buff,uint16_t Length);
uint8_t Flash_Erase(uint32_t Address,uint32_t length);


#endif
