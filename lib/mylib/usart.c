/*
	�汾��		V1.0
	��д���ڣ�	2013��9��7��
	�༭�ˣ�	����
	��ģ�����ã���ģ����Ҫ���ڳ�ʼ�������ڣ��������������ݵ��շ���������Ҫ���͵����ݷŵ����ͻ���������
	ʹ�÷�����	 
	
	�޸����ڣ�
	�޸�˵����
*/

#include "stm32f10x.h"
#include "usart.h"
#include <stdio.h>

struct receive_bf 
{
	u8	RxCount;			//���ռ�����
	u8	RxTimer;			//���ն�ʱ��
	u8	RxFlag;				//���ձ�־λ��b0(���ڽ���)
	u8	RxBuffer[RXBF_LEN];	//���ջ�����
};

/*
//===================================USART1=======================================
/*
	�������� void USART1_Init(void)
	������	 ����1�����ĳ�ʼ����������ʼ�������жϡ����ڸ��ֲ���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
/*
void USART1_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//�ж���������
	//ʹ��USART1ȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//ʹ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	//USART1�˿�����
	//����USART1 TX(PA.09)Ϊ���ù����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//����USART1 RX(PA.10)Ϊ��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//����USART1���п�
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	//ʹ�ܷ����жϣ����жϲ���ʱ��USART1�������ݼĴ���Ϊ��
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	//ʹ�ܽ����жϣ����жϲ���ʱ��USART1�������ݼĴ�����Ϊ��
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//ʹ��USART1
	USART_Cmd(USART1, ENABLE);
}
*/
/*
	�������� void USART1_Receive(void)
	������	 ���ڽ���һ֡���ݣ�USART1�жϳ����ã�����һ֡����������
			 ���յ������ݷ��ڽ��ջ�����USART1_RxBuffer[230]�У�����
			 �ŵ�USART1_RxCount��
*/
/*
#define RX1_LEN 100

u8	USART1_RxBuffer[RX1_LEN];
u8	USART1_RxCount=0;
u8	USART1_RxTimer=0;
u8	USART1_RxFlag=0;
u16 USART1_RxCRC=0;
void USART1_Receive(void)
{
 	USART1_RxTimer=0;
	USART1_RxBuffer[USART1_RxCount++] = USART_ReceiveData(USART1);
	USART_SendData(USART1, USART_ReceiveData(USART1));
	if(USART1_RxCount>RX1_LEN) USART1_RxCount=0;
 	USART1_RxFlag |= 0x01;
}
*/
//===================================USART2=======================================

//===================================USART3=======================================
/*
	�������� void USART3_Init(void)
	������	 ����1�����ĳ�ʼ����������ʼ�������жϡ����ڸ��ֲ���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void USART3_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	//ʹ��GPIODʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART3ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//ʹ��AFIOʱ��
	//GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);	//�˿�����ӳ��
	GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
	//USART3�˿�����
	//����USART3 TX(PD8)Ϊ���ù����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//����USART3 RX(PD9)Ϊ��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//����USART1���п�
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	//ʹ�ܷ����жϣ����жϲ���ʱ��USART1�������ݼĴ���Ϊ��
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	//ʹ�ܽ����жϣ����жϲ���ʱ��USART1�������ݼĴ�����Ϊ��
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	//ʹ��USART1
	USART_Cmd(USART3, ENABLE);
	//ʹ��USART3�ж�
	NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
	�������� void USART1_Receive(void)
	������	 ���ڽ���һ֡���ݣ�USART1�жϳ����ã�����һ֡����������
			 ���յ������ݷ��ڽ��ջ�����USART1_RxBuffer[230]�У�����
			 �ŵ�USART1_RxCount��
*/
#define RX3_LEN 100

struct receive_bf Serial3;		//����3���սṹ��
void USART3_Receive(u8 buffer)
{
 	if(Serial3.RxFlag & RECEIVING)
	{//���ڽ���
		if(buffer == RXLF)
		{	
			if(Serial3.RxBuffer[Serial3.RxCount-1] == RXCR)
			{//�������
				Serial3.RxFlag |= RECEIVE_SUCCESS | RECEIVE_OVER;		//�ý�����ϱ�־λ
				USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);		//�ر��ж�
			}
			else Serial3.RxBuffer[Serial3.RxCount++] = buffer;
		}
		else Serial3.RxBuffer[Serial3.RxCount++] = buffer;
		if(Serial3.RxCount >= RXBF_LEN-1)
		{
			Serial3.RxFlag |= RECEIVE_WRONG | RECEIVE_OVER;	//�ý��ճ����־λ
			USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);		//�ر��ж�
		}
	}
	if(buffer == STX) Serial3.RxFlag |=	RECEIVING;	//��ʼ����
}
/*
	�������� void USART3_DW(void)
	������	 ����3������һ֡���ݴ���
*/
void USART3_DW(void)
{
	
	if(Serial3.RxFlag & RECEIVE_OVER)
	{
		u8 i=0;
		if(Serial3.RxFlag & RECEIVE_SUCCESS)	
		{//���ճɹ�
			switch(Serial3.RxBuffer[0])
			{
				case 'R':;
				case 'r':
				{
					i=(Serial3.RxBuffer[1]-0x30)*100 + (Serial3.RxBuffer[2]-0x30)*10 + 
							Serial3.RxBuffer[3]-0x30;
					Read_Data(i);
					break;
				}
				case 'W':;
				case 'w':
				{
					break;
				}
				case 'S':;
				case 's':
				{
					break;
				}
				default: ;
			}
		}
		else if(Serial3.RxFlag & RECEIVE_WRONG)
		{//���մ���
			printf("���մ�����ϣ�\r\n");
		}
		Serial3.RxFlag =0;		//����ձ�־λ
		Serial3.RxCount=0;		//�������
		for(i=0;i<RXBF_LEN;i++) Serial3.RxBuffer[i]=0;
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);		//���ж�
	}
}

//===================================USART4=======================================

//===================================USART5=======================================

