/*
	版本：		V1.0
	编写日期：	2013年9月6号
	编辑人：	刘智
	本模块作用：该模块用于printf输出打印，带有缓冲区，缓冲区使用循环列表方式
	使用方法：	1、使用printf输出功能是，需要先将Target Options→Target设置的Use MicroLIB选项点勾
				2、初始化输出对象（如串口）的各项参数，并开启发送中断功能
				3、在打印的输出对象（如串口输出）的发送中断（如串口的发送中断）调用Printf_Send()函数
				4、修改宏定义“OPEN_PRINTF_INTERRUPT(打印中断)”、"PRINTF_ONE_DATA(打印一个字节)“、"CLOSE_PRINTF_INTERRUPT(关闭打印中断)”
				5、外部可直接使用标准库函数printf(),亦可调用Printf_StoreBuffeeData()函数直接打印一个字节数据  
	
	修改日期：
	修改说明：
*/
#include "stm32f10x.h"
#include "printf.h"
#include <stdio.h>

#define Open_Printf
#define	PRINTF_LEN				200			//打印缓冲区深度，自由定义，越大越好
#define OPEN_PRINTF_INTERRUPT   USART_ITConfig(UART4, USART_IT_TXE, ENABLE);		//启动打印中断
#define PRINTF_ONE_DATA			USART_SendData(UART4, Printf_TxBuffer[Printf_TxSendPoint++]);	//打印一个字节数据
#define CLOSE_PRINTF_INTERRUPT	USART_ITConfig(UART4, USART_IT_TXE, DISABLE);		//关闭打印中断


#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)		//与标准库函数Printf相关

unsigned char Printf_TxBuffer[PRINTF_LEN];	//发送缓冲区
unsigned char Printf_TxSendPoint=0;			//串口1发送指针
unsigned char Printf_TxStorePoint=0;		//串口1存储指针
unsigned char Printf_TxBufferLend=0;		//串口1缓存数据长度

/*
	函数原名称： int fputc(int ch, FILE *f)
	说明：	该函数由Printf输出函数调用，将需要打印的数据逐个存储到打印缓冲区中
	输入：	int ch;
	输出：	int ch
	返回：	int ch
*/
PUTCHAR_PROTOTYPE
{
	/* ARMJISHU 零等待的Printf串口打印函数将数据
            写入Buffer后立即返回(Buffer未满时) */
#ifdef Open_Printf
	Printf_StoreBuffeeData((unsigned char) ch);
#endif
	return ch;
}

/*
	函数名： void Printf_StoreBuffeeData(unsigned char ch)
	描述：	 将需要打印的数据存入打印缓冲区，按照FIFO原则打印出来每次调用后，需要启
			 动发送中断，做程序移植时，需修改最后一句语句，即启动发送中断即可
	输出：	 无
	输入：	 (unsigned char ch)需要被打印的数据
	返回：	 无
*/
void Printf_StoreBuffeeData(unsigned char ch)
{
	while(Printf_TxBufferLend == PRINTF_LEN-1);		//当缓冲区填满时，需要等待
	Printf_TxBuffer[Printf_TxStorePoint++]=ch;
	if(Printf_TxStorePoint == PRINTF_LEN) Printf_TxStorePoint=0;	//存储指针指向表顶时，修正指针指向表低
	Printf_TxBufferLend++;
	OPEN_PRINTF_INTERRUPT			//词句根据实际情况作修改，用于启动发送打印缓冲区的发送中断
}

/*
	函数名： void Printf_Send(void)
	描述：	 发送打印缓冲区的一个字节数据，发送中断常调用，每调用一次发送一个字节数据
			 属于底层驱动相关，因此程序做移植时，主要修改此函数
*/
void Printf_Send(void)
{
	PRINTF_ONE_DATA			//发送一个字节数据
	if(Printf_TxSendPoint == PRINTF_LEN) Printf_TxSendPoint=0;				//若指针指向表顶，修改指针
	Printf_TxBufferLend--;
	if(Printf_TxSendPoint == Printf_TxStorePoint) 				//判断是否发送完毕，若发送完毕则关闭发送中断
	{
		CLOSE_PRINTF_INTERRUPT		//发送完毕，关闭发送中断	
	}
}
