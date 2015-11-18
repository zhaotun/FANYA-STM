/*
	串口4,员工读卡器相关
*/
#include "serial4.h"
#include "stm32f10x.h"
#include "include.h"


void UART4_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//使能USART4时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//使能USART3时钟
	//USART4端口配置
	//配置USART3 TX(PC10)为复用功能推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//配置USART3 RX(PC11)为浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//配置USART1串行口
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);
	//使能发送中断，该中断产生时的USART1发送数据寄存器为空
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	//使能接收中断，该中断产生时，USART1接收数据寄存器不为空
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	//使能USART1
	USART_Cmd(UART4, ENABLE);
	//使能USART3中断
	NVIC_InitStructure.NVIC_IRQChannel=UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/*
	函数名：	USART3_ReceiveData
	描述：		串口3接收处理
				数据帧：起始字 设备编号 指令字 逻辑号 【长度+数据】 校验字 结束字
	输入：		u8 tmp：接收到的数据
	输出：		无
 	返回：		无
*/

u8 UART4_Flag=0;		//串口4收发标志位,b4=1(开始接收数据),b6=1(接收到正确的数据)
u8 UART4_RxCounter=0;	//串口4接收计数器
u8 UART4_RxBuffer[UART4_RxBufferSizeMax];		//串口4接收缓冲区
u8 UART4_RX_Timer=0;
u8 UART4_RXFLAG=0;
u8 UART4_RXNEXT=0;
void UART4_ReceiveDA(u8 buffer)
{
	UART4_Flag |= b4;		//标志开始接收
	UART4_RxBuffer[UART4_RxCounter++] =buffer;  
	if(buffer == EOX)
	{
		if(UART4_RxBuffer[UART4_RxCounter-2] == EOR)	//接收完成
		{
			if(IsChar(UART4_RxBuffer,UART4_RxCounter-2) && (UART4_RxCounter == 12))	//判断是否是可显示的字符
			{
				UART4_Flag &= 0x0F;		
				UART4_Flag |= b6;		//接收完成
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
	else if(UART4_RxCounter >= UART4_RxBufferSizeMax)	//等于接收缓冲区
	{
		UART4_Flag &= 0x0F;		
		UART4_Flag |= 0X40;			//接收错误
		UART4_RxCounter=0;
	}
	else if(UART4_RX_Timer >=100)		//接收超过20ms表示接收错误
	{
		UART4_Flag &= 0x0F;		
		UART4_RX_Timer=0;
		UART4_RxCounter=0;
	}	
} 

/*
*	函数名：UART4_ReceiveDW()
*	描述：	串口4数据接收处理
*	输入：	无
*	输出： 	无
*	返回：	无
*/
u8 YGTM_Flag=0;		//员工条码标志位,b1=1(当前有员工),b2=1(当前有员工条码，但接收到不同的员工条码),b3=1(当前无员工条码，接收到新的员工调码，但当前员工计数器不等于0)
u8 New_YG_Code[20];	//新的员工条码
u8 New_YG_CodeLend=0;	//新员工条码长度
u8 YUANGONG=0;			//用于第二个员工打卡关灯显示
u8 YUANGONG_TIME=0;		//关灯计时
u8 Old_New_Flag = 0;
extern u8 YG_Code[];			//员工模具条码
extern u8 YG_CodeLend;
extern u8 USART3_SendWorld;
extern u8 Work_Status_Flag;
extern u8 Online_Status;

extern u32 YG_Cnt;
extern u8 Working_Type[2];	//工种
extern u8 Working_Process[2];	//工序

void UART4_ReceiveDW()			//接收到的数据处理
{
	if (USART3_SendWorld != 0) return;
	if(YGTM_Flag & b2)
	{//当前有员工条码，接收到新的员工条码
		u8 i;
		YGTM_Flag &= ~b2;	//清标志位
		YGTM_Flag |= b1;	//标志当前有条码
		for(i=0;i<New_YG_CodeLend;i++)
		    YG_Code[i]=New_YG_Code[i];
		YG_CodeLend=New_YG_CodeLend;
//		Work_Status_Flag = Online_Status;
        Work_Status_Flag = 0x0A;	//当前有员工条码的情况下，先下岗，再上岗，然后切换到上岗页面
//		USART3_SendWorld=13;	    //发送新条员工码数据
		return;
	}
	else if(YGTM_Flag & b3)
	{//当前没有员工，接收到新的员工条码，但当前员工计数器不等于0
		u8 i;
		YGTM_Flag &= ~b3;	//清标志位
		YGTM_Flag |= b1;	
		for(i=0;i<New_YG_CodeLend;i++)
		YG_Code[i]=New_YG_Code[i];
		YG_CodeLend=New_YG_CodeLend;
		Work_Status_Flag = 0x0A;
//		USART3_SendWorld=13;
	} 
	else if(UART4_Flag & b6)
	{//接收到数据
		if(YGTM_Flag & b1)
		{//当前有员工条码
			if(YG_CodeLend == (UART4_RxCounter-2))
			{//新接收的员工条码与旧的员工条码长度相同
				if(Char_Compare(YG_Code,&UART4_RxBuffer[0],YG_CodeLend))
				{//接收到相同的条码，认为员工结束上岗		
					YGTM_Flag &= ~b1;	
					
                    Work_Status_Flag = Online_Status;		//下岗标志，给触摸屏切换到下岗页面用

				}
				else
				{//接收到不同的员工条码//有员工条码情况下接受到新条码
					u8 i;
					YGTM_Flag &= ~b1;
//					YGTM_Flag |= b2;

                    Old_New_Flag = 1;

                    Work_Status_Flag = Online_Status;
//                    USART3_SendWorld=23;			//发送默认的下岗，因为这个时候没有触摸屏控制选择下岗
					
                    for(i=0;i<UART4_RxCounter-2;i++)
					{
                        New_YG_Code[i]=UART4_RxBuffer[i];
//                        YG_Code[i]=UART4_RxBuffer[i];
                    }
					New_YG_CodeLend=UART4_RxCounter-2;
//					YG_CodeLend=UART4_RxCounter-2;

                    YUANGONG=1;							//新员工标志,用于灯闪烁
					YUANGONG_TIME=0;					//计时清零
				
                }
			}
			else
			{//新接收的员工条码与旧的员工条码长度不相同，则认为接收到新条码
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
				USART3_SendWorld=23;				//发送默认的下岗，因为这个时候没有触摸屏控制选择下岗
				for(i=0;i<UART4_RxCounter-2;i++)
				New_YG_Code[i]=UART4_RxBuffer[i];
				New_YG_CodeLend=UART4_RxCounter-2;
				YUANGONG=1;							//新员工标志,用于灯闪烁
				YUANGONG_TIME=0;					//计时清零
			}
		}
		else
		{//当前无员工条码，则认为接收到新条码
			u8 i;
			if(YG_Cnt != 0)
			{//当前员工计数器不等于0，则先发送员工下岗信息再发送上岗信息
				for(i=0;i<UART4_RxCounter-2;i++)
				    New_YG_Code[i] = UART4_RxBuffer[i];

				New_YG_CodeLend=UART4_RxCounter-2;

				YGTM_Flag |= b3;	

				USART3_SendWorld=23;		//发送默认的下岗，因为这个时候没有触摸屏控制选择下岗
			}
			else 
			{//当前员工计数器等于0
				for(i=0;i<UART4_RxCounter-2;i++)
				    YG_Code[i] = UART4_RxBuffer[i];
				
                YG_CodeLend = UART4_RxCounter-2;
				
                YGTM_Flag |= b1;	
				
                Work_Status_Flag = 0x0A;   //等于新员工上岗，直接切换到上岗页面

			}
		}
		UART4_Flag &= ~b6;
		UART4_RxCounter=0;			//清零
	}
}

/*
	函数名： u8 YuanGong_Online(void)
	返回当前是否有员工在线，若有则返回1，否则返回0
*/
u8 YuanGong_Online(void)
{
	if(YGTM_Flag & 0x01)
		return 1;
	else 
		return 0;
}

