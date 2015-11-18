/*
	����4,Ա�����������
*/
#include "serial4.h"
#include "stm32f10x.h"
#include "include.h"


void UART4_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//ʹ��USART4ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��GPIODʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//ʹ��USART3ʱ��
	//USART4�˿�����
	//����USART3 TX(PC10)Ϊ���ù����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//����USART3 RX(PC11)Ϊ��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//����USART1���п�
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);
	//ʹ�ܷ����жϣ����жϲ���ʱ��USART1�������ݼĴ���Ϊ��
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	//ʹ�ܽ����жϣ����жϲ���ʱ��USART1�������ݼĴ�����Ϊ��
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	//ʹ��USART1
	USART_Cmd(UART4, ENABLE);
	//ʹ��USART3�ж�
	NVIC_InitStructure.NVIC_IRQChannel=UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/*
	��������	USART3_ReceiveData
	������		����3���մ���
				����֡����ʼ�� �豸��� ָ���� �߼��� ������+���ݡ� У���� ������
	���룺		u8 tmp�����յ�������
	�����		��
 	���أ�		��
*/

u8 UART4_Flag=0;		//����4�շ���־λ,b4=1(��ʼ��������),b6=1(���յ���ȷ������)
u8 UART4_RxCounter=0;	//����4���ռ�����
u8 UART4_RxBuffer[UART4_RxBufferSizeMax];		//����4���ջ�����
u8 UART4_RX_Timer=0;
u8 UART4_RXFLAG=0;
u8 UART4_RXNEXT=0;
void UART4_ReceiveDA(u8 buffer)
{
	UART4_Flag |= b4;		//��־��ʼ����
	UART4_RxBuffer[UART4_RxCounter++] =buffer;  
	if(buffer == EOX)
	{
		if(UART4_RxBuffer[UART4_RxCounter-2] == EOR)	//�������
		{
			if(IsChar(UART4_RxBuffer,UART4_RxCounter-2) && (UART4_RxCounter == 12))	//�ж��Ƿ��ǿ���ʾ���ַ�
			{
				UART4_Flag &= 0x0F;		
				UART4_Flag |= b6;		//�������
				UART4_RX_Timer = 0;
				UART4_RXFLAG = 1;
				USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
				USART_Cmd(UART4, DISABLE);
			}
			else 
			{
				UART4_Flag &= 0x0F;		
				UART4_RX_Timer=0;
				UART4_RxCounter=0;
			}
		}
	}
	else if(UART4_RxCounter >= UART4_RxBufferSizeMax)	//���ڽ��ջ�����
	{
		UART4_Flag &= 0x0F;		
		UART4_Flag |= 0X40;			//���մ���
		UART4_RxCounter=0;
	}
	else if(UART4_RX_Timer >=100)		//���ճ���20ms��ʾ���մ���
	{
		UART4_Flag &= 0x0F;		
		UART4_RX_Timer=0;
		UART4_RxCounter=0;
	}	
} 

/*
*	��������UART4_ReceiveDW()
*	������	����4���ݽ��մ���
*	���룺	��
*	����� 	��
*	���أ�	��
*/
u8 YGTM_Flag=0;		//Ա�������־λ,b1=1(��ǰ��Ա��),b2=1(��ǰ��Ա�����룬�����յ���ͬ��Ա������),b3=1(��ǰ��Ա�����룬���յ��µ�Ա�����룬����ǰԱ��������������0)
u8 New_YG_Code[20];	//�µ�Ա������
u8 New_YG_CodeLend=0;	//��Ա�����볤��
u8 YUANGONG=0;			//���ڵڶ���Ա���򿨹ص���ʾ
u8 YUANGONG_TIME=0;		//�صƼ�ʱ
u8 Old_New_Flag = 0;
extern u8 YG_Code[];			//Ա��ģ������
extern u8 YG_CodeLend;
extern u8 USART3_SendWorld;
extern u8 Work_Status_Flag;
extern u8 Online_Status;

extern u32 YG_Cnt;
extern u8 Working_Type[2];	//����
extern u8 Working_Process[2];	//����

void UART4_ReceiveDW()			//���յ������ݴ���
{
	if (USART3_SendWorld != 0) return;
	if(YGTM_Flag & b2)
	{//��ǰ��Ա�����룬���յ��µ�Ա������
		u8 i;
		YGTM_Flag &= ~b2;	//���־λ
		YGTM_Flag |= b1;	//��־��ǰ������
		for(i=0;i<New_YG_CodeLend;i++)
		    YG_Code[i]=New_YG_Code[i];
		YG_CodeLend=New_YG_CodeLend;
//		Work_Status_Flag = Online_Status;
        Work_Status_Flag = 0x0A;	//��ǰ��Ա�����������£����¸ڣ����ϸڣ�Ȼ���л����ϸ�ҳ��
//		USART3_SendWorld=13;	    //��������Ա��������
		return;
	}
	else if(YGTM_Flag & b3)
	{//��ǰû��Ա�������յ��µ�Ա�����룬����ǰԱ��������������0
		u8 i;
		YGTM_Flag &= ~b3;	//���־λ
		YGTM_Flag |= b1;	
		for(i=0;i<New_YG_CodeLend;i++)
		YG_Code[i]=New_YG_Code[i];
		YG_CodeLend=New_YG_CodeLend;
		Work_Status_Flag = 0x0A;
//		USART3_SendWorld=13;
	} 
	else if(UART4_Flag & b6)
	{//���յ�����
		if(YGTM_Flag & b1)
		{//��ǰ��Ա������
			if(YG_CodeLend == (UART4_RxCounter-2))
			{//�½��յ�Ա��������ɵ�Ա�����볤����ͬ
				if(Char_Compare(YG_Code,&UART4_RxBuffer[0],YG_CodeLend))
				{//���յ���ͬ�����룬��ΪԱ�������ϸ�		
					YGTM_Flag &= ~b1;	
					
                    Work_Status_Flag = Online_Status;		//�¸ڱ�־�����������л����¸�ҳ����

				}
				else
				{//���յ���ͬ��Ա������//��Ա����������½��ܵ�������
					u8 i;
					YGTM_Flag &= ~b1;
//					YGTM_Flag |= b2;

                    Old_New_Flag = 1;

                    Work_Status_Flag = Online_Status;
//                    USART3_SendWorld=23;			//����Ĭ�ϵ��¸ڣ���Ϊ���ʱ��û�д���������ѡ���¸�
					
                    for(i=0;i<UART4_RxCounter-2;i++)
					{
                        New_YG_Code[i]=UART4_RxBuffer[i];
//                        YG_Code[i]=UART4_RxBuffer[i];
                    }
					New_YG_CodeLend=UART4_RxCounter-2;
//					YG_CodeLend=UART4_RxCounter-2;

                    YUANGONG=1;							//��Ա����־,���ڵ���˸
					YUANGONG_TIME=0;					//��ʱ����
				
                }
			}
			else
			{//�½��յ�Ա��������ɵ�Ա�����볤�Ȳ���ͬ������Ϊ���յ�������
				u8 i;
				YGTM_Flag &= ~b1;
				YGTM_Flag |= b2;		
				if(Working_Type[0] == '1')
				{
					Working_Process[0] = '4';
				}
				else if(Working_Type[0] == '2')
				{
					Working_Process[0] = '1';
				}
				USART3_SendWorld=23;				//����Ĭ�ϵ��¸ڣ���Ϊ���ʱ��û�д���������ѡ���¸�
				for(i=0;i<UART4_RxCounter-2;i++)
				New_YG_Code[i]=UART4_RxBuffer[i];
				New_YG_CodeLend=UART4_RxCounter-2;
				YUANGONG=1;							//��Ա����־,���ڵ���˸
				YUANGONG_TIME=0;					//��ʱ����
			}
		}
		else
		{//��ǰ��Ա�����룬����Ϊ���յ�������
			u8 i;
			if(YG_Cnt != 0)
			{//��ǰԱ��������������0�����ȷ���Ա���¸���Ϣ�ٷ����ϸ���Ϣ
				for(i=0;i<UART4_RxCounter-2;i++)
				    New_YG_Code[i] = UART4_RxBuffer[i];

				New_YG_CodeLend=UART4_RxCounter-2;

				YGTM_Flag |= b3;	

				USART3_SendWorld=23;		//����Ĭ�ϵ��¸ڣ���Ϊ���ʱ��û�д���������ѡ���¸�
			}
			else 
			{//��ǰԱ������������0
				for(i=0;i<UART4_RxCounter-2;i++)
				    YG_Code[i] = UART4_RxBuffer[i];
				
                YG_CodeLend = UART4_RxCounter-2;
				
                YGTM_Flag |= b1;	
				
                Work_Status_Flag = 0x0A;   //������Ա���ϸڣ�ֱ���л����ϸ�ҳ��

			}
		}
		UART4_Flag &= ~b6;
		UART4_RxCounter=0;			//����
	}
}

/*
	�������� u8 YuanGong_Online(void)
	���ص�ǰ�Ƿ���Ա�����ߣ������򷵻�1�����򷵻�0
*/
u8 YuanGong_Online(void)
{
	if(YGTM_Flag & 0x01)
		return 1;
	else 
		return 0;
}

