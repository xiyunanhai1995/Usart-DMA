#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"

typedef enum 
{
	BUSY,
	IDLE
}DMA_Flag;

extern volatile DMA_Flag DMA2_Stream7_Flag;		//USART1
extern volatile DMA_Flag DMA1_Stream6_Flag;		//USART2
extern volatile DMA_Flag DMA1_Stream3_Flag;		//USART3
extern volatile DMA_Flag DMA1_Stream4_Flag;		//UART4
extern volatile DMA_Flag DMA1_Stream7_Flag;		//UART5
extern volatile DMA_Flag DMA2_Stream6_Flag;		//USART6

#define DMA_DIR_PeripheralToMemory        1
#define DMA_DIR_MemoryToPeripheral        2
#define DMA_DIR_MemoryToMemory            3

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr,u8 dir);//配置DMAx_CHx
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx, u32 mar, u16 ndtr);	//使能一次DMA传输		   
#endif






























