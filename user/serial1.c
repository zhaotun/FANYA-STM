/*
	此串口为磨具超高频读卡器接口
*/
#include "serial1.h"

#include "stm32f10x.h"
#include "include.h"

u8  USART1_Flag;				//收发标志位
u8  USART1_TxLen;				//发送数据长度
u8  USART1_TxBuffer[255];		//发送数据缓冲区
u8  USART1_TxCounter;			//发送数据计数器

u8 	USART1_RxBuffer[USART1_RxBufferSizeMax];	//接收数据缓冲区
u8  USART1_RxCounter = 0; 						//接收数据计数器
u8  USART1_RX_Timer= 0;			                //串口1接收计时器

u16  NOT_URFID_data=0;			//无超高频数据统计
u16  New_MJ_Counter=0;			//新模具标签计数
u8	New_MJ_CodeLend=0;			//新模具长度
u8	New_MJ_Code[25];			//新模具标签数据

u8	UHRFID_Flag=0;		//超高频标签相关标志位,b1有模具,b2当前有模具，但接收到新模具,b3在没有模具条码时,读取到上一次的模具条码
						//b4,当前有模具，但读取到新模具，让灯先灭2秒,b5当前没有模具，读取到新模具，

extern u8 USART3_SendWorld;	//串口3发送指令字
extern u8 MJ_CodeLend;		//磨具条码长度
extern u8 MJ_Code[];
extern u32 MJ_Cnt;
extern u32 YG_Cnt;
extern u8 UHRFID_Manual_Input_Flag;

struct tiaoma New_MoJu,Old_MoJu,Nul_MoJu;

/*******************************************************************************
* 函数名		：USART1_Init
* 描述			：配置USART1串行口
* 输入			：无
* 输出			：无
* 返回			：无
*******************************************************************************/
void USART1_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//使能USART1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);	//使能GPIOA时钟,使用IO前必须使能相应的总线
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能USART1时钟，使用串口前必须使能相应的总线

	//配置USART1 TX(PA.09)为复用功能推挽输出				
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				//PA9为USART1的TX引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//设置IO口的最高输出速率为50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//设置该IO口为复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//初始化GPIOA

	//配置USART1 RX(PA.10)为浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				//PA9为USART1的RX引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//设置该IO口为浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//初始化GPIOA

	//配置USART1串行口
	USART_InitStructure.USART_BaudRate = 57600;          					//设置波特率为57600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;			    //8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;					//1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;						//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			//使能TX,RX
	USART_Init(USART1, &USART_InitStructure);								//初始化USART1
	//使能发送中断，该中断产生时的USART1发送数据寄存器为空
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	//使能接收中断，该中断产生时，USART1接收数据寄存器不为空
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);						    //使能USART1接收中断

	//使能USART1
	USART_Cmd(USART1, ENABLE);												//USART1使能

	//使能USART1全局中断
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;							//使能USART4中断服务程序
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;					//串口中断的抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;						//串口中断的子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							//串口中断使能
	NVIC_Init(&NVIC_InitStructure);											//初始化串口中断
}

/****************************************************************************************************
*	函数名：USART1_SendDW()
*	描述：	此函数被USART1_ReadURFID()调用，用于启动发送中断
*	输入：	无
*	输出：	无
*	返回： 	无 
****************************************************************************************************/
void USART1_SendDW()
{
	USART1_TxCounter=0;				  //发送计数器清零
	USART1_Flag &=0xF0;			      //标志位清零
	USART_SendData(USART1, USART1_TxBuffer[USART1_TxCounter++]); //发送第一个字节，后续字节由发送中断服务程序自动发送
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE); //使能串口1发送中断，启动发送
}

/*************************************************************************************
*	函数名：USART1_SendDA()	
*	描述：	此函数在串口1发送中断服务程序USART1_IRQHandler()中调用，发送一个数据后重新触发中断
*	输入：	无
*	输出：	无
*	返回：	无
**************************************************************************************/
void USART1_SendDA()
{
	USART_SendData(USART1, USART1_TxBuffer[USART1_TxCounter++]); //单字节发送函数，每次发送一个字节，该函数为系统函数   
	if(USART1_TxCounter ==USART1_TxLen)                          //判断发送计数器是否等于所需发送字节的长度
	{
		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);			 //若发送计数器等于所需发送字节的长度，则关闭发送中断
		USART1_TxCounter=0;										 //发送计数器清零
		USART1_Flag |=0x01;	                                     //置发送完成标志位
	} 
}

/********************************************************************************
*	函数名：	USART1_ReceiveDA()
*	描述：		串口1接收从超高频读卡器发送的数据并保存
*	输入：		无
*	输出：		无
* 	返回：		无
*********************************************************************************/
void USART1_ReceiveDA()
{
	if(USART1_RX_Timer>=3)		//接收到第一个数据后，等待3MS，认为该数据接收完成
	{
		USART1_RX_Timer=0;		//接收计时器清零
		USART1_Flag &=0x0F;		//清标志位，计时器停止计时
		USART1_Flag |= b6;		//置接收完成标志位
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	//关闭串口1接收中断
	}
}

/**********************************************************************************
*	函数名：USART1_ReceiveDW()
*	描述：	对串口1接收的数据进行处理，该函数在main函数中调用
*	输入：	无
*	输出： 	无
*	返回：	无
***********************************************************************************/
extern u8 USART3_TxBuffer[255];
extern u8 USART3_TxLen;
	   u8 Old_MJ_CodeLend;
	   u8 Old_MJ_Code[30];
	   u16 New_Nul_MuJu;				//当前没有条码，但收到有新条码计数器
void USART1_ReceiveDW()					
{
	USART1_ReceiveDA();					        //接收完成判断
	if(USART3_SendWorld != 0) return;
	if(UHRFID_Flag & b2)
	{//当前有模具，但接收到新的模具
		u8 i;
		UHRFID_Flag &= ~b2;
		UHRFID_Flag |= b1 | b4;				
		for(i=0;i<USART1_RxBuffer[0]-7;i++)
		{
			MJ_Code[i]=New_MoJu.Code[i];		
			New_MoJu.Code[i] = '0';
		}
		MJ_Code[i]=0;
		MJ_CodeLend=New_MoJu.Len;			//磨具条码长度
		New_MoJu.Len = 10;
		USART3_SendWorld=8;					//发送新模具信息
		//------------
		printf("当前有模具，但读取到新模具\r\n");
		printf("%s\r\n",MJ_Code);
		//------------
	}
	else if(UHRFID_Flag & b3)
	{//在没有模具条码时，读取到上一次的模具条码
		UHRFID_Flag &= ~b3;
		UHRFID_Flag |= b1;
		USART3_SendWorld = 1;		//上传数据
		//------------
		printf("当前没有模具，读取到上一次相同的模具\r\n");
		printf("%s\r\n",MJ_Code);
		//------------
	}
	else if(UHRFID_Flag & b5)
	{//读取到新模具信息
		u8 i;
		for(i=0;i<USART1_RxBuffer[0]-7;i++)
		{
			MJ_Code[i]= USART1_RxBuffer[i+6];
		}
		MJ_Code[i]=0;
		MJ_CodeLend = USART1_RxBuffer[0]-7;	
		UHRFID_Flag &= ~b5;
		UHRFID_Flag |= b1;						//置接受到新标签标志位
		USART3_SendWorld=8;						//发送读取到新模具信息
		//------------
		printf("当前没有模具，读取到新模具\r\n");
		printf("%s\r\n",MJ_Code);
		//------------
	}
	if(USART1_Flag & b6)			       //若数据接收完成
	{
		u8 i;
		if(UHRFID_Flag & b1)			   //当前有模具条码
		{
			if(USART1_RxBuffer[0]<=7)	   //当接收到的数据长度不大于7时，即认为无模具条码
										   //根据通信协议，第一个字节代表数据长度
			{
				NOT_URFID_data++;		   //串口1每50ms向超高频发送一次读模具条码命令，若未接收到模具条码NOT_URFID_data加1
				if((NOT_URFID_data >= NOT_UHRFID)&& (User_GetTimer1sValue() >= NOT_KAIMO))
				{//若连续未接收到模具条码的次数达到18000（约18分钟后），且NOT_KAIMO（900）秒钟（15分钟）内没有开模，则认为模具被移开
					
					if(UHRFID_Manual_Input_Flag == 1)
					{
						 NOT_URFID_data=0;
						 printf("手动输入的模具不清零\r\n");	
					}
					else
					{
						printf("模具被移开了了了\r\n");
						printf("%s\r\n",MJ_Code);
						//------------
						NOT_URFID_data=0;		           //未接收到模具条码的次数清零
						User_Timer1sClear();			   //用户秒定时器时钟清零
						for(i=0;i<MJ_CodeLend;i++)
						{
							Old_MoJu.Code[i] = MJ_Code[i]; //保存当前模具条码
						}
						Old_MoJu.Code[i]=0;				   //???
						Old_MoJu.Len = MJ_CodeLend;		   //保存当前模具条码长度
						Old_MoJu.Flag |= b1;			   //旧模具标志位置位
						UHRFID_Flag &= ~b1;				   //模具标志位清零
						Nul_MoJu.Count=0;				   //空模具计数清零
						Nul_MoJu.Flag |= b1;			   //空模具标志位置位
	
						if(YG_Cnt == 0)					   //当此时员工开模数据为零时
						{
							USART3_SendWorld = 7;		   //USART3发送下模信息
						}
						else
						{
							USART3_SendWorld = 20;		   //当此时员工开模数据不为零时，先发送员工当前开模数据，再下模
						}
					}
				}
			}
			else if((IsChar(&USART1_RxBuffer[6],USART1_RxCounter-8))) //若返回的数据都是字符串，则认为接收到有效模具条码
			{
				NOT_URFID_data=0;						   //接收到无模具条码的次数清零
				if(MJ_CodeLend == USART1_RxBuffer[0]-7)	   //若接收到的模具条码长度等于当前模具条码长度
				{
					if(Char_Compare(MJ_Code,&USART1_RxBuffer[6],MJ_CodeLend)) //若接收到的模具条码与当前模具条码相同
					{
						New_MJ_Counter=0;				 //新模具计数置0，表明未接收到新模具
					}
					else								 //若接收到的模具条码与当前模具条码不同
					{
						if(Char_Compare(New_MoJu.Code,&USART1_RxBuffer[6],USART1_RxBuffer[0]-7))
						{//若下一次接收到的模具条码与保存的新模具条码相同
							New_MJ_Counter++;					      //新模具计数加1
							if(New_MJ_Counter >= NEW_UHRFID)	      //若新模具计数达到10次，则认为该模具已经更换为新模具
							{
								New_MJ_Counter = 0;					  //新模具计数清零
							
								for(i=0;i<USART1_RxBuffer[0]-7;i++)
									New_MoJu.Code[i] = USART1_RxBuffer[i+6];  //保存新模具条码

								New_MoJu.Len = USART1_RxBuffer[0]-7;  //保存新模具条码长度	
								
								Nul_MoJu.Flag &= ~b1;				  //空模具标志清零
								Nul_MoJu.Count = 0;					  //空模具计数清零		
								UHRFID_Flag |= b2;					  //		
								if(YG_Cnt != 0)						  //当员工开模数据不为0
								{
									USART3_SendWorld = 20;			  //先发送员工当前开模数据，再下模			
								}
								else
								{
									USART3_SendWorld = 7;			  //员工开模数据等于0时，发送下模信息
								}
							}
						}
						else//若下一次接收到的模具条码与保存的新模具条码不同，则重新更新模具条码
						{
							New_MJ_Counter=1;						        //新模具计数置1

							for(i=0;i<New_MoJu.Len;i++)
								New_MoJu.Code[i] = USART1_RxBuffer[i+6];	//保存新的模具条码

							New_MoJu.Len = USART1_RxBuffer[0]-7;		    //保存新的模具条码长度
						}
					}	
				}
				else  //若接收到的模具条码长度不等于当前模具条码长度，则肯定是一个新的模具
				{
					if(New_MoJu.Len == USART1_RxBuffer[0]-7) //若接收到的模具条码长度等于新的模具条码长度
					{
						if(Char_Compare(New_MoJu.Code,&USART1_RxBuffer[6],USART1_RxBuffer[0]-7))
						{ //若下一次接收到的模具条码与新的模具条码相同
							New_MJ_Counter++;					     //新模具计数加1
							if(New_MJ_Counter >= NEW_UHRFID)		 
							{//若新模具计数达到10次，则认为该模具已经更换为新模具
								UHRFID_Flag |= b2;						    //新模具条码接收标志位置位
								
								for(i=0;i<USART1_RxBuffer[0]-7;i++)
									New_MoJu.Code[i] = USART1_RxBuffer[i+6];//保存新模具条码
							
								New_MoJu.Len = USART1_RxBuffer[0]-7;		//保存新模具条码长度
								Nul_MoJu.Flag &= ~b1;						//空模具标志清零
								Nul_MoJu.Count = 0;					        //空模具计数清零	
								New_MJ_Counter = 0;							//新模具计数清零
								if(YG_Cnt != 0)								//当员工开模数据不为0
								{
									USART3_SendWorld = 20;					//先发送员工当前开模数据，再下模	
								}
								else
								{
									USART3_SendWorld = 7;					//员工开模数据等于0时，发送下模信息
								}
							}
						}
						else//若下一次接收到的模具条码与新的模具条码不同 
						{
							New_MJ_Counter=1;		                	  //新模具计数置1
							
							for(i=0;i<New_MoJu.Len;i++)
								New_MoJu.Code[i] = USART1_RxBuffer[i+6];  //保存新模具条码
							
							New_MoJu.Len = USART1_RxBuffer[0]-7;		  //保存新模具条码长度
						}
					}
					else //若接收到的模具条码长度不等于当前模具条码长度，则肯定是一个新的模具
					{
						New_MJ_Counter=1;							//新模具计数置1
						
						for(i=0;i<New_MoJu.Len;i++)
							New_MoJu.Code[i] = USART1_RxBuffer[i+6];//保存新模具条码
						
						New_MoJu.Len = USART1_RxBuffer[0]-7;		//保存新模具条码长度
					}
				}
			}
			else
			{//返回的数据乱码
				NOT_URFID_data++;                        //未接收到模具条码的次数加1
				printf("读取到乱码NOT_URFID_data=%u\r\n",NOT_URFID_data);
				if((NOT_URFID_data >= NOT_UHRFID)&& (User_GetTimer1sValue() >= NOT_KAIMO)) 
				{//若连续未接收到模具条码的次数达到18000（约18分钟后），且NOT_KAIMO（900）秒钟（15分钟）内没有开模，则认为模具被移开
					if(UHRFID_Manual_Input_Flag == 1)
					{
						 NOT_URFID_data=0;
						 printf("手动输入的模具不清零\r\n");	
					}
					else
					{
						NOT_URFID_data=0;					//未接收到模具条码的次数清零
						User_Timer1sClear();				//用户秒定时器时钟清零
						
						for(i=0;i<MJ_CodeLend;i++)
							Old_MoJu.Code[i] = MJ_Code[i];	//保存当前模具信息到Old_MoJu中
						
						Old_MoJu.Len = MJ_CodeLend;			//保存当前模具条码长度
						Old_MoJu.Flag |= b1;				//旧模具标志置位
						UHRFID_Flag &= ~b1;			        //有条码标志位置位
						Nul_MoJu.Flag |= b1;
						USART3_SendWorld = 20;		        //先发送员工当前开模数据，再下模
					}
				}
			}
		}
		else   //当前无模具条码，
		{
			if(USART1_RxBuffer[0] > 7)		
			{//接收到有条码数据
				if(IsChar(&USART1_RxBuffer[6],USART1_RxCounter-8))			//有条码数据，判断是否是字符串
				{//接收到的数据是字符串
					NOT_URFID_data=0;
					if((Old_MoJu.Len == USART1_RxBuffer[0]-7) && Char_Compare(Old_MoJu.Code,&USART1_RxBuffer[6],USART1_RxBuffer[0]-7)) //判断是否跟上一次的旧条码数据相同
					{//与上一次的旧模具相同
						for(i=0;i<USART1_RxBuffer[0]-7;i++)
						{
							MJ_Code[i] = Old_MoJu.Code[i];		//保存模具条码
							Old_MoJu.Code[i] = '0';
						}
						MJ_CodeLend = Old_MoJu.Len;				//保存模具条码长度
						MJ_Code[i]=0;	                    	//结尾
						MJ_Cnt = Nul_MoJu.Count;
						YG_Cnt = Nul_MoJu.Count;
						Nul_MoJu.Count=0;
						Old_MoJu.Len = 10;
						Nul_MoJu.Flag &= ~b1;
						UHRFID_Flag |= b3;						//当前没有模具，但接受到上一次相同的模具
						USART3_SendWorld=21;					//发送读取到新模具信息
					}
					else
					{//不同
						if(Char_Compare(New_MoJu.Code,&USART1_RxBuffer[6],New_MoJu.Len)) //判断是否跟上一次的新条码数据相同
						{//接收到与上次的新标签相同的新标签
							New_MJ_Counter++;
							if(New_MJ_Counter >= NEW_UHRFID)
							{//当前没有模具，读取到新模具上模
								New_MJ_Counter = 0;						//不同标签计数清零
								Nul_MoJu.Flag &= ~b1;
								Nul_MoJu.Count = 0;					//清空计数
								UHRFID_Flag |= b5;						//置接受到新标签标志位
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
						{//若接收到不同的新标签，重新更新新标签
							New_MJ_Counter=1;
							for(i=0;i < New_MoJu.Len;i++)
							New_MoJu.Code[i] = USART1_RxBuffer[i+6];	//存储磨具条码
							New_MoJu.Len = USART1_RxBuffer[0]-7;		//磨具条码长度
						}
					}
				}
			}
		}
		USART1_Flag &= 0x0F;		//清标识位
		USART1_RxCounter=0;			//清零
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//开中断
	}
}

/*
	函数名： void Nul_MuJuCount(void)
	描述：	 当模具为空时，进行计数
	输入：	 无
	输出：	 无
*/
void Nul_MuJuCount(void)
{
	if(Nul_MoJu.Flag & b1)
	{
		Nul_MoJu.Count++;
	}
}

/*
*	函数名：USART1_SendError()
*	描述：	串口1发送”Error!\n"信息
*	输入：	无
*	输出：	无
*	返回：	无
*/
void USART1_SendError()		//暂未使用
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
	函数名：USART1_ReadURFID()
	描述：  此函数被stm32f10x_it.c头文件中的SysTick_Handler()函数调用，每50ms执行一次
	功能：  用于向超高频发送模具条码读取命令
	输入：  无
	输出：	无
	返回：	无
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
	函数名： void URFID_Timer1S(void)
	描述： 	 超高频读卡器秒时钟,秒中断常调用
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
	函数名： Receive_CRC(u8 *p,u8 len)
	描述：	 计算接收数据的CRC，入口为*p,长度为len的数据，返回为
*/

/*
	判断是否是模具“CCC008D02-1 ”，若是则返回真"1"，否则返回假"0"
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


