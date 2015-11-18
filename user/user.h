
#ifndef __USER_H
#define __USER_H

#include "stm32f10x.h"

struct machine_structure	//机器状态结构
{
	u16 Address;	//机器地址
	u8 Type[2];		//机器类型
	u8 State;		//机器运行状态
	u32 Timer;		//运行时间
	u32 Count;		//机器在线计数器
};

struct input_structure
{
	u32 Timer;		//两次开模的时间间隔，从上一次开模开始计时，到下一次开模清零重新计时
	u16 Count;		//一次没有模具条码开模计数
};

void User_Timer1s(void);			//用户定时器，1秒定时器
void User_Timer1sClear(void);		//用户1秒定时器清零
u32 User_GetTimer1sValue(void);		//返回定时器的值
void User_SetTimer1sValue(u32 buf);	//设置定时器的值

void LED_main(void);	//闪烁灯主程序，系统主程序调用
void LED_ON(void);		//开闪烁灯
void LED_OFF(void);		//关闪烁灯


void Communictae_Check(void);
void Communicate_timer(void);
void Communicate_SetOn(void);
void Communicate_SetOff(void);
void Clera_Timer1(void);
void Communicate_SaveData(u8 *p,u8 len);
void Communicate_Star(void);
void Communicate_StarON(void);

void OneSecond_Handler(void); //1S定时中断函数


#endif

