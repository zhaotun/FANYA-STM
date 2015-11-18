/*
	版本：		V1.0
	编写日期：	2013年9月7号
	编辑人：	刘智
	本模块作用：该模块主要用于初始化各串口，包括各串口数据的收发函数，需要发送的数据放到发送缓冲区即可
	使用方法：	 
	
	修改日期：
	修改说明：
*/

#include "stm32f10x.h"
#include "usart.h"
#include <stdio.h>

struct receive_bf 
{
	u8	RxCount;			//接收计数器
	u8	RxTimer;			//接收定时器
	u8	RxFlag;				//接收标志位，b0(正在接收)
	u8	RxBuffer[RXBF_LEN];	//接收缓冲区
};

/*
//===================================USART1=======================================
/*
	函数名： void USART1_Init(void)
	描述：	 串口1函数的初始化，包括初始化串口中断、串口各种参数
	输入：	 无
	输出：	 无
	返回：	 无
*/
/*
void USART1_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//中断向量配置
	//使能USART1全局中断
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	//USART1端口配置
	//配置USART1 TX(PA.09)为复用功能推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//配置USART1 RX(PA.10)为浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//配置USART1串行口
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	//使能发送中断，该中断产生时的USART1发送数据寄存器为空
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	//使能接收中断，该中断产生时，USART1接收数据寄存器不为空
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//使能USART1
	USART_Cmd(USART1, ENABLE);
}
*/
/*
	函数名： void USART1_Receive(void)
	描述：	 串口接收一帧数据，USART1中断常调用，接收一帧完整的数据
			 接收到的数据放在接收缓冲区USART1_RxBuffer[230]中，长度
			 放到USART1_RxCount中
*/
/*
#define RX1_LEN 100

u8	USART1_RxBuffer[RX1_LEN];
u8	USART1_RxCount=0;
u8	USART1_RxTimer=0;
u8	USART1_RxFlag=0;
u16 USART1_RxCRC=0;
void USART1_Receive(void)
{
 	USART1_RxTimer=0;
	USART1_RxBuffer[USART1_RxCount++] = USART_ReceiveData(USART1);
	USART_SendData(USART1, USART_ReceiveData(USART1));
	if(USART1_RxCount>RX1_LEN) USART1_RxCount=0;
 	USART1_RxFlag |= 0x01;
}
*/
//===================================USART2=======================================

//===================================USART3=======================================
/*
	函数名： void USART3_Init(void)
	描述：	 串口1函数的初始化，包括初始化串口中断、串口各种参数
	输入：	 无
	输出：	 无
	返回：	 无
*/
void USART3_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	//使能GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//使能AFIO时钟
	//GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);	//端口重新映射
	GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
	//USART3端口配置
	//配置USART3 TX(PD8)为复用功能推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//配置USART3 RX(PD9)为浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//配置USART1串行口
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	//使能发送中断，该中断产生时的USART1发送数据寄存器为空
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	//使能接收中断，该中断产生时，USART1接收数据寄存器不为空
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	//使能USART1
	USART_Cmd(USART3, ENABLE);
	//使能USART3中断
	NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
	函数名： void USART1_Receive(void)
	描述：	 串口接收一帧数据，USART1中断常调用，接收一帧完整的数据
			 接收到的数据放在接收缓冲区USART1_RxBuffer[230]中，长度
			 放到USART1_RxCount中
*/
#define RX3_LEN 100

struct receive_bf Serial3;		//串口3接收结构体
void USART3_Receive(u8 buffer)
{
 	if(Serial3.RxFlag & RECEIVING)
	{//正在接收
		if(buffer == RXLF)
		{	
			if(Serial3.RxBuffer[Serial3.RxCount-1] == RXCR)
			{//接收完毕
				Serial3.RxFlag |= RECEIVE_SUCCESS | RECEIVE_OVER;		//置接收完毕标志位
				USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);		//关闭中断
			}
			else Serial3.RxBuffer[Serial3.RxCount++] = buffer;
		}
		else Serial3.RxBuffer[Serial3.RxCount++] = buffer;
		if(Serial3.RxCount >= RXBF_LEN-1)
		{
			Serial3.RxFlag |= RECEIVE_WRONG | RECEIVE_OVER;	//置接收出错标志位
			USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);		//关闭中断
		}
	}
	if(buffer == STX) Serial3.RxFlag |=	RECEIVING;	//开始接收
}
/*
	函数名： void USART3_DW(void)
	描述：	 串口3接收完一帧数据处理
*/
void USART3_DW(void)
{
	
	if(Serial3.RxFlag & RECEIVE_OVER)
	{
		u8 i=0;
		if(Serial3.RxFlag & RECEIVE_SUCCESS)	
		{//接收成功
			switch(Serial3.RxBuffer[0])
			{
				case 'R':;
				case 'r':
				{
					i=(Serial3.RxBuffer[1]-0x30)*100 + (Serial3.RxBuffer[2]-0x30)*10 + 
							Serial3.RxBuffer[3]-0x30;
					Read_Data(i);
					break;
				}
				case 'W':;
				case 'w':
				{
					break;
				}
				case 'S':;
				case 's':
				{
					break;
				}
				default: ;
			}
		}
		else if(Serial3.RxFlag & RECEIVE_WRONG)
		{//接收错误
			printf("接收错误完毕！\r\n");
		}
		Serial3.RxFlag =0;		//清接收标志位
		Serial3.RxCount=0;		//清计数器
		for(i=0;i<RXBF_LEN;i++) Serial3.RxBuffer[i]=0;
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);		//打开中断
	}
}

//===================================USART4=======================================

//===================================USART5=======================================

