/**
  ******************************************************************************
  * @file    Project/Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    04/06/2009
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "include.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern u16  Time_Count;
extern u8   USART1_RX_Timer;		//串口1接收计时器
extern u8  	USART1_Flag;			//串口1收发相关标志位
extern u8  	USART3_Flag;			//串口1收发相关标志位
extern u8	UHRFID_Flag;		//超高频读卡器相关标志位
extern u32  MJ_Tim;		//磨具计时
extern u32  YG_Tim;		//员工计时
extern u8   Input_Flag;	//输入相关标志位

u16	 Heart_Counter=0;


u8	minute=0;			//分钟计时

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval : None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval : None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval : None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval : None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval : None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval : None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval : None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval : None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval : None
  */		
extern u8 UART4_RX_Timer;		//串口4接收计时
extern u8 UART4_Flag;			//串口4收发标志位
u8 uhfCard_Timer;				//读取模具RFID卡数据
void SysTick_Handler(void)		//1mS计时
{
	Input_InputStateCollect();		//输入管脚状态采集
	Communicate_timer();
	Time_Count++;
	if(Time_Count==1000) 
	{
		Time_Count=0;
		OneSecond_Handler();

	}
	uhfCard_Timer++;
	if(uhfCard_Timer >= 50)
	{
		uhfCard_Timer=0;
		USART1_ReadURFID();	//读取模具RFID卡数据，每100ms读取一次
	}
	//串口1相关
	if(USART1_Flag & 0x10) 
	{
		USART1_RX_Timer++;	//串口1接收数据计时
	}
	if(UART4_Flag & 0x10) 
	{
		UART4_RX_Timer++;	//串口4接收数据计时	
	}		
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/


/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern u8 Send_Flag;
/*
	TIM4中断,500us
*/
u16 tim_x;
void TIM6_IRQHandler(void)
{
	if(TIM6->SR & 0X0001)	//溢出中断
	{
		USART5_RxDW();		//串口5接收数据处理
	}
	TIM6->SR =~(1<<0);		//清除中断标志位
}


extern u8 USART1_RxBuffer[USART1_RxBufferSizeMax];	//接收数据缓冲区
extern u8  USART1_RxCounter; 						//接收数据计数器
void USART1_IRQHandler(void)
{
	//接收数据中断，读取模具标签
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART1_Flag |=0x10;			//正在接收标志位
		USART1_RX_Timer=0;
		USART1_RxBuffer[USART1_RxCounter++] =USART_ReceiveData(USART1); 
		
		if(Send_Flag & 0X01) USART_SendData(UART5,USART_ReceiveData(USART1));	//把接收到的数据向串口5发送
	}
	//发送数据中断
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{    
		USART1_SendDA();		//发送一个字节数据
//		Printf_Send();		//发送一帧数据
    }  
}

/*
	函数名： USART3_IRQHandler(void)
	描述：	 串口3中断
	输入：	 无
	输出：	 无
	返回:	 无
*/
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		USART3_ReceiveDA(USART_ReceiveData(USART3)); //读一个字节数据
		
		if(Send_Flag & 0X04) USART_SendData(UART5,USART_ReceiveData(USART3));	//把接收到的数据向串口5发送
	}
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
	{    
		USART3_SendDA();		//发送一个字节数据
		Heart_Counter=0;
	}  
}

/*
	函数名： USART4_IRQHandler(void)
	描述：	 串口4中断
	输入：	 无
	输出：	 无
	返回:	 无
*/
void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		UART4_ReceiveDA(USART_ReceiveData(UART4));
		
		if(Send_Flag & 0X08) USART_SendData(UART5,USART_ReceiveData(UART4));	//把接收到的数据向串口5发送
	} 
	if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET)
	{    
		Printf_Send();		//发送一帧数据
	}  

}

/*
	串口5中断
*/
void UART5_IRQHandler(void)
{
	//接收数据中断
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{
		USART5_Receive(USART_ReceiveData(UART5));	//接收一帧数据
	}
	//发送数据中断
	if(USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
	{    
		//Printf_Send();		//发送一帧数据
		USART5_Send();
	}  
}

/*
*	函数名： RTC_IRQHandler
*	描述：	 
*/
extern u8 USART3_SendWorld; //发送标志字
extern u32 XT_ONTime;	//系统上电计时
extern u32 RL_ONTime;	//熔炉上电计时
extern u8 YGTM_Flag;	//员工条码标志位
extern u32 MJ_Cnt;
extern u32 YG_Cnt;
extern u32 SYS_Timer;	//系统上电计时
extern u8 UART4_RXFLAG;
extern u8 UART4_RXNEXT;
extern u8 YUANGONG_TIME;
extern u8 YUANGONG;
extern u32 Ethernet_Time_Count;
extern u8 Ethernet_Reset_Count;
extern u32 CPU_Reset_Time;
		u32 SYS_OnTime=0;

u32  Machine_Timer=0;

void OneSecond_Handler(void)   //1S中断函数，在SysTick_Handler()中调用
{
  	Ethernet_Time_Count++;				//用于1小时内计时判断控制器没有接收以太网的次数
	
	minute++;		
	if(minute==5)		//分钟应用
	{
		minute=0;
		if(UHRFID_Flag & 0x01) 
		{
            MJ_Tim++;		//上模具计时
        } 			
		if(Input_Flag & 0x01) 
		{
			XT_ONTime++;		//系统上电计时，5秒
		}
	}
	
	if(Input_Flag & 0x02) 
	{
       RL_ONTime++;			//熔炉上电计时，秒
	}			 
	
	if(YGTM_Flag & 0x01) 
	{
		YG_Tim++;			//员工在岗计时，秒
	}
	if((YGTM_Flag != 0x01) && Ethernet_Reset_Count >=2)
	{
		CPU_Reset_Time++;	//当以太网重启的次数大于等于2次，而且员工不在岗，CPU复位计时
	}
	
	SYS_Timer++;
	SYS_OnTime++;
	if(SYS_Timer % 3600==0)
	{
		u8 Buffer[4];
		Buffer[0]=SYS_Timer>>24;
		Buffer[1]=SYS_Timer>>16;
		Buffer[2]=SYS_Timer>>8;
		Buffer[3]=SYS_Timer;
		SPI_Flash_Write(Buffer,SYS_Space,4);	
	}
	
	if(UART4_RXFLAG==1)	//串口4延时接收，用于打了一次卡后，隔30秒后才能再打一次
	{
		UART4_RXNEXT++;
		if((UART4_RXNEXT>=10) && (YGTM_Flag & 0x01)==0)
		{
			UART4_RXNEXT=0;
			UART4_RXFLAG=0;
			USART_Cmd(UART4, ENABLE);
			USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	//开中断
		}
		else if(UART4_RXNEXT>=10)//30  //打卡间隔时间限制
		{
			UART4_RXNEXT=0;
			UART4_RXFLAG=0;
			USART_Cmd(UART4, ENABLE);
			USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	//开中断
		}
	}
	
	Machine_Timer++;		//机器上电计时
	User_Timer1s();		//用户定时器，1秒定时器
	URFID_Timer1S();		//超高频读卡器秒时钟,秒中断常调用

//-------------------心跳包时间间隔---------------------
	Heart_Counter++;
	if((Heart_Counter >= 30) && (USART3_SendWorld == 0))
	{//每半分钟发送一次心跳包
		Heart_Counter=0;
		USART3_SendWorld=18;		
	}

//--------------------调试----------------------
	if(YUANGONG==1)
	{
		YUANGONG_TIME++;
		if(YUANGONG_TIME>=3)
		{
			YUANGONG_TIME=0;
			YUANGONG=0;
		}
	}
		 
//--------------------调试----------------------
	if(GPIO_ReadOutputDataBit(LED1)) 
	{
		GPIO_ResetBits(LED1);
	}
	else 
	{
		GPIO_SetBits(LED1);
	}
}

void RTC_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_SEC) != RESET) //RTC发生了秒中断（也有可能是溢出或者闹钟中断)
	/*RTC_IT_OW溢出中断/RTC_IT_ALR闹钟中断/RTC_IT_SEC秒中断*/
	 {
		 RTC_ClearITPendingBit(RTC_IT_SEC);
		 Ethernet_Time_Count++;				//用于1小时内计时判断控制器没有接收以太网的次数
		 minute++;		
		 if(minute==5)		//分钟应用
		 {
			 minute=0;
			 if(UHRFID_Flag & 0x01) 
			 {
          MJ_Tim++;		//上模具计时
       } 			
			 if(Input_Flag & 0x01) 
			 {
					XT_ONTime++;		//系统上电计时，5秒
			 }
		 }
		 if(Input_Flag & 0x02) 
		 {
        RL_ONTime++;			//熔炉上电计时，秒
		 }			 
		 if(YGTM_Flag & 0x01) 
		 {
			 YG_Tim++;			//员工在岗计时，秒
		 }
		 if((YGTM_Flag != 0x01) && Ethernet_Reset_Count >=2)
		 {
			 CPU_Reset_Time++;	//当以太网重启的次数大于等于2次，而且员工不在岗，CPU复位计时
		 }
		 SYS_Timer++;
		 SYS_OnTime++;
		 if(SYS_Timer % 3600==0)
		 {
			 u8 Buffer[4];
			 Buffer[0]=SYS_Timer>>24;
			 Buffer[1]=SYS_Timer>>16;
			 Buffer[2]=SYS_Timer>>8;
			 Buffer[3]=SYS_Timer;
			 SPI_Flash_Write(Buffer,SYS_Space,4);	
		 }
		 if(UART4_RXFLAG==1)	//串口4延时接收，用于打了一次卡后，隔30秒后才能再打一次
		 {
			 UART4_RXNEXT++;
			 if((UART4_RXNEXT>=10) && (YGTM_Flag & 0x01)==0)
			 {
				 UART4_RXNEXT=0;
				 UART4_RXFLAG=0;
				 USART_Cmd(UART4, ENABLE);
				 USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	//开中断
			 }
			 else if(UART4_RXNEXT>=30) 
			 {
				 UART4_RXNEXT=0;
				 UART4_RXFLAG=0;
				 USART_Cmd(UART4, ENABLE);
				 USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	//开中断
			 }
		 }
		 Machine_Timer++;		//机器上电计时
		 User_Timer1s();		//用户定时器，1秒定时器
		 URFID_Timer1S();		//超高频读卡器秒时钟,秒中断常调用
		 //-------------------心跳包---------------------
		 Heart_Counter++;
		 if((Heart_Counter >= 30) && (USART3_SendWorld == 0))
		 {//每半分钟发送一次心跳包
			 Heart_Counter=0;
			 USART3_SendWorld=18;		
		 }
		 //--------------------调试----------------------
		 if(YUANGONG==1)
		 {
			 YUANGONG_TIME++;
			 if(YUANGONG_TIME>=3)
			 {
				 YUANGONG_TIME=0;
				 YUANGONG=0;
			 }
		 }
		 
		 //--------------------调试----------------------
		 if(GPIO_ReadOutputDataBit(LED1)) 
		 {
			 GPIO_ResetBits(LED1);
		 }
		 else 
		 {
			 GPIO_SetBits(LED1);
		 }
	 }
}


/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval : None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
