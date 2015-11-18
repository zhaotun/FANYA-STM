/*
	声明：此库文件与所有串口相关定义有关
	此串口为员工高频读卡器接口
*/
#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"

#define EOR		0x0D		//回车,结束第一个字符
#define EOX		0X0A		//换行,结束第二个字符

#define UART4_RxBufferSizeMax  32		//定义串口3最大接收缓冲区


void UART4_Configuration(void);		//串口4初始化
void UART4_ReceiveDA(u8 buffer);	//串口4接收处理
void UART4_ReceiveDW(void);			//接收到的数据处理
u8 YuanGong_Online(void);			//返回当前是否有员工在线，若有则返回1，否则返回0

#endif
