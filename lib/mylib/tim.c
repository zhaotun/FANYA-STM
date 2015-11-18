/*
	版本：		V1.0
	编写日期：	2013年9月6号
	编辑人：	刘智
	本模块作用：本模块主要与定时器相关
	使用方法：	
	
	修改日期：
	修改说明：
*/
#include "tim.h"

/*
	函数名： void Tim3_Init(void)
	描述：	 定时器3初始化
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Tim2_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC->APB1ENR |=1<<0;	//使能TIM4时钟
	TIM2->ARR = 60000;		
	TIM2->PSC = 35999;
	TIM2->DIER =1<<0;		//允许更新中断
	TIM2->DIER =1<<6;		//允许触发中断
	
	TIM2->CCMR1 |= 7<<12;	//CH2 PWM2模式
	TIM2->CCMR1 |= 1<<11;	//CH2 预装载使能
	TIM2->CCER  |= 1<<4;	//OC2输出使能
	TIM2->CR1 =0X0080;		//ARPE使能
	TIM2->CR1 |=0X01;		//使能定时器4
	//配置中断
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/*
	函数名： void Tim3_Init(void)
	描述：	 定时器3初始化
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Tim3_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC->APB1ENR |=1<<1;	//使能TIM4时钟
	TIM3->ARR = 60000;		
	TIM3->PSC = 35999;
	TIM3->DIER =1<<0;		//允许更新中断
	TIM3->DIER =1<<6;		//允许触发中断
	TIM3->CR1 |=0X01;		//使能定时器4
	//配置中断
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
	函数名： void Tim3_Init(void)
	描述：	 定时器3初始化
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Tim4_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC->APB1ENR |=1<<2;	//使能TIM4时钟
	TIM4->ARR = 60000;		
	TIM4->PSC = 35999;
	TIM4->DIER =1<<0;		//允许更新中断
	TIM4->DIER =1<<6;		//允许触发中断
	TIM4->CR1 |=0X01;		//使能定时器4
	//配置中断
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
	函数名： void Tim3_Init(void)
	描述：	 定时器3初始化
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Tim5_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC->APB1ENR |=1<<3;	//使能TIM4时钟
	TIM5->ARR = 60000;		
	TIM5->PSC = 35999;
	TIM5->DIER =1<<0;		//允许更新中断
	TIM5->DIER =1<<6;		//允许触发中断
	TIM5->CR1 |=0X01;		//使能定时器4
	//配置中断
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


