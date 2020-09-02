#include "sys.h"
#include "usart.h"	
#include "delay.h"
#include "dma.h"
#include "malloc.h"
#include "led.h"


////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif

////////////////////////////////////////////////////////////////////////////////// 	 
//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 
//end

u8 USART1_TX_BUF[USART_LEN]={0};
u8 USART1_RX_BUF[USART_LEN]={0};
u8 USART2_TX_BUF[USART_LEN]={0};
u8 USART2_RX_BUF[USART_LEN]={0};
u8 USART3_TX_BUF[USART_LEN]={0};
u8 USART3_RX_BUF[USART_LEN]={0};
u8 UART4_TX_BUF[USART_LEN]={0};
u8 UART4_RX_BUF[USART_LEN]={0};
u8 UART5_TX_BUF[USART_LEN]={0};
u8 UART5_RX_BUF[USART_LEN]={0};
u8 USART6_TX_BUF[USART_LEN]={0};
u8 USART6_RX_BUF[USART_LEN]={0};


int16_t real_current;
int16_t real_velocity;
int32_t real_position;

//=======================================================================	USART1

/*------------------------------------------------
* 函数名：void Init_USART1(u32 pclk2,u32 bound)
* 功  能：初始化IO 串口1
* 参  数：pclk2: PCLK2时钟频率(Mhz)
	      bound: 波特率 
* 返回值： 无
------------------------------------------------*/
void Init_USART1(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV@OVER8=0
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->AHB1ENR|=1<<0;   	//使能PORTA口时钟  
	RCC->APB2ENR|=1<<4;  	//使能串口1时钟 
	GPIO_Set(GPIOA,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
 	GPIO_AF_Set(GPIOA,9,7);	//PA9,AF7
	GPIO_AF_Set(GPIOA,10,7);//PA10,AF7  	   
	//波特率设置
 	USART1->BRR=mantissa; 	//波特率设置	 
	USART1->CR1&=~(1<<15); 	//设置OVER8=0 
	
	USART1->CR1|=1<<3;  	//串口发送使能 
	USART1->CR3|=1<<7;      //使能串口1的DMA发送
	
#if EN_USART1_RX		  	//如果使能了接收	
	USART1->CR1|=1<<2;  	//串口接收使能
	USART1->CR3|=1<<6;      //使能串口1的DMA接收  
	USART1->CR1|=1<<4;    	//使能空闲中断	    	
	MY_NVIC_Init(3,3,USART1_IRQn,2);//组2，最低优先级 
#endif
	USART1->CR1|=1<<13;  	//串口使能
}								 

#if EN_USART1_RX

void USART1_IRQHandler(void)
{
	u8 temp;
	u16 len;
	
	if(USART1->SR&(1<<4))//检测到线路空闲
	{
//软件序列清除IDLE标志位
		temp = USART1->SR;
		temp = USART1->DR;
		
		DMA2_Stream5->CR &=~(1<<0); //关闭DMA传输,准备重新配置
		DMA2->HIFCR|=1<<11;			//清除DMA2_Steam5传输完成标志
		DMA2->HIFCR|=1<<9;			//清除DMA2_Steam5传输错误标志
	
		len = USART_LEN - (uint16_t)(DMA2_Stream5->NDTR);
		myDMAprintf(USART1,"len = %d,data: %s",len,USART1_RX_BUF);
	}
	mymemset(USART1_RX_BUF,0,(u32)len);
	MYDMA_Enable(DMA2_Stream5,(u32)USART1_RX_BUF,USART_LEN);//开始一次DMA传输！
}
#endif

//=======================================================================	USART2
//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率 
void Init_USART2(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV@OVER8=0
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->AHB1ENR|=1<<0;   	//使能PORTA口时钟  
	RCC->APB1ENR|=1<<17;  	//使能串口2时钟 
	GPIO_Set(GPIOA,PIN2|PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
 	GPIO_AF_Set(GPIOA,2,7);	//PA2,AF7		TXD
	GPIO_AF_Set(GPIOA,3,7); //PA3,AF7		RXD  	   
	//波特率设置
 	USART2->BRR=mantissa; 	//波特率设置	 
	USART2->CR1&=~(1<<15); 	//设置OVER8=0 

	USART2->CR1|=1<<3;  	//串口发送使能 
	USART2->CR3|=1<<7;      //使能串口1的DMA发送
	
#if EN_USART2_RX		  	//如果使能了接收	
	USART2->CR1|=1<<2;  	//串口接收使能
	USART2->CR3|=1<<6;      //使能串口1的DMA接收  
	USART2->CR1|=1<<4;    	//使能空闲中断	    	
	MY_NVIC_Init(3,2,USART2_IRQn,2);//组2，最低优先级 
#endif
	USART2->CR1|=1<<13;  	//串口使能
}

#if EN_USART2_RX   //如果使能了接收  
  
void USART2_IRQHandler(void)
{
	u8 temp;
	u16 len;
	
	if(USART2->SR&(1<<4))//检测到线路空闲
	{
//软件序列清除IDLE标志位
		temp = USART2->SR;
		temp = USART2->DR;
		
		DMA1_Stream5->CR &=~(1<<0); //关闭DMA传输,准备重新配置
		DMA1->HIFCR|=1<<11;			//清除DMA1_Stream5传输完成标志
		DMA1->HIFCR|=1<<9;			//清除DMA1_Stream5传输错误标志
	
		len = USART_LEN - (uint16_t)(DMA1_Stream5->NDTR);
		myDMAprintf(USART2,"len = %d,data: %s",len,USART2_RX_BUF);
	}
	mymemset(USART2_RX_BUF,0,(u32)len);
	MYDMA_Enable(DMA1_Stream5,(u32)USART2_RX_BUF,USART_LEN);//开始一次DMA传输！
} 
#endif										 


//=======================================================================	USART3

void Init_USART3(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV@OVER8=0
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->AHB1ENR|=1<<1;   	//使能PORTB口时钟  
	RCC->APB1ENR|=1<<18;  	//使能串口3时钟 
	GPIO_Set(GPIOB,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PB10,PB11,复用功能,上拉输出
 	GPIO_AF_Set(GPIOB,10,7);	//PB10,AF7		TXD
	GPIO_AF_Set(GPIOB,11,7);	//PB11,AF7  	RXD   
	//波特率设置
 	USART3->BRR=mantissa; 	//波特率设置	 
	USART3->CR1&=~(1<<15); 	//设置OVER8=0 
	
	USART3->CR1|=1<<3;  	//串口发送使能 
	USART3->CR3|=1<<7;      //使能串口3的DMA发送
	
#if EN_USART3_RX		  	//如果使能了接收	
	USART3->CR1|=1<<2;  	//串口接收使能
	USART3->CR3|=1<<6;      //使能串口3的DMA接收  
	USART3->CR1|=1<<4;    	//使能空闲中断	    	
	MY_NVIC_Init(3,1,USART3_IRQn,2);//组2，最低优先级 
#endif
	USART3->CR1|=1<<13;  	//串口使能
}

#if EN_USART3_RX   //如果使能了接收	  
  
void USART3_IRQHandler(void)
{
	u8 temp;
	u16 len;
	
	if(USART3->SR&(1<<4))//检测到线路空闲
	{
//软件序列清除IDLE标志位
		temp = USART3->SR;
		temp = USART3->DR;
		
		DMA1_Stream1->CR &=~(1<<0); //关闭DMA传输,准备重新配置
		DMA1->LIFCR|=1<<11;			//清除DMA1_Stream1传输完成标志
		DMA1->LIFCR|=1<<9;			//清除DMA1_Stream1传输错误标志
	
		len = USART_LEN - (uint16_t)(DMA1_Stream1->NDTR);
		myDMAprintf(USART3,"len = %d,data: %s",len,USART3_RX_BUF);
	}
	mymemset(USART3_RX_BUF,0,(u32)len);
	MYDMA_Enable(DMA1_Stream1,(u32)USART3_RX_BUF,USART_LEN);//开始一次DMA传输！
} 
#endif


//=======================================================================	UART4

void Init_UART4(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV@OVER8=0
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->AHB1ENR|=1<<0;   	//使能PORTA口时钟  
	RCC->APB1ENR|=1<<19;  	//使能串口4时钟 
	GPIO_Set(GPIOA,PIN0|PIN1,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PB10,PB11,复用功能,上拉输出
 	GPIO_AF_Set(GPIOA,0,8);	//PA0,AF8	TXD
	GPIO_AF_Set(GPIOA,1,8);	//PA1,AF8  	RXD   
	//波特率设置
 	UART4->BRR=mantissa; 	//波特率设置	 
	UART4->CR1&=~(1<<15); 	//设置OVER8=0 
	
	UART4->CR1|=1<<3;  	//串口发送使能 
	UART4->CR3|=1<<7;      //使能串口3的DMA发送
	
#if EN_UART4_RX		  	//如果使能了接收	
	UART4->CR1|=1<<2;  	//串口接收使能
	UART4->CR3|=1<<6;   //使能串口4的DMA接收  
	UART4->CR1|=1<<4;   //使能空闲中断	    	
	MY_NVIC_Init(3,0,UART4_IRQn,2);//组2，最低优先级 
#endif
	UART4->CR1|=1<<13;  	//串口使能
}

#if EN_UART4_RX   //如果使能了接收	  
  
void UART4_IRQHandler(void)
{
	u8 temp;
	u16 len;
	
	if(UART4->SR&(1<<4))//检测到线路空闲
	{
//软件序列清除IDLE标志位
		temp = UART4->SR;
		temp = UART4->DR;
		
		DMA1_Stream2->CR &=~(1<<0); //关闭DMA传输,准备重新配置
		DMA1->LIFCR|=1<<21;			//清除DMA1_Stream2传输完成标志
		DMA1->LIFCR|=1<<19;			//清除DMA1_Stream2传输错误标志
	
		len = USART_LEN - (uint16_t)(DMA1_Stream2->NDTR);
		myDMAprintf(UART4,"len = %d,data: %s",len,UART4_RX_BUF);
	}
	mymemset(UART4_RX_BUF,0,(u32)len);
	MYDMA_Enable(DMA1_Stream2,(u32)UART4_RX_BUF,USART_LEN);//开始一次DMA传输！
} 
#endif


//=======================================================================	UART5

void Init_UART5(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV@OVER8=0
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->AHB1ENR|=1<<2;   	//使能PORTC口时钟 
	RCC->AHB1ENR|=1<<3;   	//使能PORTD口时钟	
	RCC->APB1ENR|=1<<20;  	//使能串口5时钟 
	GPIO_Set(GPIOC,PIN12,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PB10,PB11,复用功能,上拉输出
	GPIO_Set(GPIOD,PIN2,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PB10,PB11,复用功能,上拉输出
 	GPIO_AF_Set(GPIOC,12,8);	//PC12,AF8	TXD
	GPIO_AF_Set(GPIOD,2,8);		//PD2, AF8  RXD   
	//波特率设置
 	UART5->BRR=mantissa; 	//波特率设置	 
	UART5->CR1&=~(1<<15); 	//设置OVER8=0 
	
	UART5->CR1|=1<<3;  	//串口发送使能 
	UART5->CR3|=1<<7;      //使能串口3的DMA发送
	
#if EN_UART5_RX		  	//如果使能了接收	
	UART5->CR1|=1<<2;  	//串口接收使能
	UART5->CR3|=1<<6;   //使能串口4的DMA接收  
	UART5->CR1|=1<<4;   //使能空闲中断	    	
	MY_NVIC_Init(2,3,UART5_IRQn,2);//组2，最低优先级 
#endif
	UART5->CR1|=1<<13;  	//串口使能
}

#if EN_UART5_RX   //如果使能了接收	  
  
void UART5_IRQHandler(void)
{
	u8 temp;
	u16 len;
	
	if(UART5->SR&(1<<4))//检测到线路空闲
	{
//软件序列清除IDLE标志位
		temp = UART5->SR;
		temp = UART5->DR;
		
		DMA1_Stream0->CR &=~(1<<0); //关闭DMA传输,准备重新配置
		DMA1->LIFCR|=1<<5;			//清除DMA1_Stream0传输完成标志
		DMA1->LIFCR|=1<<3;			//清除DMA1_Stream0传输错误标志
	
		len = USART_LEN - (uint16_t)(DMA1_Stream0->NDTR);
		myDMAprintf(UART5,"len = %d,data: %s",len,UART5_RX_BUF);
	}
	mymemset(UART5_RX_BUF,0,(u32)len);
	MYDMA_Enable(DMA1_Stream0,(u32)UART5_RX_BUF,USART_LEN);//开始一次DMA传输！
} 
#endif

//=======================================================================	USART6

void Init_USART6(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV@OVER8=0
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->AHB1ENR|=1<<2;   	//使能PORTC口时钟  
	RCC->APB2ENR|=1<<5;  	//使能串口6时钟 
	GPIO_Set(GPIOC,PIN6|PIN7,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PB10,PB11,复用功能,上拉输出
 	GPIO_AF_Set(GPIOC,6,8);	//PC6,AF8	TXD
	GPIO_AF_Set(GPIOC,7,8);	//PC7,AF8  	RXD   
	//波特率设置
 	USART6->BRR=mantissa; 	//波特率设置	 
	USART6->CR1&=~(1<<15); 	//设置OVER8=0 
	
	USART6->CR1|=1<<3;  	//串口发送使能 
	USART6->CR3|=1<<7;      //使能串口6的DMA发送
	
#if EN_USART6_RX		  	//如果使能了接收	
	USART6->CR1|=1<<2;  	//串口接收使能
	USART6->CR3|=1<<6;      //使能串口3的DMA接收  
	USART6->CR1|=1<<4;    	//使能空闲中断	    	
	MY_NVIC_Init(2,2,USART6_IRQn,2);//组2，最低优先级 
#endif
	USART6->CR1|=1<<13;  	//串口使能
}

#if EN_USART6_RX   //如果使能了接收	  
  
void USART6_IRQHandler(void)
{
	u8 temp;
	u16 len;
	
	if(USART6->SR&(1<<4))//检测到线路空闲
	{
//软件序列清除IDLE标志位
		temp = USART6->SR;
		temp = USART6->DR;
		
		DMA2_Stream1->CR &=~(1<<0); //关闭DMA传输,准备重新配置
		DMA2->LIFCR|=1<<11;			//清除DMA2_Stream1传输完成标志
		DMA2->LIFCR|=1<<9;			//清除DMA2_Stream1传输错误标志
	
		len = USART_LEN - (uint16_t)(DMA2_Stream1->NDTR);
		myDMAprintf(USART6,"len = %d,data: %s",len,USART6_RX_BUF);
	}
	mymemset(USART6_RX_BUF,0,(u32)len);
	MYDMA_Enable(DMA2_Stream1,(u32)USART6_RX_BUF,USART_LEN);//开始一次DMA传输！
} 
#endif


/*------------------------------------------------
* 函数名：Status myDMAprintf(USART_TypeDef *USARTx, const char *format, ...)
* 功  能：仿 printf 函数 
* 参  数：*USARTx: 串口号
	      *pString: 打印内容
	      ... : 变量
* 返回值： 状态
------------------------------------------------*/
Status myDMAprintf(USART_TypeDef *USARTx, const char *format, ...)
{
	va_list args; 	
	u16 len;
	
	if(format == NULL)
		return 1;
	va_start(args, format);
	
	if(USARTx == USART1)
	{
		len = vsnprintf((char *)USART1_TX_BUF, USART_LEN, format, args);
		while(DMA2_Stream7_Flag != IDLE);
		MYDMA_Enable(DMA2_Stream7,(u32)USART1_TX_BUF,len);
		DMA2_Stream7_Flag = BUSY;
	}
	else if(USARTx == USART2)
	{
		len = vsnprintf((char *)USART2_TX_BUF, USART_LEN, format, args);
		while(DMA1_Stream6_Flag != IDLE);
		MYDMA_Enable(DMA1_Stream6,(u32)USART2_TX_BUF,len);
		DMA1_Stream6_Flag = BUSY;
	}	
	else if(USARTx == USART3)
	{
		len = vsnprintf((char *)USART3_TX_BUF, USART_LEN, format, args);
		while(DMA1_Stream3_Flag != IDLE);
		MYDMA_Enable(DMA1_Stream3,(u32)USART3_TX_BUF,len);
		DMA1_Stream3_Flag = BUSY;
	}	
	else if(USARTx == UART4)
	{
		len = vsnprintf((char *)UART4_TX_BUF, USART_LEN, format, args);
		while(DMA1_Stream4_Flag != IDLE);
		MYDMA_Enable(DMA1_Stream4,(u32)UART4_TX_BUF,len);
		DMA1_Stream4_Flag = BUSY;
	}	
	else if(USARTx == UART5)
	{
		len = vsnprintf((char *)UART5_TX_BUF, USART_LEN, format, args);
		while(DMA1_Stream7_Flag != IDLE);
		MYDMA_Enable(DMA1_Stream7,(u32)UART5_TX_BUF,len);
		DMA1_Stream7_Flag = BUSY;
	}	
	else if(USARTx == USART6)
	{
		len = vsnprintf((char *)USART6_TX_BUF, USART_LEN, format, args);
		while(DMA2_Stream6_Flag != IDLE);
		MYDMA_Enable(DMA2_Stream6,(u32)USART6_TX_BUF,len);
		DMA2_Stream6_Flag = BUSY;
	}
	else
		return 2;
	
	va_end(args);
	
	return 0;
}

/*------------------------------------------------
* 函数名：Status myDMAarray(USART_TypeDef *USARTx, u32 mar, u16 ndtr)
* 功  能：串口DMA方式发送数组
* 参  数：*USARTx: 串口号
	      mar: 存储器地址
		  ndtr: 数据传输量
* 返回值： 状态
------------------------------------------------*/
Status myDMAarray(USART_TypeDef *USARTx, u32 mar, u16 ndtr)
{
	if(ndtr == 0)
		return 1;
	
	if(USARTx == USART1)
	{	
		while(DMA2_Stream7_Flag != IDLE);
		MYDMA_Enable(DMA2_Stream7,mar,ndtr);
		DMA2_Stream7_Flag = BUSY;
	}
	else if(USARTx == USART2)
	{
		while(DMA1_Stream6_Flag != IDLE);
		MYDMA_Enable(DMA1_Stream6,mar,ndtr);
		DMA1_Stream6_Flag = BUSY;
	}	
	else if(USARTx == USART3)
	{
		while(DMA1_Stream3_Flag != IDLE);
		MYDMA_Enable(DMA1_Stream3,mar,ndtr);
		DMA1_Stream3_Flag = BUSY;
	}	
	else if(USARTx == UART4)
	{
		while(DMA1_Stream4_Flag != IDLE);
		MYDMA_Enable(DMA1_Stream4,mar,ndtr);
		DMA1_Stream4_Flag = BUSY;
	}	
	else if(USARTx == UART5)
	{
		while(DMA1_Stream7_Flag != IDLE);
		MYDMA_Enable(DMA1_Stream7,mar,ndtr);
		DMA1_Stream7_Flag = BUSY;
	}	
	else if(USARTx == USART6)
	{
		while(DMA2_Stream6_Flag != IDLE);
		MYDMA_Enable(DMA2_Stream6,mar,ndtr);
		DMA2_Stream6_Flag = BUSY;
	}
	else
		return 2;
	
	return 0;
}






