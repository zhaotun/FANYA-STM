/*
	�汾��		V1.0
	��д���ڣ�	2013��9��6��
	�༭�ˣ�	����
	��ģ�����ã���ģ����Ҫ����ͨѶ�������Դ��������ֲ����Ķ�ȡ��д�룬����
	ʹ�÷�����	
	
	�޸����ڣ�
	�޸�˵����
*/
#include "communication.h"
#include <stdio.h>

/*
	��������void Read_DATA(u8 Command)
	������	��Command��������ֵ���ص�printf��ӡ���
	���룺  Command����
	�����	��printf���
	���أ�	��
*/
void Read_Data(u8 Command)
{
	switch(Command)
	{
		case 10:
		{//��ȡ��ǰʼ��ֵ
			printf("%u\r\n",RTC_GetCounter());		//��ӡ��
			break;
		}
		case 201:
		{
			printf("���ƼĴ���1��TIM4_CR1=0X%x\r\n",TIM4->CR1);
			printf("���ƼĴ���2��TIM4_CR2=0X%x\r\n",TIM4->CR2);
			printf("��������     TIM4_CNT=0X%u\r\n",TIM4->CNT);
			printf("Ԥ��Ƶ����   TIM4_PSC=0X%x\r\n",TIM4->PSC);
			printf("���ؼĴ����� TIM4_ARR=0X%x\r\n",TIM4->ARR);
			printf("״̬�Ĵ����� TIM4_SR =0X%x\r\n",TIM4->SR);
			printf("�¼��������� TIM4_EGR=0X%x\r\n",TIM4->EGR);
			printf("��ģʽ������ TIM4_SMCR=0X%x\r\n",TIM4->SMCR);
			break;
		}
		case 202:
		{
			printf("��������TIM4_CNT=%u\r\n",TIM4->CNT);
			break;
		}
		default:
		{
			printf("�޴������˶ԣ�\r\n");	
			break;
		}
	}
}


