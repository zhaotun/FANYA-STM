#include "serial5.h"

extern u8 USART3_SendWorld;
extern u8 YG_Code[];
extern u8 UART4_RxBuffer[UART4_RxBufferSizeMax];	
extern u8 UART4_Flag;
extern u8 Work_Status_Flag;
extern u8 MJ_CodeLend;

extern u8 YUANGONG;
extern u8 YGTM_Flag;	//Ա�������־λ
extern u8 UHRFID_Flag;

extern  u8  JOB[72];
extern  u8  MJ_Code[];

extern  u8 Old_New_Flag;


u8 Working_Type[2] = "00";
u8 Working_Process[2] = "00";
u8 On_Duty_Flag = 0;

u8 Work_Status_Flag;
u8 Online_Status;

u8 UHRFID_Manual_Input_Flag;

u8 QC_Input_Data[10] = "0000000000";
u8 Mould_Defect[32]  = "00000000000000000000000000000000";

u8 QC_Name[2] = "00";
u8 QC_PassWord[2] = "00";

u8 QCBad_QianZhu[4]  = "0000";
u8 QCBad_LaMo[4]     = "0000";
u8 QCBad_ShaKong[4]  = "0000";
u8 QCBad_LengGe[4]   = "0000";
u8 QCBad_BianXing[4] = "0000";
u8 QCBad_BengQue[4]  = "0000";
u8 QCBad_QiPao[4]    = "0000";
u8 QCBad_LuGuan[4]   = "0000";
u8 QCBad_JiTan[4]    = "0000";
u8 QCBad_ChiChen[4]  = "0000";
u8 QCBad_HeiYin[4]   = "0000";
u8 QCBad_QiTa[4]     = "0000";
u8 QC_ChengFen[10]   = "0000000000";


/*
	��������void Init_Serial(void)
	������	���п�1��ʼ��
	���룺	��
	�����	��
	���أ�	��
*/
void Init_Serial5(void)	//u8 x)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	//USART1�˿�����
	//����USART1 TX(PC.12)Ϊ���ù����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//����USART1 RX(PD.2)Ϊ��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//����USART1���п�
	USART_InitStructure.USART_BaudRate = USART5_BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);
	//ʹ�ܷ����жϣ����жϲ���ʱ��USART1�������ݼĴ���Ϊ��
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	//ʹ�ܽ����жϣ����жϲ���ʱ��USART1�������ݼĴ�����Ϊ��
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	//ʹ��USART1
	USART_Cmd(UART5, ENABLE);
	//ʹ��USART1ȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannel=UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

struct USART_Struct
{
	u8	TxBuffer[200];		//���ͻ�����
	u8 	RxBuffer[100];		//���ջ�����
	u8	TxCount;			//���ͼ�����
	u8	TxLen;				//���ͳ���
	u8	TxFlag;				//���ͱ�־��
	u8	RxCount;			//���ռ�����
	u8	RxTimer;			//���ռ�ʱ
	u8	RxFlag;				//���ձ�־��
};

struct USART_Struct Serial5;	//����5���������ݻ�����
	u8	Send_Flag;	//����5�������������
	u8	RUN_WORD;	//���в�����
	u8 	UAT5_FLAG=0;
	u8 i;
/*
	�������� void USART1_Send(void)
	������	 ����1����һ֡���ݣ�USART1�жϳ����ã����ݷ��뷢�ͻ�
			 ����USART1_TxBuffer[50]��,�����ݳ���д��USART1_TxLen
			 ����USART1���������жϼ���(USART_ITConfig(USART1, USART_IT_TXE, ENABLE);)
	���룺	 USART1_TxBuffer[50]��USART1_TxLen
	�����	 ��
	���أ�	 ��
*/
void USART5_Send(void)
{
	USART_SendData(UART5, Serial5.TxBuffer[Serial5.TxCount++]);
	if(Serial5.TxCount==Serial5.TxLen)
	{//�����������
		USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
		Serial5.TxCount=0;		
		Serial5.TxLen=0;
	}
}

/*
	�������� void USART1_Receive(void)
	������	 ���ڽ���һ֡���ݣ�USART1�жϳ����ã�����һ֡����������
			 ���յ������ݷ��ڽ��ջ�����USART1_RxBuffer[230]�У�����
			 �ŵ�USART1_RxCount��
*/
void USART5_Receive(u8 buf)
{
 	Serial5.RxTimer=0;
	Serial5.RxBuffer[Serial5.RxCount++] = buf;
	if(Serial5.RxCount > RX5_LEN) 
		Serial5.RxCount=0;
 	Serial5.RxFlag |= 0x01;

	
	//if(Send_Flag & 0X10) USART_SendData(UART5,USART_ReceiveData(UART5));	//�ѽ��յ��������򴮿�5����
}

extern u8 UART4_RxCounter;	//����4���ռ�����
extern u8 UART4_RxBuffer[UART4_RxBufferSizeMax];		//����4���ջ�����
extern u8 UART4_Flag ;
//���������ݺ���
void USART5_RxDW(void)
{
	if(Serial5.RxFlag & 0x01) 
	{
		Serial5.RxTimer++;
		if(Serial5.RxTimer>=RX5_TIMER)
		{//������ɴ���
			USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);			
			Serial5.RxFlag &= 0xFE;		//�忪ʼ����λ
			Serial5.RxTimer = 0;
			if(Serial5.RxBuffer[1] == 0x03) 
			{
				HMI_Touch_Data();
			}
			else if(Serial5.RxBuffer[1]==0x05)
			{
				Serial5_DW(Serial5.RxBuffer[8]);
			}
			else if(Serial5.RxBuffer[1]==0x10)
			{
				 switch( Serial5.RxBuffer[8] )
                 {
                    case 0x1C://��QC����ĺϸ���
                    {
    					for(i=0;i<10;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
    							QC_Input_Data[i] = ' ';
    						else
    							QC_Input_Data[i] = Serial5.RxBuffer[i+9];
    					 }

    					 USART3_SendWorld = 24;
    					 Work_Status_Flag = 0;

                         break;
                     }
                    case 0x1F://��QCԭ�Ϸ����ɷ�
                    {
    					for(i=0;i<10;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
    							QC_ChengFen[i] = '0';
    						else if(Serial5.RxBuffer[i+9] == 0x01)
    							QC_ChengFen[i] = '1';
    					}

    					USART3_SendWorld = 45;
    
                         break;
                     }
                    case 0x6E://����ģȱ��
                    {
    					for(i=0;i<32;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
    							Mould_Defect[i] = '0';
    						else if(Serial5.RxBuffer[i+9] == 0x01)
    							Mould_Defect[i] = '1';
    					}

    					USART3_SendWorld = 25;
    					Work_Status_Flag = 0;
    
    					UHRFID_Manual_Input_Flag = 0;
    					printf("��ģȱ�ݷ������\r\n");

                         break;
                     }
                    case 0xA1://���ֶ������Ա������,Ҳ���ɴ򿨴���
                    {
    					for(i=0;i<10;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
//    							YG_Code[i] = ' ';
                                UART4_RxBuffer[i] = ' ';
    						else
//    							YG_Code[i] = Serial5.RxBuffer[i+9];
                                UART4_RxBuffer[i] = Serial5.RxBuffer[i+9];
    					}

                        UART4_RxCounter = 12;
                        UART4_Flag |= b6;
                        UART4_ReceiveDW();

//    				    YGTM_Flag = 1;
//    					Work_Status_Flag = 0x0A; 

                         break;
                     }
                    case 0xA0://���ֶ������ģ������
                    {
    					for(i=0;i<16;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
    							MJ_Code[i] = ' ';
    						else
    							MJ_Code[i] = Serial5.RxBuffer[i+9];
    					}
    					MJ_CodeLend = 16;
    					USART3_SendWorld = 8; 					
    					UHRFID_Flag = b1;
    				
    					UHRFID_Manual_Input_Flag = 1;
    					printf("�ֶ�������ģ������\r\n");

                         break;
                     }
                    case 0x6F://��QC��¼���û���
                    {
                        QC_Name[0] = Serial5.RxBuffer[9] / 10 + 48;
                        QC_Name[1] = Serial5.RxBuffer[9] % 10 + 48;
                        
    					USART3_SendWorld = 31; 					
    				
                        break;    
                     }
                    case 0x70://��QC��¼����
                    {
                        QC_PassWord[0] = Serial5.RxBuffer[9] / 10 + 48;
                        QC_PassWord[1] = Serial5.RxBuffer[9] % 10 + 48;

    					USART3_SendWorld = 32; 					
    				
                        break;    
                     }
                    case 0x66://��QC����ͳ��-Ƿ��
                    {
    					for(i=0;i<4;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
    						    QCBad_QianZhu[i] = ' ';
    						else
    							QCBad_QianZhu[i] = Serial5.RxBuffer[i+9];
    					}

    					USART3_SendWorld = 33; 					
    				
                        break;    
                     }
                    case 0x67://��QC����ͳ��-��ģ
                    {
    					for(i=0;i<4;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
    						    QCBad_LaMo[i] = ' ';
    						else
    							QCBad_LaMo[i] = Serial5.RxBuffer[i+9];
    					}

    					USART3_SendWorld = 34; 					
    				
                        break;    
                     }
                    case 0x68://��QC����ͳ��-ɰ��
                    {
    					for(i=0;i<4;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
    						    QCBad_ShaKong[i] = ' ';
    						else
    							QCBad_ShaKong[i] = Serial5.RxBuffer[i+9];
    					}

    					USART3_SendWorld = 35; 					
    				
                        break;    
                     }
                    case 0x69://��QC����ͳ��-���
                    {
    					for(i=0;i<4;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
    						    QCBad_LengGe[i] = ' ';
    						else
    							QCBad_LengGe[i] = Serial5.RxBuffer[i+9];
    					}

    					USART3_SendWorld = 36; 					
    				
                        break;    
                     }
                    case 0x6A://��QC����ͳ��-����
                    {
    					for(i=0;i<4;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
    						    QCBad_BianXing[i] = ' ';
    						else
    							QCBad_BianXing[i] = Serial5.RxBuffer[i+9];
    					}

    					USART3_SendWorld = 37; 					
    				
                        break;    
                     }
                    case 0x6B://��QC����ͳ��-��ȱ
                    {
    					for(i=0;i<4;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
    						    QCBad_BengQue[i] = ' ';
    						else
    							QCBad_BengQue[i] = Serial5.RxBuffer[i+9];
    					}

    					USART3_SendWorld = 38; 					
    				
                        break;    
                     }
                    case 0x6C://��QC����ͳ��-����
                    {
    					for(i=0;i<4;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
    						    QCBad_QiPao[i] = ' ';
    						else
    							QCBad_QiPao[i] = Serial5.RxBuffer[i+9];
    					}

    					USART3_SendWorld = 39; 					
    				
                        break;    
                     }
                    case 0x6D://��QC����ͳ��-¶��
                    {
    					for(i=0;i<4;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
    						    QCBad_LuGuan[i] = ' ';
    						else
    							QCBad_LuGuan[i] = Serial5.RxBuffer[i+9];
    					}

    					USART3_SendWorld = 40; 					
    				
                        break;    
                     }
                    case 0x71://��QC����ͳ��-��̿
                    {
    					for(i=0;i<4;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
    						    QCBad_JiTan[i] = ' ';
    						else
    							QCBad_JiTan[i] = Serial5.RxBuffer[i+9];
    					}

    					USART3_SendWorld = 41; 					
    				
                        break;    
                     }
                    case 0x72://��QC����ͳ��-�ߴ�
                    {
    					for(i=0;i<4;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
    						    QCBad_ChiChen[i] = ' ';
    						else
    							QCBad_ChiChen[i] = Serial5.RxBuffer[i+9];
    					}

    					USART3_SendWorld = 42; 					
    				
                        break;    
                     }
                    case 0x73://��QC����ͳ��-��ӡ
                    {
    					for(i=0;i<4;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
    						    QCBad_HeiYin[i] = ' ';
    						else
    							QCBad_HeiYin[i] = Serial5.RxBuffer[i+9];
    					}

    					USART3_SendWorld = 43; 					
    				
                        break;    
                     }
                    case 0x74://��QC����ͳ��-����
                    {
    					for(i=0;i<4;i++)
    					{
    						if(Serial5.RxBuffer[i+9] == 0)
    						    QCBad_QiTa[i] = ' ';
    						else
    							QCBad_QiTa[i] = Serial5.RxBuffer[i+9];
    					}

    					USART3_SendWorld = 44; 					
    				
                        break;    
                     }

					 default:
						break;
                  }
          	}
			Serial5.RxCount=0;
			USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
		}
	}
}

/*
	����5���������
*/
void Serial5_DW(u8 command)
{
//	u8 i;
	
	switch(command)
	{
		case 0x01:
		{//ơ��ѡ���ϸ�
            Online_Status = 0x14;

			break;

		}
		case 0x02:
		{//QCѡ���ϸ�
			Working_Type[1] = '0';
			Working_Type[0] = '3';
			
			Working_Process[1] = '0';
			Working_Process[0] = '1';
			USART3_SendWorld = 22;
			Online_Status = 0;
//            Work_Status_Flag = 0;

			break;
		}
		case 0x03:
		{//��������Աѡ���ϸ�
            Online_Status = 0x15;

			break;
		}
		case 0x0B://11
		{//ơ���ϸ�ѡ��ơ��
			Working_Type[1] = '0';
			Working_Type[0] = '1';
			Working_Process[1] = '0';
			Working_Process[0] = '1';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;
//			USART3_SendWorld = 13;
			
			break;
		}
		case 0x0C://12
		{//ơ���ϸ�ѡ��ơ��
			Working_Type[1] = '0';
			Working_Type[0] = '1';
			Working_Process[1] = '0';
			Working_Process[0] = '2';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;
			break;
		}
        case 0x0D://13
		{//ơ���ϸ�ѡ������ģ
			Working_Type[1] = '0';
			Working_Type[0] = '1';
			Working_Process[1] = '0';
			Working_Process[0] = '3';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;
			break;
		}
         case 0x1D://29  
		{//QC-ע��
		
        	USART3_SendWorld = 46;
            Work_Status_Flag = 0;
			break;
		}

        case 0x1E://30  QCfanhui
		{//QC-����
			Working_Type[1] = '0';
			Working_Type[0] = '3';
			Working_Process[1] = '0';
			Working_Process[0] = '2';
			USART3_SendWorld = 23;
			Work_Status_Flag = 0;

            YGTM_Flag &= ~b1;

			break;
		}
		case 0x28:
		{//	��������Ա�ϸ�-��ģ



			break;
		}
    	case 0x29:
		{//��������Ա�ϸ�-��ģ
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '2';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;

			break;
		}
		case 0x2A:
		{//��������Ա�ϸ�-������
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '3';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;


			break;
		}
		case 0x2B:
		{//��������Ա�ϸ�-�޻�
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '4';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;

			
			break;
		}
		case 0x2C:
		{//��������Ա�ϸ�-ơ��
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '7';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;
			
			break;
		}
		case 0x2D:
		{//��������Ա�ϸ�-�����
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '8';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;


			break;
		}
		case 0x2E:
		{//	��������Ա�ϸ�-����������޸�
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '9';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;

	
			break;
		}
        case 0x2F:
		{//��������Ա�ϸ�-����
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '5';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;
			
			break;
		}
		case 0x30:
		{//��������Աѡ���ϸڷ���
			break;
		}
		case 0x3C:
		{//����ǰ��ģ

			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '1';
			Working_Process[0] = '6';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;

			break;
		}
		case 0x3D:
		{//����ǰFEP

			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '2';
			Working_Process[0] = '6';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;

			break;
		}
		case 0x3E:
		{//������ơ��

			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '3';
			Working_Process[0] = '6';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;

			break;
		}
		case 0x78:
		{//ơ���¸�-�°�
			Working_Type[1] = '0';
			Working_Type[0] = '1';
			Working_Process[1] = '0';
			Working_Process[0] = '1';
            if( Old_New_Flag == 1)
            {
                 Old_New_Flag = 0;
                 USART3_SendWorld = 30;
                 YGTM_Flag |= b2;
//                 Work_Status_Flag = 0x0A;
            }
            else
            {
                USART3_SendWorld = 23;            
            }
			Work_Status_Flag = 0;
			break;
		}
		case 0x79:
		{//ơ���¸�-����
			Working_Type[1] = '0';
			Working_Type[0] = '1';
			Working_Process[1] = '0';
			Working_Process[0] = '2';			
            if( Old_New_Flag == 1)
            {
                 Old_New_Flag = 0;
                 USART3_SendWorld = 30;
                 YGTM_Flag |= b2;
//                 Work_Status_Flag = 0x0A;
            }
            else
            {
                USART3_SendWorld = 23;            
            }
			Work_Status_Flag = 0;
			break;
		}		
		case 0x7A:
		{//ơ���¸�-�쳣
			Working_Type[1] = '0';
			Working_Type[0] = '1';
			Working_Process[1] = '0';
			Working_Process[0] = '3';
            if( Old_New_Flag == 1)
            {
                 Old_New_Flag = 0;
                 USART3_SendWorld = 30;
                 YGTM_Flag |= b2;
//                 Work_Status_Flag = 0x0A;
            }
            else
            {
                USART3_SendWorld = 23;            
            }
			Work_Status_Flag = 0;
			break;
		}
		case 0x7B:
		{//ơ���¸�-����
			Working_Type[1] = '0';
			Working_Type[0] = '1';
			Working_Process[1] = '0';
			Working_Process[0] = '4';
            if( Old_New_Flag == 1)
            {
                 Old_New_Flag = 0;
                 USART3_SendWorld = 30;
                 YGTM_Flag |= b2;
//                 Work_Status_Flag = 0x0A;
            }
            else
            {
                USART3_SendWorld = 23;            
            }
			Work_Status_Flag = 0;
			break;
		}
		case 0x82:
		{//��������Ա�¸�-�������
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '1';
            if( Old_New_Flag == 1)
            {
                 Old_New_Flag = 0;
                 USART3_SendWorld = 30;
                 YGTM_Flag |= b2;
//                 Work_Status_Flag = 0x0A;
            }
            else
            {
                USART3_SendWorld = 23;            
            }
			Work_Status_Flag = 0;
			break;
		}		
		case 0x83:
		{//��������Ա�¸�-�Զ�ơ
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '4';
            if( Old_New_Flag == 1)
            {
                 Old_New_Flag = 0;
                 USART3_SendWorld = 30;
                 YGTM_Flag |= b2;
//                 Work_Status_Flag = 0x0A;
            }
            else
            {
                USART3_SendWorld = 23;            
            }
			Work_Status_Flag = 0;
			break;
		}
    	case 0x84:
		{//��������Ա�¸�-����ѡ��

//			On_Duty_Flag = 0x02;
			break;
		}
		case 0x85:
		{//��������Ա�¸�-ǩ��
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '2';
            if( Old_New_Flag == 1)
            {
                 Old_New_Flag = 0;
                 USART3_SendWorld = 30;
//                 YGTM_Flag |= b2;
                 Work_Status_Flag = 0x0A;
            }
            else
            {
                USART3_SendWorld = 23;            
            }
			Work_Status_Flag = 0;
			break;
		}		
		case 0x86:
		{//��������Ա�¸�-�쳣������ͣ��
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '3';
            if( Old_New_Flag == 1)
            {
                 Old_New_Flag = 0;
                 USART3_SendWorld = 30;
                 YGTM_Flag |= b2;
//                 Work_Status_Flag = 0x0A;
            }
            else
            {
                USART3_SendWorld = 23;            
            }
			Work_Status_Flag = 0;
			break;
		}
		case 0x8C:
		{//	JOBNO-01
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '1';
			Working_Process[0] = '5';
			USART3_SendWorld = 23;
			Work_Status_Flag = 0;

			for(i=0;i<72;i++)
			{
				JOB[i] = 0;					
			}

			break;
		}
		case 0x8D:
		{//	JOBNO-02
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '2';
			Working_Process[0] = '5';
			USART3_SendWorld = 23;
			Work_Status_Flag = 0;

			for(i=0;i<72;i++)
			{
				JOB[i] = 0;					
			}

			break;
		}
		case 0x8E:
		{//	JOBNO-03
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '3';
			Working_Process[0] = '5';
			USART3_SendWorld = 23;
			Work_Status_Flag = 0;

			for(i=0;i<72;i++)
			{
				JOB[i] = 0;					
			}

			break;
		}
		case 0x8F:
		{//	JOBNO-04
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '4';
			Working_Process[0] = '5';
			USART3_SendWorld = 23;
			Work_Status_Flag = 0;

			for(i=0;i<72;i++)
			{
				JOB[i] = 0;					
			}

			break;
		}
		case 0x90:
		{//	JOBNO-05
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '5';
			Working_Process[0] = '5';
			USART3_SendWorld = 23;
			Work_Status_Flag = 0;

			for(i=0;i<72;i++)
			{
				JOB[i] = 0;					
			}

			break;
		}
		case 0x91:
		{//	JOBNO-06
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '6';
			Working_Process[0] = '5';
			USART3_SendWorld = 23;
			Work_Status_Flag = 0;

			for(i=0;i<72;i++)
			{
				JOB[i] = 0;					
			}

			break;
		}
		case 0x92:
		{//����ѡ��󷵻�
			break;
		}
		case 0xA2:
		{//	�ֶ��¸�
			YGTM_Flag = 0;
//			YUANGONG = 0;
			Work_Status_Flag = Online_Status;

			break;
		}

		default:
		{
			break;
		}	
		
	}
}

extern u8 ADDRESS[3];	//������ַ
extern u8 RUN_STATE;
extern u8 YG_CodeLend;	//Ա������
extern u8 YG_Code[30];
extern u8 MJ_CodeLend;
extern u8 MJ_Code[30];
extern u32 YG_Cnt;
extern u32 MJ_Cnt;
extern u32 YG_Tim;
extern u32 MJ_Tim;

extern u8 YG_NameLend;	//Ա���������ȣ���ʼ��Ϊ6Ϊ
extern u8 YG_Name[10];

extern u8 RUN_STATE_01;

/*
	��HMI���͵�һ������
*/
void HMI_Touch_Data(void)
{
	u8 i;
	u16 crc;
	u32 a;
	for(i=0;i<110;i++)
		Serial5.TxBuffer[i]=0;
	Serial5.TxBuffer[0]=(ADDRESS[0]-0x30)*100+(ADDRESS[1]-0x30)*10+(ADDRESS[2]-0x30); //��ַ
	Serial5.TxBuffer[1]=0x03;	//������
	Serial5.TxBuffer[2]=0;		//�Ĵ�����ַ���ֽ�
	Serial5.TxBuffer[3]=RUN_STATE_01;		//�Ĵ�����ַ���ֽ�
	Serial5.TxBuffer[4]=RUN_STATE;		//����״̬��
	//Ա����ģ����
	Serial5.TxBuffer[5]=YG_Cnt>>24;
	Serial5.TxBuffer[6]=YG_Cnt>>16;
	Serial5.TxBuffer[7]=YG_Cnt>>8;
	Serial5.TxBuffer[8]=YG_Cnt;
	//ģ�߿�ģ����
	Serial5.TxBuffer[9]=MJ_Cnt>>24;
	Serial5.TxBuffer[10]=MJ_Cnt>>16;
	Serial5.TxBuffer[11]=MJ_Cnt>>8;
	Serial5.TxBuffer[12]=MJ_Cnt;
	//Ա���ϸ�ʱ��
	Serial5.TxBuffer[13]=YG_Tim>>24;
	Serial5.TxBuffer[14]=YG_Tim>>16;
	Serial5.TxBuffer[15]=YG_Tim>>8;
	Serial5.TxBuffer[16]=YG_Tim;
	//ģ����ģʱ��
	a=MJ_Tim*5;
	Serial5.TxBuffer[17]=a>>24;
	Serial5.TxBuffer[18]=a>>16;
	Serial5.TxBuffer[19]=a>>8;
	Serial5.TxBuffer[20]=a;

	//Ա������  ��ǰ����10���ֽڳ��ȣ���Ԥ������Ա�����볤�����ֻ��Ϊ10�ֽ�
	for(i=0;i<YG_CodeLend;i++)
		Serial5.TxBuffer[i+21]=YG_Code[i];

	//Ա������  ��ǰ����10���ֽڳ��ȣ���Ԥ������Ա�����ֳ������ֻ��Ϊ10�ֽ�
	for(i=0;i<YG_NameLend;i++)
//		Serial5.TxBuffer[i+41]=YG_Name[i];
		Serial5.TxBuffer[i+31]=YG_Name[i];     

	//ģ������	ģ�����볤�������Ϊ16�ֽ�
	for(i=0;i<MJ_CodeLend;i++)
//		Serial5.TxBuffer[i+51]=MJ_Code[i];
		Serial5.TxBuffer[i+41]=MJ_Code[i];	   

//	Serial5.TxBuffer[81] = Work_Status_Flag;
	Serial5.TxBuffer[57] = Work_Status_Flag;   

	//JOB 6*12=72���ֽ�
	for( i = 0;i < 72; i++)
		Serial5.TxBuffer[ 58 + i ] = JOB[ i ];


	//CRCУ����
	crc=CRC16(Serial5.TxBuffer,130);
//	Serial5.TxBuffer[104]=crc>>8;
//	Serial5.TxBuffer[105]=crc;
	Serial5.TxBuffer[130]=crc>>8;
	Serial5.TxBuffer[131]=crc;
	//����
//	Serial5.TxLen=106;
	Serial5.TxLen=132;
	USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
}


