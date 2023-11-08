/**
 * sys.c
 *
 * Created: 2023/6/5 17:14:39
 * @Author: CJQ2805
 * 
 */ 
#include "sys.h"


uint8_t crc8(uint8_t *ptr, uint8_t len,uint8_t key)
{
	uint8_t i;
	uint8_t crc=0;
	while(len--!=0)
	{
		for(i=0x80; i!=0; i/=2)
		{
			if((crc & 0x80) != 0)
			{
				crc *= 2;
				crc ^= key;
			}
			else
			crc *= 2;

			if((*ptr & i)!=0)
			crc ^= key;
		}
		ptr++;
	}
	return(crc);
}


uint32_t crc32_compute(uint8_t * p_data, uint16_t size, uint32_t* p_crc)
{
    uint16_t i = 0;
    uint16_t j = 0;
    uint32_t crc;
    crc = ~(*p_crc);
    for (i = 0; i < size; i++)
    {
        crc = crc ^ p_data[i];
        for (j = 8; j > 0; j--)
        {
            crc = (crc >> 1) ^ (0xEDB88320U & ((crc & 1) ? 0xFFFFFFFF : 0));
        }
    }

    return ~crc;
}
