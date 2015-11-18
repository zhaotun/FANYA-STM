#include "tiaoshi.h"

void tiaoshi(void)
{
	if(GPIO_ReadInputDataBit(INPUT1)==0) GPIO_ResetBits(LED1);
	else GPIO_SetBits(LED1);
	if(GPIO_ReadInputDataBit(INPUT13)==0) GPIO_ResetBits(LED2);
	else GPIO_SetBits(LED2);
	if(GPIO_ReadInputDataBit(INPUT14)==0) GPIO_ResetBits(LED3);
	else GPIO_SetBits(LED3);
}


//与触摸屏通讯
/*
	函数名： void Send_HMI_Cmd(void)
	描述：	 发送数据
	输入：	 无
	返回：	 无
*/
void HMI_Send_Cmd(u8 *p,u8 len)
{
	
}

u8 HMI_Receive_Buffer[30];
void HMI_Receive_Cmd(void)
{
	
}


