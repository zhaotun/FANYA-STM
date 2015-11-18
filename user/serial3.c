//�ô���������̫��ͨѶ
#include "serial3.h"
#include "include.h"
#include "string.h"

extern u8 ADDRESS[3];		//�豸��ַ
extern u8 MACHINE_TYPE[2];
extern u8 RUN_STATE; 		//������ǰ����״̬
extern u8 RUN_STATE; 		//������ǰ����״̬
extern u8 Code_Mode;		//��д�ش�ָ��
extern u8 Command[3];		//��������
extern u8 YG_CodeLend;		//Ա�����볤��
extern u8 YG_Code[];
extern u8 MJ_CodeLend;		//ĥ������
extern u8 MJ_Code[];
extern char MO_Time[6];		//ģ�߼�ʱ
extern char MJ_Timing[6];	//ģ�߼�ʱ
extern char YG_Time[6];		//Ա����ʱ
extern u8 Working_Type[2];	//����
extern u8 Working_Process[2];	//����
extern u8 On_Duty_Flag;

extern u16 Address;		//������ַ
extern u32 MJ_Cnt;		//ĥ�߼���
extern u32 MJ_Tim;		//ĥ�߼�ʱ
extern u32 YG_Cnt;		//Ա����ģ����
extern u32 YG_Tim;		//Ա����ʱ
extern u32 XT_ONTime;	//ϵͳ�ϵ��ʱ
extern u32 RL_ONTime;	//��¯�ϵ��ʱ
extern u8 YGTM_Flag;	//Ա�������־λ
extern u8 New_YG_Code[30];	//�µ�Ա������
extern u8 New_YG_CodeLend;	//��Ա�����볤��
extern u32 Machine_Timer;

extern u8 Work_Status_Flag;

extern u8 QC_Input_Data[10];
extern u8 Mould_Defect[32];

extern u8 QC_Name[2];
extern u8 QC_PassWord[2];

extern u8 QCBad_QianZhu[4]  ;
extern u8 QCBad_LaMo[4]     ;
extern u8 QCBad_ShaKong[4]  ;
extern u8 QCBad_LengGe[4]   ;
extern u8 QCBad_BianXing[4] ;
extern u8 QCBad_BengQue[4]  ;
extern u8 QCBad_QiPao[4]    ;
extern u8 QCBad_LuGuan[4]   ;
extern u8 QCBad_JiTan[4]    ;
extern u8 QCBad_ChiChen[4]  ;
extern u8 QCBad_HeiYin[4]   ;
extern u8 QCBad_QiTa[4]     ;
extern u8 QC_ChengFen[10]   ;


	u8 USART3_TxBuffer[255];
	u8 USART3_TxLen;
	u8 USART3_TxCounter;
	u8 USART3_SendWorld=0;		//���ͱ�־��
	u8 USART3_Flag=0;			//�շ�״̬��־��
	u8 USART3_RxBuffer[USART3_RxBufferSizeMax];
	u8 USART3_RxCounter = 0; 

u8  JOB[72];
/*
	�������� USART3_Init()
	������	 ����3��ʼ������
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void USART3_Configuration(void)
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
	USART_InitStructure.USART_BaudRate = 9600;
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
	�������� USART3_SendDA()
	������	 ����3�������ݴ���
			 ���ڷ����жϳ�����
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void USART3_SendDA()
{	
	if(USART3_TxCounter >= USART3_TxLen)
	{
		//��ɷ��ͣ��رշ����ж�
		USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
		USART3_TxCounter=0;				//���ͼ���������
		USART3_Flag &=0xFE;				//�÷�����ɱ�־λ
	}
	else 
	{
		USART_SendData(USART3, USART3_TxBuffer[USART3_TxCounter++]);   	//����һ���ֽ�
	}
}

/*
	�������� USART3_SendDW(void)
	������	 
*/
void USART3_SendDW(void)
{
	USART3_TxCounter=0;
	USART3_Flag &=0xF0;
	USART3_Flag |=0x01;		//���ڷ�����	
	Communicate_SaveData(USART3_TxBuffer,USART3_TxLen);
	USART_SendData(USART3, ':');	//������ʼλ
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

/*
	��������	USART3_ReceiveData
	������		����3���մ���
				����֡����ʼ�� �豸��� ָ���� �߼��� ������+���ݡ� У���� ������
	���룺		u8 tmp�����յ�������
	�����		��
 	���أ�		��
*/
void USART3_ReceiveDA(u8 buffer)
{
	Communicate_SetOff();	//�����̫�������ر�־λ
	Clera_Timer1();//���δ�յ���ʱ
	if(USART3_Flag & 0x10)
	{
		USART3_RxBuffer[USART3_RxCounter++] =buffer;  
		if(buffer==EOX)
		{
			if(USART3_RxBuffer[USART3_RxCounter-2]==EOR)	//�������
			{
				USART3_Flag&=0x0F;		
				USART3_Flag|=0X20;		//�������
				USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
			}
		}
		else if(USART3_RxCounter >= USART3_RxBufferSizeMax)	//���ڽ��ջ�����
		{
			USART3_Flag &= 0x0F;		
			USART3_Flag |= 0X40;			//���մ���
			USART3_RxCounter=0;
			USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);	
		}
	}
	else if(buffer==STX) USART3_Flag |= 0x10;		//���տ�ʼ
}

/*
*	��������USART3_ReceiveDW()
*	������	����3���ݽ��մ���
*	���룺	��
*	����� 	��
*	���أ�	��
*/
void USART3_ReceiveDW()			//���յ������ݴ���
{
	if(USART3_Flag & 0x20)
	{
		u8 tem=(USART3_RxBuffer[1]-0x30)*10+USART3_RxBuffer[2]-0x30;	//����������
		switch(USART3_RxBuffer[0])
		{
			case 'R':
			{	//������
				switch(tem)
				{
					case 1: //���ͱ�����ַ
					{
						Send_Address();	
						Clear_RxBuffer();
						break;
					}
					case 2: //���ͱ�����������
					{
						Send_MACHINE_TYPE();	
						Clear_RxBuffer();
						break;
					}
					case 3: //���ͱ�����ǰ����״̬
					{
						USART3_TxBuffer[0]=RUN_STATE;
						USART3_TxBuffer[1]=0x0D;		//������
						USART3_TxBuffer[2]=0x0A;
						USART3_TxLen=3;
						USART3_SendDW();	
						Clear_RxBuffer();
						break;
					}
					case 4: //���ͻ�����ǰģ������
					{
						SendMJCode();
						Clear_RxBuffer();
						break;
					}
					case 5: //���͵�ǰԱ������  
					{
						SendYGCode();
						Clear_RxBuffer();
						break;
					}
					case 6: //���ͻ�����ǰģ�߿�ģ����
					{
						Send_MMYY(MJ_Cnt);
						Clear_RxBuffer();
						break;
					}
					case 7: //���ͻ�����ǰģ�߰�װʱ��
					{
						Send_MMYY(MJ_Tim);
						Clear_RxBuffer();
						break;
					}
					case 8: //���ͻ�����ǰԱ����ģ����
					{
						Send_MMYY(YG_Cnt);
						Clear_RxBuffer();
						break;
					}
					case 9: //���ͻ�����ǰԱ���ڸ�ʱ��
					{
						Send_MMYY(YG_Tim);
						Clear_RxBuffer();
						break;
					}
					case 20: //����ָ����001����
					{
						USART3_SendWorld=1;
						Clear_RxBuffer();
						break;
					}
					case 21: //����ָ����110����
					{
						USART3_SendWorld=2;
						Clear_RxBuffer();
						break;
					}
					case 22: //����ָ����111����
					{
						USART3_SendWorld=3;
						Clear_RxBuffer();
						break;
					}
					case 23: //����ָ����112����
					{
						USART3_SendWorld=4;
						Clear_RxBuffer();
						break;
					}
					case 24: //����ָ����113����
					{
						USART3_SendWorld=5;
						Clear_RxBuffer();
						break;
					}
					case 25: //����ָ����114����
					{
						USART3_SendWorld=6;
						Clear_RxBuffer();
						break;
					}
					case 26: //����ָ����110����
					{
						USART3_SendWorld=19;
						Clear_RxBuffer();
						break;
					}
					case 27: //����ָ����111����
					{
						USART3_SendWorld=8;
						Clear_RxBuffer();
						break;
					}
					case 28: //����ָ����112����
					{
						USART3_SendWorld=9;
						Clear_RxBuffer();
						break;
					}
					case 29: //����ָ����113����
					{
						USART3_SendWorld=10;
						Clear_RxBuffer();
						break;
					}
					case 30: //����ָ����114����
					{
						USART3_SendWorld=11;
						Clear_RxBuffer();
						break;
					}
					case 31: //����ָ����115����
					{
						USART3_SendWorld=12;
						Clear_RxBuffer();
						break;
					}
					case 32: //����ָ����116����
					{
						USART3_SendWorld=13;
						Clear_RxBuffer();
						break;
					}
					case 33: //����ָ����117����
					{
						USART3_SendWorld=14;
						Clear_RxBuffer();
						break;
					}
					case 34: 	//����ϵͳ�ϵ���Ϣ
					{
						USART3_SendWorld=15;
						Clear_RxBuffer();
						break;
					}
					case 35:	//����Ա���ϸ���Ϣ
					{
						USART3_SendWorld=16;
						Clear_RxBuffer();
						break;
					}
					case 36:	//����ģ�߰�װ��Ϣ
					{
						USART3_SendWorld=17;
						Clear_RxBuffer();
						break;
					}
					case 37:	//����ģ�߰�װ��Ϣ
					{
						USART3_SendWorld=19;
						Clear_RxBuffer();
						break;
					}
					default :
					{	//�������
						Send_Error();
						Clear_RxBuffer();
						break;
					}
				}
				break;
			}
			case 'W':
			{	//д����
				switch(tem)
				{
					case 1:		//�޸ı�����ַ
					{	
						SPI_Flash_Write(&USART3_RxBuffer[3],Flash_Base0,3);	//�޸ı���վ��
						SPI_Flash_Read(ADDRESS,Flash_Base0,3);		//��ȡ��ַ
						Send_Address();		//�����޸ĺ�ĵ�ַ
						Clear_RxBuffer();
						break;
					}
					case 2:		//�޸ı�����������
					{	
						SPI_Flash_Write(&USART3_RxBuffer[3],Flash_Base1,2);	//�޸ı�����������
						SPI_Flash_Read(MACHINE_TYPE,Flash_Base1,2);		//��ȡ��������
						Send_MACHINE_TYPE();		//�����޸ĺ�Ļ�������
						Clear_RxBuffer();
						break;
					}
					case 3:		//�޸�ģ������
					{
						Clear_RxBuffer();
						break;
					}
					case 4:		//�޸�Ա������
					{
						u8 i;
						if(YGTM_Flag & 0x01)
						{//��ǰ��Ա������
							if(YG_CodeLend == (USART3_RxCounter-5))
							{	//�½��յ�Ա��������ɵ�Ա�����볤����ͬ
								if(Char_Compare(YG_Code,&USART3_RxBuffer[3],YG_CodeLend)==1)
								{//���յ���ͬ�����룬��ΪԱ�������ϸ�		
									YGTM_Flag =0;		//��Ա�������־λ
									USART3_SendWorld=14;	//����Ա���ϸڽ���
								}
								else
								{//���յ���ͬ������
									YGTM_Flag = 0;
									YGTM_Flag |= 0x02;		//��Ա�������־λ
									USART3_SendWorld=14;	//����Ա���ϸڽ���
									for(i=0;i<USART3_RxCounter-5;i++)
									New_YG_Code[i]=USART3_RxBuffer[i+3];
									New_YG_CodeLend=USART3_RxCounter-5;
								}
							}
							else
							{//�½��յ�Ա��������ɵ�Ա�����볤�Ȳ���ͬ������Ϊ���յ�������
								YGTM_Flag =0;
								YGTM_Flag |=02;		//��Ա�������־λ
								USART3_SendWorld=14;	//����Ա���ϸڽ���
								for(i=0;i<USART3_RxCounter-5;i++)
								New_YG_Code[i]=USART3_RxBuffer[i+3];
								New_YG_CodeLend=USART3_RxCounter-5;
							}
						}
						else
						{//��ǰ��Ա�����룬����Ϊ���յ�������
							YGTM_Flag |=0X01;	//��־��ǰ������
							for(i=0;i<USART3_RxCounter-5;i++)
							YG_Code[i]=USART3_RxBuffer[i+3];
							YG_CodeLend=USART3_RxCounter-5;
							USART3_SendWorld=13;	//��������Ա��������
						}
						Clear_RxBuffer();
						break;
					}
					case 5:
					{//����������ϣ�ָʾ����˸
						LED_ON();
						Clear_RxBuffer();
						break;
					}
					case 6:
					{//ָʾ��ֹͣ��˸
						LED_OFF();
						Clear_RxBuffer();
						break;
					}
					case 7:
					{//�޸�ģ�߿�ģ����,:W07|1234
						MJ_Cnt=(USART3_RxBuffer[5]-0x30)*1000+(USART3_RxBuffer[6]-0x30)*100+(USART3_RxBuffer[7]-0x30)*10+USART3_RxBuffer[8]-0x30;	//����������
						YG_Cnt=MJ_Cnt;
						Clear_RxBuffer();
						break;
					}
					default :
					{	//�������
						Send_Error();
						Clear_RxBuffer();
						break;
					}
				}
				break;
			}
			case 'S':
			{	//ϵͳ�������
				switch(tem)
				{
					case 1:	
					{	//ϵͳ�����λ
						NVIC_SystemReset();		//ϵͳ�����λ
						Clear_RxBuffer();
						break;
					}
					case 2:
					{	//��ȡϵͳ��ǰ����ʱ��
						USART3_SendWorld=100;
						Clear_RxBuffer();
						break;
					}
					case 3:
					{//���ڽ�����λ�����ص���Ϣ��������
						Clear_RxBuffer();
						break;
					}
					default:
					{	//����û������
						Send_Error();
						Clear_RxBuffer();
						break;
					}
				}
				break;
			}
			default: 
			{	//�������
// 				Send_Error();
				break;
			}
		}
		Communicate_Computer();
		USART3_Flag &= 0x0F;		//���ʶλ
		USART3_RxCounter=0;			//����
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	//���ж�
	}
}
//ͨѶ��׼Э�鴦��
void Communicate_Computer(void)
{
	//����:|001|W|007|6|J1309050AA|CD2013D00869|CD2013D00870|CD2013D00897|CD2013D00900|CD2013D00903|У����\n
	//����:|001|W|005|XXXX\n��
	//����:|001|DY|R|W|005|XXXX\n
	u8 tem=(USART3_RxBuffer[7]-0x30)*100+(USART3_RxBuffer[8]-0x30)*10+USART3_RxBuffer[9]-0x30;	//����������
	switch(USART3_RxBuffer[5])
	{
		case 'w':
		case 'W':
		{
			switch(tem)
			{
				case 5:
				{//����˸��
					u8 i;
					USART3_TxBuffer[0]='|';
					for(i=0;i<3;i++)
					USART3_TxBuffer[i+1]=ADDRESS[i];
					USART3_TxBuffer[4]='|';
					for(i=0;i<2;i++)
					USART3_TxBuffer[i+5]=MACHINE_TYPE[i];
					USART3_TxBuffer[7]='|';
					USART3_TxBuffer[8]=RUN_STATE;			//��������״̬
					USART3_TxBuffer[9]='|';
					USART3_TxBuffer[10]='W';
					USART3_TxBuffer[11]='|';
					USART3_TxBuffer[12]='0';
					USART3_TxBuffer[13]='0';
					USART3_TxBuffer[14]='5';
					USART3_TxBuffer[15]='|';
					USART3_TxLen=22;
					Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
					USART3_TxBuffer[20]=0x0D;		//������
					USART3_TxBuffer[21]=0x0A;
					USART3_SendDW();
					LED_ON();
					Clear_RxBuffer();
					break;
				}
				case 6:
				{//����˸��
					u8 i;
					USART3_TxBuffer[0]='|';
					for(i=0;i<3;i++)
					USART3_TxBuffer[i+1]=ADDRESS[i];
					USART3_TxBuffer[4]='|';
					for(i=0;i<2;i++)
					USART3_TxBuffer[i+5]=MACHINE_TYPE[i];
					USART3_TxBuffer[7]='|';
					USART3_TxBuffer[8]=RUN_STATE;			//��������״̬
					USART3_TxBuffer[9]='|';
					USART3_TxBuffer[10]='W';
					USART3_TxBuffer[11]='|';
					USART3_TxBuffer[12]='0';
					USART3_TxBuffer[13]='0';
					USART3_TxBuffer[14]='6';
					USART3_TxBuffer[15]='|';
					USART3_TxLen=22;
					Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
					USART3_TxBuffer[20]=0x0D;		//������
					USART3_TxBuffer[21]=0x0A;
					USART3_SendDW();
					LED_OFF();
					Clear_RxBuffer();
					break;
				}
				case 7:			//����JOB��Ϣ
				{
					u8 i,j;

					for(i=0;i<72;i++)
						JOB[i]=USART3_RxBuffer[11+i];

					USART3_TxBuffer[0]='|';
					USART3_TxBuffer[1]='0';
					USART3_TxBuffer[2]='0';
					USART3_TxBuffer[3]='7';
					USART3_TxBuffer[4]='|';

//					for(j=0;j<72;j++)
//						USART3_TxBuffer[5+j] = JOB[j];
//
//					USART3_TxBuffer[77]='|';
//					USART3_TxLen=84;
					USART3_TxLen=11;
					Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
					USART3_TxBuffer[9]=0x0D;		//������
					USART3_TxBuffer[10]=0x0A;
					USART3_SendDW();
//					LED_ON();
					Clear_RxBuffer();


					break;
				}
			}
			break;
		}
	}
}

/*
	�������� void Send_Error(void)
	������	 ����̫�����ʹ���������ʾ
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_Error(void)
{
	USART3_TxBuffer[0]='E';
	USART3_TxBuffer[1]='r';
	USART3_TxBuffer[2]='r';
	USART3_TxBuffer[3]='o';
	USART3_TxBuffer[4]='r';
	USART3_TxBuffer[5]=0x0D;		//������
	USART3_TxBuffer[6]=0x0A;
	USART3_TxLen=7;
	USART3_SendDW();
}

/*
	�������� void Send_Address(void)
	������	 ����̫����������ַ
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_Address(void)
{
	u8 i;
	for(i=0;i<3;i++)
	USART3_TxBuffer[i]=ADDRESS[i];
	USART3_TxBuffer[3]=0x0D;		//������
	USART3_TxBuffer[4]=0x0A;
	USART3_TxLen=5;
	USART3_SendDW();
}

/*
	�������� void Send_MACHINE_TYPE(void)
	������	 ����̫����������������
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_MACHINE_TYPE(void)
{
	u8 i;
	for(i=0;i<2;i++)
	USART3_TxBuffer[i]=MACHINE_TYPE[i];
	USART3_TxBuffer[2]=0x0D;		//������
	USART3_TxBuffer[3]=0x0A;
	USART3_TxLen=4;
	USART3_SendDW();
}

/*
	�������� void SendMJCode(void)
	������	 ����̫��������ģ������
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void SendMJCode(void)
{
	u8 i;
	for(i=0;i<MJ_CodeLend;i++)
	USART3_TxBuffer[i]=MJ_Code[i];
	USART3_TxBuffer[MJ_CodeLend]=0x0D;		//������
	USART3_TxBuffer[MJ_CodeLend+1]=0x0A;
	USART3_TxLen=MJ_CodeLend+2;
	USART3_SendDW();
}

/*
	�������� void SendYGCode(void)
	������	 ����̫��������Ա������
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void SendYGCode(void)
{
	u8 i;
	for(i=0;i<YG_CodeLend;i++)
	USART3_TxBuffer[i]=YG_Code[i];
	USART3_TxBuffer[YG_CodeLend]=0x0D;		//������
	USART3_TxBuffer[YG_CodeLend+1]=0x0A;
	USART3_TxLen=YG_CodeLend+2;
	USART3_SendDW();
}

/*
	�������� void Send_MMYY(void)
	������	 ����̫��������ģ�߼�����ģ�߼�ʱ��Ա���ϸڿ�ģ������Ա���ϸڼ�ʱ
	���룺	 u32 tem��ģ�߼���/ģ�߼�ʱ/Ա������/Ա����ʱ
	�����	 ��
	���أ�	 ��
*/
void Send_MMYY(u32 tem)
{
	Cu32_To_Char(tem, &USART3_TxBuffer[0]);
	USART3_TxBuffer[6]=0x0D;		//������
	USART3_TxBuffer[7]=0x0A;
	USART3_TxLen=8;
	USART3_SendDW();
}

/*
	�������� void Send_ETH(void)
	������	 ����̫���˿ڷ�������
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
u8 Heart_RxCounter=0;
u8 Heart_World=0;
extern u8  Input_Flag;			//������ر�־λ
void Send_ETH()
{
	if((USART3_Flag & 0x01)==0)	//��һ֡�����Ƿ�����ɣ�ֻ����һ֡���ݷ�����ɺ�ſ��Խ�����һ֡����
	{
		if(USART3_SendWorld != 0)
		{
			Communicate_SetOn();
		}
		switch(USART3_SendWorld)	
		{
			u8 i;
			case 0: break;		//0����������
			case 1:			//��ģ��������		
			{
				Send_AMR();		//���ͻ�����š����ͣ���ţ�״̬��ָ����������15������
				Cu8_To_Char(&USART3_TxBuffer[16], YG_CodeLend+MJ_CodeLend+31); //���ݳ���
				USART3_TxBuffer[18]='|';	
				for(i=0;i<MJ_CodeLend;i++)		//ģ������
				USART3_TxBuffer[19+i]=MJ_Code[i];		
 				USART3_TxBuffer[19+i]='|';	
				for(i=0;i<YG_CodeLend;i++)		//Ա������
 				USART3_TxBuffer[20+MJ_CodeLend+i]=YG_Code[i];		
				USART3_TxBuffer[20+MJ_CodeLend+YG_CodeLend]='|';	
				Cu32_To_Char(MJ_Cnt, &USART3_TxBuffer[21+MJ_CodeLend+YG_CodeLend]);	//��ģ����ֵ,6���ֽ�
				USART3_TxBuffer[27+MJ_CodeLend+YG_CodeLend]='|';
				Cu32_To_Char(MJ_Tim,&USART3_TxBuffer[28+MJ_CodeLend+YG_CodeLend]);	//����ģ�߰�װʱ�䣬6���ֽ�
				USART3_TxBuffer[34+MJ_CodeLend+YG_CodeLend]='|';
				Cu32_To_Char(YG_Cnt,&USART3_TxBuffer[35+MJ_CodeLend+YG_CodeLend]);	//Ա���ϸڿ�ģ������6���ֽ�
				USART3_TxBuffer[41+MJ_CodeLend+YG_CodeLend]='|';
				Cu32_To_Char(YG_Tim,&USART3_TxBuffer[42+MJ_CodeLend+YG_CodeLend]);	//Ա���ϸ�ʱ�䣬6���ֽ�
				USART3_TxBuffer[48+MJ_CodeLend+YG_CodeLend]='|';
				Cu32_To_Char(XT_ONTime,&USART3_TxBuffer[49+MJ_CodeLend+YG_CodeLend]);	//ϵͳ�ϵ硢�ϵ���Ϣ
				USART3_TxBuffer[55+MJ_CodeLend+YG_CodeLend]='|';
				USART3_TxLen=YG_CodeLend+MJ_CodeLend+62;		//���㷢�����ݳ���
				Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
				USART3_TxBuffer[USART3_TxLen-2]=0x0D;		//������
				USART3_TxBuffer[USART3_TxLen-1]=0x0A;
				USART3_SendDW();		//��������
				USART3_SendWorld=0;
				break;	
			} 
			case 2:			//����Ա����������
			{
				Send_AMR();			//���ͻ�����š����ͣ���ţ�״̬��ָ����������15������
				Cu8_To_Char(&USART3_TxBuffer[10], YG_CodeLend+1); //���ݳ���
				for(i=0;i<YG_CodeLend;i++)
				USART3_TxBuffer[12+i]=YG_Code[i];		//Ա������
 				USART3_TxBuffer[12+i]=' ';				//�ո��ʾ�ָ���
				USART3_TxLen=19+YG_CodeLend;			//���㷢�����ݳ���
				Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
				USART3_TxBuffer[USART3_TxLen-2]=0x0D;		//������
				USART3_TxBuffer[USART3_TxLen-1]=0x0A;
				USART3_SendDW();		//��������
				USART3_SendWorld=0;
				break;
			}
			case 3:			//����ĥ������	
			{
				Send_AMR();			//���ͻ�����š����ͣ���ţ�״̬��ָ����������15������
				Cu8_To_Char(&USART3_TxBuffer[15], YG_CodeLend+1); //���ݳ��ȣ�2��
				for(i=0;i<MJ_CodeLend;i++)
				USART3_TxBuffer[17+i]=MJ_Code[i];		//Ա������
 				USART3_TxBuffer[17+i]='|';				//�ո��ʾ�ָ���
				USART3_TxLen=19+MJ_CodeLend;			//���㷢�����ݳ���
				Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
				USART3_TxBuffer[USART3_TxLen-2]=0x0D;		//������
				USART3_TxBuffer[USART3_TxLen-1]=0x0A;
				USART3_SendDW();		//��������
				USART3_SendWorld=0;
				break;
			}
			case 4:			//��⵽��¯�򿪻�ر�
			{
				Send_AMR();		//���ͻ�����š����ͣ���ţ�״̬��ָ�����
				Cu8_To_Char(&USART3_TxBuffer[10], YG_CodeLend+MJ_CodeLend+8); //���ݳ���
				
				for(i=0;i<YG_CodeLend;i++)
				USART3_TxBuffer[12+i]=YG_Code[i];		//Ա������
 				USART3_TxBuffer[12+i]='|';				//�ո��ʾ�ָ���
				for(i=0;i<MJ_CodeLend;i++)
				USART3_TxBuffer[13+YG_CodeLend+i]=MJ_Code[i];		//Ա������
     			USART3_TxBuffer[13+YG_CodeLend+i]='|';	//�ָ���
 				Cu32_To_Char(MJ_Cnt,&USART3_TxBuffer[14+YG_CodeLend+i]);	//��ģ����ֵ
				USART3_TxLen=26+YG_CodeLend+MJ_CodeLend;		//���㷢�����ݳ���
				Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
				USART3_TxBuffer[USART3_TxLen-2]=0x0D;		//������
				USART3_TxBuffer[USART3_TxLen-1]=0x0A;
				USART3_SendDW();		//��������
				USART3_SendWorld=0;
				break;	
				
			}
			case 5:			//��⵽�����Դ�򿪻�ر�
			{
				Send_AMR();			//���ͻ�����š����ͣ���ţ�״̬��ָ�����
				USART3_SendWorld=0;
				break;
			}
			case 6:			//���ر�����ǰʱ��
			{
				Send_AMR();			//���ͻ�����š����ͣ���ţ�״̬��ָ�����
				USART3_SendWorld=0;
				break;
			}
			case 7:			
			{//��ȡ��ģ�߱��ƿ�,ͬʱ����������
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='0';
				Send_MJ_Code();	
				MJ_Tim=0;
				MJ_Cnt=0;
				YG_Cnt=0;
				//���ģ����Ϣ
				for(i=0;i<10;i++)
				{
					MJ_Code[i]=0x30;	//��ĥ�������Ϊ10��ASCII��0
				}
				MJ_Code[i] = 0;
				MJ_CodeLend=10;		//ĥ�����볤��
				USART3_SendWorld=0;
				break;
			}
			case 8:			
			{//������ģ����Ϣ��ͬʱ��ʼ���������
				MJ_Tim=0;
				MJ_Cnt=0;
				YG_Cnt=0;
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='1';
				Send_MJ_Code();
				USART3_SendWorld=0;
				break;
			}
			case 9:				//����ϵͳ�ϵ���Ϣ
			{
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='2';
				Send_XTRL_Time();
				USART3_SendWorld=0;
				break;
			}
			case 10:			//����ϵͳ�ϵ���Ϣ
			{
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='3';
				Send_XTRL_Time();
				XT_ONTime=0;
				USART3_SendWorld=0;
				break;
			}
			case 11:			//������¯�ϵ���Ϣ
			{
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='4';
				Send_XTRL_Time();
				USART3_SendWorld=0;
				break;
			}
			case 12:			//������¯�ϵ���Ϣ
			{
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='5';
				Send_XTRL_Time();
				RL_ONTime=0;	//����¯��ʱ
				USART3_SendWorld=0;
				break;
			}
			case 13:			
			{//����Ա���ϸ���Ϣ,ͬʱ��ʼ���������

								
					YG_Cnt=0;		//Ա����ģ����
					YG_Tim=0;		//Ա����ʱ
					USART3_TxBuffer[12]='1';
					USART3_TxBuffer[13]='1';
					USART3_TxBuffer[14]='6';
					Send_RGXX_Time();
					On_Duty_Flag = 0;
					USART3_SendWorld=0;
				break;
			}
			case 14:			
			{//����Ա���ϸڽ�����Ϣ��ͬʱ����������
				
				
					USART3_TxBuffer[12]='1';
					USART3_TxBuffer[13]='1';
					USART3_TxBuffer[14]='7';
					Send_RGXX_Time();
					for(i=0;i<10;i++)	//������
					YG_Code[i]=0x30;
					YG_CodeLend=10;
					YG_Cnt=0;		//Ա����ģ����
					YG_Tim=0;		//Ա����ʱ
					On_Duty_Flag = 0;
					USART3_SendWorld=0;
				break;
			}
			case 15:
			{//�����ϵ�����
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='8';
				Change_TxBuffer();
				Cu32_To_Char(XT_ONTime,&USART3_TxBuffer[16]);	//ϵͳ�ϵ硢�ϵ���Ϣ
				USART3_TxBuffer[22]='|';
				USART3_TxLen=29;		//���㷢�����ݳ���
				Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
				USART3_TxBuffer[USART3_TxLen-2]=0x0D;			//������
				USART3_TxBuffer[USART3_TxLen-1]=0x0A;
				USART3_SendDW();
				USART3_SendWorld=0;
				break;
			}
			case 16:
			{	//����Ա���ϸ���Ϣ
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='9';
				Send_RGXX_Time();
				USART3_SendWorld=0;
				break;
			}
			case 17:
			{	//����ģ�������Ϣ
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='2';
				USART3_TxBuffer[14]='0';
				Send_MJ_Code();	
				USART3_SendWorld=0;
				break;
			}
			case 18:
			{//������
				Send_AMR();		//���ͻ�����š����ͣ���ţ�״̬��ָ����������15������
				USART3_TxBuffer[10]='S';			//ָ����
				Cu8_To_Char(&USART3_TxBuffer[16], YG_CodeLend+MJ_CodeLend+31); //���ݳ���
				USART3_TxBuffer[18]='|';	
				for(i=0;i<MJ_CodeLend;i++)		//ģ������
				USART3_TxBuffer[19+i]=MJ_Code[i];		
 				USART3_TxBuffer[19+i]='|';	
				for(i=0;i<YG_CodeLend;i++)		//Ա������
 				USART3_TxBuffer[20+MJ_CodeLend+i]=YG_Code[i];		
				USART3_TxBuffer[20+MJ_CodeLend+YG_CodeLend]='|';	
				Cu32_To_Char(MJ_Cnt, &USART3_TxBuffer[21+MJ_CodeLend+YG_CodeLend]);	//��ģ����ֵ,6���ֽ�
				USART3_TxBuffer[27+MJ_CodeLend+YG_CodeLend]='|';
				Cu32_To_Char(MJ_Tim,&USART3_TxBuffer[28+MJ_CodeLend+YG_CodeLend]);	//����ģ�߰�װʱ�䣬6���ֽ�
				USART3_TxBuffer[34+MJ_CodeLend+YG_CodeLend]='|';
				Cu32_To_Char(YG_Cnt,&USART3_TxBuffer[35+MJ_CodeLend+YG_CodeLend]);	//Ա���ϸڿ�ģ������6���ֽ�
				USART3_TxBuffer[41+MJ_CodeLend+YG_CodeLend]='|';
				Cu32_To_Char(YG_Tim,&USART3_TxBuffer[42+MJ_CodeLend+YG_CodeLend]);	//Ա���ϸ�ʱ�䣬6���ֽ�
				USART3_TxBuffer[48+MJ_CodeLend+YG_CodeLend]='|';
				Cu32_To_Char(XT_ONTime,&USART3_TxBuffer[49+MJ_CodeLend+YG_CodeLend]);	//ϵͳ�ϵ��ʱ
				USART3_TxBuffer[55+MJ_CodeLend+YG_CodeLend]='|';
				USART3_TxLen=YG_CodeLend+MJ_CodeLend+62;		//���㷢�����ݳ���
				Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
				USART3_TxBuffer[USART3_TxLen-2]=0x0D;		//������
				USART3_TxBuffer[USART3_TxLen-1]=0x0A;
				USART3_SendDW();		//��������
				USART3_SendWorld=0;
				break;
			}
			case 19:			
			{//����ȡ��ģ�߱��ƿ������ģ��������ݲ�����
				break;
			}
			case 20:
			{//ģ�߱��ƿ����ͣ�����Ա���¸���Ϣ,Ȼ����������ģ���ƿ�USART3_SendWorld=7;		
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='2';
				USART3_TxBuffer[14]='1';
				Send_RGXX_Time();
				USART3_SendWorld=7;			
				break;
			}
			case 21:				
			{//��ǰû��ģ�ߣ�����ȡ����һ�ε�ģ�ߣ�����ģ����Ϣ
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='1';
				Send_MJ_Code();
				USART3_SendWorld=0;
				break;
			}
			case 22:
			{//ͨ��������������ϸ�
					YG_Cnt=0;		//Ա����ģ����
					YG_Tim=0;		//Ա����ʱ
					USART3_TxBuffer[12]='2';
					USART3_TxBuffer[13]='0';
					USART3_TxBuffer[14]='1';
					Send_RGXX_Time();
					On_Duty_Flag = 0;
					USART3_SendWorld=0;

					Working_Type[1] = '0';
					Working_Type[0] = '0';
					Working_Process[1] = '0';
					Working_Process[0] = '0';

				break;
			}
			case 23:
			{//ͨ��������������¸�				
					USART3_TxBuffer[12]='2';
					USART3_TxBuffer[13]='0';
					USART3_TxBuffer[14]='2';
					Send_RGXX_Time();
					for(i=0;i<10;i++)	//������
					YG_Code[i]=0x30;
					YG_CodeLend=10;
					YG_Cnt=0;		//Ա����ģ����
					YG_Tim=0;		//Ա����ʱ
					On_Duty_Flag = 0;
					USART3_SendWorld=0;
				break;
			}
			case 24:
			{//����QC����ĺϸ���				
					USART3_TxBuffer[12]='2';
					USART3_TxBuffer[13]='2';
					USART3_TxBuffer[14]='2';

					Send_QC_Input_Data();

					for(i=0;i<10;i++)	//������
						QC_Input_Data[i]=0x30;

//					YG_CodeLend=10;
//					YG_Cnt=0;		//Ա����ģ����        20151028 �ֳ�Ա����Ӧ QC�����ὫԱ����������
//					YG_Tim=0;		//Ա����ʱ                   M3892   099902

					USART3_SendWorld=0;
				break;
			}
			case 25:
			{//������ģȱ��				
					USART3_TxBuffer[12]='2';
					USART3_TxBuffer[13]='2';
					USART3_TxBuffer[14]='3';

					Send_Mould_Defect();

					for(i=0;i<32;i++)	//������
						Mould_Defect[i]=0x30;

					YG_CodeLend=10;
					YG_Cnt=0;		//Ա����ģ����
					YG_Tim=0;		//Ա����ʱ

					USART3_SendWorld=0;
				break;
			}
			case 26:				//�����ֶ�ģʽ
			{
				USART3_TxBuffer[12]='3';
				USART3_TxBuffer[13]='0';
				USART3_TxBuffer[14]='1';
				Send_XTRL_Time();
				USART3_SendWorld=0;
				break;
			}
			case 27:			    //���Ͱ��Զ�ģʽ
			{
				USART3_TxBuffer[12]='3';
				USART3_TxBuffer[13]='0';
				USART3_TxBuffer[14]='2';
				Send_XTRL_Time();
				XT_ONTime=0;
				USART3_SendWorld=0;
				break;
			}
			case 28:			    //�����Զ�ģʽ
			{
				USART3_TxBuffer[12]='3';
				USART3_TxBuffer[13]='0';
				USART3_TxBuffer[14]='3';
				Send_XTRL_Time();
				XT_ONTime=0;
				USART3_SendWorld=0;
				break;
			}

			case 29:			//����ϵͳ�ϵ���Ϣ
			{
				USART3_TxBuffer[12]='3';
				USART3_TxBuffer[13]='0';
				USART3_TxBuffer[14]='4';
				Send_XTRL_Time();
				XT_ONTime=0;
				USART3_SendWorld=0;
				break;
			}
			case 30:
			{//ͨ��������������¸�				
					USART3_TxBuffer[12]='2';
					USART3_TxBuffer[13]='0';
					USART3_TxBuffer[14]='2';
					Send_RGXX_Time();
//					for(i=0;i<10;i++)	//������
//					    YG_Code[i]=0x30;
					YG_CodeLend=10;
					YG_Cnt=0;		//Ա����ģ����
					YG_Tim=0;		//Ա����ʱ
					On_Duty_Flag = 0;
					USART3_SendWorld=0;
				break;
			}
			case 31:
			{//����QC�û���				
					USART3_TxBuffer[12]='2';
					USART3_TxBuffer[13]='2';
					USART3_TxBuffer[14]='5';
					
                    Send_QC_Name();

					for(i=0;i<2;i++)	//������
    					QC_Name[ i ] = 0x30;

					USART3_SendWorld=0;
				break;
			}
            case 32:
			{//����QC����				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '2';
					USART3_TxBuffer[14] = '6';
					
                    Send_QC_PassWord();

//					for(i=0;i<2;i++)	//������
//    					QC_PassWord[ i ] = ' ';
					
					USART3_SendWorld=0;
				break;
			}
            case 33:
			{//��QC����ͳ��-Ƿ��				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '2';
					USART3_TxBuffer[14] = '7';
					
                    Send_QCBad_QianZhu();

					for(i=0;i<4;i++)	//������
    					QCBad_QianZhu[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 34:
			{//��QC����ͳ��-��ģ				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '2';
					USART3_TxBuffer[14] = '8';
					
                    Send_QCBad_LaMo();

					for(i=0;i<4;i++)	//������
    					QCBad_LaMo[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 35:
			{//��QC����ͳ��-ɰ��				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '2';
					USART3_TxBuffer[14] = '9';
					
                    Send_QCBad_ShaKong();

					for(i=0;i<4;i++)	//������
    					QCBad_ShaKong[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 36:
			{//��QC����ͳ��-���				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '0';
					
                    Send_QCBad_LengGe();

					for(i=0;i<4;i++)	//������
    					QCBad_LengGe[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 37:
			{//��QC����ͳ��-����				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '1';
					
                    Send_QCBad_BianXing();

					for(i=0;i<4;i++)	//������
    					QCBad_BianXing[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 38:
			{//��QC����ͳ��-��ȱ				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '2';
					
                    Send_QCBad_BengQue();

					for(i=0;i<4;i++)	//������
    					QCBad_BengQue[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 39:
			{//��QC����ͳ��-����				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '3';
					
                    Send_QCBad_QiPao();

					for(i=0;i<4;i++)	//������
    					QCBad_QiPao[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 40:
			{//��QC����ͳ��-¶��				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '4';
					
                    Send_QCBad_LuGuan();

					for(i=0;i<4;i++)	//������
    					QCBad_LuGuan[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 41:
			{//��QC����ͳ��-��̿				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '5';
					
                    Send_QCBad_JiTan();

					for(i=0;i<4;i++)	//������
    					QCBad_JiTan[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 42:
			{//��QC����ͳ��-�ߴ�				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '6';
					
                    Send_QCBad_ChiChen();

					for(i=0;i<4;i++)	//������
    					QCBad_ChiChen[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 43:
			{//��QC����ͳ��-��ӡ				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '7';
					
                    Send_QCBad_HeiYin();

					for(i=0;i<4;i++)	//������
    					QCBad_HeiYin[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 44:
			{//��QC����ͳ��-����				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '8';
					
                    Send_QCBad_QiTa();

					for(i=0;i<4;i++)	//������
    					QCBad_QiTa[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 45:
			{//��QCԭ�ϳɷַ���				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '9';
					
                    Send_QC_ChengFen();

					for(i=0;i<10;i++)	//������
    					QC_ChengFen[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
			case 46:
			{//����QCע����Ϣ				
					USART3_TxBuffer[12]='2';
					USART3_TxBuffer[13]='4';
					USART3_TxBuffer[14]='0';
					
                    Send_QC_ZhuXiao();

//					for(i=0;i<2;i++)	//������
//    					QC_Name[ i ] = 0x30;

					USART3_SendWorld=0;
				break;
			}


			case 100:
			{
				USART3_TxBuffer[12]='2';
				USART3_TxBuffer[13]='0';
				USART3_TxBuffer[14]='0';
				Send_RGXX_Time();
				USART3_SendWorld=0;
				break;
			}
			default: 
			{
				USART3_SendWorld=0;
				break;
			}
		}
	}
}

/*
	�������� Send_AMR()
	������	 ��������ַ���������͡�����״ָ̬�������뷢�ͻ�����
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_AMR(void)		
{
	u8 i;
	USART3_TxBuffer[0]='|';	
	for(i=0;i<3;i++)
	USART3_TxBuffer[i+1]=ADDRESS[i];			//������ַ
	USART3_TxBuffer[4]='|';	
	for(i=3;i<5;i++)
	USART3_TxBuffer[i+2]=MACHINE_TYPE[i-3];	//��������
	USART3_TxBuffer[7]='|';	
	USART3_TxBuffer[8]=RUN_STATE;			//��������״̬
	USART3_TxBuffer[9]='|';	
	USART3_TxBuffer[10]=Code_Mode;			//ָ����
	USART3_TxBuffer[11]='|';	
	for(i=0;i<3;i++)
	USART3_TxBuffer[12+i]=Command[i];		//����
	USART3_TxBuffer[15]='|';
}



/*
	�������� Receive_ETH()
	������	 ����̫�����յ����ݴ���
	���룺	 ��
	���:	 ��
	���أ�	 ��
*/
void Receive_ETH()
{
	if(USART3_Flag & 0X20)		//�鿴����������
	{
		if(ADDRESS[0]==USART3_RxBuffer[0] && ADDRESS[1]==USART3_RxBuffer[1] \
			&& ADDRESS[2]==USART3_RxBuffer[2] )  //�鿴�Ƿ��Ǳ������
		{
			if(USART3_RxBuffer[USART3_RxCounter-1]==0 && \
				USART3_RxBuffer[USART3_RxCounter-2]==0 && \
				 USART3_RxBuffer[USART3_RxCounter-3]==0 && \
				  USART3_RxBuffer[USART3_RxCounter-4]==0)		//�鿴У��λ�Ƿ�Ϊ0
			{
				
			}
			else 
			{
				
			}
		}
	}
}


void Send_MJCode()
{
	u8 i;
	USART3_TxBuffer[0]='|';
	u16_char(Address,&USART3_TxBuffer[1]);		//��ַ
	USART3_TxBuffer[4]='|';
	USART3_TxBuffer[5]='A';
	USART3_TxBuffer[6]='|';
	USART3_TxBuffer[7]='1';
	USART3_TxBuffer[8]='0';
	USART3_TxBuffer[9]='3';
	USART3_TxBuffer[10]='|';
	USART3_TxBuffer[11]=(MJ_CodeLend%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[12]=MJ_CodeLend%10+48;		//���ݳ��ȣ���λ��
	USART3_TxBuffer[13]='|';
	for(i=0;i<MJ_CodeLend;i++)
	USART3_TxBuffer[14+i]=MJ_Code[i];		//ĥ������
	USART3_TxBuffer[14+MJ_CodeLend]='|';
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[15+MJ_CodeLend], 15+MJ_CodeLend);	//����У����
	USART3_TxBuffer[19+MJ_CodeLend]=0x0D;		//������
	USART3_TxBuffer[20+MJ_CodeLend]=0x0A;
	USART3_TxLen=21+MJ_CodeLend;
	USART3_SendDW();
}

/*
	�������ƣ� Send_MJ_Over()
	������	   ģ�߱��𿪣����ͼ�ʱ������111
	���룺	   ��
	�����	   ��
	���أ�	   ��
*/
void Send_MJ_Code()
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((MJ_CodeLend+16)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(MJ_CodeLend+16)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';
	for(i=0;i<MJ_CodeLend;i++)
	USART3_TxBuffer[19+i]=MJ_Code[i];		//ģ������
	USART3_TxBuffer[19+MJ_CodeLend]='|';	//�ָ���
	Cu32_To_Char(MJ_Cnt,&USART3_TxBuffer[20+MJ_CodeLend]);	//��ģ����ֵ
	USART3_TxBuffer[26+MJ_CodeLend]='|';
	Cu32_To_Char(MJ_Tim,&USART3_TxBuffer[27+MJ_CodeLend]);	//��ģ����ֵ
	USART3_TxBuffer[33+MJ_CodeLend]='|';
	USART3_TxLen=40+MJ_CodeLend;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();		//��������
}

/*
	�������� Send_XTRL_Time()
	������	 ����ϵͳ�ϵ���Ϣ���ϵ���Ϣ����¯�ϵ���Ϣ����¯�ϵ���Ϣ
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_XTRL_Time(void)
{
	Change_TxBuffer();
	if((USART3_SendWorld==9) || (USART3_SendWorld==10))
	Cu32_To_Char(XT_ONTime,&USART3_TxBuffer[16]);	//ϵͳ�ϵ硢�ϵ���Ϣ
	else
	Cu32_To_Char(RL_ONTime,&USART3_TxBuffer[16]);	//��¯�ϵ硢�ϵ���Ϣ
	USART3_TxBuffer[22]='|';
	USART3_TxLen=29;		//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;			//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();	
}

/*
	�������� Send_RGXX_Time(void)
	������	 ����Ա���ϸڣ��ϸڽ�����Ϣ
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_RGXX_Time(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+22)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+22)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';
	for(i=0;i<YG_CodeLend;i++)
		USART3_TxBuffer[19+i]=YG_Code[i];		//Ա������
	
	USART3_TxBuffer[19+YG_CodeLend]='|';	//�ָ���
	Cu32_To_Char(YG_Cnt,&USART3_TxBuffer[20+YG_CodeLend]);	//��Ա����ģ����ֵ
	USART3_TxBuffer[26+YG_CodeLend]='|';
	Cu32_To_Char(YG_Tim,&USART3_TxBuffer[27+YG_CodeLend]);	//��Ա���ϸ�ʱ��
	USART3_TxBuffer[33+YG_CodeLend]='|';
	USART3_TxBuffer[34+YG_CodeLend]= Working_Type[1];			//���͹��֣�ͨ��������
	USART3_TxBuffer[35+YG_CodeLend]= Working_Type[0];
	USART3_TxBuffer[36+YG_CodeLend]= '|';
	USART3_TxBuffer[37+YG_CodeLend]= Working_Process[1];	//���͹���
	USART3_TxBuffer[38+YG_CodeLend]= Working_Process[0];
	USART3_TxBuffer[39+YG_CodeLend]= '|';	
	USART3_TxLen=46+YG_CodeLend;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}
/*
	�������� Send_QC_Input_Data(void)
	������	 ����QC����ĺϸ���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QC_Input_Data(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+22)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+22)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend;i++)
		USART3_TxBuffer[19+i]=YG_Code[i];		//Ա������

	USART3_TxBuffer[19+YG_CodeLend]='|';

	for(i=0;i<YG_CodeLend;i++)
		USART3_TxBuffer[30+i]=QC_Input_Data[i];		//QC���������
	
	USART3_TxBuffer[40]='|';	//�ָ���
//	Cu32_To_Char(YG_Cnt,&USART3_TxBuffer[20+YG_CodeLend]);	//��Ա����ģ����ֵ
//	USART3_TxBuffer[26+YG_CodeLend]='|';
//	Cu32_To_Char(YG_Tim,&USART3_TxBuffer[27+YG_CodeLend]);	//��Ա���ϸ�ʱ��
//	USART3_TxBuffer[33+YG_CodeLend]='|';
//	USART3_TxBuffer[34+YG_CodeLend]= '0';			//���͹��֣�ͨ��������
//	USART3_TxBuffer[35+YG_CodeLend]= '0';
//	USART3_TxBuffer[36+YG_CodeLend]= '|';
//	USART3_TxBuffer[37+YG_CodeLend]= '0';	//���͹���
//	USART3_TxBuffer[38+YG_CodeLend]= '0';
//	USART3_TxBuffer[39+YG_CodeLend]= '|';	
	USART3_TxLen=47;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}

/*
	�������� Send_Mould_Defect(void)
	������	 ����ģ��ȱ��
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_Mould_Defect(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+22)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+22)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';
	for(i=0;i<YG_CodeLend+22;i++)
		USART3_TxBuffer[19+i]=Mould_Defect[i];		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend+22]='|';	//�ָ���
//	Cu32_To_Char(YG_Cnt,&USART3_TxBuffer[20+YG_CodeLend]);	//��Ա����ģ����ֵ
//	USART3_TxBuffer[26+YG_CodeLend]='|';
//	Cu32_To_Char(YG_Tim,&USART3_TxBuffer[27+YG_CodeLend]);	//��Ա���ϸ�ʱ��
//	USART3_TxBuffer[33+YG_CodeLend]='|';
//	USART3_TxBuffer[34+YG_CodeLend]= '0';			//���͹��֣�ͨ��������
//	USART3_TxBuffer[35+YG_CodeLend]= '0';
//	USART3_TxBuffer[36+YG_CodeLend]= '|';
//	USART3_TxBuffer[37+YG_CodeLend]= '0';	//���͹���
//	USART3_TxBuffer[38+YG_CodeLend]= '0';
//	USART3_TxBuffer[39+YG_CodeLend]= '|';	
	USART3_TxLen=26+YG_CodeLend+22;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}
/*
	�������� Send_QC_Name(void)
	������	 ����QC�û���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QC_Name(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-8;i++)
		USART3_TxBuffer[19+i] = QC_Name[i] ;		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend-8]='|';	//�ָ���
	USART3_TxLen=26+YG_CodeLend-8;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}
/*
	�������� Send_QC_PassWord(void)
	������	 ����QC�û���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QC_PassWord(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-8;i++)
		USART3_TxBuffer[19+i] = QC_PassWord[i] ;		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend-8]='|';	//�ָ���

	USART3_TxLen=26+YG_CodeLend-8;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}
/*
	�������� Send_QCBad_QianZhu(void)
	������	 ����QC�û���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QCBad_QianZhu(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_QianZhu[i];		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//�ָ���

	USART3_TxLen=26+YG_CodeLend-6;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}
/*
	�������� Send_QCBad_LaMo(void)
	������	 ����QC�û���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QCBad_LaMo(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_LaMo[i];		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//�ָ���

	USART3_TxLen=26+YG_CodeLend-6;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}
/*
	�������� Send_QCBad_ShaKong(void)
	������	 ����QC�û���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QCBad_ShaKong(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_ShaKong[i];		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//�ָ���

	USART3_TxLen=26+YG_CodeLend-6;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}
/*
	�������� Send_QCBad_LengGe(void)
	������	 ����QC�û���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QCBad_LengGe(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_LengGe[i];		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//�ָ���

	USART3_TxLen=26+YG_CodeLend-6;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}
/*
	�������� Send_QCBad_BianXing(void)
	������	 ����QC�û���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QCBad_BianXing(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_BianXing[i];		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//�ָ���

	USART3_TxLen=26+YG_CodeLend-6;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}

/*
	�������� Send_QCBad_BengQue(void)
	������	 ����QC�û���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QCBad_BengQue(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_BengQue[i];		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//�ָ���

	USART3_TxLen=26+YG_CodeLend-6;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}

/*
	�������� Send_QCBad_QiPao(void)
	������	 ����QC�û���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QCBad_QiPao(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_QiPao[i];		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//�ָ���

	USART3_TxLen=26+YG_CodeLend-6;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}

/*
	�������� Send_QCBad_LuGuan(void)
	������	 ����QC�û���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QCBad_LuGuan(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_LuGuan[i];		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//�ָ���

	USART3_TxLen=26+YG_CodeLend-6;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}

/*
	�������� Send_QCBad_JiTan(void)
	������	 ����QC�û���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QCBad_JiTan(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_JiTan[i];		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//�ָ���

	USART3_TxLen=26+YG_CodeLend-6;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}

/*
	�������� Send_QCBad_ChiChen(void)
	������	 ����QC�û���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QCBad_ChiChen(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_ChiChen[i];		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//�ָ���

	USART3_TxLen=26+YG_CodeLend-6;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}

/*
	�������� Send_QCBad_HeiYin(void)
	������	 ����QC�û���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QCBad_HeiYin(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_HeiYin[i];		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//�ָ���

	USART3_TxLen=26+YG_CodeLend-6;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}

/*
	�������� Send_QCBad_QiTa(void)
	������	 ����QC�û���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QCBad_QiTa(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_QiTa[i];		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//�ָ���

	USART3_TxLen=26+YG_CodeLend-6;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}
/*
	�������� Send_QC_ChengFen(void)
	������	 ����QC�û���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QC_ChengFen(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend;i++)
		USART3_TxBuffer[19+i] = QC_ChengFen[i];		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend]='|';	//�ָ���

	USART3_TxLen=26+YG_CodeLend;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}
/*
	�������� Send_QC_ZhuXiao(void)
	������	 ����QC�û���
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Send_QC_ZhuXiao(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend;i++)
		USART3_TxBuffer[19+i] = '1';		//QC���������
	
	USART3_TxBuffer[19+YG_CodeLend]='|';	//�ָ���

	USART3_TxLen=26+YG_CodeLend;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//��������
}
/*
	�������� Change_TxBuffer()
	������	 ���ķ��ͻ�������ĵ�ַ���������ͣ�����״̬��ָ����
	���룺	 ��
	�����	 ��
	����	 ��
*/
void Change_TxBuffer(void)
{
	u8 i;
	USART3_TxBuffer[0]='|';	
	for(i=0;i<3;i++)
	USART3_TxBuffer[i+1]=ADDRESS[i];			//������ַ
	USART3_TxBuffer[4]='|';	
	for(i=3;i<5;i++)
	USART3_TxBuffer[i+2]=MACHINE_TYPE[i-3];	//��������
	USART3_TxBuffer[7]='|';	
	USART3_TxBuffer[8]=RUN_STATE;			//��������״̬
	USART3_TxBuffer[9]='|';	
	USART3_TxBuffer[10]=Code_Mode;			//ָ����
	USART3_TxBuffer[11]='|';	
	USART3_TxBuffer[15]='|';
}

void Send_MoJuYiKai(void)
{
	u8 i,len;
	USART3_TxBuffer[12]='1';
	USART3_TxBuffer[13]='1';
	USART3_TxBuffer[14]='0';
	Change_TxBuffer();
	USART3_TxBuffer[16]=((MJ_CodeLend+16)%100)/10+48;	//���ݳ��ȣ�ʮλ��
	USART3_TxBuffer[17]=(MJ_CodeLend+16)%10+48;			//���ݳ��ȣ���λ��
	USART3_TxBuffer[18]='|';
	for(i=0;i<MJ_CodeLend;i++)
	USART3_TxBuffer[19+i]=MJ_Code[i];		//ģ������
	USART3_TxBuffer[19+MJ_CodeLend]='|';	//�ָ���
	Cu32_To_Char(MJ_Cnt,&USART3_TxBuffer[20+MJ_CodeLend]);	//��ģ����ֵ
	USART3_TxBuffer[26+MJ_CodeLend]='|';
	Cu32_To_Char(MJ_Tim,&USART3_TxBuffer[27+MJ_CodeLend]);	//��ģ����ֵ
	USART3_TxBuffer[33+MJ_CodeLend]='|';
	USART3_TxLen=40+MJ_CodeLend;			//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[len-6], len-6);	//����У����
	USART3_TxBuffer[len-2]=0x0D;	//������
	USART3_TxBuffer[len-1]=0x0A;
	USART3_SendDW();
}

/*
	�������� void Reset_Msg(void)
	������	 ����������ʾ
*/
void Reset_Mag(void)
{
	Send_AMR();		//���ͻ�����š����ͣ���ţ�״̬��ָ����������15������
	USART3_TxBuffer[10]='S';
	USART3_TxBuffer[12]='0';
	USART3_TxBuffer[13]='0';
	USART3_TxBuffer[14]='0';
	USART3_TxLen=22;		//���㷢�����ݳ���
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//����У����
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;		//������
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();		//��������*/
}

void Clear_RxBuffer(void)
{
	u8 i;
	for (i=0;i<42;i++)
		USART3_RxBuffer[i] = 0;
}
