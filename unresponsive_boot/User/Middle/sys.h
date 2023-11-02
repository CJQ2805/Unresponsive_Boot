/**
 *  sys.h
 *  @Author: CJQ2805
 *  2022/12/1  13:16
 */ 

#ifndef __SYS_H
#define __SYS_H

#include "stm32g0xx_it.h"
#include "stm32g0xx_hal.h"

#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "main.h"


typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  
typedef const int16_t sc16;  
typedef const int8_t sc8;  

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  
typedef __I int16_t vsc16; 
typedef __I int8_t vsc8;   


typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  
typedef const uint16_t uc16;  
typedef const uint8_t uc8; 

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  
typedef __I uint16_t vuc16; 
typedef __I uint8_t vuc8;





/*************************************************/
/**
	@brief CJQ2805 简要
	 Cortex - M0+ STM32 地址映射操作
*/
typedef struct
{
    u16 bit0 : 1;
    u16 bit1 : 1;
    u16 bit2 : 1;
    u16 bit3 : 1;
    u16 bit4 : 1;
    u16 bit5 : 1;
    u16 bit6 : 1;
    u16 bit7 : 1;
    u16 bit8 : 1;
    u16 bit9 : 1;
    u16 bit10 : 1;
    u16 bit11 : 1;
    u16 bit12 : 1;
    u16 bit13 : 1;
    u16 bit14 : 1;
    u16 bit15 : 1;
	
} Bits16_TypeDef;

#define PAin(n) ( ( GPIOA->IDR&(1 << (n)) )>>n )
#define PBin(n) ( ( GPIOB->IDR&(1 << (n)) )>>n )
#define PCin(n) ( ( GPIOC->IDR&(1 << (n)) )>>n )
#define PDin(n) ( ( GPIOD->IDR&(1 << (n)) )>>n )
#define PEin(n) ( ( GPIOE->IDR&(1 << (n)) )>>n )
#define PFin(n) ( ( GPIOF->IDR&(1 << (n)) )>>n )

#define PAout(n)  ( ((Bits16_TypeDef *)(&(GPIOA->ODR)))->bit##n )
#define PBout(n)  ( ((Bits16_TypeDef *)(&(GPIOB->ODR)))->bit##n )
#define PCout(n)  ( ((Bits16_TypeDef *)(&(GPIOC->ODR)))->bit##n )
#define PDout(n)  ( ((Bits16_TypeDef *)(&(GPIOD->ODR)))->bit##n )
#define PEout(n)  ( ((Bits16_TypeDef *)(&(GPIOE->ODR)))->bit##n )
#define PFout(n)  ( ((Bits16_TypeDef *)(&(GPIOF->ODR)))->bit##n )

/*************************************************/

typedef enum
{
    Ok = 0u,                       ///< No error
    Error = 1u,                    ///< Non-specific error code
    ErrorAddressAlignment = 2u,    ///< Address alignment does not match
    ErrorAccessRights = 3u,        ///< Wrong mode (e.g. user/system) mode is set
    ErrorInvalidParameter = 4u,    ///< Provided parameter is not valid
    ErrorOperationInProgress = 5u, ///< A conflicting or requested operation is still in progress
    ErrorInvalidMode = 6u,         ///< Operation not allowed in current mode
    ErrorUninitialized = 7u,       ///< Module (or part of it) was not initialized properly
    ErrorBufferFull = 8u,          ///< Circular buffer can not be written because the buffer is full
    ErrorTimeout = 9u,             ///< Time Out error occurred (e.g. I2C arbitration lost, Flash time-out, etc.)
    ErrorNotReady = 10u,           ///< A requested final state is not reached
    ErrorCommunication = 11u,      ///< Communication error occured between MCU and peripheral
    ErrorVerificationFailed = 12u, ///<Datas verification failed(e.g. CRC/checksum)
    OperationInProgress = 13u      ///< Indicator for operation in progress
} en_result_t;

/* Delay counter type */
typedef uint32_t u32TimDly_t;



/*Timer*/
#define TMR0_INTERVAL 10UL /* ms */
#define TMR0_DELAY_MS(nms) ((nms) / TMR0_INTERVAL)
#define TMR0_DELAY_SEC(nsec) ((nsec)*TMR0_DELAY_MS(1000))
#define TMR0_DELAY_MIN(nmin) ((nmin)*TMR0_DELAY_SEC(60))


/* Returns the minimum value out of two values */
#define MINIMUM(X, Y) ((X) < (Y) ? (X) : (Y))
/* Returns the maximum value out of two values */
#define MAXIMUM(X, Y) ((X) > (Y) ? (X) : (Y))



uint8_t crc8(uint8_t *ptr, uint8_t len,uint8_t key);
uint32_t crc32_compute(uint8_t * p_data, uint16_t size, uint32_t* p_crc);

#endif
