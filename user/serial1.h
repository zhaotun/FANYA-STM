/*
	�������˿��ļ������д�����ض����й�
	�˴���Ϊĥ�߳���Ƶ�������ӿ�
*/
#ifndef __SERIAL1_H
#define __SERIAL1_H

#include "stm32f10x.h"


//USART1��غ궨��
#define STX		0x3A		//":"��������ַ���ʼ��־λ
#define EOR		0x0D		//�س�,������һ���ַ�
#define EOX		0X0A		//����,�����ڶ����ַ�
#define ID		"AB"		//�豸���

#define NOT_UHRFID  15*60*20	//400	//18000	//���ö��ٴ�û�ж�������Ƶ��ǩ������Ϊģ�߱�����
#define NEW_UHRFID  10		//ͳ�ƶ�ȡ�±�ǩ���ݴ���
#define NOT_KAIMO	15*60		//20		//900

struct tiaoma
{
	u8 Len;			//���볤��
	u8 Code[30];	//����
	u32 Count;		//�����������
	u32 Timer;		//����������ʱ��
	u8 Flag;		//�������־λ
};

//USART1��غ���
void USART1_Configuration(void);	//����1ʱ������
void USART1_SendDW(void);			//����1������������
void USART1_SendDA(void);			//����1����һ���ֽ�����
void USART1_ReceiveDA(void);		//����1�������ݴ���
void USART1_ReceiveDW(void);		//����1���������ݴ���
void USART1_SendError(void);		//����1���ʹ�����Ϣ
void USART1_ReadURFID(void);		//��ȡ����Ƶ����������

void Nul_MuJuCount(void);			//ģ��Ϊ��ʱ��ģ����
void URFID_Timer1S(void);			//����Ƶ��������ʱ��,���жϳ�����

#endif

