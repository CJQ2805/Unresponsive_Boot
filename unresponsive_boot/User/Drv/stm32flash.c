/**
 *  stm32flash.c
 *  @Author: CJQ2805
 *  2022/12/17  11:16
 */ 
#include "stm32flash.h"
#include "SEGGER_RTT.h"

FLASH_EraseInitTypeDef EraseInitStruct;

uint8_t Flash_Write(uint32_t Address, uint8_t *buffer, uint32_t length)
{
	 uint8_t count_Buff_local = 0;
	 uint64_t data = 0;
	 uint64_t data_tmp = 0;
	 uint16_t count_write = 0;
	 uint32_t addr = Address;
	 uint8_t flag_erase = 0;
	 
	 uint32_t PageError;
	 if(Address < FLASH_BASE)
	 {
		return 0;  
	 }
	 if((Address + length) > ((((Address - FLASH_BASE_ADDR)/0x800)*0x800) + 0x800 + FLASH_BASE_ADDR))
	 {
		flag_erase = 1;
	 }
	 HAL_FLASH_Unlock();
	 __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	 while(count_write < length)
	 {
		if(((addr - FLASH_BASE_ADDR)% 0x800 == 0))
		{
		 EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
		if(addr >= 0x8020000)
		{
			EraseInitStruct.Banks       = FLASH_BANK_2;
			EraseInitStruct.Page        = (Address - FLASH_BASE_ADDR)/0x800 + (256-64);
		}
		else
		{
			EraseInitStruct.Banks       = FLASH_BANK_1;
			EraseInitStruct.Page        = (Address - FLASH_BASE_ADDR)/0x800;
		}
		 EraseInitStruct.NbPages     = 1;

		 if(HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
		 {
			HAL_FLASH_Lock();
			return 0;
		 }
		}
		if(flag_erase == 1)
		{
			 flag_erase = 0;
			 EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
				if(addr >= 0x8020000)		
				{
					EraseInitStruct.Banks       = FLASH_BANK_2;
					EraseInitStruct.Page        = (Address - FLASH_BASE_ADDR)/0x800 + (256-64);
				}
				else
				{
					EraseInitStruct.Banks       = FLASH_BANK_1;
					EraseInitStruct.Page        = (Address - FLASH_BASE_ADDR)/0x800;
				}
				EraseInitStruct.NbPages     = 1;

			 if(HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
			 {
				HAL_FLASH_Lock();
				return 0;
			 }
		}
		data = 0;
		for(count_Buff_local = 0;count_Buff_local < 8;count_Buff_local++)
		{
		 data_tmp = *(buffer ++);
		 data = (data_tmp << (8*count_Buff_local)) + data;
		}
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, data) == HAL_OK)
		{
			addr = addr + 8;  /* increment to next double word*/
		}
		else
		{
			HAL_FLASH_Lock();
			return 0;
		}
		count_write += 8;
	}

	HAL_FLASH_Lock();
	return 1;
}

/**
@CJQ2805
此函数只能在STM32G0B1的片子上使用，因为页码是被我写死的，其他片子需要修改BANK的地址
*/
uint8_t Flash_Erase(uint32_t Address,uint32_t length)
{
	uint32_t addr = Address;

	uint32_t PageError;
	if(Address < FLASH_BASE)
	{
	return 0;  
	}
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	
	if(addr >= 0x8020000)
	{
		EraseInitStruct.Banks       = FLASH_BANK_2;
		EraseInitStruct.Page        = (Address - FLASH_BASE_ADDR)/0x800 + (256-64);
	}
	else
	{
		EraseInitStruct.Banks       = FLASH_BANK_1;
		EraseInitStruct.Page        = (Address - FLASH_BASE_ADDR)/0x800;
	}
	
	if(!(length % FALSH_SECTOR_SIZE))
	{
		EraseInitStruct.NbPages = (length/FALSH_SECTOR_SIZE);		
	}	
	else
	{
		EraseInitStruct.NbPages = (length/FALSH_SECTOR_SIZE + 1);			
	}
	
	if(HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
	{
		HAL_FLASH_Lock();
		return 0;
	}
	
	HAL_FLASH_Lock();
	return 1;
}



void Flash_Read(uint32_t Address,uint8_t * Buff,uint16_t Length)
{
	uint16_t count_ReadLength_local = 0;
	uint32_t data = 0;
	while (count_ReadLength_local < Length)
	{
		data = *(__IO uint32_t *)Address;
		Address = Address + 4;
		count_ReadLength_local = count_ReadLength_local + 4;
		*(Buff++) = data;
		*(Buff++) = data >> 8;
		*(Buff++) = data >> 16;
		*(Buff++) = (data >> 24);
	}
}

