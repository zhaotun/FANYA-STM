/*
用户程序区
用户程序运行主程序
*/
#include "user.h"

struct machine_structure Machine_Structure;		//机器结构体
struct input_structure Input_Structure ={120,0};			//输入结构体

struct led_struct{
	u16 timer;
	u8 flag;	//b1=1,灯闪烁;b1=0停止闪烁,b2=1，每隔2秒闪烁一下
	u8 Blue_LED_on;
	} LED_Struct;

/*
	函数名： void User_Timer1S(void)
	描述：	 用户1秒定时器，秒定时常调用
*/
void User_Timer1s(void)
{
	Input_Structure.Timer++;
	LED_Struct.timer++;
// 	if((LED_Struct.timer % 2) == 0)
// 	{
// 		if(LED_Struct.flag & b2)
// 		{
// 			LED_Struct.flag &= ~b2;
// 		}
// 		else 
// 		{
// 			LED_Struct.flag |= b2;
// 		}
// 	}
}

/*
	函数名： void User_Timer1SClear(void)
	描述：	 用户秒定时器时钟清零
*/
void User_Timer1sClear(void)
{
	Input_Structure.Timer=0;
}

/*
	函数名： u32 User_GetTimer1sValue(void)
	描述：	 返回两次开模的时间间隔，单位，秒
*/
u32 User_GetTimer1sValue(void)
{
	return Input_Structure.Timer;
}

void User_SetTimer1sValue(u32 buf)
{
	Input_Structure.Timer=buf;
}

//生产指示灯控制
void LED_main(void)
{
	if(LED_Struct.Blue_LED_on & 1)
	{
	//开灯
		if(LED_Struct.flag & b1)
		{
			GPIO_ResetBits(OUTPUT4);
		}
		else if(LED_Struct.flag &b2)
		{
			GPIO_SetBits(OUTPUT4);
		}
		if(User_GetTimer1sValue() > 300 && LED_Struct.timer > 300)
		{//若五分钟内没有开模，则停止闪烁
			LED_Struct.timer=0;
			LED_Struct.Blue_LED_on = 0;
			GPIO_SetBits(OUTPUT4);
		}
	}
}

//命令指示灯闪烁
void LED_ON(void)
{
	LED_Struct.flag &= ~b2;
	LED_Struct.flag |= b1;
	LED_Struct.Blue_LED_on =1;
}

//命令指示灯停止闪烁
void LED_OFF(void)
{
	LED_Struct.flag &= ~b1;
	LED_Struct.flag |= b2;
}

/*
	行数名： void Communictae_Check(void)
	描述：	 该函数用于检测以太网通讯是否通畅，若不通畅则重启以太网模块，注意，为了等待以太网
			 模块充分启动，该模块需要启动3S以后才启动
			 算法描述：当检测到以太网端口有数据发送后，开始计时，若3秒后仍然没有收到返回数据
			 则重新发送数据，若连续3次没有收到上位机返回的数据，则认为以太网模块出了问题，重
			 新启动以太网模块，若连续启动3次，则认为当前网络故障，需要人工干预
*/
struct Communicate_struct
{
	u8 flag;	//b1(启动数据发送),b2(当前通讯状态,0正常，1异常),b3(模块启动了5S)
	u16 timer;
	u32 timer1;
	u8	count1;	//重新发送数据次数
	u8	count2;	//重启次数
	u8  TxBuffer[255];		//用于保存需要发送的数据
	u8  TxLen;	//用于保存当前发送数据的长度
} comm_str;

u32 Ethernet_Time_Count;
extern u8 USART3_TxBuffer[255];
extern u8 USART3_TxLen;
u8 Ethernet_Reset_Count;

void Communictae_Check(void)
{
	if(comm_str.flag & b3)
	{//模块已经启动
		if(comm_str.timer >=12000)
		{//接收超时
			comm_str.timer=0;
			comm_str.count1++;
			if((comm_str.count1>=3) && (Ethernet_Time_Count>3600))		//1小时内没有收到回复大于等于3次，网络重启
			{//重新启动模块
				//重启模块
//				printf("以太网模块重启了\r\n");
				comm_str.flag=0;
				Ethernet_Time_Count=0;
				comm_str.count1=0;
				comm_str.timer=0;
				comm_str.TxLen=0;
				Ethernet_Reset_Count++;	//重启计数统计
				GPIO_SetBits(OUTPUT8);		//重启模块
//				GPIO_ResetBits(RESET_PIN);		//重启模块

			}
			else if(comm_str.count1>=3)
			{
//				printf("连续发送了6次\r\n");
//				printf("timer1=%u\r\n",Ethernet_Time_Count);
				Communicate_SetOff();
			}
			else
			{//没有收到返回，发送上一次发送的数据
				u8 i;
				for(i=0;i<comm_str.TxLen;i++)
				{
					USART3_TxBuffer[i]=comm_str.TxBuffer[i];
				}
				USART3_TxLen=comm_str.TxLen;
				USART3_SendDW();
//				printf("timer1=%u\r\n",Ethernet_Time_Count);
			}
		}
	}
	else if(comm_str.timer >=3000)
	{//以太网启动
		Communicate_SetOff();
		comm_str.flag |= b3;		//启动模块
		if(comm_str.TxLen>0)
		{
			u8 i;
			for(i=0;i<comm_str.TxLen;i++)
			{
				USART3_TxBuffer[i]=comm_str.TxBuffer[i];
			}
			USART3_TxLen=comm_str.TxLen;
			USART3_SendDW();
		}
	}
	else if(comm_str.timer >=500)
	{//重启以太网模块
		GPIO_ResetBits(OUTPUT8);
//		GPIO_SetBits(RESET_PIN);

	}
}

/*
	函数名： void Communicate_timer(void)
	描述：	 通讯检测定时器，该函数由1ms定时器调用
			 当以太网模块重新启动时，需要等待3S
*/
void Communicate_timer(void)
{
	if((comm_str.flag & b3) == 0)
	{//模块以及启动
		comm_str.timer++;
	}
	else if(comm_str.flag & b1)
	{//有数据发送，启动计时
		comm_str.timer++;
	}
}

/*
	函数名： void Communicate_SetIn(void)
	描述：	 设置comm_str.flag的b1位，启动计时,发送数据时调用
*/
void Communicate_SetOn(void)
{
	comm_str.flag |= b1;
	comm_str.timer=0;
}
/*
	清除未收到数据计时
*/
void Clera_Timer1(void)
{
	Ethernet_Time_Count=0;
}

/*
	函数名： void Communicate_SetIn(void)
	描述：	 清comm_str.flag的b1位，停止计时，同时清掉当前的计数器
*/
void Communicate_SetOff(void)
{
	comm_str.flag &= ~(b1 | b2);
	comm_str.timer=0;
	comm_str.count1=0;
	comm_str.count2=0;
}

/*
	函数名： void Communicate_SaveData(void)
	描述：   保存当前需要发送的数据
	输入：	 u8 *p(需要发送数据的首地址),u8 len(需要发送数据的长度)
*/
void Communicate_SaveData(u8 *p,u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		comm_str.TxBuffer[i]=*p++;
	}
	comm_str.TxLen=len;
}

/*
	函数名： void Communicate_Star(void)
	描述：	 程序气动时调用，启动时先延时3S，然后再继续，以便让以太网模块启动
*/
void Communicate_Star(void)
{
	GPIO_ResetBits(OUTPUT8);
//	GPIO_SetBits(RESET_PIN);
	while(comm_str.timer<1500);
	comm_str.timer=0;
}

void Communicate_StarON(void)
{
	while(comm_str.timer<1000);
	comm_str.timer=0;
	comm_str.flag =0;
	comm_str.flag |= b3;		//启动模块
	comm_str.TxLen=0;
}




