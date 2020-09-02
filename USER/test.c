/*
	该工程为寄存器版本，不过在库函数中仍可添加使用

*/
#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "dma.h"
#include "malloc.h"

u8 TestData[10] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A};

int main(void)
{
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz
	delay_init(168);			//延时初始化 
	
	Init_USART1(84,115200);		//初始化串口波特率为115200
	Init_USART2(42,115200);		
	Init_USART3(42,115200);		
	Init_UART4(42,115200);		
	Init_UART5(42,115200);		
	Init_USART6(84,115200);		

	my_mem_init(SRAMIN);		//初始化内部内存池
	my_mem_init(SRAMCCM);		//初始化内部CCM内存池 
	
//USART1发送 --- DMA2,数据流7,CH4---USART1_TXD 外设为串口1,存储器为USART1_TX_BUF,长度为:USART_LEN
	MYDMA_Config(DMA2_Stream7,4,(u32)&USART1->DR,(u32)USART1_TX_BUF,USART_LEN,DMA_DIR_MemoryToPeripheral);
//USART1接收 --- DMA2,数据流5,CH4---USART1_RXD 外设为串口1,存储器为USART1_RX_BUF,长度为:USART_LEN
	MYDMA_Config(DMA2_Stream5,4,(u32)&USART1->DR,(u32)USART1_RX_BUF,USART_LEN,DMA_DIR_PeripheralToMemory);
	MYDMA_Enable(DMA2_Stream5,(u32)USART1_RX_BUF,USART_LEN);//开始一次DMA传输！
	
//USART2发送 --- DMA1,数据流6,CH4---USART2_TXD 外设为串口2,存储器为USART2_TX_BUF,长度为:USART_LEN
	MYDMA_Config(DMA1_Stream6,4,(u32)&USART2->DR,(u32)USART2_TX_BUF,USART_LEN,DMA_DIR_MemoryToPeripheral);
//USART2接收 --- DMA1,数据流5,CH4---USART2_RXD 外设为串口2,存储器为USART2_RX_BUF,长度为:USART_LEN
	MYDMA_Config(DMA1_Stream5,4,(u32)&USART2->DR,(u32)USART2_RX_BUF,USART_LEN,DMA_DIR_PeripheralToMemory);	
	MYDMA_Enable(DMA1_Stream5,(u32)USART2_RX_BUF,USART_LEN);

//USART3发送 --- DMA1,数据流3,CH4---USART3_TXD 外设为串口3,存储器为USART3_TX_BUF,长度为:USART_LEN
	MYDMA_Config(DMA1_Stream3,4,(u32)&USART3->DR,(u32)USART3_TX_BUF,USART_LEN,DMA_DIR_MemoryToPeripheral);
//USART3接收 --- DMA1,数据流1,CH4---USART3_RXD 外设为串口3,存储器为USART3_RX_BUF,长度为:USART_LEN
	MYDMA_Config(DMA1_Stream1,4,(u32)&USART3->DR,(u32)USART3_RX_BUF,USART_LEN,DMA_DIR_PeripheralToMemory);	
	MYDMA_Enable(DMA1_Stream1,(u32)USART3_RX_BUF,USART_LEN);

//USART4发送 --- DMA1,数据流4,CH4---USART4_TXD 外设为串口4,存储器为USART4_TX_BUF,长度为:USART_LEN
	MYDMA_Config(DMA1_Stream4,4,(u32)&UART4->DR,(u32)UART4_TX_BUF,USART_LEN,DMA_DIR_MemoryToPeripheral);
//USART4接收 --- DMA1,数据流2,CH4---USART4_RXD 外设为串口4,存储器为USART4_RX_BUF,长度为:USART_LEN
	MYDMA_Config(DMA1_Stream2,4,(u32)&UART4->DR,(u32)UART4_RX_BUF,USART_LEN,DMA_DIR_PeripheralToMemory);
	MYDMA_Enable(DMA1_Stream2,(u32)UART4_RX_BUF,USART_LEN);
	
//USART5发送 --- DMA1,数据流7,CH4---UART5_TXD 外设为串口5,存储器为UART5_TX_BUF,长度为:USART_LEN
	MYDMA_Config(DMA1_Stream7,4,(u32)&UART5->DR,(u32)UART5_TX_BUF,USART_LEN,DMA_DIR_MemoryToPeripheral);
//USART5接收 --- DMA1,数据流0,CH4---UART5_RXD 外设为串口5,存储器为UART5_RX_BUF,长度为:USART_LEN
	MYDMA_Config(DMA1_Stream0,4,(u32)&UART5->DR,(u32)UART5_RX_BUF,USART_LEN,DMA_DIR_PeripheralToMemory);
	MYDMA_Enable(DMA1_Stream0,(u32)UART5_RX_BUF,USART_LEN);

//USART6发送 --- DMA2,数据流6,CH5---USART6_TXD 外设为串口6,存储器为USART6_TX_BUF,长度为:USART_LEN
	MYDMA_Config(DMA2_Stream6,5,(u32)&USART6->DR,(u32)USART6_TX_BUF,USART_LEN,DMA_DIR_MemoryToPeripheral);
//USART6接收 --- DMA2,数据流1,CH5---USART6_RXD 外设为串口6,存储器为USART6_RX_BUF,长度为:USART_LEN
	MYDMA_Config(DMA2_Stream1,5,(u32)&USART6->DR,(u32)USART6_RX_BUF,USART_LEN,DMA_DIR_PeripheralToMemory);
	MYDMA_Enable(DMA2_Stream1,(u32)USART6_RX_BUF,USART_LEN);
	
	while(1)
	{
//		myDMAprintf(USART1,"usart = %d\tch = %f\r\n",1,1.567);
//		myDMAprintf(USART2,"usart = %d\tch = %f\r\n",2,1.567);
		myDMAprintf(USART3,"usart = %d\tch = %f\r\n",3,1.567);
		myDMAprintf(USART3,"usart = %d\tch = %f\r\n",3,1.567);
//		myDMAprintf(UART4,"usart = %d\tch = %f\r\n",4,1.567);
//		myDMAprintf(UART5,"usart = %d\tch = %f\r\n",5,1.567);
		myDMAprintf(USART6,"usart = %d\tch = %f\r\n",1,1.567);
		myDMAprintf(USART6,"usart = %d\tch = %f\r\n",1,1.567);
		
//		myDMAarray(USART1,(u32)TestData,10);
//		myDMAarray(USART2,(u32)TestData,10);
//		myDMAarray(USART3,(u32)TestData,10);
//		myDMAarray(USART3,(u32)TestData,10);
//		myDMAarray(UART4,(u32)TestData,10);
//		myDMAarray(UART5,(u32)TestData,10);
//		myDMAarray(USART6,(u32)TestData,10); 
//		myDMAarray(USART6,(u32)TestData,10);   		
		
		delay_ms(1000);
	}
}

















