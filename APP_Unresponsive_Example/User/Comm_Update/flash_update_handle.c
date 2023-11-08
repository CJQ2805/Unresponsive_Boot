/**
 *  flash_update_handle.c
 *  @Author: CJQ2805
 *  2023/11/3  8:55
 */ 
#include "flash_update_handle.h"
#include "comm_update_handle.h"
#include "stm32flash.h"

void ChipFlashDownload_Process(void)
{
	if(gt_comm_update_handle.tcomm_update_data.u8FWUpateStatus != FIRM_RECEIVING)
	{
		return;
	}	

	u8 au8data[8] = {0};
	uint8_t au8bootTempData[4] = {0};	
	uint32_t u32AddrApp = FLASH_APPBACKUPS_START_ADDR;	
	uint32_t u32TempCRC32 = 0;	
	uint32_t  i,u32cnt;	
	
	if(gt_comm_update_handle.u8fw_apply == 0x01)
	{
		gt_comm_update_handle.u8fw_apply = 0;
		SEGGER_RTT_printf(0,"check = %d\r\n",gt_comm_update_handle.u8fw_apply);			
		if(gt_comm_update_handle.u16bin_num != 0)
		{
			gt_comm_update_handle.u16bin_num = 0;
			Flash_Write(gt_comm_update_handle.u32bin_addr,gt_comm_update_handle.au8bin_buf,256);
			memset(gt_comm_update_handle.au8bin_buf, 0x00, sizeof(gt_comm_update_handle.au8bin_buf));
			gt_comm_update_handle.u32bin_addr = FLASH_APPBACKUPS_START_ADDR;
		}
		
		if(gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum % 256)
		{
			u32cnt = gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum / 256 + 1;
			
			for(i = 0; i < u32cnt; i++)
			{
				Flash_Read(u32AddrApp,gt_comm_update_handle.au8bin_buf,256);				
			
				u32AddrApp = u32AddrApp + 256;

				if(i == (u32cnt - 1))
				{
					u32TempCRC32 = crc32_compute((uint8_t*)gt_comm_update_handle.au8bin_buf, gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum - (i * 256), &u32TempCRC32);
				}
				else
				{
					u32TempCRC32 = crc32_compute((uint8_t*)gt_comm_update_handle.au8bin_buf, 256, &u32TempCRC32);
				}			
			}			
		}
		else
		{
			u32cnt = gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum / 256;
			for( i = 0; i < u32cnt; i ++ )
			{
				Flash_Read(u32AddrApp,gt_comm_update_handle.au8bin_buf,256);	
				
				u32AddrApp = u32AddrApp + 256;
				u32TempCRC32 = crc32_compute((uint8_t*)gt_comm_update_handle.au8bin_buf, 256, &u32TempCRC32);
			}
		}
		
		if(u32TempCRC32 != gt_comm_update_handle.tcomm_update_data.u32FWCalcBinCRC)
		{		
			gt_comm_update_handle.u8update_flag = 0;
			gt_comm_update_handle.tcomm_update_data.u32FWCalcBinCRC = 0;
			gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum = 0;
			au8data[0] = 0x01;
			can_tx(BMS_UPDATE_ERROR,au8data,1);			
			return ;
		}
		SEGGER_RTT_printf(0,"HAL_NVIC_SystemReset \r\n");		
        //修改标志位 传递包数
		HAL_Delay(100);
		au8bootTempData[0] = (uint8_t)APP_UPDATING;
		au8bootTempData[1] = (uint8_t)APP_UPDATING;
		au8bootTempData[2] = (uint8_t)APP_UPDATING;
		au8bootTempData[3] = (uint8_t)APP_UPDATING;
		Flash_Write(FLASH_BOOTFLAG_ADDRESS, au8bootTempData, 4);

		au8bootTempData[0] = (uint8_t)gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum;
		au8bootTempData[1] = (uint8_t)(gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum >> 8);
		au8bootTempData[2] = (uint8_t)(gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum >> 16);
		au8bootTempData[3] = (uint8_t)(gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum >> 24);
		Flash_Write(FLASH_APPLEN_ADDRESS, au8bootTempData, 4);		

        //开始重启跳转		
		HAL_NVIC_SystemReset();
	}
}


