#include "stm32f10x.h"
#include "calculation.h" 

#define RX5_LEN 		100
#define RX5_TIMER 		20
#define USART5_BaudRate 9600

void Init_Serial5(void);		//����5��ʼ��
void USART5_Send(void);			//����5����һ֡����
void USART5_Receive(u8 buf);		//����5����һ֡����
void USART5_RxDW(void);			//����5���մ�����ʱ��6������
void Serial5_DW(u8 command);	//����5���յ����ݺ������
void HMI_Touch_Data(void);		//����������һ������

