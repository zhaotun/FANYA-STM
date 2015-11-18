//该串口用于以太网通讯
#include "serial3.h"
#include "include.h"
#include "string.h"

extern u8 ADDRESS[3];		//设备地址
extern u8 MACHINE_TYPE[2];
extern u8 RUN_STATE; 		//本机当前运行状态
extern u8 RUN_STATE; 		//本机当前运行状态
extern u8 Code_Mode;		//读写回答指令
extern u8 Command[3];		//运行命令
extern u8 YG_CodeLend;		//员工条码长度
extern u8 YG_Code[];
extern u8 MJ_CodeLend;		//磨具条码
extern u8 MJ_Code[];
extern char MO_Time[6];		//模具计时
extern char MJ_Timing[6];	//模具计时
extern char YG_Time[6];		//员工计时
extern u8 Working_Type[2];	//工种
extern u8 Working_Process[2];	//工序
extern u8 On_Duty_Flag;

extern u16 Address;		//本机地址
extern u32 MJ_Cnt;		//磨具计数
extern u32 MJ_Tim;		//磨具计时
extern u32 YG_Cnt;		//员工开模计数
extern u32 YG_Tim;		//员工计时
extern u32 XT_ONTime;	//系统上电计时
extern u32 RL_ONTime;	//熔炉上电计时
extern u8 YGTM_Flag;	//员工条码标志位
extern u8 New_YG_Code[30];	//新的员工条码
extern u8 New_YG_CodeLend;	//新员工条码长度
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
	u8 USART3_SendWorld=0;		//发送标志字
	u8 USART3_Flag=0;			//收发状态标志字
	u8 USART3_RxBuffer[USART3_RxBufferSizeMax];
	u8 USART3_RxCounter = 0; 

u8  JOB[72];
/*
	函数名： USART3_Init()
	描述：	 串口3初始化函数
	输入：	 无
	输出：	 无
	返回：	 无
*/
void USART3_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	//使能GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//使能AFIO时钟
	//GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);	//端口重新映射
	GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
	//USART3端口配置
	//配置USART3 TX(PD8)为复用功能推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//配置USART3 RX(PD9)为浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//配置USART1串行口
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	//使能发送中断，该中断产生时的USART1发送数据寄存器为空
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	//使能接收中断，该中断产生时，USART1接收数据寄存器不为空
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	//使能USART1
	USART_Cmd(USART3, ENABLE);
	//使能USART3中断
	NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
	函数名： USART3_SendDA()
	描述：	 串口3发送数据处理
			 串口发送中断常调用
	输入：	 无
	输出：	 无
	返回：	 无
*/
void USART3_SendDA()
{	
	if(USART3_TxCounter >= USART3_TxLen)
	{
		//完成发送，关闭发送中断
		USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
		USART3_TxCounter=0;				//发送计数器清零
		USART3_Flag &=0xFE;				//置发送完成标志位
	}
	else 
	{
		USART_SendData(USART3, USART3_TxBuffer[USART3_TxCounter++]);   	//发送一个字节
	}
}

/*
	函数名： USART3_SendDW(void)
	描述：	 
*/
void USART3_SendDW(void)
{
	USART3_TxCounter=0;
	USART3_Flag &=0xF0;
	USART3_Flag |=0x01;		//正在发送中	
	Communicate_SaveData(USART3_TxBuffer,USART3_TxLen);
	USART_SendData(USART3, ':');	//发送起始位
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

/*
	函数名：	USART3_ReceiveData
	描述：		串口3接收处理
				数据帧：起始字 设备编号 指令字 逻辑号 【长度+数据】 校验字 结束字
	输入：		u8 tmp：接收到的数据
	输出：		无
 	返回：		无
*/
void USART3_ReceiveDA(u8 buffer)
{
	Communicate_SetOff();	//清掉以太网检测相关标志位
	Clera_Timer1();//清除未收到计时
	if(USART3_Flag & 0x10)
	{
		USART3_RxBuffer[USART3_RxCounter++] =buffer;  
		if(buffer==EOX)
		{
			if(USART3_RxBuffer[USART3_RxCounter-2]==EOR)	//接收完成
			{
				USART3_Flag&=0x0F;		
				USART3_Flag|=0X20;		//接收完成
				USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
			}
		}
		else if(USART3_RxCounter >= USART3_RxBufferSizeMax)	//等于接收缓冲区
		{
			USART3_Flag &= 0x0F;		
			USART3_Flag |= 0X40;			//接收错误
			USART3_RxCounter=0;
			USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);	
		}
	}
	else if(buffer==STX) USART3_Flag |= 0x10;		//接收开始
}

/*
*	函数名：USART3_ReceiveDW()
*	描述：	串口3数据接收处理
*	输入：	无
*	输出： 	无
*	返回：	无
*/
void USART3_ReceiveDW()			//接收到的数据处理
{
	if(USART3_Flag & 0x20)
	{
		u8 tem=(USART3_RxBuffer[1]-0x30)*10+USART3_RxBuffer[2]-0x30;	//计算命令字
		switch(USART3_RxBuffer[0])
		{
			case 'R':
			{	//读命令
				switch(tem)
				{
					case 1: //发送本机地址
					{
						Send_Address();	
						Clear_RxBuffer();
						break;
					}
					case 2: //发送本机机器类型
					{
						Send_MACHINE_TYPE();	
						Clear_RxBuffer();
						break;
					}
					case 3: //发送本机当前运行状态
					{
						USART3_TxBuffer[0]=RUN_STATE;
						USART3_TxBuffer[1]=0x0D;		//结束符
						USART3_TxBuffer[2]=0x0A;
						USART3_TxLen=3;
						USART3_SendDW();	
						Clear_RxBuffer();
						break;
					}
					case 4: //发送机器当前模具条码
					{
						SendMJCode();
						Clear_RxBuffer();
						break;
					}
					case 5: //发送当前员工条码  
					{
						SendYGCode();
						Clear_RxBuffer();
						break;
					}
					case 6: //发送机器当前模具开模总数
					{
						Send_MMYY(MJ_Cnt);
						Clear_RxBuffer();
						break;
					}
					case 7: //发送机器当前模具安装时间
					{
						Send_MMYY(MJ_Tim);
						Clear_RxBuffer();
						break;
					}
					case 8: //发送机器当前员工开模计数
					{
						Send_MMYY(YG_Cnt);
						Clear_RxBuffer();
						break;
					}
					case 9: //发送机器当前员工在岗时间
					{
						Send_MMYY(YG_Tim);
						Clear_RxBuffer();
						break;
					}
					case 20: //返回指令字001命令
					{
						USART3_SendWorld=1;
						Clear_RxBuffer();
						break;
					}
					case 21: //返回指令字110命令
					{
						USART3_SendWorld=2;
						Clear_RxBuffer();
						break;
					}
					case 22: //返回指令字111命令
					{
						USART3_SendWorld=3;
						Clear_RxBuffer();
						break;
					}
					case 23: //返回指令字112命令
					{
						USART3_SendWorld=4;
						Clear_RxBuffer();
						break;
					}
					case 24: //返回指令字113命令
					{
						USART3_SendWorld=5;
						Clear_RxBuffer();
						break;
					}
					case 25: //返回指令字114命令
					{
						USART3_SendWorld=6;
						Clear_RxBuffer();
						break;
					}
					case 26: //返回指令字110命令
					{
						USART3_SendWorld=19;
						Clear_RxBuffer();
						break;
					}
					case 27: //返回指令字111命令
					{
						USART3_SendWorld=8;
						Clear_RxBuffer();
						break;
					}
					case 28: //返回指令字112命令
					{
						USART3_SendWorld=9;
						Clear_RxBuffer();
						break;
					}
					case 29: //返回指令字113命令
					{
						USART3_SendWorld=10;
						Clear_RxBuffer();
						break;
					}
					case 30: //返回指令字114命令
					{
						USART3_SendWorld=11;
						Clear_RxBuffer();
						break;
					}
					case 31: //返回指令字115命令
					{
						USART3_SendWorld=12;
						Clear_RxBuffer();
						break;
					}
					case 32: //返回指令字116命令
					{
						USART3_SendWorld=13;
						Clear_RxBuffer();
						break;
					}
					case 33: //返回指令字117命令
					{
						USART3_SendWorld=14;
						Clear_RxBuffer();
						break;
					}
					case 34: 	//返回系统上电信息
					{
						USART3_SendWorld=15;
						Clear_RxBuffer();
						break;
					}
					case 35:	//返回员工上岗信息
					{
						USART3_SendWorld=16;
						Clear_RxBuffer();
						break;
					}
					case 36:	//返回模具安装信息
					{
						USART3_SendWorld=17;
						Clear_RxBuffer();
						break;
					}
					case 37:	//返回模具安装信息
					{
						USART3_SendWorld=19;
						Clear_RxBuffer();
						break;
					}
					default :
					{	//命令错误
						Send_Error();
						Clear_RxBuffer();
						break;
					}
				}
				break;
			}
			case 'W':
			{	//写命令
				switch(tem)
				{
					case 1:		//修改本机地址
					{	
						SPI_Flash_Write(&USART3_RxBuffer[3],Flash_Base0,3);	//修改本机站号
						SPI_Flash_Read(ADDRESS,Flash_Base0,3);		//读取地址
						Send_Address();		//返回修改后的地址
						Clear_RxBuffer();
						break;
					}
					case 2:		//修改本机机器类型
					{	
						SPI_Flash_Write(&USART3_RxBuffer[3],Flash_Base1,2);	//修改本机机器类型
						SPI_Flash_Read(MACHINE_TYPE,Flash_Base1,2);		//读取机器类型
						Send_MACHINE_TYPE();		//返回修改后的机器类型
						Clear_RxBuffer();
						break;
					}
					case 3:		//修改模具条码
					{
						Clear_RxBuffer();
						break;
					}
					case 4:		//修改员工条码
					{
						u8 i;
						if(YGTM_Flag & 0x01)
						{//当前有员工条码
							if(YG_CodeLend == (USART3_RxCounter-5))
							{	//新接收的员工条码与旧的员工条码长度相同
								if(Char_Compare(YG_Code,&USART3_RxBuffer[3],YG_CodeLend)==1)
								{//接收到相同的条码，认为员工结束上岗		
									YGTM_Flag =0;		//清员工条码标志位
									USART3_SendWorld=14;	//发送员工上岗结束
								}
								else
								{//接收到不同的条码
									YGTM_Flag = 0;
									YGTM_Flag |= 0x02;		//清员工条码标志位
									USART3_SendWorld=14;	//发送员工上岗结束
									for(i=0;i<USART3_RxCounter-5;i++)
									New_YG_Code[i]=USART3_RxBuffer[i+3];
									New_YG_CodeLend=USART3_RxCounter-5;
								}
							}
							else
							{//新接收的员工条码与旧的员工条码长度不相同，则认为接收到新条码
								YGTM_Flag =0;
								YGTM_Flag |=02;		//清员工条码标志位
								USART3_SendWorld=14;	//发送员工上岗结束
								for(i=0;i<USART3_RxCounter-5;i++)
								New_YG_Code[i]=USART3_RxBuffer[i+3];
								New_YG_CodeLend=USART3_RxCounter-5;
							}
						}
						else
						{//当前无员工条码，则认为接收到新条码
							YGTM_Flag |=0X01;	//标志当前有条码
							for(i=0;i<USART3_RxCounter-5;i++)
							YG_Code[i]=USART3_RxBuffer[i+3];
							YG_CodeLend=USART3_RxCounter-5;
							USART3_SendWorld=13;	//发送新条员工码数据
						}
						Clear_RxBuffer();
						break;
					}
					case 5:
					{//订单生产完毕，指示灯闪烁
						LED_ON();
						Clear_RxBuffer();
						break;
					}
					case 6:
					{//指示灯停止闪烁
						LED_OFF();
						Clear_RxBuffer();
						break;
					}
					case 7:
					{//修改模具开模数量,:W07|1234
						MJ_Cnt=(USART3_RxBuffer[5]-0x30)*1000+(USART3_RxBuffer[6]-0x30)*100+(USART3_RxBuffer[7]-0x30)*10+USART3_RxBuffer[8]-0x30;	//计算命令字
						YG_Cnt=MJ_Cnt;
						Clear_RxBuffer();
						break;
					}
					default :
					{	//命令错误
						Send_Error();
						Clear_RxBuffer();
						break;
					}
				}
				break;
			}
			case 'S':
			{	//系统相关命令
				switch(tem)
				{
					case 1:	
					{	//系统软件复位
						NVIC_SystemReset();		//系统软件复位
						Clear_RxBuffer();
						break;
					}
					case 2:
					{	//读取系统当前运行时间
						USART3_SendWorld=100;
						Clear_RxBuffer();
						break;
					}
					case 3:
					{//用于接收上位机返回的信息，不处理
						Clear_RxBuffer();
						break;
					}
					default:
					{	//错误，没此命令
						Send_Error();
						Clear_RxBuffer();
						break;
					}
				}
				break;
			}
			default: 
			{	//命令错误
// 				Send_Error();
				break;
			}
		}
		Communicate_Computer();
		USART3_Flag &= 0x0F;		//清标识位
		USART3_RxCounter=0;			//清零
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	//开中断
	}
}
//通讯标准协议处理
void Communicate_Computer(void)
{
	//接收:|001|W|007|6|J1309050AA|CD2013D00869|CD2013D00870|CD2013D00897|CD2013D00900|CD2013D00903|校验码\n
	//接收:|001|W|005|XXXX\n”
	//返回:|001|DY|R|W|005|XXXX\n
	u8 tem=(USART3_RxBuffer[7]-0x30)*100+(USART3_RxBuffer[8]-0x30)*10+USART3_RxBuffer[9]-0x30;	//计算命令字
	switch(USART3_RxBuffer[5])
	{
		case 'w':
		case 'W':
		{
			switch(tem)
			{
				case 5:
				{//开闪烁灯
					u8 i;
					USART3_TxBuffer[0]='|';
					for(i=0;i<3;i++)
					USART3_TxBuffer[i+1]=ADDRESS[i];
					USART3_TxBuffer[4]='|';
					for(i=0;i<2;i++)
					USART3_TxBuffer[i+5]=MACHINE_TYPE[i];
					USART3_TxBuffer[7]='|';
					USART3_TxBuffer[8]=RUN_STATE;			//机器运行状态
					USART3_TxBuffer[9]='|';
					USART3_TxBuffer[10]='W';
					USART3_TxBuffer[11]='|';
					USART3_TxBuffer[12]='0';
					USART3_TxBuffer[13]='0';
					USART3_TxBuffer[14]='5';
					USART3_TxBuffer[15]='|';
					USART3_TxLen=22;
					Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
					USART3_TxBuffer[20]=0x0D;		//结束符
					USART3_TxBuffer[21]=0x0A;
					USART3_SendDW();
					LED_ON();
					Clear_RxBuffer();
					break;
				}
				case 6:
				{//关闪烁灯
					u8 i;
					USART3_TxBuffer[0]='|';
					for(i=0;i<3;i++)
					USART3_TxBuffer[i+1]=ADDRESS[i];
					USART3_TxBuffer[4]='|';
					for(i=0;i<2;i++)
					USART3_TxBuffer[i+5]=MACHINE_TYPE[i];
					USART3_TxBuffer[7]='|';
					USART3_TxBuffer[8]=RUN_STATE;			//机器运行状态
					USART3_TxBuffer[9]='|';
					USART3_TxBuffer[10]='W';
					USART3_TxBuffer[11]='|';
					USART3_TxBuffer[12]='0';
					USART3_TxBuffer[13]='0';
					USART3_TxBuffer[14]='6';
					USART3_TxBuffer[15]='|';
					USART3_TxLen=22;
					Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
					USART3_TxBuffer[20]=0x0D;		//结束符
					USART3_TxBuffer[21]=0x0A;
					USART3_SendDW();
					LED_OFF();
					Clear_RxBuffer();
					break;
				}
				case 7:			//接收JOB信息
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
					Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
					USART3_TxBuffer[9]=0x0D;		//结束符
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
	函数名： void Send_Error(void)
	描述：	 向以太网发送错误命令提示
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_Error(void)
{
	USART3_TxBuffer[0]='E';
	USART3_TxBuffer[1]='r';
	USART3_TxBuffer[2]='r';
	USART3_TxBuffer[3]='o';
	USART3_TxBuffer[4]='r';
	USART3_TxBuffer[5]=0x0D;		//结束符
	USART3_TxBuffer[6]=0x0A;
	USART3_TxLen=7;
	USART3_SendDW();
}

/*
	函数名： void Send_Address(void)
	描述：	 向以太网发本机地址
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_Address(void)
{
	u8 i;
	for(i=0;i<3;i++)
	USART3_TxBuffer[i]=ADDRESS[i];
	USART3_TxBuffer[3]=0x0D;		//结束符
	USART3_TxBuffer[4]=0x0A;
	USART3_TxLen=5;
	USART3_SendDW();
}

/*
	函数名： void Send_MACHINE_TYPE(void)
	描述：	 向以太网发本机机器类型
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_MACHINE_TYPE(void)
{
	u8 i;
	for(i=0;i<2;i++)
	USART3_TxBuffer[i]=MACHINE_TYPE[i];
	USART3_TxBuffer[2]=0x0D;		//结束符
	USART3_TxBuffer[3]=0x0A;
	USART3_TxLen=4;
	USART3_SendDW();
}

/*
	函数名： void SendMJCode(void)
	描述：	 向以太网发本机模具条码
	输入：	 无
	输出：	 无
	返回：	 无
*/
void SendMJCode(void)
{
	u8 i;
	for(i=0;i<MJ_CodeLend;i++)
	USART3_TxBuffer[i]=MJ_Code[i];
	USART3_TxBuffer[MJ_CodeLend]=0x0D;		//结束符
	USART3_TxBuffer[MJ_CodeLend+1]=0x0A;
	USART3_TxLen=MJ_CodeLend+2;
	USART3_SendDW();
}

/*
	函数名： void SendYGCode(void)
	描述：	 向以太网发本机员工条码
	输入：	 无
	输出：	 无
	返回：	 无
*/
void SendYGCode(void)
{
	u8 i;
	for(i=0;i<YG_CodeLend;i++)
	USART3_TxBuffer[i]=YG_Code[i];
	USART3_TxBuffer[YG_CodeLend]=0x0D;		//结束符
	USART3_TxBuffer[YG_CodeLend+1]=0x0A;
	USART3_TxLen=YG_CodeLend+2;
	USART3_SendDW();
}

/*
	函数名： void Send_MMYY(void)
	描述：	 向以太网发本机模具计数、模具计时、员工上岗开模计数、员工上岗计时
	输入：	 u32 tem，模具计数/模具计时/员工计数/员工计时
	输出：	 无
	返回：	 无
*/
void Send_MMYY(u32 tem)
{
	Cu32_To_Char(tem, &USART3_TxBuffer[0]);
	USART3_TxBuffer[6]=0x0D;		//结束符
	USART3_TxBuffer[7]=0x0A;
	USART3_TxLen=8;
	USART3_SendDW();
}

/*
	函数名： void Send_ETH(void)
	描述：	 向以太网端口发送数据
	输入：	 无
	输出：	 无
	返回：	 无
*/
u8 Heart_RxCounter=0;
u8 Heart_World=0;
extern u8  Input_Flag;			//输入相关标志位
void Send_ETH()
{
	if((USART3_Flag & 0x01)==0)	//上一帧数据是否发送完成，只有上一帧数据发送完成后才可以进入下一帧发送
	{
		if(USART3_SendWorld != 0)
		{
			Communicate_SetOn();
		}
		switch(USART3_SendWorld)	
		{
			u8 i;
			case 0: break;		//0不发送数据
			case 1:			//开模发送数据		
			{
				Send_AMR();		//发送机器编号、类型，编号，状态、指令、命令，以上有15个数据
				Cu8_To_Char(&USART3_TxBuffer[16], YG_CodeLend+MJ_CodeLend+31); //数据长度
				USART3_TxBuffer[18]='|';	
				for(i=0;i<MJ_CodeLend;i++)		//模具条码
				USART3_TxBuffer[19+i]=MJ_Code[i];		
 				USART3_TxBuffer[19+i]='|';	
				for(i=0;i<YG_CodeLend;i++)		//员工条码
 				USART3_TxBuffer[20+MJ_CodeLend+i]=YG_Code[i];		
				USART3_TxBuffer[20+MJ_CodeLend+YG_CodeLend]='|';	
				Cu32_To_Char(MJ_Cnt, &USART3_TxBuffer[21+MJ_CodeLend+YG_CodeLend]);	//开模计数值,6个字节
				USART3_TxBuffer[27+MJ_CodeLend+YG_CodeLend]='|';
				Cu32_To_Char(MJ_Tim,&USART3_TxBuffer[28+MJ_CodeLend+YG_CodeLend]);	//本机模具安装时间，6个字节
				USART3_TxBuffer[34+MJ_CodeLend+YG_CodeLend]='|';
				Cu32_To_Char(YG_Cnt,&USART3_TxBuffer[35+MJ_CodeLend+YG_CodeLend]);	//员工上岗开模计数，6个字节
				USART3_TxBuffer[41+MJ_CodeLend+YG_CodeLend]='|';
				Cu32_To_Char(YG_Tim,&USART3_TxBuffer[42+MJ_CodeLend+YG_CodeLend]);	//员工上岗时间，6个字节
				USART3_TxBuffer[48+MJ_CodeLend+YG_CodeLend]='|';
				Cu32_To_Char(XT_ONTime,&USART3_TxBuffer[49+MJ_CodeLend+YG_CodeLend]);	//系统上电、断电信息
				USART3_TxBuffer[55+MJ_CodeLend+YG_CodeLend]='|';
				USART3_TxLen=YG_CodeLend+MJ_CodeLend+62;		//计算发送数据长度
				Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
				USART3_TxBuffer[USART3_TxLen-2]=0x0D;		//结束符
				USART3_TxBuffer[USART3_TxLen-1]=0x0A;
				USART3_SendDW();		//发送数据
				USART3_SendWorld=0;
				break;	
			} 
			case 2:			//发送员工条码数据
			{
				Send_AMR();			//发送机器编号、类型，编号，状态、指令、命令，以上有15个数据
				Cu8_To_Char(&USART3_TxBuffer[10], YG_CodeLend+1); //数据长度
				for(i=0;i<YG_CodeLend;i++)
				USART3_TxBuffer[12+i]=YG_Code[i];		//员工条码
 				USART3_TxBuffer[12+i]=' ';				//空格表示分隔符
				USART3_TxLen=19+YG_CodeLend;			//计算发送数据长度
				Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
				USART3_TxBuffer[USART3_TxLen-2]=0x0D;		//结束符
				USART3_TxBuffer[USART3_TxLen-1]=0x0A;
				USART3_SendDW();		//发送数据
				USART3_SendWorld=0;
				break;
			}
			case 3:			//发送磨具条码	
			{
				Send_AMR();			//发送机器编号、类型，编号，状态、指令、命令，以上有15个数据
				Cu8_To_Char(&USART3_TxBuffer[15], YG_CodeLend+1); //数据长度，2个
				for(i=0;i<MJ_CodeLend;i++)
				USART3_TxBuffer[17+i]=MJ_Code[i];		//员工条码
 				USART3_TxBuffer[17+i]='|';				//空格表示分隔符
				USART3_TxLen=19+MJ_CodeLend;			//计算发送数据长度
				Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
				USART3_TxBuffer[USART3_TxLen-2]=0x0D;		//结束符
				USART3_TxBuffer[USART3_TxLen-1]=0x0A;
				USART3_SendDW();		//发送数据
				USART3_SendWorld=0;
				break;
			}
			case 4:			//检测到电炉打开或关闭
			{
				Send_AMR();		//发送机器编号、类型，编号，状态、指令、命令
				Cu8_To_Char(&USART3_TxBuffer[10], YG_CodeLend+MJ_CodeLend+8); //数据长度
				
				for(i=0;i<YG_CodeLend;i++)
				USART3_TxBuffer[12+i]=YG_Code[i];		//员工条码
 				USART3_TxBuffer[12+i]='|';				//空格表示分隔符
				for(i=0;i<MJ_CodeLend;i++)
				USART3_TxBuffer[13+YG_CodeLend+i]=MJ_Code[i];		//员工条码
     			USART3_TxBuffer[13+YG_CodeLend+i]='|';	//分隔符
 				Cu32_To_Char(MJ_Cnt,&USART3_TxBuffer[14+YG_CodeLend+i]);	//开模计数值
				USART3_TxLen=26+YG_CodeLend+MJ_CodeLend;		//计算发送数据长度
				Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
				USART3_TxBuffer[USART3_TxLen-2]=0x0D;		//结束符
				USART3_TxBuffer[USART3_TxLen-1]=0x0A;
				USART3_SendDW();		//发送数据
				USART3_SendWorld=0;
				break;	
				
			}
			case 5:			//检测到机柜电源打开或关闭
			{
				Send_AMR();			//发送机器编号、类型，编号，状态、指令、命令
				USART3_SendWorld=0;
				break;
			}
			case 6:			//返回本机当前时钟
			{
				Send_AMR();			//发送机器编号、类型，编号，状态、指令、命令
				USART3_SendWorld=0;
				break;
			}
			case 7:			
			{//读取到模具被移开,同时清除相关数据
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='0';
				Send_MJ_Code();	
				MJ_Tim=0;
				MJ_Cnt=0;
				YG_Cnt=0;
				//清空模具信息
				for(i=0;i<10;i++)
				{
					MJ_Code[i]=0x30;	//将磨具条码变为10个ASCII码0
				}
				MJ_Code[i] = 0;
				MJ_CodeLend=10;		//磨具条码长度
				USART3_SendWorld=0;
				break;
			}
			case 8:			
			{//发送新模具信息，同时初始化相关数据
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
			case 9:				//发送系统上电信息
			{
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='2';
				Send_XTRL_Time();
				USART3_SendWorld=0;
				break;
			}
			case 10:			//发送系统断电信息
			{
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='3';
				Send_XTRL_Time();
				XT_ONTime=0;
				USART3_SendWorld=0;
				break;
			}
			case 11:			//发送熔炉上电信息
			{
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='4';
				Send_XTRL_Time();
				USART3_SendWorld=0;
				break;
			}
			case 12:			//发送熔炉断电信息
			{
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='5';
				Send_XTRL_Time();
				RL_ONTime=0;	//清熔炉计时
				USART3_SendWorld=0;
				break;
			}
			case 13:			
			{//发送员工上岗信息,同时初始化相关数据

								
					YG_Cnt=0;		//员工开模计数
					YG_Tim=0;		//员工计时
					USART3_TxBuffer[12]='1';
					USART3_TxBuffer[13]='1';
					USART3_TxBuffer[14]='6';
					Send_RGXX_Time();
					On_Duty_Flag = 0;
					USART3_SendWorld=0;
				break;
			}
			case 14:			
			{//发送员工上岗结束信息，同时清除相关数据
				
				
					USART3_TxBuffer[12]='1';
					USART3_TxBuffer[13]='1';
					USART3_TxBuffer[14]='7';
					Send_RGXX_Time();
					for(i=0;i<10;i++)	//清条码
					YG_Code[i]=0x30;
					YG_CodeLend=10;
					YG_Cnt=0;		//员工开模计数
					YG_Tim=0;		//员工计时
					On_Duty_Flag = 0;
					USART3_SendWorld=0;
				break;
			}
			case 15:
			{//返回上电数据
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='8';
				Change_TxBuffer();
				Cu32_To_Char(XT_ONTime,&USART3_TxBuffer[16]);	//系统上电、断电信息
				USART3_TxBuffer[22]='|';
				USART3_TxLen=29;		//计算发送数据长度
				Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
				USART3_TxBuffer[USART3_TxLen-2]=0x0D;			//结束符
				USART3_TxBuffer[USART3_TxLen-1]=0x0A;
				USART3_SendDW();
				USART3_SendWorld=0;
				break;
			}
			case 16:
			{	//返回员工上岗信息
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='9';
				Send_RGXX_Time();
				USART3_SendWorld=0;
				break;
			}
			case 17:
			{	//返回模具相关信息
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='2';
				USART3_TxBuffer[14]='0';
				Send_MJ_Code();	
				USART3_SendWorld=0;
				break;
			}
			case 18:
			{//心跳包
				Send_AMR();		//发送机器编号、类型，编号，状态、指令、命令，以上有15个数据
				USART3_TxBuffer[10]='S';			//指令字
				Cu8_To_Char(&USART3_TxBuffer[16], YG_CodeLend+MJ_CodeLend+31); //数据长度
				USART3_TxBuffer[18]='|';	
				for(i=0;i<MJ_CodeLend;i++)		//模具条码
				USART3_TxBuffer[19+i]=MJ_Code[i];		
 				USART3_TxBuffer[19+i]='|';	
				for(i=0;i<YG_CodeLend;i++)		//员工条码
 				USART3_TxBuffer[20+MJ_CodeLend+i]=YG_Code[i];		
				USART3_TxBuffer[20+MJ_CodeLend+YG_CodeLend]='|';	
				Cu32_To_Char(MJ_Cnt, &USART3_TxBuffer[21+MJ_CodeLend+YG_CodeLend]);	//开模计数值,6个字节
				USART3_TxBuffer[27+MJ_CodeLend+YG_CodeLend]='|';
				Cu32_To_Char(MJ_Tim,&USART3_TxBuffer[28+MJ_CodeLend+YG_CodeLend]);	//本机模具安装时间，6个字节
				USART3_TxBuffer[34+MJ_CodeLend+YG_CodeLend]='|';
				Cu32_To_Char(YG_Cnt,&USART3_TxBuffer[35+MJ_CodeLend+YG_CodeLend]);	//员工上岗开模计数，6个字节
				USART3_TxBuffer[41+MJ_CodeLend+YG_CodeLend]='|';
				Cu32_To_Char(YG_Tim,&USART3_TxBuffer[42+MJ_CodeLend+YG_CodeLend]);	//员工上岗时间，6个字节
				USART3_TxBuffer[48+MJ_CodeLend+YG_CodeLend]='|';
				Cu32_To_Char(XT_ONTime,&USART3_TxBuffer[49+MJ_CodeLend+YG_CodeLend]);	//系统上电计时
				USART3_TxBuffer[55+MJ_CodeLend+YG_CodeLend]='|';
				USART3_TxLen=YG_CodeLend+MJ_CodeLend+62;		//计算发送数据长度
				Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
				USART3_TxBuffer[USART3_TxLen-2]=0x0D;		//结束符
				USART3_TxBuffer[USART3_TxLen-1]=0x0A;
				USART3_SendDW();		//发送数据
				USART3_SendWorld=0;
				break;
			}
			case 19:			
			{//发送取到模具被移开命令，但模具相关数据不清零
				break;
			}
			case 20:
			{//模具被移开后发送，发送员工下岗信息,然后跳到发送模具移开USART3_SendWorld=7;		
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='2';
				USART3_TxBuffer[14]='1';
				Send_RGXX_Time();
				USART3_SendWorld=7;			
				break;
			}
			case 21:				
			{//当前没有模具，但读取到上一次的模具，则发送模具信息
				USART3_TxBuffer[12]='1';
				USART3_TxBuffer[13]='1';
				USART3_TxBuffer[14]='1';
				Send_MJ_Code();
				USART3_SendWorld=0;
				break;
			}
			case 22:
			{//通过触摸屏处理的上岗
					YG_Cnt=0;		//员工开模计数
					YG_Tim=0;		//员工计时
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
			{//通过触摸屏处理的下岗				
					USART3_TxBuffer[12]='2';
					USART3_TxBuffer[13]='0';
					USART3_TxBuffer[14]='2';
					Send_RGXX_Time();
					for(i=0;i<10;i++)	//清条码
					YG_Code[i]=0x30;
					YG_CodeLend=10;
					YG_Cnt=0;		//员工开模计数
					YG_Tim=0;		//员工计时
					On_Duty_Flag = 0;
					USART3_SendWorld=0;
				break;
			}
			case 24:
			{//发送QC输入的合格率				
					USART3_TxBuffer[12]='2';
					USART3_TxBuffer[13]='2';
					USART3_TxBuffer[14]='2';

					Send_QC_Input_Data();

					for(i=0;i<10;i++)	//清条码
						QC_Input_Data[i]=0x30;

//					YG_CodeLend=10;
//					YG_Cnt=0;		//员工开模计数        20151028 现场员工反应 QC输入后会将员工计数清零
//					YG_Tim=0;		//员工计时                   M3892   099902

					USART3_SendWorld=0;
				break;
			}
			case 25:
			{//发送试模缺陷				
					USART3_TxBuffer[12]='2';
					USART3_TxBuffer[13]='2';
					USART3_TxBuffer[14]='3';

					Send_Mould_Defect();

					for(i=0;i<32;i++)	//清条码
						Mould_Defect[i]=0x30;

					YG_CodeLend=10;
					YG_Cnt=0;		//员工开模计数
					YG_Tim=0;		//员工计时

					USART3_SendWorld=0;
				break;
			}
			case 26:				//发送手动模式
			{
				USART3_TxBuffer[12]='3';
				USART3_TxBuffer[13]='0';
				USART3_TxBuffer[14]='1';
				Send_XTRL_Time();
				USART3_SendWorld=0;
				break;
			}
			case 27:			    //发送半自动模式
			{
				USART3_TxBuffer[12]='3';
				USART3_TxBuffer[13]='0';
				USART3_TxBuffer[14]='2';
				Send_XTRL_Time();
				XT_ONTime=0;
				USART3_SendWorld=0;
				break;
			}
			case 28:			    //发送自动模式
			{
				USART3_TxBuffer[12]='3';
				USART3_TxBuffer[13]='0';
				USART3_TxBuffer[14]='3';
				Send_XTRL_Time();
				XT_ONTime=0;
				USART3_SendWorld=0;
				break;
			}

			case 29:			//发送系统断电信息
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
			{//通过触摸屏处理的下岗				
					USART3_TxBuffer[12]='2';
					USART3_TxBuffer[13]='0';
					USART3_TxBuffer[14]='2';
					Send_RGXX_Time();
//					for(i=0;i<10;i++)	//清条码
//					    YG_Code[i]=0x30;
					YG_CodeLend=10;
					YG_Cnt=0;		//员工开模计数
					YG_Tim=0;		//员工计时
					On_Duty_Flag = 0;
					USART3_SendWorld=0;
				break;
			}
			case 31:
			{//发送QC用户名				
					USART3_TxBuffer[12]='2';
					USART3_TxBuffer[13]='2';
					USART3_TxBuffer[14]='5';
					
                    Send_QC_Name();

					for(i=0;i<2;i++)	//清条码
    					QC_Name[ i ] = 0x30;

					USART3_SendWorld=0;
				break;
			}
            case 32:
			{//发送QC密码				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '2';
					USART3_TxBuffer[14] = '6';
					
                    Send_QC_PassWord();

//					for(i=0;i<2;i++)	//清条码
//    					QC_PassWord[ i ] = ' ';
					
					USART3_SendWorld=0;
				break;
			}
            case 33:
			{//读QC不良统计-欠铸				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '2';
					USART3_TxBuffer[14] = '7';
					
                    Send_QCBad_QianZhu();

					for(i=0;i<4;i++)	//清条码
    					QCBad_QianZhu[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 34:
			{//读QC不良统计-拉模				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '2';
					USART3_TxBuffer[14] = '8';
					
                    Send_QCBad_LaMo();

					for(i=0;i<4;i++)	//清条码
    					QCBad_LaMo[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 35:
			{//读QC不良统计-砂孔				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '2';
					USART3_TxBuffer[14] = '9';
					
                    Send_QCBad_ShaKong();

					for(i=0;i<4;i++)	//清条码
    					QCBad_ShaKong[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 36:
			{//读QC不良统计-冷隔				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '0';
					
                    Send_QCBad_LengGe();

					for(i=0;i<4;i++)	//清条码
    					QCBad_LengGe[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 37:
			{//读QC不良统计-变形				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '1';
					
                    Send_QCBad_BianXing();

					for(i=0;i<4;i++)	//清条码
    					QCBad_BianXing[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 38:
			{//读QC不良统计-崩缺				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '2';
					
                    Send_QCBad_BengQue();

					for(i=0;i<4;i++)	//清条码
    					QCBad_BengQue[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 39:
			{//读QC不良统计-起泡				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '3';
					
                    Send_QCBad_QiPao();

					for(i=0;i<4;i++)	//清条码
    					QCBad_QiPao[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 40:
			{//读QC不良统计-露管				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '4';
					
                    Send_QCBad_LuGuan();

					for(i=0;i<4;i++)	//清条码
    					QCBad_LuGuan[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 41:
			{//读QC不良统计-积炭				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '5';
					
                    Send_QCBad_JiTan();

					for(i=0;i<4;i++)	//清条码
    					QCBad_JiTan[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 42:
			{//读QC不良统计-尺寸				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '6';
					
                    Send_QCBad_ChiChen();

					for(i=0;i<4;i++)	//清条码
    					QCBad_ChiChen[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 43:
			{//读QC不良统计-黑印				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '7';
					
                    Send_QCBad_HeiYin();

					for(i=0;i<4;i++)	//清条码
    					QCBad_HeiYin[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 44:
			{//读QC不良统计-其他				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '8';
					
                    Send_QCBad_QiTa();

					for(i=0;i<4;i++)	//清条码
    					QCBad_QiTa[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
            case 45:
			{//发QC原料成分分析				
					USART3_TxBuffer[12] = '2';
					USART3_TxBuffer[13] = '3';
					USART3_TxBuffer[14] = '9';
					
                    Send_QC_ChengFen();

					for(i=0;i<10;i++)	//清条码
    					QC_ChengFen[i] = 0x30;
					
					USART3_SendWorld=0;
				break;
			}
			case 46:
			{//发送QC注销信息				
					USART3_TxBuffer[12]='2';
					USART3_TxBuffer[13]='4';
					USART3_TxBuffer[14]='0';
					
                    Send_QC_ZhuXiao();

//					for(i=0;i<2;i++)	//清条码
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
	函数名： Send_AMR()
	描述：	 将本机地址、机器类型、运行状态指令字输入发送缓冲区
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_AMR(void)		
{
	u8 i;
	USART3_TxBuffer[0]='|';	
	for(i=0;i<3;i++)
	USART3_TxBuffer[i+1]=ADDRESS[i];			//本机地址
	USART3_TxBuffer[4]='|';	
	for(i=3;i<5;i++)
	USART3_TxBuffer[i+2]=MACHINE_TYPE[i-3];	//机器类型
	USART3_TxBuffer[7]='|';	
	USART3_TxBuffer[8]=RUN_STATE;			//机器运行状态
	USART3_TxBuffer[9]='|';	
	USART3_TxBuffer[10]=Code_Mode;			//指令字
	USART3_TxBuffer[11]='|';	
	for(i=0;i<3;i++)
	USART3_TxBuffer[12+i]=Command[i];		//命令
	USART3_TxBuffer[15]='|';
}



/*
	函数名： Receive_ETH()
	描述：	 从以太网接收到数据处理
	输入：	 无
	输出:	 无
	返回：	 无
*/
void Receive_ETH()
{
	if(USART3_Flag & 0X20)		//查看有无新数据
	{
		if(ADDRESS[0]==USART3_RxBuffer[0] && ADDRESS[1]==USART3_RxBuffer[1] \
			&& ADDRESS[2]==USART3_RxBuffer[2] )  //查看是否是本机编号
		{
			if(USART3_RxBuffer[USART3_RxCounter-1]==0 && \
				USART3_RxBuffer[USART3_RxCounter-2]==0 && \
				 USART3_RxBuffer[USART3_RxCounter-3]==0 && \
				  USART3_RxBuffer[USART3_RxCounter-4]==0)		//查看校验位是否为0
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
	u16_char(Address,&USART3_TxBuffer[1]);		//地址
	USART3_TxBuffer[4]='|';
	USART3_TxBuffer[5]='A';
	USART3_TxBuffer[6]='|';
	USART3_TxBuffer[7]='1';
	USART3_TxBuffer[8]='0';
	USART3_TxBuffer[9]='3';
	USART3_TxBuffer[10]='|';
	USART3_TxBuffer[11]=(MJ_CodeLend%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[12]=MJ_CodeLend%10+48;		//数据长度，个位数
	USART3_TxBuffer[13]='|';
	for(i=0;i<MJ_CodeLend;i++)
	USART3_TxBuffer[14+i]=MJ_Code[i];		//磨具条码
	USART3_TxBuffer[14+MJ_CodeLend]='|';
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[15+MJ_CodeLend], 15+MJ_CodeLend);	//计算校验码
	USART3_TxBuffer[19+MJ_CodeLend]=0x0D;		//结束符
	USART3_TxBuffer[20+MJ_CodeLend]=0x0A;
	USART3_TxLen=21+MJ_CodeLend;
	USART3_SendDW();
}

/*
	函数名称： Send_MJ_Over()
	描述：	   模具被拆开，发送计时，命令111
	输入：	   无
	输出：	   无
	返回：	   无
*/
void Send_MJ_Code()
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((MJ_CodeLend+16)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(MJ_CodeLend+16)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';
	for(i=0;i<MJ_CodeLend;i++)
	USART3_TxBuffer[19+i]=MJ_Code[i];		//模具条码
	USART3_TxBuffer[19+MJ_CodeLend]='|';	//分隔符
	Cu32_To_Char(MJ_Cnt,&USART3_TxBuffer[20+MJ_CodeLend]);	//开模计数值
	USART3_TxBuffer[26+MJ_CodeLend]='|';
	Cu32_To_Char(MJ_Tim,&USART3_TxBuffer[27+MJ_CodeLend]);	//开模计数值
	USART3_TxBuffer[33+MJ_CodeLend]='|';
	USART3_TxLen=40+MJ_CodeLend;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();		//发送数据
}

/*
	函数名： Send_XTRL_Time()
	描述：	 发送系统上电信息，断电信息；熔炉上电信息，熔炉断电信息
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_XTRL_Time(void)
{
	Change_TxBuffer();
	if((USART3_SendWorld==9) || (USART3_SendWorld==10))
	Cu32_To_Char(XT_ONTime,&USART3_TxBuffer[16]);	//系统上电、断电信息
	else
	Cu32_To_Char(RL_ONTime,&USART3_TxBuffer[16]);	//熔炉上电、断电信息
	USART3_TxBuffer[22]='|';
	USART3_TxLen=29;		//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;			//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();	
}

/*
	函数名： Send_RGXX_Time(void)
	描述：	 发送员工上岗，上岗结束信息
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_RGXX_Time(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+22)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+22)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';
	for(i=0;i<YG_CodeLend;i++)
		USART3_TxBuffer[19+i]=YG_Code[i];		//员工条码
	
	USART3_TxBuffer[19+YG_CodeLend]='|';	//分隔符
	Cu32_To_Char(YG_Cnt,&USART3_TxBuffer[20+YG_CodeLend]);	//该员工开模计数值
	USART3_TxBuffer[26+YG_CodeLend]='|';
	Cu32_To_Char(YG_Tim,&USART3_TxBuffer[27+YG_CodeLend]);	//该员工上岗时间
	USART3_TxBuffer[33+YG_CodeLend]='|';
	USART3_TxBuffer[34+YG_CodeLend]= Working_Type[1];			//发送工种，通过触摸屏
	USART3_TxBuffer[35+YG_CodeLend]= Working_Type[0];
	USART3_TxBuffer[36+YG_CodeLend]= '|';
	USART3_TxBuffer[37+YG_CodeLend]= Working_Process[1];	//发送工序
	USART3_TxBuffer[38+YG_CodeLend]= Working_Process[0];
	USART3_TxBuffer[39+YG_CodeLend]= '|';	
	USART3_TxLen=46+YG_CodeLend;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}
/*
	函数名： Send_QC_Input_Data(void)
	描述：	 发送QC输入的合格率
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QC_Input_Data(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+22)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+22)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend;i++)
		USART3_TxBuffer[19+i]=YG_Code[i];		//员工条码

	USART3_TxBuffer[19+YG_CodeLend]='|';

	for(i=0;i<YG_CodeLend;i++)
		USART3_TxBuffer[30+i]=QC_Input_Data[i];		//QC输入的数据
	
	USART3_TxBuffer[40]='|';	//分隔符
//	Cu32_To_Char(YG_Cnt,&USART3_TxBuffer[20+YG_CodeLend]);	//该员工开模计数值
//	USART3_TxBuffer[26+YG_CodeLend]='|';
//	Cu32_To_Char(YG_Tim,&USART3_TxBuffer[27+YG_CodeLend]);	//该员工上岗时间
//	USART3_TxBuffer[33+YG_CodeLend]='|';
//	USART3_TxBuffer[34+YG_CodeLend]= '0';			//发送工种，通过触摸屏
//	USART3_TxBuffer[35+YG_CodeLend]= '0';
//	USART3_TxBuffer[36+YG_CodeLend]= '|';
//	USART3_TxBuffer[37+YG_CodeLend]= '0';	//发送工序
//	USART3_TxBuffer[38+YG_CodeLend]= '0';
//	USART3_TxBuffer[39+YG_CodeLend]= '|';	
	USART3_TxLen=47;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}

/*
	函数名： Send_Mould_Defect(void)
	描述：	 发送模具缺陷
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_Mould_Defect(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+22)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+22)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';
	for(i=0;i<YG_CodeLend+22;i++)
		USART3_TxBuffer[19+i]=Mould_Defect[i];		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend+22]='|';	//分隔符
//	Cu32_To_Char(YG_Cnt,&USART3_TxBuffer[20+YG_CodeLend]);	//该员工开模计数值
//	USART3_TxBuffer[26+YG_CodeLend]='|';
//	Cu32_To_Char(YG_Tim,&USART3_TxBuffer[27+YG_CodeLend]);	//该员工上岗时间
//	USART3_TxBuffer[33+YG_CodeLend]='|';
//	USART3_TxBuffer[34+YG_CodeLend]= '0';			//发送工种，通过触摸屏
//	USART3_TxBuffer[35+YG_CodeLend]= '0';
//	USART3_TxBuffer[36+YG_CodeLend]= '|';
//	USART3_TxBuffer[37+YG_CodeLend]= '0';	//发送工序
//	USART3_TxBuffer[38+YG_CodeLend]= '0';
//	USART3_TxBuffer[39+YG_CodeLend]= '|';	
	USART3_TxLen=26+YG_CodeLend+22;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}
/*
	函数名： Send_QC_Name(void)
	描述：	 发送QC用户名
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QC_Name(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-8;i++)
		USART3_TxBuffer[19+i] = QC_Name[i] ;		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend-8]='|';	//分隔符
	USART3_TxLen=26+YG_CodeLend-8;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}
/*
	函数名： Send_QC_PassWord(void)
	描述：	 发送QC用户名
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QC_PassWord(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-8;i++)
		USART3_TxBuffer[19+i] = QC_PassWord[i] ;		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend-8]='|';	//分隔符

	USART3_TxLen=26+YG_CodeLend-8;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}
/*
	函数名： Send_QCBad_QianZhu(void)
	描述：	 发送QC用户名
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QCBad_QianZhu(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_QianZhu[i];		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//分隔符

	USART3_TxLen=26+YG_CodeLend-6;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}
/*
	函数名： Send_QCBad_LaMo(void)
	描述：	 发送QC用户名
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QCBad_LaMo(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_LaMo[i];		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//分隔符

	USART3_TxLen=26+YG_CodeLend-6;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}
/*
	函数名： Send_QCBad_ShaKong(void)
	描述：	 发送QC用户名
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QCBad_ShaKong(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_ShaKong[i];		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//分隔符

	USART3_TxLen=26+YG_CodeLend-6;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}
/*
	函数名： Send_QCBad_LengGe(void)
	描述：	 发送QC用户名
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QCBad_LengGe(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_LengGe[i];		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//分隔符

	USART3_TxLen=26+YG_CodeLend-6;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}
/*
	函数名： Send_QCBad_BianXing(void)
	描述：	 发送QC用户名
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QCBad_BianXing(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_BianXing[i];		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//分隔符

	USART3_TxLen=26+YG_CodeLend-6;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}

/*
	函数名： Send_QCBad_BengQue(void)
	描述：	 发送QC用户名
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QCBad_BengQue(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_BengQue[i];		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//分隔符

	USART3_TxLen=26+YG_CodeLend-6;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}

/*
	函数名： Send_QCBad_QiPao(void)
	描述：	 发送QC用户名
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QCBad_QiPao(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_QiPao[i];		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//分隔符

	USART3_TxLen=26+YG_CodeLend-6;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}

/*
	函数名： Send_QCBad_LuGuan(void)
	描述：	 发送QC用户名
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QCBad_LuGuan(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_LuGuan[i];		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//分隔符

	USART3_TxLen=26+YG_CodeLend-6;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}

/*
	函数名： Send_QCBad_JiTan(void)
	描述：	 发送QC用户名
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QCBad_JiTan(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_JiTan[i];		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//分隔符

	USART3_TxLen=26+YG_CodeLend-6;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}

/*
	函数名： Send_QCBad_ChiChen(void)
	描述：	 发送QC用户名
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QCBad_ChiChen(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_ChiChen[i];		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//分隔符

	USART3_TxLen=26+YG_CodeLend-6;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}

/*
	函数名： Send_QCBad_HeiYin(void)
	描述：	 发送QC用户名
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QCBad_HeiYin(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_HeiYin[i];		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//分隔符

	USART3_TxLen=26+YG_CodeLend-6;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}

/*
	函数名： Send_QCBad_QiTa(void)
	描述：	 发送QC用户名
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QCBad_QiTa(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend-6;i++)
		USART3_TxBuffer[19+i] = QCBad_QiTa[i];		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend-6]='|';	//分隔符

	USART3_TxLen=26+YG_CodeLend-6;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}
/*
	函数名： Send_QC_ChengFen(void)
	描述：	 发送QC用户名
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QC_ChengFen(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend;i++)
		USART3_TxBuffer[19+i] = QC_ChengFen[i];		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend]='|';	//分隔符

	USART3_TxLen=26+YG_CodeLend;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}
/*
	函数名： Send_QC_ZhuXiao(void)
	描述：	 发送QC用户名
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Send_QC_ZhuXiao(void)
{
	u8 i;
	Change_TxBuffer();
	USART3_TxBuffer[16]=((YG_CodeLend+4)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(YG_CodeLend+4)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';

	for(i=0;i<YG_CodeLend;i++)
		USART3_TxBuffer[19+i] = '1';		//QC输入的数据
	
	USART3_TxBuffer[19+YG_CodeLend]='|';	//分隔符

	USART3_TxLen=26+YG_CodeLend;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;	//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();						//发送数据
}
/*
	函数名： Change_TxBuffer()
	描述：	 更改发送缓冲区里的地址，机器类型，运行状态，指令字
	输入：	 无
	输出：	 无
	返回	 无
*/
void Change_TxBuffer(void)
{
	u8 i;
	USART3_TxBuffer[0]='|';	
	for(i=0;i<3;i++)
	USART3_TxBuffer[i+1]=ADDRESS[i];			//本机地址
	USART3_TxBuffer[4]='|';	
	for(i=3;i<5;i++)
	USART3_TxBuffer[i+2]=MACHINE_TYPE[i-3];	//机器类型
	USART3_TxBuffer[7]='|';	
	USART3_TxBuffer[8]=RUN_STATE;			//机器运行状态
	USART3_TxBuffer[9]='|';	
	USART3_TxBuffer[10]=Code_Mode;			//指令字
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
	USART3_TxBuffer[16]=((MJ_CodeLend+16)%100)/10+48;	//数据长度，十位数
	USART3_TxBuffer[17]=(MJ_CodeLend+16)%10+48;			//数据长度，个位数
	USART3_TxBuffer[18]='|';
	for(i=0;i<MJ_CodeLend;i++)
	USART3_TxBuffer[19+i]=MJ_Code[i];		//模具条码
	USART3_TxBuffer[19+MJ_CodeLend]='|';	//分隔符
	Cu32_To_Char(MJ_Cnt,&USART3_TxBuffer[20+MJ_CodeLend]);	//开模计数值
	USART3_TxBuffer[26+MJ_CodeLend]='|';
	Cu32_To_Char(MJ_Tim,&USART3_TxBuffer[27+MJ_CodeLend]);	//开模计数值
	USART3_TxBuffer[33+MJ_CodeLend]='|';
	USART3_TxLen=40+MJ_CodeLend;			//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[len-6], len-6);	//计算校验码
	USART3_TxBuffer[len-2]=0x0D;	//结束符
	USART3_TxBuffer[len-1]=0x0A;
	USART3_SendDW();
}

/*
	函数名： void Reset_Msg(void)
	描述：	 重启发送提示
*/
void Reset_Mag(void)
{
	Send_AMR();		//发送机器编号、类型，编号，状态、指令、命令，以上有15个数据
	USART3_TxBuffer[10]='S';
	USART3_TxBuffer[12]='0';
	USART3_TxBuffer[13]='0';
	USART3_TxBuffer[14]='0';
	USART3_TxLen=22;		//计算发送数据长度
	Data_CRC(USART3_TxBuffer, &USART3_TxBuffer[USART3_TxLen-6], USART3_TxLen-6);	//计算校验码
	USART3_TxBuffer[USART3_TxLen-2]=0x0D;		//结束符
	USART3_TxBuffer[USART3_TxLen-1]=0x0A;
	USART3_SendDW();		//发送数据*/
}

void Clear_RxBuffer(void)
{
	u8 i;
	for (i=0;i<42;i++)
		USART3_RxBuffer[i] = 0;
}
