/**
 *  comm_update_handle.c
 *  @Author: CJQ2805
 *  2023/11/3  8:55
 */ 
#include "comm_update_handle.h"
#include "CAN_CircularBuf.h"
#include "stm32flash.h"
#include "can.h"
uint8_t binString1[] = "SCUD-BMS";
uint8_t binString2[] = "COMOTOMO";
uint8_t binString3[] = "00000000";

struct _comm_update_handle_t   gt_comm_update_handle;

void bms_update_start(pt_can_packet  ptrx_packet);
void bms_UpdateChuck(pt_can_packet  ptrx_packet);
void bms_update_apply(pt_can_packet  ptrx_packet);
void bms_get_update_status(void);

void CAN_DataDecode(void)
{
	u8 u8comm_err_flag = 0;
	static u32 u32cnt = 0;	
    enum _comm_bms_update_e  ecomm_bms_update;
	
	t_can_packet   trx_packet;
	memset(&trx_packet, 0x00, sizeof(trx_packet));

	if(can_take_data(&gt_STDID_can1_circul_buf, &trx_packet) <= 0 )
	{
		u32cnt++;
		if(u32cnt> 500)
		{
			u32cnt = 0;
			u8comm_err_flag = 1;
		}
		return;
	}
	u32cnt = 0;	
	
	ecomm_bms_update = trx_packet.u32id;
	
	switch(ecomm_bms_update)
	{
		
		case BMS_UPDATE_START: 	       //  R/W
		
		bms_update_start(&trx_packet);
		
		break;
		
		case BMS_UPDATE_STATUS:	       //  R
		
		bms_get_update_status();
		
		break;
		
		case BMS_UPDATE_ACKED_CHUCK:   //  R
			
		
		break;
		
		case BMS_UPDATE_CHUCK:		  //  R/W
 		
		bms_UpdateChuck(&trx_packet);
		
		break;
		
		case BMS_UPDATE_APPLY:        //  R/W
			
		bms_update_apply(&trx_packet);
		
		break;
		
	
		default:
			
		break;
	}
	
}

void bms_update_start(pt_can_packet  ptrx_packet)
{	
	//整包CRC和整包Bin大小已经不要了，在后续的发包中使用
//	gt_comm_update_handle.tcomm_update_data.u32FWUpateBinSize = ((uint32_t)ptrx_packet->au8data[0]) << 24;
//	gt_comm_update_handle.tcomm_update_data.u32FWUpateBinSize = ((uint32_t)ptrx_packet->au8data[1]) << 16;
//	gt_comm_update_handle.tcomm_update_data.u32FWUpateBinSize = ((uint32_t)ptrx_packet->au8data[2]) << 8;
//	gt_comm_update_handle.tcomm_update_data.u32FWUpateBinSize = ((uint32_t)ptrx_packet->au8data[3]);	
//	
//	gt_comm_update_handle.tcomm_update_data.u32FWUpateBinCRC = ((uint32_t)ptrx_packet->au8data[4]) << 24;
//	gt_comm_update_handle.tcomm_update_data.u32FWUpateBinCRC = ((uint32_t)ptrx_packet->au8data[5]) << 16;
//	gt_comm_update_handle.tcomm_update_data.u32FWUpateBinCRC = ((uint32_t)ptrx_packet->au8data[6]) << 8;
//	gt_comm_update_handle.tcomm_update_data.u32FWUpateBinCRC = ((uint32_t)ptrx_packet->au8data[7]);	
	

	gt_comm_update_handle.tcomm_update_data.u8FWUpateStatus = RECEIVING;
	
	gt_comm_update_handle.ebms_update_chuck = BMS_UPDATE_CHUCK_FIRST_PACK;
	gt_comm_update_handle.u32bin_addr = FLASH_APPBACKUPS_START_ADDR;
	
	gt_comm_update_handle.u16bin_num = 0;	
	gt_comm_update_handle.tcomm_update_data.u32FWCalcBinCRC = 0;
	gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum = 0;
	gt_comm_update_handle.tcomm_update_data.u32FWInsideBinCRC = 0;
	gt_comm_update_handle.tcomm_update_data.u32FWInsideBinSize = 0;
	
	u8 au8data[8] = {0};	
	can_tx(BMS_UPDATE_START,au8data,0);
	
}

void bms_get_update_status(void)
{
	u8 au8data[8] = {0};
	au8data[0] = gt_comm_update_handle.tcomm_update_data.u8FWUpateStatus;
	can_tx(BMS_UPDATE_STATUS,au8data,1);
}


void bms_UpdateChuck(pt_can_packet  ptrx_packet)
{
	uint8_t u8lenEnd = 0;
	
	if(gt_comm_update_handle.tcomm_update_data.u8FWUpateStatus == RECEIVING)
	{
		
		switch(gt_comm_update_handle.ebms_update_chuck)
		{
			case BMS_UPDATE_CHUCK_FIRST_PACK:
			
			if(0 == strcmp((char *)ptrx_packet->au8data, (char *)binString1))
			{
				gt_comm_update_handle.ebms_update_chuck = BMS_UPDATE_CHUCK_SECOND_PACK;
			}
			else
			{
				gt_comm_update_handle.ebms_update_chuck = BMS_UPDATE_CHUCK_FIRST_PACK;			
			}
			
			break;
			
			case BMS_UPDATE_CHUCK_SECOND_PACK:
			
			if(0 == strcmp((char *)ptrx_packet->au8data, (char *)binString2))
			{
				gt_comm_update_handle.ebms_update_chuck = BMS_UPDATE_CHUCK_THIRD_PACK;
			}
			else
			{
				gt_comm_update_handle.ebms_update_chuck = BMS_UPDATE_CHUCK_FIRST_PACK;	
			}
		
			break;
			
			case BMS_UPDATE_CHUCK_THIRD_PACK:
				
			if(0 == strcmp((char *)ptrx_packet->au8data, (char *)binString3))
			{
				gt_comm_update_handle.ebms_update_chuck = BMS_UPDATE_CHUCK_CRC_SIZE_PACK;
			}
			else
			{
				gt_comm_update_handle.ebms_update_chuck = BMS_UPDATE_CHUCK_FIRST_PACK;	
			}			
			
			break;
			
			case BMS_UPDATE_CHUCK_CRC_SIZE_PACK:
					
			gt_comm_update_handle.tcomm_update_data.u32FWInsideBinSize = ((uint32_t)ptrx_packet->au8data[0]) << 24 |  
																		 ((uint32_t)ptrx_packet->au8data[1]) << 16 | 
																		 ((uint32_t)ptrx_packet->au8data[2]) << 8  |
																		 ((uint32_t)ptrx_packet->au8data[3]);	
			
			gt_comm_update_handle.tcomm_update_data.u32FWInsideBinCRC =  ((uint32_t)ptrx_packet->au8data[4]) << 24 |  
																		 ((uint32_t)ptrx_packet->au8data[5]) << 16 | 
																		 ((uint32_t)ptrx_packet->au8data[6]) << 8  |
																		 ((uint32_t)ptrx_packet->au8data[7]);
		
			
			if(gt_comm_update_handle.tcomm_update_data.u32FWInsideBinSize > (FLASH_APPBACKUPS_END_ADDR - FLASH_APPBACKUPS_START_ADDR))
			{
				gt_comm_update_handle.ebms_update_chuck = BMS_UPDATE_CHUCK_FIRST_PACK;		
				gt_comm_update_handle.tcomm_update_data.u32FWInsideBinSize = 0;
				gt_comm_update_handle.tcomm_update_data.u32FWInsideBinCRC = 0;
			}
			else
			{
				gt_comm_update_handle.ebms_update_chuck = BMS_UPDATE_CHUCK_DATA_PACK;
			}
			
			break;
			
			case BMS_UPDATE_CHUCK_DATA_PACK:
			
			gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum = gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum + 8;
			
			if(gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum >= gt_comm_update_handle.tcomm_update_data.u32FWInsideBinSize )
			{
				u8lenEnd = gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum - gt_comm_update_handle.tcomm_update_data.u32FWInsideBinSize;
				gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum = gt_comm_update_handle.tcomm_update_data.u32FWInsideBinSize;
				gt_comm_update_handle.tcomm_update_data.u32FWCalcBinCRC = crc32_compute((uint8_t*)ptrx_packet->au8data, 8 - u8lenEnd, &gt_comm_update_handle.tcomm_update_data.u32FWCalcBinCRC);
			}
			else
			{
				gt_comm_update_handle.tcomm_update_data.u32FWCalcBinCRC = crc32_compute((uint8_t*)ptrx_packet->au8data, 8, &gt_comm_update_handle.tcomm_update_data.u32FWCalcBinCRC);					
			}
			
			
			for(uint8_t i = 0; i < 8; i++)
			{
				if(gt_comm_update_handle.u16bin_num >= 256)
				{
					gt_comm_update_handle.u16bin_num = 0;
				}
				gt_comm_update_handle.au8bin_buf[gt_comm_update_handle.u16bin_num++] = ptrx_packet->au8data[i];
			
			}
			
			if((gt_comm_update_handle.u16bin_num == 256) && (gt_comm_update_handle.u32bin_addr < (FLASH_APPBACKUPS_END_ADDR - 256)))
			{
				gt_comm_update_handle.u16bin_num = 0;
				Flash_Write(gt_comm_update_handle.u32bin_addr,gt_comm_update_handle.au8bin_buf,256);
				memset(gt_comm_update_handle.au8bin_buf, 0x00, 256);
				gt_comm_update_handle.u32bin_addr = gt_comm_update_handle.u32bin_addr + 256;
			}
			
			break;
		
		}
	}

}


void bms_update_apply(pt_can_packet  ptrx_packet)
{
	
	gt_comm_update_handle.ebms_update_chuck = BMS_UPDATE_CHUCK_FIRST_PACK;
	
	u8 au8data[8] = {0};

	
	if(ptrx_packet->au8data[0] == 0x00)   //读
	{
		
	}
	else if(ptrx_packet->au8data[0] == 0x01) // 写
	{
		if(ptrx_packet->au8data[1] == 0x01)
		{
			if((gt_comm_update_handle.tcomm_update_data.u32FWCalcBinCRC == gt_comm_update_handle.tcomm_update_data.u32FWInsideBinCRC) && \
			   (gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum == gt_comm_update_handle.tcomm_update_data.u32FWInsideBinSize) && \
			   (gt_comm_update_handle.tcomm_update_data.u32FWInsideBinSize != 0))
			{
				gt_comm_update_handle.u8fw_apply = ptrx_packet->au8data[1];
				au8data[0] = ptrx_packet->au8data[0];
				au8data[1] = ptrx_packet->au8data[1];
				can_tx(BMS_UPDATE_STATUS,au8data,2);	
			}
			else
			{
				
				if((gt_comm_update_handle.tcomm_update_data.u32FWCalcBinCRC != gt_comm_update_handle.tcomm_update_data.u32FWInsideBinCRC) || \
					(gt_comm_update_handle.tcomm_update_data.u32FWInsideBinSize == 0))
				{
					//CRC错误
					gt_comm_update_handle.u8fw_apply = ptrx_packet->au8data[1];
					au8data[0] = ptrx_packet->au8data[0];
					au8data[1] = 0xF0;
					can_tx(BMS_UPDATE_STATUS,au8data,2);						
					
				}
				else
				{
					gt_comm_update_handle.u8fw_apply = ptrx_packet->au8data[1];
					au8data[0] = ptrx_packet->au8data[0];
					au8data[1] = 0xF1;
					can_tx(BMS_UPDATE_STATUS,au8data,2);					
					//ERROR CHUNK
				}
				
				gt_comm_update_handle.tcomm_update_data.u8FWUpateStatus = IN_BOOTLOADER;
				gt_comm_update_handle.tcomm_update_data.u32FWInsideBinCRC = 0;
				gt_comm_update_handle.tcomm_update_data.u32FWCalcBinCRC = 0;
				gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum = 0;
				gt_comm_update_handle.tcomm_update_data.u32FWInsideBinSize = 0;				
			}
		}
		else if(ptrx_packet->au8data[1] == 0x00)
		{
			//取消升级
			gt_comm_update_handle.tcomm_update_data.u8FWUpateStatus = IN_BOOTLOADER;
			gt_comm_update_handle.tcomm_update_data.u32FWInsideBinCRC = 0;
			gt_comm_update_handle.tcomm_update_data.u32FWCalcBinCRC = 0;
			gt_comm_update_handle.tcomm_update_data.u32FWCalcBinNum = 0;
			gt_comm_update_handle.tcomm_update_data.u32FWInsideBinSize = 0;	
			au8data[0] = ptrx_packet->au8data[0];
			au8data[1] = ptrx_packet->au8data[1];
			can_tx(BMS_UPDATE_STATUS,au8data,2);			
		}
		else
		{
			au8data[0] = ptrx_packet->au8data[0];
			au8data[1] = 0xF2;
			can_tx(BMS_UPDATE_STATUS,au8data,2);				
			//错误的写
		}
					
	}
}



void bms_update_acked_chuck(pt_can_packet  ptrx_packet)
{

}




