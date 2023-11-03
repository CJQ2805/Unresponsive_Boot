/**
 *  comm_cir_handle.h
 *  @Author: CJQ2805
 *  2023/1/28  8:55
 */ 
#ifndef __COMM_CIR_HANDLE_H
#define __COMM_CIR_HANDLE_H
#include "sys.h"

//Firmware update status
#define		IDLE							0x00
#define 	IN_BOOTLOADER					0x01
#define		PREPARING						0x02
#define		RECEIVING						0x03
#define		APPLYING						0x10


enum _bms_update_chuck_e{
	
	BMS_UPDATE_CHUCK_FIRST_PACK = 0,
	BMS_UPDATE_CHUCK_SECOND_PACK,
	BMS_UPDATE_CHUCK_THIRD_PACK,
	BMS_UPDATE_CHUCK_CRC_SIZE_PACK,
	BMS_UPDATE_CHUCK_DATA_PACK,

};


enum _comm_bms_update_e{
	
	BMS_UPDATE_NULL ,
	BMS_UPDATE_START  = 0xAD00,
	BMS_UPDATE_STATUS = 0xAD01,
	BMS_UPDATE_ACKED_CHUCK = 0xAD02,
	BMS_UPDATE_CHUCK = 0xAD05,
	BMS_UPDATE_APPLY = 0xAD10,
	
};


struct _comm_update_data_t{

	uint32_t u32FWInsideBinSize;
	uint32_t u32FWInsideBinCRC;
	uint8_t  u8FWUpateStatus;
	
	uint32_t u32FWCalcBinNum;
	uint32_t u32FWCalcBinCRC;
};


struct _comm_update_handle_t{
	
	u8  u8comm_stat;
	enum _bms_update_chuck_e    ebms_update_chuck;
	struct _comm_update_data_t  tcomm_update_data;
	
	u8  au8bin_buf[256];
	u16 u16bin_num;
	u32 u32bin_addr;
	
	u8 u8fw_apply;
	
};

extern struct _comm_update_handle_t   gt_comm_update_handle;

void ChipFlashDownload_Process(void);
void CAN_DataDecode(void);
#endif
