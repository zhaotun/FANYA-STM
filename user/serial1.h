/*
	声明：此库文件与所有串口相关定义有关
	此串口为磨具超高频读卡器接口
*/
#ifndef __SERIAL1_H
#define __SERIAL1_H

#include "stm32f10x.h"


//USART1相关宏定义
#define STX		0x3A		//":"定义接收字符开始标志位
#define EOR		0x0D		//回车,结束第一个字符
#define EOX		0X0A		//换行,结束第二个字符
#define ID		"AB"		//设备编号

#define NOT_UHRFID  15*60*20	//400	//18000	//设置多少次没有读到超高频标签数据认为模具被移走
#define NEW_UHRFID  10		//统计读取新标签数据次数
#define NOT_KAIMO	15*60		//20		//900

struct tiaoma
{
	u8 Len;			//条码长度
	u8 Code[30];	//条码
	u32 Count;		//该条码计数器
	u32 Timer;		//该条码在线时间
	u8 Flag;		//该条码标志位
};

//USART1相关函数
void USART1_Configuration(void);	//串口1时钟配置
void USART1_SendDW(void);			//串口1发送启动函数
void USART1_SendDA(void);			//串口1发送一个字节数据
void USART1_ReceiveDA(void);		//串口1接收数据处理
void USART1_ReceiveDW(void);		//串口1接收完数据处理
void USART1_SendError(void);		//串口1发送错误信息
void USART1_ReadURFID(void);		//度取超高频读卡器数据

void Nul_MuJuCount(void);			//模具为空时开模计数
void URFID_Timer1S(void);			//超高频读卡器秒时钟,秒中断常调用

#endif

