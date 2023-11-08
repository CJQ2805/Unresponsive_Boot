/*
 * CAN_CircularBuf.h
 *
 * Created: 2022/12/10 17:15:22
 *  Author: CJQ2805
 */ 
#ifndef CAN_CIRCULARBUF_H
#define CAN_CIRCULARBUF_H
#include "sys.h"
typedef __packed  struct  _can_packet_t {

	uint32_t   u32id;
	uint8_t    u8len;
	uint8_t    au8data[8];

}t_can_packet, *pt_can_packet;

typedef struct _can_circul_buf_t {
	
	uint8_t  u8write_pos;
	uint8_t  u8read_pos;
	uint16_t u16CirulBufLen;
	pt_can_packet ptcircul_packet;
	
}t_can_circul_buf ,*pt_can_circul_buf;


extern t_can_circul_buf  gt_STDID_can1_circul_buf;



int can_rx_data(uint8_t u8can_channel ,void * pvcan_rx_header, u8* pu8data);
int can_take_data(pt_can_circul_buf ptcircul_buf, pt_can_packet ptrx_packet);
int can_circul_buf_empty(uint8_t u8channel);
void can_circulbuf_init(void);
#endif
