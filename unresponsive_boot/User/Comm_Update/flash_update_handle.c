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
			gt_comm_update_handle.tcomm_update_data.u32FWCalcBinCRC = 0;
			gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum = 0;
			au8data[0] = 0x01;
			can_tx(BMS_UPDATE_ERROR,au8data,1);			
			return ;
		}
		
        //修改标志位 传递包数
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

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;

void JumpToAPP(void)
{
    uint32_t JumpAddress;
    __disable_irq();
    JumpAddress = *(__IO uint32_t*) (FLASH_APP_START_ADDR + 4);
    Jump_To_Application = (pFunction) JumpAddress;
    __set_MSP(*(__IO uint32_t*) FLASH_APP_START_ADDR);
    SCB->VTOR = FLASH_APP_START_ADDR;
    Jump_To_Application();
    while(1);
}

uint32_t BootFlagState_Read(void)
{
    uint8_t data[4];
    uint32_t u16Ret;

    Flash_Read(FLASH_BOOTFLAG_ADDRESS,data,4);
    u16Ret = data[0];
    u16Ret = (u16Ret << 8) + data[1];
    u16Ret = (u16Ret << 8) + data[2];
    u16Ret = (u16Ret << 8) + data[3];

    return u16Ret;
}

uint16_t BootFlagState_Write(uint8_t* data)
{

    if(Flash_Write(FLASH_BOOTFLAG_ADDRESS,data,4)!=1)
    {
        return 1;
    }

    return 0;

}

uint8_t BootFlag_Diag(void)
{
    uint32_t u32BootState_Flag = 0;

    u32BootState_Flag = BootFlagState_Read();
    if((u32BootState_Flag == APP_OK)  || (u32BootState_Flag == APP_INIT))
    {
        JumpToAPP();
		HAL_Delay(50);
    }
    return 0;
}


uint8_t AppUpdate_Process(void)
{
	//SEGGER_RTT_printf(0,"AppUpdate_Process\r\n");
    uint32_t  i,cnt;
    uint32_t u32BootState_Flag = 0;
    uint32_t u32AppUpdatePacKLen = 0;
    uint32_t u32WriteAddr = FLASH_APP_START_ADDR;
    uint32_t u32WriteAddrBoot = FLASH_APPBACKUPS_START_ADDR;
    uint8_t  data[4]= {0};
    uint8_t  u8Tmp[256]= {0};
    u32BootState_Flag = BootFlagState_Read();
    if(u32BootState_Flag == APP_UPDATING)
    {
        Flash_Read(FLASH_APPLEN_ADDRESS,data,4);
        u32AppUpdatePacKLen = data[0];
        u32AppUpdatePacKLen = u32AppUpdatePacKLen + (((uint32_t)data[1]) << 8);
        u32AppUpdatePacKLen = u32AppUpdatePacKLen + (((uint32_t)data[2]) << 16);
        u32AppUpdatePacKLen = u32AppUpdatePacKLen + (((uint32_t)data[3]) << 24);
        if(u32AppUpdatePacKLen % 256)
        {
            cnt = u32AppUpdatePacKLen / 256 + 1;
        }
        else
        {
            cnt = u32AppUpdatePacKLen/256;
        }
        for( i = 0; i < cnt; i ++)
        {
            Flash_Read(u32WriteAddrBoot, u8Tmp, 256);
            Flash_Write(u32WriteAddr, u8Tmp, 256);
            u32WriteAddr = u32WriteAddr + 256;
            u32WriteAddrBoot = u32WriteAddrBoot + 256;
        }
        // if(u32WriteAddr - FLASH_BOOTFLAG_DATA_END_ADDR >= u32AppUpdatePacKLen)
        {
            data[0] = (uint8_t)APP_UPDATA_JUMP;
            data[1] = (uint8_t)APP_UPDATA_JUMP;
            data[2] = (uint8_t)APP_UPDATA_JUMP;
            data[3] = (uint8_t)APP_UPDATA_JUMP;
            BootFlagState_Write(data);
            JumpToAPP();
			HAL_Delay(50);
        }
    }

    return 0;
}
