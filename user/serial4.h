/*
	�������˿��ļ������д�����ض����й�
	�˴���ΪԱ����Ƶ�������ӿ�
*/
#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"

#define EOR		0x0D		//�س�,������һ���ַ�
#define EOX		0X0A		//����,�����ڶ����ַ�

#define UART4_RxBufferSizeMax  32		//���崮��3�����ջ�����


void UART4_Configuration(void);		//����4��ʼ��
void UART4_ReceiveDA(u8 buffer);	//����4���մ���
void UART4_ReceiveDW(void);			//���յ������ݴ���
u8 YuanGong_Online(void);			//���ص�ǰ�Ƿ���Ա�����ߣ������򷵻�1�����򷵻�0

#endif
