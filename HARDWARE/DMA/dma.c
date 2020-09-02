#include "dma.h"
#include "usart.h"
#include "delay.h"

volatile DMA_Flag DMA2_Stream7_Flag = IDLE;		//USART1
volatile DMA_Flag DMA1_Stream6_Flag = IDLE;		//USART2
volatile DMA_Flag DMA1_Stream3_Flag = IDLE;		//USART3
volatile DMA_Flag DMA1_Stream4_Flag = IDLE;		//UART4
volatile DMA_Flag DMA1_Stream7_Flag = IDLE;		//UART5
volatile DMA_Flag DMA2_Stream6_Flag = IDLE;		//USART6

/*------------------------------------------------
* 函数名：void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr,u8 dir)
* 功  能：配置DMA
* 参  数：DMA_Streamx: DMA数据流（DMA1_Stream0~7/DMA2_Stream0~7）
	     chx: DMA通道选择（范围:0~7）
	     par: 外设地址
		 mar: 存储器地址
		 ndtr: 数据传输量
		 dir： 数据传输方向（DMA_DIR_PeripheralToMemory / DMA_DIR_MemoryToPeripheral / DMA_DIR_MemoryToMemory）
* 返回值： 无
------------------------------------------------*/
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr,u8 dir)
{ 
	DMA_TypeDef *DMAx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<22;//DMA2时钟使能 
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<21;//DMA1时钟使能 
	}
	while(DMA_Streamx->CR&0X01);//等待DMA可配置 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;		//得到stream通道号
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);	//清空之前该stream上的所有中断标志
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);    //清空之前该stream上的所有中断标志
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);//清空之前该stream上的所有中断标志
	else DMAx->LIFCR|=0X3D<<6*streamx;						//清空之前该stream上的所有中断标志
	
	DMA_Streamx->PAR=par;		//DMA外设地址
	DMA_Streamx->M0AR=mar;		//DMA存储器地址
	DMA_Streamx->NDTR=ndtr;		//n个数据项
	DMA_Streamx->CR=0;			//先全部复位CR寄存器值 

	switch(dir)
	{
		case DMA_DIR_PeripheralToMemory: //外设到存储器模式
			DMA_Streamx->CR&=~(1<<6);
			DMA_Streamx->CR&=~(1<<7);
			break;
		case DMA_DIR_MemoryToPeripheral: 
			DMA_Streamx->CR|=1<<6;
			DMA_Streamx->CR&=~(1<<7);
			break;
		case DMA_DIR_MemoryToMemory: 
			DMA_Streamx->CR&=~(1<<6);
			DMA_Streamx->CR|=1<<7;
			break;
		default: break;
	}
	DMA_Streamx->CR|=0<<8;		//非循环模式(即使用普通模式)
	DMA_Streamx->CR|=0<<9;		//外设非增量模式
	DMA_Streamx->CR|=1<<10;		//存储器增量模式
	DMA_Streamx->CR|=0<<11;		//外设数据长度:8位
	DMA_Streamx->CR|=0<<13;		//存储器数据长度:8位
	DMA_Streamx->CR|=1<<16;		//中等优先级
	DMA_Streamx->CR|=0<<21;		//外设突发单次传输
	DMA_Streamx->CR|=0<<23;		//存储器突发单次传输
	DMA_Streamx->CR|=(u32)chx<<25;//通道选择
	//DMA_Streamx->FCR=0x21;	//FIFO控制寄存器

	if(DMA_Streamx == DMA2_Stream7)	//USART1发送
	{
		DMA2_Stream7->CR|=1<<4;		//使能传输完成中断
		MY_NVIC_Init(2,1,DMA2_Stream7_IRQn,2);
	}
	else if(DMA_Streamx == DMA1_Stream6)//USART2发送
	{
		DMA1_Stream6->CR|=1<<4;
		MY_NVIC_Init(2,0,DMA1_Stream6_IRQn,2);
	}
	else if(DMA_Streamx == DMA1_Stream3)//USART3发送
	{
		DMA1_Stream3->CR|=1<<4;
		MY_NVIC_Init(1,3,DMA1_Stream3_IRQn,2);
	}
	else if(DMA_Streamx == DMA1_Stream4)//USART4发送
	{
		DMA1_Stream4->CR|=1<<4;
		MY_NVIC_Init(1,2,DMA1_Stream4_IRQn,2);
	}
	else if(DMA_Streamx == DMA1_Stream7)//USART5发送
	{
		DMA1_Stream7->CR|=1<<4;
		MY_NVIC_Init(1,1,DMA1_Stream7_IRQn,2);
	}
	else if(DMA_Streamx == DMA2_Stream6)//USART6发送
	{
		DMA2_Stream6->CR|=1<<4;
		MY_NVIC_Init(1,0,DMA2_Stream6_IRQn,2);
	}
} 


/*------------------------------------------------
* 函数名：void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx, u32 mar, u16 ndtr)
* 功  能：开启DMA传输
* 参  数：DMA_Streamx: DMA数据流（DMA1_Stream0~7 / DMA2_Stream0~7）
		 mar: 存储器地址
		 ndtr: 数据传输量
* 返回值： 无
------------------------------------------------*/
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx, u32 mar, u16 ndtr)
{
	DMA_Streamx->CR&=~(1<<0); 	//关闭DMA传输 
	while(DMA_Streamx->CR&0X1);	//确保DMA可以被设置  
	DMA_Streamx->M0AR=mar;		//DMA存储器地址
	DMA_Streamx->NDTR=ndtr;		//DMA传输数据量 
	DMA_Streamx->CR|=1<<0;		//开启DMA传输
}	  

//对应USART1发送
void DMA2_Stream7_IRQHandler(void)
{
	if((DMA2->HISR&(1<<27)))
	{
		DMA2->HIFCR|=1<<27;
		DMA2_Stream7_Flag = IDLE;
	}
}

void DMA1_Stream6_IRQHandler(void)
{
	if((DMA1->HISR&(1<<21)))
	{
		DMA1->HIFCR|=1<<21;
		DMA1_Stream6_Flag = IDLE;
	}
}

void DMA1_Stream3_IRQHandler(void)
{
	if((DMA1->LISR&(1<<27)))
	{
		DMA1->LIFCR|=1<<27;
		DMA1_Stream3_Flag = IDLE;
	}
}

void DMA1_Stream4_IRQHandler(void)
{
	if((DMA1->HISR&(1<<5)))
	{
		DMA1->HIFCR|=1<<5;
		DMA1_Stream4_Flag = IDLE;
	}
}

void DMA1_Stream7_IRQHandler(void)
{
	if((DMA1->HISR&(1<<27)))
	{
		DMA1->HIFCR|=1<<27;
		DMA1_Stream7_Flag = IDLE;
	}
}

void DMA2_Stream6_IRQHandler(void)
{
	if((DMA2->HISR&(1<<21)))
	{
		DMA2->HIFCR|=1<<21;
		DMA2_Stream6_Flag = IDLE;
	}
}


