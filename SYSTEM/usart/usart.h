#ifndef __USART_H
#define __USART_H 
#include "sys.h"
#include "stdio.h"	
#include <string.h>
#include <stdarg.h>

#define OK		0
#define ERROR	1

typedef int Status;

#define USART_LEN  				50  	//定义最大接收字节数 50

#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口1接收
#define EN_USART3_RX 			1		//使能（1）/禁止（0）串口1接收
#define EN_UART4_RX 			1		//使能（1）/禁止（0）串口1接收
#define EN_UART5_RX 			1		//使能（1）/禁止（0）串口1接收
#define EN_USART6_RX 			1		//使能（1）/禁止（0）串口1接收

extern u8 USART1_TX_BUF[USART_LEN];
extern u8 USART1_RX_BUF[USART_LEN];
extern u8 USART2_TX_BUF[USART_LEN];
extern u8 USART2_RX_BUF[USART_LEN];
extern u8 USART3_TX_BUF[USART_LEN];
extern u8 USART3_RX_BUF[USART_LEN];
extern u8 UART4_TX_BUF[USART_LEN];
extern u8 UART4_RX_BUF[USART_LEN];
extern u8 UART5_TX_BUF[USART_LEN];
extern u8 UART5_RX_BUF[USART_LEN];
extern u8 USART6_TX_BUF[USART_LEN];
extern u8 USART6_RX_BUF[USART_LEN];


void Init_USART1(u32 pclk2,u32 bound);
void Init_USART2(u32 pclk2,u32 bound);
void Init_USART3(u32 pclk2,u32 bound); 
void Init_UART4(u32 pclk2,u32 bound);
void Init_UART5(u32 pclk2,u32 bound);
void Init_USART6(u32 pclk2,u32 bound);

Status myDMAprintf(USART_TypeDef *USARTx, const char *format, ...);
Status myDMAarray(USART_TypeDef *USARTx, u32 mar, u16 ndtr);

#endif	   
















