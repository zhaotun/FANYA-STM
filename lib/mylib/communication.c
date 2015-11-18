/*
	版本：		V1.0
	编写日期：	2013年9月6号
	编辑人：	刘智
	本模块作用：该模块主要用于通讯，包括对处理器各种参数的读取，写入，设置
	使用方法：	
	
	修改日期：
	修改说明：
*/
#include "communication.h"
#include <stdio.h>

/*
	函数名：void Read_DATA(u8 Command)
	描述：	将Command的命令数值返回到printf打印输出
	输入：  Command命令
	输出：	由printf输出
	返回：	无
*/
void Read_Data(u8 Command)
{
	switch(Command)
	{
		case 10:
		{//读取当前始终值
			printf("%u\r\n",RTC_GetCounter());		//打印秒
			break;
		}
		case 201:
		{
			printf("控制寄存器1：TIM4_CR1=0X%x\r\n",TIM4->CR1);
			printf("控制寄存器2：TIM4_CR2=0X%x\r\n",TIM4->CR2);
			printf("计数器：     TIM4_CNT=0X%u\r\n",TIM4->CNT);
			printf("预分频器：   TIM4_PSC=0X%x\r\n",TIM4->PSC);
			printf("重载寄存器： TIM4_ARR=0X%x\r\n",TIM4->ARR);
			printf("状态寄存器： TIM4_SR =0X%x\r\n",TIM4->SR);
			printf("事件产生器： TIM4_EGR=0X%x\r\n",TIM4->EGR);
			printf("从模式存器： TIM4_SMCR=0X%x\r\n",TIM4->SMCR);
			break;
		}
		case 202:
		{
			printf("计数器：TIM4_CNT=%u\r\n",TIM4->CNT);
			break;
		}
		default:
		{
			printf("无此命令，请核对！\r\n");	
			break;
		}
	}
}


