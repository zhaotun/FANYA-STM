/*
	�汾��		V1.0
	��д���ڣ�	2013��9��6��
	�༭�ˣ�	����
	��ģ�����ã���ģ����Ҫ�붨ʱ�����
	ʹ�÷�����	
	
	�޸����ڣ�
	�޸�˵����
*/
#include "tim.h"

/*
	�������� void Tim3_Init(void)
	������	 ��ʱ��3��ʼ��
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Tim2_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC->APB1ENR |=1<<0;	//ʹ��TIM4ʱ��
	TIM2->ARR = 60000;		
	TIM2->PSC = 35999;
	TIM2->DIER =1<<0;		//��������ж�
	TIM2->DIER =1<<6;		//�������ж�
	
	TIM2->CCMR1 |= 7<<12;	//CH2 PWM2ģʽ
	TIM2->CCMR1 |= 1<<11;	//CH2 Ԥװ��ʹ��
	TIM2->CCER  |= 1<<4;	//OC2���ʹ��
	TIM2->CR1 =0X0080;		//ARPEʹ��
	TIM2->CR1 |=0X01;		//ʹ�ܶ�ʱ��4
	//�����ж�
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/*
	�������� void Tim3_Init(void)
	������	 ��ʱ��3��ʼ��
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Tim3_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC->APB1ENR |=1<<1;	//ʹ��TIM4ʱ��
	TIM3->ARR = 60000;		
	TIM3->PSC = 35999;
	TIM3->DIER =1<<0;		//��������ж�
	TIM3->DIER =1<<6;		//�������ж�
	TIM3->CR1 |=0X01;		//ʹ�ܶ�ʱ��4
	//�����ж�
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
	�������� void Tim3_Init(void)
	������	 ��ʱ��3��ʼ��
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Tim4_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC->APB1ENR |=1<<2;	//ʹ��TIM4ʱ��
	TIM4->ARR = 60000;		
	TIM4->PSC = 35999;
	TIM4->DIER =1<<0;		//��������ж�
	TIM4->DIER =1<<6;		//�������ж�
	TIM4->CR1 |=0X01;		//ʹ�ܶ�ʱ��4
	//�����ж�
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
	�������� void Tim3_Init(void)
	������	 ��ʱ��3��ʼ��
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Tim5_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC->APB1ENR |=1<<3;	//ʹ��TIM4ʱ��
	TIM5->ARR = 60000;		
	TIM5->PSC = 35999;
	TIM5->DIER =1<<0;		//��������ж�
	TIM5->DIER =1<<6;		//�������ж�
	TIM5->CR1 |=0X01;		//ʹ�ܶ�ʱ��4
	//�����ж�
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


