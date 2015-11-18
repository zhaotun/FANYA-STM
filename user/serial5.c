#include "serial5.h"

extern u8 USART3_SendWorld;
extern u8 YG_Code[];
extern u8 UART4_RxBuffer[UART4_RxBufferSizeMax];	
extern u8 UART4_Flag;
extern u8 Work_Status_Flag;
extern u8 MJ_CodeLend;

extern u8 YUANGONG;
extern u8 YGTM_Flag;	//员工条码标志位
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
	函数名：void Init_Serial(void)
	描述：	串行口1初始化
	输入：	无
	输出：	无
	返回：	无
*/
void Init_Serial5(void)	//u8 x)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	//USART1端口配置
	//配置USART1 TX(PC.12)为复用功能推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//配置USART1 RX(PD.2)为浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//配置USART1串行口
	USART_InitStructure.USART_BaudRate = USART5_BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);
	//使能发送中断，该中断产生时的USART1发送数据寄存器为空
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	//使能接收中断，该中断产生时，USART1接收数据寄存器不为空
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	//使能USART1
	USART_Cmd(UART5, ENABLE);
	//使能USART1全局中断
	NVIC_InitStructure.NVIC_IRQChannel=UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

struct USART_Struct
{
	u8	TxBuffer[200];		//发送缓冲区
	u8 	RxBuffer[100];		//接收缓冲区
	u8	TxCount;			//发送计数器
	u8	TxLen;				//发送长度
	u8	TxFlag;				//发送标志字
	u8	RxCount;			//接收计数器
	u8	RxTimer;			//接收计时
	u8	RxFlag;				//接收标志字
};

struct USART_Struct Serial5;	//定义5个串口数据缓冲区
	u8	Send_Flag;	//串口5与其他串口相关
	u8	RUN_WORD;	//运行步骤字
	u8 	UAT5_FLAG=0;
	u8 i;
/*
	函数名： void USART1_Send(void)
	描述：	 串口1发送一帧数据，USART1中断常调用，数据放入发送缓
			 冲区USART1_TxBuffer[50]后,把数据长度写入USART1_TxLen
			 启动USART1缓冲区空中断即可(USART_ITConfig(USART1, USART_IT_TXE, ENABLE);)
	输入：	 USART1_TxBuffer[50]，USART1_TxLen
	输出：	 无
	返回：	 无
*/
void USART5_Send(void)
{
	USART_SendData(UART5, Serial5.TxBuffer[Serial5.TxCount++]);
	if(Serial5.TxCount==Serial5.TxLen)
	{//发送数据完成
		USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
		Serial5.TxCount=0;		
		Serial5.TxLen=0;
	}
}

/*
	函数名： void USART1_Receive(void)
	描述：	 串口接收一帧数据，USART1中断常调用，接收一帧完整的数据
			 接收到的数据放在接收缓冲区USART1_RxBuffer[230]中，长度
			 放到USART1_RxCount中
*/
void USART5_Receive(u8 buf)
{
 	Serial5.RxTimer=0;
	Serial5.RxBuffer[Serial5.RxCount++] = buf;
	if(Serial5.RxCount > RX5_LEN) 
		Serial5.RxCount=0;
 	Serial5.RxFlag |= 0x01;

	
	//if(Send_Flag & 0X10) USART_SendData(UART5,USART_ReceiveData(UART5));	//把接收到的数据向串口5发送
}

extern u8 UART4_RxCounter;	//串口4接收计数器
extern u8 UART4_RxBuffer[UART4_RxBufferSizeMax];		//串口4接收缓冲区
extern u8 UART4_Flag ;
//接收完数据后处理
void USART5_RxDW(void)
{
	if(Serial5.RxFlag & 0x01) 
	{
		Serial5.RxTimer++;
		if(Serial5.RxTimer>=RX5_TIMER)
		{//接收完成处理
			USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);			
			Serial5.RxFlag &= 0xFE;		//清开始接收位
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
                    case 0x1C://读QC输入的合格率
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
                    case 0x1F://读QC原料分析成分
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
                    case 0x6E://读试模缺陷
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
    					printf("试模缺陷发送完成\r\n");

                         break;
                     }
                    case 0xA1://读手动输入的员工卡号,也当成打卡处理
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
                    case 0xA0://读手动输入的模具条码
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
    					printf("手动输入了模具条码\r\n");

                         break;
                     }
                    case 0x6F://读QC登录的用户名
                    {
                        QC_Name[0] = Serial5.RxBuffer[9] / 10 + 48;
                        QC_Name[1] = Serial5.RxBuffer[9] % 10 + 48;
                        
    					USART3_SendWorld = 31; 					
    				
                        break;    
                     }
                    case 0x70://读QC登录密码
                    {
                        QC_PassWord[0] = Serial5.RxBuffer[9] / 10 + 48;
                        QC_PassWord[1] = Serial5.RxBuffer[9] % 10 + 48;

    					USART3_SendWorld = 32; 					
    				
                        break;    
                     }
                    case 0x66://读QC不良统计-欠铸
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
                    case 0x67://读QC不良统计-拉模
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
                    case 0x68://读QC不良统计-砂孔
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
                    case 0x69://读QC不良统计-冷隔
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
                    case 0x6A://读QC不良统计-变形
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
                    case 0x6B://读QC不良统计-崩缺
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
                    case 0x6C://读QC不良统计-起泡
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
                    case 0x6D://读QC不良统计-露管
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
                    case 0x71://读QC不良统计-积炭
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
                    case 0x72://读QC不良统计-尺寸
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
                    case 0x73://读QC不良统计-黑印
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
                    case 0x74://读QC不良统计-其他
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
	串口5接收命令处理
*/
void Serial5_DW(u8 command)
{
//	u8 i;
	
	switch(command)
	{
		case 0x01:
		{//啤工选择上岗
            Online_Status = 0x14;

			break;

		}
		case 0x02:
		{//QC选择上岗
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
		{//非生产人员选择上岗
            Online_Status = 0x15;

			break;
		}
		case 0x0B://11
		{//啤工上岗选择啤货
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
		{//啤工上岗选择啤办
			Working_Type[1] = '0';
			Working_Type[0] = '1';
			Working_Process[1] = '0';
			Working_Process[0] = '2';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;
			break;
		}
        case 0x0D://13
		{//啤工上岗选择上落模
			Working_Type[1] = '0';
			Working_Type[0] = '1';
			Working_Process[1] = '0';
			Working_Process[0] = '3';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;
			break;
		}
         case 0x1D://29  
		{//QC-注销
		
        	USART3_SendWorld = 46;
            Work_Status_Flag = 0;
			break;
		}

        case 0x1E://30  QCfanhui
		{//QC-返回
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
		{//	非生产人员上岗-试模



			break;
		}
    	case 0x29:
		{//非生产人员上岗-修模
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '2';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;

			break;
		}
		case 0x2A:
		{//非生产人员上岗-清批锋
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '3';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;


			break;
		}
		case 0x2B:
		{//非生产人员上岗-修机
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '4';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;

			
			break;
		}
		case 0x2C:
		{//非生产人员上岗-啤办
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '7';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;
			
			break;
		}
		case 0x2D:
		{//非生产人员上岗-换镶件
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '8';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;


			break;
		}
		case 0x2E:
		{//	非生产人员上岗-质量问题待修复
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '9';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;

	
			break;
		}
        case 0x2F:
		{//非生产人员上岗-加料
			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '0';
			Working_Process[0] = '5';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;
			
			break;
		}
		case 0x30:
		{//非生产人员选择上岗返回
			break;
		}
		case 0x3C:
		{//生产前试模

			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '1';
			Working_Process[0] = '6';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;

			break;
		}
		case 0x3D:
		{//生产前FEP

			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '2';
			Working_Process[0] = '6';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;

			break;
		}
		case 0x3E:
		{//生产后啤办

			Working_Type[1] = '0';
			Working_Type[0] = '2';
			Working_Process[1] = '3';
			Working_Process[0] = '6';
			USART3_SendWorld = 22;
			Work_Status_Flag = 0;

			break;
		}
		case 0x78:
		{//啤工下岗-下班
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
		{//啤工下岗-够数
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
		{//啤工下岗-异常
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
		{//啤工下岗-其他
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
		{//非生产人员下岗-工作完成
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
		{//非生产人员下岗-自动啤
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
		{//非生产人员下岗-排期选择

//			On_Duty_Flag = 0x02;
			break;
		}
		case 0x85:
		{//非生产人员下岗-签办
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
		{//非生产人员下岗-异常问题需停机
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
		{//排期选择后返回
			break;
		}
		case 0xA2:
		{//	手动下岗
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

extern u8 ADDRESS[3];	//本机地址
extern u8 RUN_STATE;
extern u8 YG_CodeLend;	//员工长度
extern u8 YG_Code[30];
extern u8 MJ_CodeLend;
extern u8 MJ_Code[30];
extern u32 YG_Cnt;
extern u32 MJ_Cnt;
extern u32 YG_Tim;
extern u32 MJ_Tim;

extern u8 YG_NameLend;	//员工姓名长度，初始化为6为
extern u8 YG_Name[10];

extern u8 RUN_STATE_01;

/*
	向HMI发送的一串数据
*/
void HMI_Touch_Data(void)
{
	u8 i;
	u16 crc;
	u32 a;
	for(i=0;i<110;i++)
		Serial5.TxBuffer[i]=0;
	Serial5.TxBuffer[0]=(ADDRESS[0]-0x30)*100+(ADDRESS[1]-0x30)*10+(ADDRESS[2]-0x30); //地址
	Serial5.TxBuffer[1]=0x03;	//功能码
	Serial5.TxBuffer[2]=0;		//寄存器地址高字节
	Serial5.TxBuffer[3]=RUN_STATE_01;		//寄存器地址低字节
	Serial5.TxBuffer[4]=RUN_STATE;		//运行状态字
	//员工开模数据
	Serial5.TxBuffer[5]=YG_Cnt>>24;
	Serial5.TxBuffer[6]=YG_Cnt>>16;
	Serial5.TxBuffer[7]=YG_Cnt>>8;
	Serial5.TxBuffer[8]=YG_Cnt;
	//模具开模数据
	Serial5.TxBuffer[9]=MJ_Cnt>>24;
	Serial5.TxBuffer[10]=MJ_Cnt>>16;
	Serial5.TxBuffer[11]=MJ_Cnt>>8;
	Serial5.TxBuffer[12]=MJ_Cnt;
	//员工上岗时间
	Serial5.TxBuffer[13]=YG_Tim>>24;
	Serial5.TxBuffer[14]=YG_Tim>>16;
	Serial5.TxBuffer[15]=YG_Tim>>8;
	Serial5.TxBuffer[16]=YG_Tim;
	//模具上模时间
	a=MJ_Tim*5;
	Serial5.TxBuffer[17]=a>>24;
	Serial5.TxBuffer[18]=a>>16;
	Serial5.TxBuffer[19]=a>>8;
	Serial5.TxBuffer[20]=a;

	//员工条码  当前定义10个字节长度，无预留，即员工条码长度最多只能为10字节
	for(i=0;i<YG_CodeLend;i++)
		Serial5.TxBuffer[i+21]=YG_Code[i];

	//员工名字  当前定义10个字节长度，无预留，即员工名字长度最多只能为10字节
	for(i=0;i<YG_NameLend;i++)
//		Serial5.TxBuffer[i+41]=YG_Name[i];
		Serial5.TxBuffer[i+31]=YG_Name[i];     

	//模具条码	模具条码长度最多能为16字节
	for(i=0;i<MJ_CodeLend;i++)
//		Serial5.TxBuffer[i+51]=MJ_Code[i];
		Serial5.TxBuffer[i+41]=MJ_Code[i];	   

//	Serial5.TxBuffer[81] = Work_Status_Flag;
	Serial5.TxBuffer[57] = Work_Status_Flag;   

	//JOB 6*12=72个字节
	for( i = 0;i < 72; i++)
		Serial5.TxBuffer[ 58 + i ] = JOB[ i ];


	//CRC校验码
	crc=CRC16(Serial5.TxBuffer,130);
//	Serial5.TxBuffer[104]=crc>>8;
//	Serial5.TxBuffer[105]=crc;
	Serial5.TxBuffer[130]=crc>>8;
	Serial5.TxBuffer[131]=crc;
	//发送
//	Serial5.TxLen=106;
	Serial5.TxLen=132;
	USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
}


