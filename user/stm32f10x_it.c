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
extern u8   USART1_RX_Timer;		//����1���ռ�ʱ��
extern u8  	USART1_Flag;			//����1�շ���ر�־λ
extern u8  	USART3_Flag;			//����1�շ���ر�־λ
extern u8	UHRFID_Flag;		//����Ƶ��������ر�־λ
extern u32  MJ_Tim;		//ĥ�߼�ʱ
extern u32  YG_Tim;		//Ա����ʱ
extern u8   Input_Flag;	//������ر�־λ

u16	 Heart_Counter=0;


u8	minute=0;			//���Ӽ�ʱ

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
extern u8 UART4_RX_Timer;		//����4���ռ�ʱ
extern u8 UART4_Flag;			//����4�շ���־λ
u8 uhfCard_Timer;				//��ȡģ��RFID������
void SysTick_Handler(void)		//1mS��ʱ
{
	Input_InputStateCollect();		//����ܽ�״̬�ɼ�
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
		USART1_ReadURFID();	//��ȡģ��RFID�����ݣ�ÿ100ms��ȡһ��
	}
	//����1���
	if(USART1_Flag & 0x10) 
	{
		USART1_RX_Timer++;	//����1�������ݼ�ʱ
	}
	if(UART4_Flag & 0x10) 
	{
		UART4_RX_Timer++;	//����4�������ݼ�ʱ	
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
	TIM4�ж�,500us
*/
u16 tim_x;
void TIM6_IRQHandler(void)
{
	if(TIM6->SR & 0X0001)	//����ж�
	{
		USART5_RxDW();		//����5�������ݴ���
	}
	TIM6->SR =~(1<<0);		//����жϱ�־λ
}


extern u8 USART1_RxBuffer[USART1_RxBufferSizeMax];	//�������ݻ�����
extern u8  USART1_RxCounter; 						//�������ݼ�����
void USART1_IRQHandler(void)
{
	//���������жϣ���ȡģ�߱�ǩ
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART1_Flag |=0x10;			//���ڽ��ձ�־λ
		USART1_RX_Timer=0;
		USART1_RxBuffer[USART1_RxCounter++] =USART_ReceiveData(USART1); 
		
		if(Send_Flag & 0X01) USART_SendData(UART5,USART_ReceiveData(USART1));	//�ѽ��յ��������򴮿�5����
	}
	//���������ж�
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{    
		USART1_SendDA();		//����һ���ֽ�����
//		Printf_Send();		//����һ֡����
    }  
}

/*
	�������� USART3_IRQHandler(void)
	������	 ����3�ж�
	���룺	 ��
	�����	 ��
	����:	 ��
*/
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		USART3_ReceiveDA(USART_ReceiveData(USART3)); //��һ���ֽ�����
		
		if(Send_Flag & 0X04) USART_SendData(UART5,USART_ReceiveData(USART3));	//�ѽ��յ��������򴮿�5����
	}
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
	{    
		USART3_SendDA();		//����һ���ֽ�����
		Heart_Counter=0;
	}  
}

/*
	�������� USART4_IRQHandler(void)
	������	 ����4�ж�
	���룺	 ��
	�����	 ��
	����:	 ��
*/
void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		UART4_ReceiveDA(USART_ReceiveData(UART4));
		
		if(Send_Flag & 0X08) USART_SendData(UART5,USART_ReceiveData(UART4));	//�ѽ��յ��������򴮿�5����
	} 
	if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET)
	{    
		Printf_Send();		//����һ֡����
	}  

}

/*
	����5�ж�
*/
void UART5_IRQHandler(void)
{
	//���������ж�
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{
		USART5_Receive(USART_ReceiveData(UART5));	//����һ֡����
	}
	//���������ж�
	if(USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
	{    
		//Printf_Send();		//����һ֡����
		USART5_Send();
	}  
}

/*
*	�������� RTC_IRQHandler
*	������	 
*/
extern u8 USART3_SendWorld; //���ͱ�־��
extern u32 XT_ONTime;	//ϵͳ�ϵ��ʱ
extern u32 RL_ONTime;	//��¯�ϵ��ʱ
extern u8 YGTM_Flag;	//Ա�������־λ
extern u32 MJ_Cnt;
extern u32 YG_Cnt;
extern u32 SYS_Timer;	//ϵͳ�ϵ��ʱ
extern u8 UART4_RXFLAG;
extern u8 UART4_RXNEXT;
extern u8 YUANGONG_TIME;
extern u8 YUANGONG;
extern u32 Ethernet_Time_Count;
extern u8 Ethernet_Reset_Count;
extern u32 CPU_Reset_Time;
		u32 SYS_OnTime=0;

u32  Machine_Timer=0;

void OneSecond_Handler(void)   //1S�жϺ�������SysTick_Handler()�е���
{
  	Ethernet_Time_Count++;				//����1Сʱ�ڼ�ʱ�жϿ�����û�н�����̫���Ĵ���
	
	minute++;		
	if(minute==5)		//����Ӧ��
	{
		minute=0;
		if(UHRFID_Flag & 0x01) 
		{
            MJ_Tim++;		//��ģ�߼�ʱ
        } 			
		if(Input_Flag & 0x01) 
		{
			XT_ONTime++;		//ϵͳ�ϵ��ʱ��5��
		}
	}
	
	if(Input_Flag & 0x02) 
	{
       RL_ONTime++;			//��¯�ϵ��ʱ����
	}			 
	
	if(YGTM_Flag & 0x01) 
	{
		YG_Tim++;			//Ա���ڸڼ�ʱ����
	}
	if((YGTM_Flag != 0x01) && Ethernet_Reset_Count >=2)
	{
		CPU_Reset_Time++;	//����̫�������Ĵ������ڵ���2�Σ�����Ա�����ڸڣ�CPU��λ��ʱ
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
	
	if(UART4_RXFLAG==1)	//����4��ʱ���գ����ڴ���һ�ο��󣬸�30�������ٴ�һ��
	{
		UART4_RXNEXT++;
		if((UART4_RXNEXT>=10) && (YGTM_Flag & 0x01)==0)
		{
			UART4_RXNEXT=0;
			UART4_RXFLAG=0;
			USART_Cmd(UART4, ENABLE);
			USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	//���ж�
		}
		else if(UART4_RXNEXT>=10)//30  //�򿨼��ʱ������
		{
			UART4_RXNEXT=0;
			UART4_RXFLAG=0;
			USART_Cmd(UART4, ENABLE);
			USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	//���ж�
		}
	}
	
	Machine_Timer++;		//�����ϵ��ʱ
	User_Timer1s();		//�û���ʱ����1�붨ʱ��
	URFID_Timer1S();		//����Ƶ��������ʱ��,���жϳ�����

//-------------------������ʱ����---------------------
	Heart_Counter++;
	if((Heart_Counter >= 30) && (USART3_SendWorld == 0))
	{//ÿ����ӷ���һ��������
		Heart_Counter=0;
		USART3_SendWorld=18;		
	}

//--------------------����----------------------
	if(YUANGONG==1)
	{
		YUANGONG_TIME++;
		if(YUANGONG_TIME>=3)
		{
			YUANGONG_TIME=0;
			YUANGONG=0;
		}
	}
		 
//--------------------����----------------------
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
	if(RTC_GetITStatus(RTC_IT_SEC) != RESET) //RTC���������жϣ�Ҳ�п�����������������ж�)
	/*RTC_IT_OW����ж�/RTC_IT_ALR�����ж�/RTC_IT_SEC���ж�*/
	 {
		 RTC_ClearITPendingBit(RTC_IT_SEC);
		 Ethernet_Time_Count++;				//����1Сʱ�ڼ�ʱ�жϿ�����û�н�����̫���Ĵ���
		 minute++;		
		 if(minute==5)		//����Ӧ��
		 {
			 minute=0;
			 if(UHRFID_Flag & 0x01) 
			 {
          MJ_Tim++;		//��ģ�߼�ʱ
       } 			
			 if(Input_Flag & 0x01) 
			 {
					XT_ONTime++;		//ϵͳ�ϵ��ʱ��5��
			 }
		 }
		 if(Input_Flag & 0x02) 
		 {
        RL_ONTime++;			//��¯�ϵ��ʱ����
		 }			 
		 if(YGTM_Flag & 0x01) 
		 {
			 YG_Tim++;			//Ա���ڸڼ�ʱ����
		 }
		 if((YGTM_Flag != 0x01) && Ethernet_Reset_Count >=2)
		 {
			 CPU_Reset_Time++;	//����̫�������Ĵ������ڵ���2�Σ�����Ա�����ڸڣ�CPU��λ��ʱ
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
		 if(UART4_RXFLAG==1)	//����4��ʱ���գ����ڴ���һ�ο��󣬸�30�������ٴ�һ��
		 {
			 UART4_RXNEXT++;
			 if((UART4_RXNEXT>=10) && (YGTM_Flag & 0x01)==0)
			 {
				 UART4_RXNEXT=0;
				 UART4_RXFLAG=0;
				 USART_Cmd(UART4, ENABLE);
				 USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	//���ж�
			 }
			 else if(UART4_RXNEXT>=30) 
			 {
				 UART4_RXNEXT=0;
				 UART4_RXFLAG=0;
				 USART_Cmd(UART4, ENABLE);
				 USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	//���ж�
			 }
		 }
		 Machine_Timer++;		//�����ϵ��ʱ
		 User_Timer1s();		//�û���ʱ����1�붨ʱ��
		 URFID_Timer1S();		//����Ƶ��������ʱ��,���жϳ�����
		 //-------------------������---------------------
		 Heart_Counter++;
		 if((Heart_Counter >= 30) && (USART3_SendWorld == 0))
		 {//ÿ����ӷ���һ��������
			 Heart_Counter=0;
			 USART3_SendWorld=18;		
		 }
		 //--------------------����----------------------
		 if(YUANGONG==1)
		 {
			 YUANGONG_TIME++;
			 if(YUANGONG_TIME>=3)
			 {
				 YUANGONG_TIME=0;
				 YUANGONG=0;
			 }
		 }
		 
		 //--------------------����----------------------
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
