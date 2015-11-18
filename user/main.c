/**
  ******************************************************************************
  * @file    Project/Template/main.c 
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    04/06/2009
  * @brief   Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "include.h"
#include "time.h"
#include <stdio.h>


/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/	
	
extern u8  USART1_Flag;
extern u8  USART1_TxLen;
extern u8  USART1_TxBuffer[255];
extern u8  USART1_RxCounter;
extern u8  USART3_SendWorld;	//����3����ָ����
extern u32 input_cdata[1];

extern u16 Input_Rising_Edge;	//����������
extern u16 Input_Falling_Edge;	//�����½���
extern u16 Input_State;			//����״̬
extern u8 YGTM_Flag;	//Ա�������־λ
extern u8 UHRFID_Flag;	//ģ�������־λ
extern u8 UAT5_FLAG;
extern u8 YUANGONG;		//�ڶ���Ա���鿴
extern u8 Ethernet_Reset_Count;	//��̫�������Ĵ���

extern u8  AutoMode_Flag;
extern u8  SemiAutoMode_Flag;
extern u8  ManualMode_Flag;

extern u8  Old_Flag ;
/*
	���õ�ȫ�ֱ���
	���ݱ�
*/
	u16 RUN_MODE=0;			//�豸����״̬��
	
	u8 ADDRESS[3]="001";	//�豸��ַ
	u8 MACHINE_TYPE[2]="DY";//��������
	u8 RUN_STATE='T'; 		//������ǰ����״̬
	u8 Code_Mode='A';		//��д�ش�ָ��b
	u8 Command[3]="001";	//��������
	u8 YG_CodeLend=10;		//Ա�����볤��
	u8 YG_Code[]="0000000000";
	u8 MJ_CodeLend=10;		//ĥ�����볤��
	u8 MJ_Code[]="0000000000";
	u8 MJ_Counter[6]="000000";		//��ģ����
	char MJ_Timing[6]="000000";		//ģ�߼�ʱ
	char MO_Time[6]="000000";		//ģ�߼�ʱ
	char YG_Time[6]="000000";		//Ա����ʱ
	
	u16 DataIn=0;
	u8 Output=0;
	u16 Time_Count=0;
	u8 RTC_Blank=0;			//�����жϵ���Ƿ��е�
	u32 DeviceID=0;
	
	u16 Address=1;	//������ַ0~65535
	u32 MJ_Cnt=0;		//ĥ�߿�ģ����
	u32 MJ_Tim=0;		//ĥ�߼�ʱ
	u32 YG_Cnt=0;		//Ա����ģ����
	u32 YG_Tim=0;		//Ա����ʱ
	u32 XT_ONTime=0;	//ϵͳ�ϵ��ʱ
	u32 RL_ONTime=0;	//��¯�ϵ��ʱ
	u32 SYS_Timer=0;	//ϵͳ�ϵ��ʱ
	u8 Run_Flag=0;
	u32 CPU_Reset_Time;
	u8 CPU_Reset_Time_Flag = 0;//CPU������ʱ���־λ
	
	u8 YG_NameLend=6;	//Ա���������ȣ���ʼ��Ϊ6Ϊ
	u8 YG_Name[10]="�°���";

    u8 RUN_STATE_01 = 'X'; 
    u8 Mode_STATE = 'X';


/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);		//GPIO�����ú���
void RTC_Configuration(void);		//RTCʱ�ӳ�ʼ��
void Initialize(void);				//ϵͳ��ʼ������
void Init_Tim6(void);


void IWDG_Init(u8 prer,u16 rlr);	//�������Ź���ʼ��
void IWDG_Feed(void);			//ι��

/* Private functions ---------------------------------------------------------*/
int main(void)
{
	u32 i;
	Initialize();
	Communicate_Star();
	Reset_Mag();		//������Ϣ
	Communicate_StarON();
	printf("�����أ�������!\r\n");
	User_SetTimer1sValue(181);
	while (1)
	{
		IWDG_Feed();		//ι��
		MJ_KMCounter();		//��ģ����ͳ��
		Send_ETH();			//����̫���˿ڷ�������
		
		USART1_ReceiveDW();	//����1�������ݴ���,���Գɹ�
		USART3_ReceiveDW();	//����3�������ݴ���	
		UART4_ReceiveDW();	//����4���յ����ݴ���
		IWDG_Init(4,625);	//���Ź���ʼ����ԼΪ1S
		LED_main();			//ָʾ����˸����
		Communictae_Check();//��̫��ģ����ϼ��
		
		if (CPU_Reset_Time > 10)	//Ա���¸ں�������������ﵽ��2�λ����ϣ�����ʱ3���ӣ�����10�룩����CPU��λ
		{
			CPU_Reset_Time = 0;
			Ethernet_Reset_Count = 0;
			NVIC_SystemReset();
		}
        // test
	    //   OUTPUT1  -- �Ƶ� --  ����
        //   OUTPUT2  -- ��� --  ͣ��
        //   OUTPUT3  -- �̵� --  ����
        //   OUTPUT4  -- ���� --  ����
		if(User_GetTimer1sValue() < 180)
		{//3�����ڣ��п�ģ������Ϊ������״̬
            GPIO_ResetBits(OUTPUT1); //�п�ģ�����������������̵�
            
            GPIO_SetBits(OUTPUT2);
            GPIO_SetBits(OUTPUT3);
            GPIO_SetBits(OUTPUT4);
            
            RUN_STATE = 'R';
            RUN_STATE_01= Mode_STATE;
		}
		else if(Input_State & 0x1000)
		{//ϵͳ�е磬��û������������Ϊ�Ǵ���״̬
            GPIO_ResetBits(OUTPUT3); //ϵͳ���������Ƶ�
            
            GPIO_SetBits(OUTPUT1);
            GPIO_SetBits(OUTPUT2);
            GPIO_SetBits(OUTPUT4);

//            Old_Flag = 0;
            RUN_STATE_01='X';
            RUN_STATE='D';			//ϵͳ�������״̬
		}
		else
		{//ϵͳû�磬����Ϊ��ͣ��״̬
			
            GPIO_ResetBits(OUTPUT2); //ϵͳͣ���������
            
            GPIO_SetBits(OUTPUT1);
            GPIO_SetBits(OUTPUT3);
            GPIO_SetBits(OUTPUT4);

//            Old_Flag = 0;
            RUN_STATE_01='X';
            RUN_STATE='T';			//ϵͳ����ͣ��״̬
		}
			
		//-------------------------------------------------------------------
		if(Time_Count==0) 	
		{
 			GPIO_ResetBits(LED2);
			i++;
		}
		if(Time_Count==500)
		{
			GPIO_SetBits(LED2);	
		}
		if(i==10)
		{
			i=1;
			//USART3_SendWorld=1;	
		}
		//-------------------------------------------------------------------		
	}
}

/*
	�������� void IWDG_Init(u8 prer,u16 rlr)
	������   �������Ź���ʼ��
			 prer����Ƶ����0~7��ֻ�е���λ��Ч��
			 ��Ƶ����=4��2^prer,���ֵΪ256
			 rlr����װ�ؼĴ���ֵ����11λ��Ч
			 ʱ����㣨��ţ���Tout=((4��2^prer)��rlr)/40	(ms)
	���룺	 ��Ƶ������װ��ֵ
*/
void IWDG_Init(u8 prer,u16 rlr)
{
	IWDG->KR = 0X5555;	//ʹ�ܶ�IWDG->PR��IWDG->RLR��д
	IWDG->PR = prer;	//���÷�Ƶϵ��
	IWDG->RLR = rlr;	//�ؼ��ؼĴ��� 
	IWDG->KR = 0XAAAA;
	IWDG->KR = 0XCCCC;	//ʹ�ܿ��Ź�
}

/*
	ι������
*/
void IWDG_Feed(void)
{
	IWDG->KR = 0XAAAA;
}

/*
	ϵͳ��ʼ������
*/
void Initialize(void)
{
	u8 Buffer[6];
	u16 SYS_Count;	//ϵͳͣ��������
	SystemInit();				//ϵͳ��ʼ��
	GPIO_Configuration();		//GPIO����
//	RTC_Configuration();		//RTC��ʼ��
	spi1_configuration();				
	USART1_Configuration();		//����1��ʼ��
	USART3_Configuration();				//����3��ʼ��
	UART4_Configuration();		//����4��ʼ��
	Init_Serial5();
	Init_Tim6();
	
	GPIO_SetBits(LED1);
	GPIO_SetBits(LED2);
	GPIO_SetBits(LED3);
	GPIO_SetBits(LED4);
	GPIO_SetBits(LED5);
	
 	if (SysTick_Config(SystemFrequency / 1000))  //����SysTick
 	{
		while (1);
 	}
	SPI_Flash_Read(ADDRESS,Flash_Base0,3);		//��ȡ������ַ
	SPI_Flash_Read(MACHINE_TYPE,Flash_Base1,2);	//��ȡ��������
	SPI_Flash_Read(Buffer,SYS_Space,6);		//��ȡϵͳ�����Ϣ
	SYS_Timer=Buffer[0];		//ϵͳ�ϵ��ʱ
	SYS_Timer=SYS_Timer<<8;
	SYS_Timer |= Buffer[1];
	SYS_Timer=SYS_Timer<<8;
	SYS_Timer |= Buffer[2];
	SYS_Timer=SYS_Timer<<8;
	SYS_Timer |= Buffer[3];
	SYS_Count=Buffer[4];		//�ϵ��ȡ����
	SYS_Count=SYS_Count<<8;
	SYS_Count|=Buffer[5];
	SYS_Count++;
	Buffer[4]=SYS_Count>>8;
	Buffer[5]=SYS_Count;
	SPI_Flash_Write(&Buffer[4],SYS_Space+4,2);	
}

/*
	�������� void Tim4_Init(void)
	������	 ��ʱ��6��ʼ��
	���룺	 ��
	�����	 ��
	���أ�	 ��
*/
void Init_Tim6(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC->APB1ENR |=1<<4;	//ʹ��TIM4ʱ��
	TIM6->ARR=50*10-1;		//10us
	TIM6->PSC=71;			//Ԥ��Ƶ��71+1���õ�72MHz�ļ���ʱ��
	TIM6->DIER |=1<<0;		//��������ж�
 	TIM6->DIER |=1<<6;		//�������ж�	
	TIM6->CR1 |=  0X01;		//ʹ�ܶ�ʱ��6
	//�����ж�
	NVIC_InitStructure.NVIC_IRQChannel=TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
*	�������� RTC_Configuration
*	������	 RTCʱ�ӳ�ʼ��
*	���룺	 ��
*	�����	 ��
* 	���أ�	 ��
*/
void RTC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/*PWRʱ�ӣ���Դ���ƣ���BKPʱ�ӣ�RTC�󱸼Ĵ�����ʹ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	/*ʹ��RTC�ͺ󱸼Ĵ�������*/
	PWR_BackupAccessCmd(ENABLE);
	/*��ָ���ĺ󱸼Ĵ�����BKP_DR1���ж�������*/
	if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		/* ������BKP��ȫ���Ĵ�������Ϊȱʡֵ */
		BKP_DeInit(); 
		/* ���� LSE���ⲿ���پ���*/
		RCC_LSEConfig(RCC_LSE_ON); 
		/*�ȴ��ⲿ������ ��Ҫ�ȴ��Ƚϳ���ʱ��*/
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
		/*ʹ���ⲿ����32.768K��ΪRTCʱ��*/
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);  
		//����RTC
		RCC_RTCCLKCmd(ENABLE);
		//�ȴ�RTC�Ĵ���ͬ�� ԭ������˵��
		RTC_WaitForSynchro();
		//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_WaitForLastTask();
		//����RTC�����ж�(���������жϺ�����жϿ�����)
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		//���ǵȴ���һ�ζ�RTC�Ĵ�����д�������
		RTC_WaitForLastTask();
		//32768����Ԥ��Ƶֵ��32767,����һ����˵���񶼲���ô׼
		RTC_SetPrescaler(32776);  //�����ҪУ׼����,���޸Ĵ˷�Ƶֵ
		RTC_WaitForLastTask();
		//д��RTC�󱸼Ĵ���1 0xa5a5
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);  
		RTC_Blank=1; /*�����־����RTC��û��Ԥ���(����˵��û����Ŧ�۵��) �ô���ѽɶ���������*/
		//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_WaitForLastTask();
		/* Sets Counter value to 0x0 */
		RTC_SetCounter(0xFFFFFFFA);	
	}
	//���RTC�Ѿ�����
	else
	{
		//�ȴ�RTC��APBͬ��
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		//ʹ�����ж� 
		RTC_ITConfig(RTC_IT_SEC, ENABLE);  //�����Էŵ�ǰ����
		RTC_WaitForLastTask(); //�ֵ�....
    }
	//�����־
	RCC_ClearFlag(); 
	//ʹ��RTC�ж�
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* ������		��GPIO_Configuration
* ����			�����ò�ͬ��GPIO�˿�
* ����			����
* ���			����
* ����			����
*******************************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC \
	|RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
	//GPIOA�˿���������
	//�������
	//LED5
	GPIO_InitStructure.GPIO_Pin =PIN8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, PIN8);		//��ʼ��Ϊ�ߵ�ƽ
	//��������
	//INPUT2,INPUT3,INPUT4
	GPIO_InitStructure.GPIO_Pin = PIN1|PIN2|PIN3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//GPIOB�˿���������
	//�������
	//��������
	//INPUT7,INPUT8
	GPIO_InitStructure.GPIO_Pin = PIN0|PIN1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//GPIOC�˿���������
	//�������
	//OUTPUT6,OUTPUT7,LED1,LED2,LED3,LED4
	GPIO_InitStructure.GPIO_Pin = PIN0 | PIN1 |PIN2  | PIN6 | PIN7 | PIN8 | PIN9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, PIN0 | PIN1| PIN2| PIN6 | PIN7 | PIN8 | PIN9);	//��ʼ��Ϊ�ߵ�ƽ
	//��������
	//INPUT1,INPUT5,INPUT6,OUTPUT8(PIN2)
	GPIO_InitStructure.GPIO_Pin =  PIN3 | PIN4 | PIN5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//GPIOD�˿���������
	//�������
	GPIO_InitStructure.GPIO_Pin = PIN6|PIN7|PIN8|PIN9;			//LED�ƿ��ƶ˿�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD, PIN6 | PIN7 | PIN8 | PIN9);				//��ʼΪ�ߵ�ƽ
	//��������
	GPIO_InitStructure.GPIO_Pin = PIN6|PIN7|PIN8|PIN9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//GPIOE�˿���������
	//�������
	//OUTPUT1,OUTPUT2,OUTPUT3,OUTPUT4,OUTPUT5
	GPIO_InitStructure.GPIO_Pin =PIN1| PIN2| PIN3 | PIN4 | PIN5 | PIN6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_SetBits(GPIOE, PIN1| PIN2 | PIN3 | PIN4 | PIN5 | PIN6);				//��ʼΪ�ߵ�ƽ
	//��������
	//INPUT9,INPUT10,INPUT11,INPUT12,INPUT13,INPUT14,INPUT15,INPUT16
	GPIO_InitStructure.GPIO_Pin = PIN7 | PIN8 | PIN9 | PIN10 | PIN11 | PIN12| PIN13 | PIN14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
