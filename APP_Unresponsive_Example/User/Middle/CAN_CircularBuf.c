/**
 * CAN_CircularBuf.c
 *
 * Created: 2022/12/10 17:14:39
 * @Author: CJQ2805
 * 
 */ 
#include "CAN_CircularBuf.h"
#include "stdlib.h"
#include "can.h"

#define CAN1_CIRCUL_BUF_LEN  100

t_can_circul_buf  gt_STDID_can1_circul_buf;

int can_reset_buf(pt_can_circul_buf ptcircul_buf)
{
	if(NULL == ptcircul_buf)
	{
#if RTTENABLE
		SEGGER_RTT_printf(0,"the circul_buf is null \r\n");
#endif		
		
		return -1;
	}
	ptcircul_buf->u8write_pos = ptcircul_buf->u8read_pos = 0;
	
	return 0;
}

int can_is_full(pt_can_circul_buf ptcircul_buf)
{
	if(NULL ==  ptcircul_buf)
	{
#if RTTENABLE
		SEGGER_RTT_printf(0,"the circul_buf is null \r\n");
#endif		

		return 0;		
	}
	
	return	(((ptcircul_buf->u8write_pos + 1) % ptcircul_buf->u16CirulBufLen) == ptcircul_buf->u8read_pos);
}

int can_is_empty(pt_can_circul_buf ptcircul_buf)
{
	if(NULL == ptcircul_buf)
	{
#if RTTENABLE
		SEGGER_RTT_printf(0,"the circul_buf is null \r\n");
#endif		

		return 1;
	}
	
	return (ptcircul_buf->u8write_pos == ptcircul_buf->u8read_pos);
}


int can_put_data(pt_can_circul_buf ptcircul_buf , pt_can_packet ptrx_packet)
{
	if( ( NULL == ptcircul_buf ) || ( NULL == ptrx_packet ) || ( NULL == ptcircul_buf->ptcircul_packet ))
	{
#if RTTENABLE
		SEGGER_RTT_printf(0,"put data is null \r\n");
#endif		
		
		return -1;
	}

	if (can_is_full(ptcircul_buf))
	{
		return 0;
	}
	else
	{
		memcpy(&ptcircul_buf->ptcircul_packet[ptcircul_buf->u8write_pos] , ptrx_packet , sizeof(t_can_packet) );		
		ptcircul_buf->u8write_pos = (ptcircul_buf->u8write_pos + 1) % ptcircul_buf->u16CirulBufLen;
	
		return 1;
	}
	
}



int can_take_data(pt_can_circul_buf ptcircul_buf, pt_can_packet ptrx_packet)
{

	if( ( NULL == ptcircul_buf ) || ( NULL == ptrx_packet ) )
	{
#if RTTENABLE
		SEGGER_RTT_printf(0,"the circul_buf is null \r\n");
#endif	
		return -1;
	}


	if (can_is_empty(ptcircul_buf))
	return 0;
	else
	{
		memcpy(ptrx_packet , &ptcircul_buf->ptcircul_packet[ptcircul_buf->u8read_pos] , sizeof(t_can_packet));
		ptcircul_buf->u8read_pos = (ptcircul_buf->u8read_pos + 1) % ptcircul_buf->u16CirulBufLen;
		return 1;
	}
}


int can_rx_data(uint8_t u8can_channel ,void * pvcan_rx_header, u8* pu8data)
{
	t_can_packet     tpacket;

	FDCAN_RxHeaderTypeDef* FDCAN_RxHeader = (FDCAN_RxHeaderTypeDef*)pvcan_rx_header;
	
	tpacket.u32id = FDCAN_RxHeader->Identifier;
	tpacket.u8len = (FDCAN_RxHeader->DataLength >> 16);
	memcpy(tpacket.au8data, pu8data, sizeof(tpacket.au8data));

	if(sizeof(tpacket.au8data) != FDCAN_RxHeader->DataLength)
	{
		for(uint8_t i = 0; i < (sizeof(tpacket.au8data) - tpacket.u8len); i++ )
		tpacket.au8data[i+tpacket.u8len] = 0x00;	
	}
	
	return can_put_data(&gt_STDID_can1_circul_buf, &tpacket);		
}


void can_circulbuf_init(void)
{
	memset(&gt_STDID_can1_circul_buf, 0, sizeof(gt_STDID_can1_circul_buf));
	gt_STDID_can1_circul_buf.ptcircul_packet = malloc(sizeof(t_can_packet)*CAN1_CIRCUL_BUF_LEN);
	gt_STDID_can1_circul_buf.u16CirulBufLen = CAN1_CIRCUL_BUF_LEN;	
}
