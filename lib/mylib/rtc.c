/*
	版本：		V1.0
	编写日期：	2013年9月7号
	编辑人：	刘智
	本模块作用：该模块为配置RTC实时时钟，包括将32位秒转换为当前日期函数
	使用方法：	 
	
	修改日期：
	修改说明：
*/
#include "rtc.h"
#include "stm32f10x.h"
#include <stdio.h>

/*
	函数名： RTC_Init(void)
	描述：	 RTC实时时钟初始化
	输入：	 无
	输出：	 无
	返回;	 无
*/
void RTC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/*PWR时钟（电源控制）与BKP时钟（RTC后备寄存器）使能*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	/*使能RTC和后备寄存器访问*/
	PWR_BackupAccessCmd(ENABLE);
	/*从指定的后备寄存器（BKP_DR1）中读出数据*/
	if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		/* 将外设BKP的全部寄存器重设为缺省值 */
		BKP_DeInit(); 
		/* 启用 LSE（外部低速晶振）*/
		RCC_LSEConfig(RCC_LSE_ON); 
		/*等待外部晶振震荡 需要等待比较长的时间*/
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
		/*使用外部晶振32.768K作为RTC时钟*/
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);  
		//允许RTC
		RCC_RTCCLKCmd(ENABLE);
		//等待RTC寄存器同步 原因下文说明
		RTC_WaitForSynchro();
		//等待最近一次对RTC寄存器的写操作完成
		RTC_WaitForLastTask();
		//允许RTC的秒中断(还有闹钟中断和溢出中断可设置)
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		//又是等待上一次对RTC寄存器的写操作完成
		RTC_WaitForLastTask();
		//32768晶振预分频值是32767,不过一般来说晶振都不那么准
		RTC_SetPrescaler(32776);  //如果需要校准晶振,可修改此分频值
		RTC_WaitForLastTask();
		//写入RTC后备寄存器1 0xa5a5
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);  
		printf("RTC NO POWER");
// 		RTC_Blank=1; /*这个标志代表RTC是没有预设的(或者说是没有上纽扣电池) 用串口呀啥的输出来。*/
		//等待最近一次对RTC寄存器的写操作完成
		RTC_WaitForLastTask();
		/* Sets Counter value to 0x0 */
		RTC_SetCounter(0xFFFFFFFA);	
	}
	//如果RTC已经设置
	else
	{
		//等待RTC与APB同步
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		//使能秒中断 
		RTC_ITConfig(RTC_IT_SEC, ENABLE);  //这句可以放到前面吗？
		RTC_WaitForLastTask(); //又等....
    }
	//清除标志
	RCC_ClearFlag(); 
	//使能RTC中断
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

