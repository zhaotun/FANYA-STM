/*
	�汾��		V1.0
	��д���ڣ�	2013��9��6��
	�༭�ˣ�	����
	��ģ�����ã���ģ������printf�����ӡ�����л�������������ʹ��ѭ���б�ʽ
	ʹ�÷�����	1��ʹ��printf��������ǣ���Ҫ�Ƚ�Target Options��Target���õ�Use MicroLIBѡ��㹴
				2����ʼ����������紮�ڣ��ĸ�������������������жϹ���
				3���ڴ�ӡ����������紮��������ķ����жϣ��紮�ڵķ����жϣ�����Printf_Send()����
				4���޸ĺ궨�塰OPEN_PRINTF_INTERRUPT(��ӡ�ж�)����"PRINTF_ONE_DATA(��ӡһ���ֽ�)����"CLOSE_PRINTF_INTERRUPT(�رմ�ӡ�ж�)��
				5���ⲿ��ֱ��ʹ�ñ�׼�⺯��printf(),��ɵ���Printf_StoreBuffeeData()����ֱ�Ӵ�ӡһ���ֽ�����  
	
	�޸����ڣ�
	�޸�˵����
*/
#include "stm32f10x.h"
#include "printf.h"
#include <stdio.h>

#define Open_Printf
#define	PRINTF_LEN				200			//��ӡ��������ȣ����ɶ��壬Խ��Խ��
#define OPEN_PRINTF_INTERRUPT   USART_ITConfig(UART4, USART_IT_TXE, ENABLE);		//������ӡ�ж�
#define PRINTF_ONE_DATA			USART_SendData(UART4, Printf_TxBuffer[Printf_TxSendPoint++]);	//��ӡһ���ֽ�����
#define CLOSE_PRINTF_INTERRUPT	USART_ITConfig(UART4, USART_IT_TXE, DISABLE);		//�رմ�ӡ�ж�


#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)		//���׼�⺯��Printf���

unsigned char Printf_TxBuffer[PRINTF_LEN];	//���ͻ�����
unsigned char Printf_TxSendPoint=0;			//����1����ָ��
unsigned char Printf_TxStorePoint=0;		//����1�洢ָ��
unsigned char Printf_TxBufferLend=0;		//����1�������ݳ���

/*
	����ԭ���ƣ� int fputc(int ch, FILE *f)
	˵����	�ú�����Printf����������ã�����Ҫ��ӡ����������洢����ӡ��������
	���룺	int ch;
	�����	int ch
	���أ�	int ch
*/
PUTCHAR_PROTOTYPE
{
	/* ARMJISHU ��ȴ���Printf���ڴ�ӡ����������
            д��Buffer����������(Bufferδ��ʱ) */
#ifdef Open_Printf
	Printf_StoreBuffeeData((unsigned char) ch);
#endif
	return ch;
}

/*
	�������� void Printf_StoreBuffeeData(unsigned char ch)
	������	 ����Ҫ��ӡ�����ݴ����ӡ������������FIFOԭ���ӡ����ÿ�ε��ú���Ҫ��
			 �������жϣ���������ֲʱ�����޸����һ����䣬�����������жϼ���
	�����	 ��
	���룺	 (unsigned char ch)��Ҫ����ӡ������
	���أ�	 ��
*/
void Printf_StoreBuffeeData(unsigned char ch)
{
	while(Printf_TxBufferLend == PRINTF_LEN-1);		//������������ʱ����Ҫ�ȴ�
	Printf_TxBuffer[Printf_TxStorePoint++]=ch;
	if(Printf_TxStorePoint == PRINTF_LEN) Printf_TxStorePoint=0;	//�洢ָ��ָ���ʱ������ָ��ָ����
	Printf_TxBufferLend++;
	OPEN_PRINTF_INTERRUPT			//�ʾ����ʵ��������޸ģ������������ʹ�ӡ�������ķ����ж�
}

/*
	�������� void Printf_Send(void)
	������	 ���ʹ�ӡ��������һ���ֽ����ݣ������жϳ����ã�ÿ����һ�η���һ���ֽ�����
			 ���ڵײ�������أ���˳�������ֲʱ����Ҫ�޸Ĵ˺���
*/
void Printf_Send(void)
{
	PRINTF_ONE_DATA			//����һ���ֽ�����
	if(Printf_TxSendPoint == PRINTF_LEN) Printf_TxSendPoint=0;				//��ָ��ָ������޸�ָ��
	Printf_TxBufferLend--;
	if(Printf_TxSendPoint == Printf_TxStorePoint) 				//�ж��Ƿ�����ϣ������������رշ����ж�
	{
		CLOSE_PRINTF_INTERRUPT		//������ϣ��رշ����ж�	
	}
}
