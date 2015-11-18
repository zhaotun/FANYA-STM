/*
	�˴���Ϊĥ�߳���Ƶ�������ӿ�
*/
#include "serial1.h"

#include "stm32f10x.h"
#include "include.h"

u8  USART1_Flag;				//�շ���־λ
u8  USART1_TxLen;				//�������ݳ���
u8  USART1_TxBuffer[255];		//�������ݻ�����
u8  USART1_TxCounter;			//�������ݼ�����

u8 	USART1_RxBuffer[USART1_RxBufferSizeMax];	//�������ݻ�����
u8  USART1_RxCounter = 0; 						//�������ݼ�����
u8  USART1_RX_Timer= 0;			                //����1���ռ�ʱ��

u16  NOT_URFID_data=0;			//�޳���Ƶ����ͳ��
u16  New_MJ_Counter=0;			//��ģ�߱�ǩ����
u8	New_MJ_CodeLend=0;			//��ģ�߳���
u8	New_MJ_Code[25];			//��ģ�߱�ǩ����

u8	UHRFID_Flag=0;		//����Ƶ��ǩ��ر�־λ,b1��ģ��,b2��ǰ��ģ�ߣ������յ���ģ��,b3��û��ģ������ʱ,��ȡ����һ�ε�ģ������
						//b4,��ǰ��ģ�ߣ�����ȡ����ģ�ߣ��õ�����2��,b5��ǰû��ģ�ߣ���ȡ����ģ�ߣ�

extern u8 USART3_SendWorld;	//����3����ָ����
extern u8 MJ_CodeLend;		//ĥ�����볤��
extern u8 MJ_Code[];
extern u32 MJ_Cnt;
extern u32 YG_Cnt;
extern u8 UHRFID_Manual_Input_Flag;

struct tiaoma New_MoJu,Old_MoJu,Nul_MoJu;

/*******************************************************************************
* ������		��USART1_Init
* ����			������USART1���п�
* ����			����
* ���			����
* ����			����
*******************************************************************************/
void USART1_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//ʹ��USART1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);	//ʹ��GPIOAʱ��,ʹ��IOǰ����ʹ����Ӧ������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//ʹ��USART1ʱ�ӣ�ʹ�ô���ǰ����ʹ����Ӧ������

	//����USART1 TX(PA.09)Ϊ���ù����������				
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				//PA9ΪUSART1��TX����
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//����IO�ڵ�����������Ϊ50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//���ø�IO��Ϊ�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//��ʼ��GPIOA

	//����USART1 RX(PA.10)Ϊ��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				//PA9ΪUSART1��RX����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//���ø�IO��Ϊ��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//��ʼ��GPIOA

	//����USART1���п�
	USART_InitStructure.USART_BaudRate = 57600;          					//���ò�����Ϊ57600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;			    //8λ����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;					//1λֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;						//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			//ʹ��TX,RX
	USART_Init(USART1, &USART_InitStructure);								//��ʼ��USART1
	//ʹ�ܷ����жϣ����жϲ���ʱ��USART1�������ݼĴ���Ϊ��
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	//ʹ�ܽ����жϣ����жϲ���ʱ��USART1�������ݼĴ�����Ϊ��
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);						    //ʹ��USART1�����ж�

	//ʹ��USART1
	USART_Cmd(USART1, ENABLE);												//USART1ʹ��

	//ʹ��USART1ȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;							//ʹ��USART4�жϷ������
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;					//�����жϵ���ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;						//�����жϵ������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							//�����ж�ʹ��
	NVIC_Init(&NVIC_InitStructure);											//��ʼ�������ж�
}

/****************************************************************************************************
*	��������USART1_SendDW()
*	������	�˺�����USART1_ReadURFID()���ã��������������ж�
*	���룺	��
*	�����	��
*	���أ� 	�� 
****************************************************************************************************/
void USART1_SendDW()
{
	USART1_TxCounter=0;				  //���ͼ���������
	USART1_Flag &=0xF0;			      //��־λ����
	USART_SendData(USART1, USART1_TxBuffer[USART1_TxCounter++]); //���͵�һ���ֽڣ������ֽ��ɷ����жϷ�������Զ�����
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE); //ʹ�ܴ���1�����жϣ���������
}

/*************************************************************************************
*	��������USART1_SendDA()	
*	������	�˺����ڴ���1�����жϷ������USART1_IRQHandler()�е��ã�����һ�����ݺ����´����ж�
*	���룺	��
*	�����	��
*	���أ�	��
**************************************************************************************/
void USART1_SendDA()
{
	USART_SendData(USART1, USART1_TxBuffer[USART1_TxCounter++]); //���ֽڷ��ͺ�����ÿ�η���һ���ֽڣ��ú���Ϊϵͳ����   
	if(USART1_TxCounter ==USART1_TxLen)                          //�жϷ��ͼ������Ƿ�������跢���ֽڵĳ���
	{
		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);			 //�����ͼ������������跢���ֽڵĳ��ȣ���رշ����ж�
		USART1_TxCounter=0;										 //���ͼ���������
		USART1_Flag |=0x01;	                                     //�÷�����ɱ�־λ
	} 
}

/********************************************************************************
*	��������	USART1_ReceiveDA()
*	������		����1���մӳ���Ƶ���������͵����ݲ�����
*	���룺		��
*	�����		��
* 	���أ�		��
*********************************************************************************/
void USART1_ReceiveDA()
{
	if(USART1_RX_Timer>=3)		//���յ���һ�����ݺ󣬵ȴ�3MS����Ϊ�����ݽ������
	{
		USART1_RX_Timer=0;		//���ռ�ʱ������
		USART1_Flag &=0x0F;		//���־λ����ʱ��ֹͣ��ʱ
		USART1_Flag |= b6;		//�ý�����ɱ�־λ
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	//�رմ���1�����ж�
	}
}

/**********************************************************************************
*	��������USART1_ReceiveDW()
*	������	�Դ���1���յ����ݽ��д����ú�����main�����е���
*	���룺	��
*	����� 	��
*	���أ�	��
***********************************************************************************/
extern u8 USART3_TxBuffer[255];
extern u8 USART3_TxLen;
	   u8 Old_MJ_CodeLend;
	   u8 Old_MJ_Code[30];
	   u16 New_Nul_MuJu;				//��ǰû�����룬���յ��������������
void USART1_ReceiveDW()					
{
	USART1_ReceiveDA();					        //��������ж�
	if(USART3_SendWorld != 0) return;
	if(UHRFID_Flag & b2)
	{//��ǰ��ģ�ߣ������յ��µ�ģ��
		u8 i;
		UHRFID_Flag &= ~b2;
		UHRFID_Flag |= b1 | b4;				
		for(i=0;i<USART1_RxBuffer[0]-7;i++)
		{
			MJ_Code[i]=New_MoJu.Code[i];		
			New_MoJu.Code[i] = '0';
		}
		MJ_Code[i]=0;
		MJ_CodeLend=New_MoJu.Len;			//ĥ�����볤��
		New_MoJu.Len = 10;
		USART3_SendWorld=8;					//������ģ����Ϣ
		//------------
		printf("��ǰ��ģ�ߣ�����ȡ����ģ��\r\n");
		printf("%s\r\n",MJ_Code);
		//------------
	}
	else if(UHRFID_Flag & b3)
	{//��û��ģ������ʱ����ȡ����һ�ε�ģ������
		UHRFID_Flag &= ~b3;
		UHRFID_Flag |= b1;
		USART3_SendWorld = 1;		//�ϴ�����
		//------------
		printf("��ǰû��ģ�ߣ���ȡ����һ����ͬ��ģ��\r\n");
		printf("%s\r\n",MJ_Code);
		//------------
	}
	else if(UHRFID_Flag & b5)
	{//��ȡ����ģ����Ϣ
		u8 i;
		for(i=0;i<USART1_RxBuffer[0]-7;i++)
		{
			MJ_Code[i]= USART1_RxBuffer[i+6];
		}
		MJ_Code[i]=0;
		MJ_CodeLend = USART1_RxBuffer[0]-7;	
		UHRFID_Flag &= ~b5;
		UHRFID_Flag |= b1;						//�ý��ܵ��±�ǩ��־λ
		USART3_SendWorld=8;						//���Ͷ�ȡ����ģ����Ϣ
		//------------
		printf("��ǰû��ģ�ߣ���ȡ����ģ��\r\n");
		printf("%s\r\n",MJ_Code);
		//------------
	}
	if(USART1_Flag & b6)			       //�����ݽ������
	{
		u8 i;
		if(UHRFID_Flag & b1)			   //��ǰ��ģ������
		{
			if(USART1_RxBuffer[0]<=7)	   //�����յ������ݳ��Ȳ�����7ʱ������Ϊ��ģ������
										   //����ͨ��Э�飬��һ���ֽڴ������ݳ���
			{
				NOT_URFID_data++;		   //����1ÿ50ms�򳬸�Ƶ����һ�ζ�ģ�����������δ���յ�ģ������NOT_URFID_data��1
				if((NOT_URFID_data >= NOT_UHRFID)&& (User_GetTimer1sValue() >= NOT_KAIMO))
				{//������δ���յ�ģ������Ĵ����ﵽ18000��Լ18���Ӻ󣩣���NOT_KAIMO��900�����ӣ�15���ӣ���û�п�ģ������Ϊģ�߱��ƿ�
					
					if(UHRFID_Manual_Input_Flag == 1)
					{
						 NOT_URFID_data=0;
						 printf("�ֶ������ģ�߲�����\r\n");	
					}
					else
					{
						printf("ģ�߱��ƿ�������\r\n");
						printf("%s\r\n",MJ_Code);
						//------------
						NOT_URFID_data=0;		           //δ���յ�ģ������Ĵ�������
						User_Timer1sClear();			   //�û��붨ʱ��ʱ������
						for(i=0;i<MJ_CodeLend;i++)
						{
							Old_MoJu.Code[i] = MJ_Code[i]; //���浱ǰģ������
						}
						Old_MoJu.Code[i]=0;				   //???
						Old_MoJu.Len = MJ_CodeLend;		   //���浱ǰģ�����볤��
						Old_MoJu.Flag |= b1;			   //��ģ�߱�־λ��λ
						UHRFID_Flag &= ~b1;				   //ģ�߱�־λ����
						Nul_MoJu.Count=0;				   //��ģ�߼�������
						Nul_MoJu.Flag |= b1;			   //��ģ�߱�־λ��λ
	
						if(YG_Cnt == 0)					   //����ʱԱ����ģ����Ϊ��ʱ
						{
							USART3_SendWorld = 7;		   //USART3������ģ��Ϣ
						}
						else
						{
							USART3_SendWorld = 20;		   //����ʱԱ����ģ���ݲ�Ϊ��ʱ���ȷ���Ա����ǰ��ģ���ݣ�����ģ
						}
					}
				}
			}
			else if((IsChar(&USART1_RxBuffer[6],USART1_RxCounter-8))) //�����ص����ݶ����ַ���������Ϊ���յ���Чģ������
			{
				NOT_URFID_data=0;						   //���յ���ģ������Ĵ�������
				if(MJ_CodeLend == USART1_RxBuffer[0]-7)	   //�����յ���ģ�����볤�ȵ��ڵ�ǰģ�����볤��
				{
					if(Char_Compare(MJ_Code,&USART1_RxBuffer[6],MJ_CodeLend)) //�����յ���ģ�������뵱ǰģ��������ͬ
					{
						New_MJ_Counter=0;				 //��ģ�߼�����0������δ���յ���ģ��
					}
					else								 //�����յ���ģ�������뵱ǰģ�����벻ͬ
					{
						if(Char_Compare(New_MoJu.Code,&USART1_RxBuffer[6],USART1_RxBuffer[0]-7))
						{//����һ�ν��յ���ģ�������뱣�����ģ��������ͬ
							New_MJ_Counter++;					      //��ģ�߼�����1
							if(New_MJ_Counter >= NEW_UHRFID)	      //����ģ�߼����ﵽ10�Σ�����Ϊ��ģ���Ѿ�����Ϊ��ģ��
							{
								New_MJ_Counter = 0;					  //��ģ�߼�������
							
								for(i=0;i<USART1_RxBuffer[0]-7;i++)
									New_MoJu.Code[i] = USART1_RxBuffer[i+6];  //������ģ������

								New_MoJu.Len = USART1_RxBuffer[0]-7;  //������ģ�����볤��	
								
								Nul_MoJu.Flag &= ~b1;				  //��ģ�߱�־����
								Nul_MoJu.Count = 0;					  //��ģ�߼�������		
								UHRFID_Flag |= b2;					  //		
								if(YG_Cnt != 0)						  //��Ա����ģ���ݲ�Ϊ0
								{
									USART3_SendWorld = 20;			  //�ȷ���Ա����ǰ��ģ���ݣ�����ģ			
								}
								else
								{
									USART3_SendWorld = 7;			  //Ա����ģ���ݵ���0ʱ��������ģ��Ϣ
								}
							}
						}
						else//����һ�ν��յ���ģ�������뱣�����ģ�����벻ͬ�������¸���ģ������
						{
							New_MJ_Counter=1;						        //��ģ�߼�����1

							for(i=0;i<New_MoJu.Len;i++)
								New_MoJu.Code[i] = USART1_RxBuffer[i+6];	//�����µ�ģ������

							New_MoJu.Len = USART1_RxBuffer[0]-7;		    //�����µ�ģ�����볤��
						}
					}	
				}
				else  //�����յ���ģ�����볤�Ȳ����ڵ�ǰģ�����볤�ȣ���϶���һ���µ�ģ��
				{
					if(New_MoJu.Len == USART1_RxBuffer[0]-7) //�����յ���ģ�����볤�ȵ����µ�ģ�����볤��
					{
						if(Char_Compare(New_MoJu.Code,&USART1_RxBuffer[6],USART1_RxBuffer[0]-7))
						{ //����һ�ν��յ���ģ���������µ�ģ��������ͬ
							New_MJ_Counter++;					     //��ģ�߼�����1
							if(New_MJ_Counter >= NEW_UHRFID)		 
							{//����ģ�߼����ﵽ10�Σ�����Ϊ��ģ���Ѿ�����Ϊ��ģ��
								UHRFID_Flag |= b2;						    //��ģ��������ձ�־λ��λ
								
								for(i=0;i<USART1_RxBuffer[0]-7;i++)
									New_MoJu.Code[i] = USART1_RxBuffer[i+6];//������ģ������
							
								New_MoJu.Len = USART1_RxBuffer[0]-7;		//������ģ�����볤��
								Nul_MoJu.Flag &= ~b1;						//��ģ�߱�־����
								Nul_MoJu.Count = 0;					        //��ģ�߼�������	
								New_MJ_Counter = 0;							//��ģ�߼�������
								if(YG_Cnt != 0)								//��Ա����ģ���ݲ�Ϊ0
								{
									USART3_SendWorld = 20;					//�ȷ���Ա����ǰ��ģ���ݣ�����ģ	
								}
								else
								{
									USART3_SendWorld = 7;					//Ա����ģ���ݵ���0ʱ��������ģ��Ϣ
								}
							}
						}
						else//����һ�ν��յ���ģ���������µ�ģ�����벻ͬ 
						{
							New_MJ_Counter=1;		                	  //��ģ�߼�����1
							
							for(i=0;i<New_MoJu.Len;i++)
								New_MoJu.Code[i] = USART1_RxBuffer[i+6];  //������ģ������
							
							New_MoJu.Len = USART1_RxBuffer[0]-7;		  //������ģ�����볤��
						}
					}
					else //�����յ���ģ�����볤�Ȳ����ڵ�ǰģ�����볤�ȣ���϶���һ���µ�ģ��
					{
						New_MJ_Counter=1;							//��ģ�߼�����1
						
						for(i=0;i<New_MoJu.Len;i++)
							New_MoJu.Code[i] = USART1_RxBuffer[i+6];//������ģ������
						
						New_MoJu.Len = USART1_RxBuffer[0]-7;		//������ģ�����볤��
					}
				}
			}
			else
			{//���ص���������
				NOT_URFID_data++;                        //δ���յ�ģ������Ĵ�����1
				printf("��ȡ������NOT_URFID_data=%u\r\n",NOT_URFID_data);
				if((NOT_URFID_data >= NOT_UHRFID)&& (User_GetTimer1sValue() >= NOT_KAIMO)) 
				{//������δ���յ�ģ������Ĵ����ﵽ18000��Լ18���Ӻ󣩣���NOT_KAIMO��900�����ӣ�15���ӣ���û�п�ģ������Ϊģ�߱��ƿ�
					if(UHRFID_Manual_Input_Flag == 1)
					{
						 NOT_URFID_data=0;
						 printf("�ֶ������ģ�߲�����\r\n");	
					}
					else
					{
						NOT_URFID_data=0;					//δ���յ�ģ������Ĵ�������
						User_Timer1sClear();				//�û��붨ʱ��ʱ������
						
						for(i=0;i<MJ_CodeLend;i++)
							Old_MoJu.Code[i] = MJ_Code[i];	//���浱ǰģ����Ϣ��Old_MoJu��
						
						Old_MoJu.Len = MJ_CodeLend;			//���浱ǰģ�����볤��
						Old_MoJu.Flag |= b1;				//��ģ�߱�־��λ
						UHRFID_Flag &= ~b1;			        //�������־λ��λ
						Nul_MoJu.Flag |= b1;
						USART3_SendWorld = 20;		        //�ȷ���Ա����ǰ��ģ���ݣ�����ģ
					}
				}
			}
		}
		else   //��ǰ��ģ�����룬
		{
			if(USART1_RxBuffer[0] > 7)		
			{//���յ�����������
				if(IsChar(&USART1_RxBuffer[6],USART1_RxCounter-8))			//���������ݣ��ж��Ƿ����ַ���
				{//���յ����������ַ���
					NOT_URFID_data=0;
					if((Old_MoJu.Len == USART1_RxBuffer[0]-7) && Char_Compare(Old_MoJu.Code,&USART1_RxBuffer[6],USART1_RxBuffer[0]-7)) //�ж��Ƿ����һ�εľ�����������ͬ
					{//����һ�εľ�ģ����ͬ
						for(i=0;i<USART1_RxBuffer[0]-7;i++)
						{
							MJ_Code[i] = Old_MoJu.Code[i];		//����ģ������
							Old_MoJu.Code[i] = '0';
						}
						MJ_CodeLend = Old_MoJu.Len;				//����ģ�����볤��
						MJ_Code[i]=0;	                    	//��β
						MJ_Cnt = Nul_MoJu.Count;
						YG_Cnt = Nul_MoJu.Count;
						Nul_MoJu.Count=0;
						Old_MoJu.Len = 10;
						Nul_MoJu.Flag &= ~b1;
						UHRFID_Flag |= b3;						//��ǰû��ģ�ߣ������ܵ���һ����ͬ��ģ��
						USART3_SendWorld=21;					//���Ͷ�ȡ����ģ����Ϣ
					}
					else
					{//��ͬ
						if(Char_Compare(New_MoJu.Code,&USART1_RxBuffer[6],New_MoJu.Len)) //�ж��Ƿ����һ�ε�������������ͬ
						{//���յ����ϴε��±�ǩ��ͬ���±�ǩ
							New_MJ_Counter++;
							if(New_MJ_Counter >= NEW_UHRFID)
							{//��ǰû��ģ�ߣ���ȡ����ģ����ģ
								New_MJ_Counter = 0;						//��ͬ��ǩ��������
								Nul_MoJu.Flag &= ~b1;
								Nul_MoJu.Count = 0;					//��ռ���
								UHRFID_Flag |= b5;						//�ý��ܵ��±�ǩ��־λ
								if(MJ_Cnt != 0)
								{
									if(YG_Cnt != 0)
									{
										USART3_SendWorld=20;				
									}
									else
									{
										USART3_SendWorld=7;
									}
								}
							}
						}
						else 
						{//�����յ���ͬ���±�ǩ�����¸����±�ǩ
							New_MJ_Counter=1;
							for(i=0;i < New_MoJu.Len;i++)
							New_MoJu.Code[i] = USART1_RxBuffer[i+6];	//�洢ĥ������
							New_MoJu.Len = USART1_RxBuffer[0]-7;		//ĥ�����볤��
						}
					}
				}
			}
		}
		USART1_Flag &= 0x0F;		//���ʶλ
		USART1_RxCounter=0;			//����
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//���ж�
	}
}

/*
	�������� void Nul_MuJuCount(void)
	������	 ��ģ��Ϊ��ʱ�����м���
	���룺	 ��
	�����	 ��
*/
void Nul_MuJuCount(void)
{
	if(Nul_MoJu.Flag & b1)
	{
		Nul_MoJu.Count++;
	}
}

/*
*	��������USART1_SendError()
*	������	����1���͡�Error!\n"��Ϣ
*	���룺	��
*	�����	��
*	���أ�	��
*/
void USART1_SendError()		//��δʹ��
{
	USART1_TxBuffer[0]='E';
	USART1_TxBuffer[1]='r';
	USART1_TxBuffer[2]='r';
	USART1_TxBuffer[3]='o';
	USART1_TxBuffer[4]='r';
	USART1_TxBuffer[5]='!';
	USART1_TxBuffer[6]=0x0D;
	USART1_TxBuffer[7]=0x0A;
	USART1_TxLen=8;
	USART1_SendDW();
	USART1_Flag &= 0x0F;	
	USART1_RxCounter=0;
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}


/*
	��������USART1_ReadURFID()
	������  �˺�����stm32f10x_it.cͷ�ļ��е�SysTick_Handler()�������ã�ÿ50msִ��һ��
	���ܣ�  �����򳬸�Ƶ����ģ�������ȡ����
	���룺  ��
	�����	��
	���أ�	��
*/
void USART1_ReadURFID()
{
	USART1_TxBuffer[0]=0X04;
	USART1_TxBuffer[1]=0X00;
	USART1_TxBuffer[2]=0X01;
	USART1_TxBuffer[3]=0XDB;
	USART1_TxBuffer[4]=0X4B;
	USART1_TxLen=5;
	USART1_SendDW();
}


/*
	�������� void URFID_Timer1S(void)
	������ 	 ����Ƶ��������ʱ��,���жϳ�����
*/
u8 UHRFID_New_Timer=0;
void URFID_Timer1S(void)
{
	if(UHRFID_Flag & b4)
	{
		UHRFID_New_Timer++;
		if(UHRFID_New_Timer >= 3)
		{
			UHRFID_New_Timer=0;
			UHRFID_Flag &= ~b4;
		}
	}
}

/*
	�������� Receive_CRC(u8 *p,u8 len)
	������	 ����������ݵ�CRC�����Ϊ*p,����Ϊlen�����ݣ�����Ϊ
*/

/*
	�ж��Ƿ���ģ�ߡ�CCC008D02-1 ���������򷵻���"1"�����򷵻ؼ�"0"
*/
u8 No_SpecialChar(u8 *p,u8 len)
{
	if(len == 12)
	{
		u8 i;
		u8 a[12]="CCC008D02-1 ";
		for(i=0;i<12;i++)
		{
			if(*p == a[i])
				p++;
			else return(1);
		}
		return(0);
	}
	else return(1);
}


