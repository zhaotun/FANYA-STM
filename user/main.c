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
extern u8  USART3_SendWorld;	//串口3发送指令字
extern u32 input_cdata[1];

extern u16 Input_Rising_Edge;	//输入上升沿
extern u16 Input_Falling_Edge;	//输入下降沿
extern u16 Input_State;			//输入状态
extern u8 YGTM_Flag;	//员工条码标志位
extern u8 UHRFID_Flag;	//模具条码标志位
extern u8 UAT5_FLAG;
extern u8 YUANGONG;		//第二个员工查看
extern u8 Ethernet_Reset_Count;	//以太网重启的次数

extern u8  AutoMode_Flag;
extern u8  SemiAutoMode_Flag;
extern u8  ManualMode_Flag;

extern u8  Old_Flag ;
/*
	常用的全局变量
	数据表
*/
	u16 RUN_MODE=0;			//设备运行状态字
	
	u8 ADDRESS[3]="001";	//设备地址
	u8 MACHINE_TYPE[2]="DY";//机器类型
	u8 RUN_STATE='T'; 		//本机当前运行状态
	u8 Code_Mode='A';		//读写回答指令b
	u8 Command[3]="001";	//运行命令
	u8 YG_CodeLend=10;		//员工条码长度
	u8 YG_Code[]="0000000000";
	u8 MJ_CodeLend=10;		//磨具条码长度
	u8 MJ_Code[]="0000000000";
	u8 MJ_Counter[6]="000000";		//开模数据
	char MJ_Timing[6]="000000";		//模具计时
	char MO_Time[6]="000000";		//模具计时
	char YG_Time[6]="000000";		//员工计时
	
	u16 DataIn=0;
	u8 Output=0;
	u16 Time_Count=0;
	u8 RTC_Blank=0;			//用于判断电池是否有电
	u32 DeviceID=0;
	
	u16 Address=1;	//本机地址0~65535
	u32 MJ_Cnt=0;		//磨具开模计数
	u32 MJ_Tim=0;		//磨具计时
	u32 YG_Cnt=0;		//员工开模计数
	u32 YG_Tim=0;		//员工计时
	u32 XT_ONTime=0;	//系统上电计时
	u32 RL_ONTime=0;	//熔炉上电计时
	u32 SYS_Timer=0;	//系统上电计时
	u8 Run_Flag=0;
	u32 CPU_Reset_Time;
	u8 CPU_Reset_Time_Flag = 0;//CPU重启的时间标志位
	
	u8 YG_NameLend=6;	//员工姓名长度，初始化为6为
	u8 YG_Name[10]="奥巴马";

    u8 RUN_STATE_01 = 'X'; 
    u8 Mode_STATE = 'X';


/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);		//GPIO口配置函数
void RTC_Configuration(void);		//RTC时钟初始化
void Initialize(void);				//系统初始化函数
void Init_Tim6(void);


void IWDG_Init(u8 prer,u16 rlr);	//独立看门狗初始化
void IWDG_Feed(void);			//喂狗

/* Private functions ---------------------------------------------------------*/
int main(void)
{
	u32 i;
	Initialize();
	Communicate_Star();
	Reset_Mag();		//重启信息
	Communicate_StarON();
	printf("呜呜呜，重启啦!\r\n");
	User_SetTimer1sValue(181);
	while (1)
	{
		IWDG_Feed();		//喂狗
		MJ_KMCounter();		//开模数据统计
		Send_ETH();			//向以太网端口发送数据
		
		USART1_ReceiveDW();	//串口1接收数据处理,调试成功
		USART3_ReceiveDW();	//串口3接收数据处理	
		UART4_ReceiveDW();	//串口4接收到数据处理
		IWDG_Init(4,625);	//看门狗初始化，约为1S
		LED_main();			//指示灯闪烁处理
		Communictae_Check();//以太网模块故障检测
		
		if (CPU_Reset_Time > 10)	//员工下岗后如果重启次数达到了2次或以上，先延时3分钟（测试10秒），再CPU软复位
		{
			CPU_Reset_Time = 0;
			Ethernet_Reset_Count = 0;
			NVIC_SystemReset();
		}
        // test
	    //   OUTPUT1  -- 黄灯 --  待机
        //   OUTPUT2  -- 红灯 --  停机
        //   OUTPUT3  -- 绿灯 --  生产
        //   OUTPUT4  -- 蓝灯 --  够数
		if(User_GetTimer1sValue() < 180)
		{//3分钟内，有开模，则认为是生产状态
            GPIO_ResetBits(OUTPUT1); //有开模，表明在生产，亮绿灯
            
            GPIO_SetBits(OUTPUT2);
            GPIO_SetBits(OUTPUT3);
            GPIO_SetBits(OUTPUT4);
            
            RUN_STATE = 'R';
            RUN_STATE_01= Mode_STATE;
		}
		else if(Input_State & 0x1000)
		{//系统有电，但没有生产，则认为是待机状态
            GPIO_ResetBits(OUTPUT3); //系统待机，亮黄灯
            
            GPIO_SetBits(OUTPUT1);
            GPIO_SetBits(OUTPUT2);
            GPIO_SetBits(OUTPUT4);

//            Old_Flag = 0;
            RUN_STATE_01='X';
            RUN_STATE='D';			//系统进入待机状态
		}
		else
		{//系统没电，则认为是停机状态
			
            GPIO_ResetBits(OUTPUT2); //系统停机，亮红灯
            
            GPIO_SetBits(OUTPUT1);
            GPIO_SetBits(OUTPUT3);
            GPIO_SetBits(OUTPUT4);

//            Old_Flag = 0;
            RUN_STATE_01='X';
            RUN_STATE='T';			//系统进入停机状态
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
	函数名： void IWDG_Init(u8 prer,u16 rlr)
	描述：   独立看门狗初始化
			 prer：分频数：0~7（只有低三位有效）
			 分频因子=4×2^prer,最大值为256
			 rlr：重装载寄存器值：低11位有效
			 时间计算（大概）：Tout=((4×2^prer)×rlr)/40	(ms)
	输入：	 分频数，重装载值
*/
void IWDG_Init(u8 prer,u16 rlr)
{
	IWDG->KR = 0X5555;	//使能对IWDG->PR和IWDG->RLR的写
	IWDG->PR = prer;	//设置分频系数
	IWDG->RLR = rlr;	//重加载寄存器 
	IWDG->KR = 0XAAAA;
	IWDG->KR = 0XCCCC;	//使能看门狗
}

/*
	喂狗程序
*/
void IWDG_Feed(void)
{
	IWDG->KR = 0XAAAA;
}

/*
	系统初始化函数
*/
void Initialize(void)
{
	u8 Buffer[6];
	u16 SYS_Count;	//系统停开机次数
	SystemInit();				//系统初始化
	GPIO_Configuration();		//GPIO配置
//	RTC_Configuration();		//RTC初始化
	spi1_configuration();				
	USART1_Configuration();		//串口1初始化
	USART3_Configuration();				//串口3初始化
	UART4_Configuration();		//串口4初始化
	Init_Serial5();
	Init_Tim6();
	
	GPIO_SetBits(LED1);
	GPIO_SetBits(LED2);
	GPIO_SetBits(LED3);
	GPIO_SetBits(LED4);
	GPIO_SetBits(LED5);
	
 	if (SysTick_Config(SystemFrequency / 1000))  //开启SysTick
 	{
		while (1);
 	}
	SPI_Flash_Read(ADDRESS,Flash_Base0,3);		//读取本机地址
	SPI_Flash_Read(MACHINE_TYPE,Flash_Base1,2);	//读取机器类型
	SPI_Flash_Read(Buffer,SYS_Space,6);		//读取系统相关信息
	SYS_Timer=Buffer[0];		//系统上电计时
	SYS_Timer=SYS_Timer<<8;
	SYS_Timer |= Buffer[1];
	SYS_Timer=SYS_Timer<<8;
	SYS_Timer |= Buffer[2];
	SYS_Timer=SYS_Timer<<8;
	SYS_Timer |= Buffer[3];
	SYS_Count=Buffer[4];		//上电读取计数
	SYS_Count=SYS_Count<<8;
	SYS_Count|=Buffer[5];
	SYS_Count++;
	Buffer[4]=SYS_Count>>8;
	Buffer[5]=SYS_Count;
	SPI_Flash_Write(&Buffer[4],SYS_Space+4,2);	
}

/*
	函数名： void Tim4_Init(void)
	描述：	 定时器6初始化
	输入：	 无
	输出：	 无
	返回：	 无
*/
void Init_Tim6(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC->APB1ENR |=1<<4;	//使能TIM4时钟
	TIM6->ARR=50*10-1;		//10us
	TIM6->PSC=71;			//预分频器71+1，得到72MHz的计数时钟
	TIM6->DIER |=1<<0;		//允许更新中断
 	TIM6->DIER |=1<<6;		//允许触发中断	
	TIM6->CR1 |=  0X01;		//使能定时器6
	//配置中断
	NVIC_InitStructure.NVIC_IRQChannel=TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
*	函数名： RTC_Configuration
*	描述：	 RTC时钟初始化
*	输入：	 无
*	输出：	 无
* 	返回：	 无
*/
void RTC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/*PWR时钟（电源控制）与BKP时钟（RTC后备寄存器）使能*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	/*使能RTC和后备寄存器访问*/
	PWR_BackupAccessCmd(ENABLE);
	/*从指定的后备寄存器（BKP_DR1）中读出数据*/
	if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		/* 将外设BKP的全部寄存器重设为缺省值 */
		BKP_DeInit(); 
		/* 启用 LSE（外部低速晶振）*/
		RCC_LSEConfig(RCC_LSE_ON); 
		/*等待外部晶振震荡 需要等待比较长的时间*/
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
		/*使用外部晶振32.768K作为RTC时钟*/
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);  
		//允许RTC
		RCC_RTCCLKCmd(ENABLE);
		//等待RTC寄存器同步 原因下文说明
		RTC_WaitForSynchro();
		//等待最近一次对RTC寄存器的写操作完成
		RTC_WaitForLastTask();
		//允许RTC的秒中断(还有闹钟中断和溢出中断可设置)
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		//又是等待上一次对RTC寄存器的写操作完成
		RTC_WaitForLastTask();
		//32768晶振预分频值是32767,不过一般来说晶振都不那么准
		RTC_SetPrescaler(32776);  //如果需要校准晶振,可修改此分频值
		RTC_WaitForLastTask();
		//写入RTC后备寄存器1 0xa5a5
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);  
		RTC_Blank=1; /*这个标志代表RTC是没有预设的(或者说是没有上纽扣电池) 用串口呀啥的输出来。*/
		//等待最近一次对RTC寄存器的写操作完成
		RTC_WaitForLastTask();
		/* Sets Counter value to 0x0 */
		RTC_SetCounter(0xFFFFFFFA);	
	}
	//如果RTC已经设置
	else
	{
		//等待RTC与APB同步
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		//使能秒中断 
		RTC_ITConfig(RTC_IT_SEC, ENABLE);  //这句可以放到前面吗？
		RTC_WaitForLastTask(); //又等....
    }
	//清除标志
	RCC_ClearFlag(); 
	//使能RTC中断
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* 函数名		：GPIO_Configuration
* 描述			：配置不同的GPIO端口
* 输入			：无
* 输出			：无
* 返回			：无
*******************************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC \
	|RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
	//GPIOA端口引脚配置
	//推挽输出
	//LED5
	GPIO_InitStructure.GPIO_Pin =PIN8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, PIN8);		//初始化为高电平
	//浮空输入
	//INPUT2,INPUT3,INPUT4
	GPIO_InitStructure.GPIO_Pin = PIN1|PIN2|PIN3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//GPIOB端口引脚配置
	//推挽输出
	//浮空输入
	//INPUT7,INPUT8
	GPIO_InitStructure.GPIO_Pin = PIN0|PIN1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//GPIOC端口引脚配置
	//推挽输出
	//OUTPUT6,OUTPUT7,LED1,LED2,LED3,LED4
	GPIO_InitStructure.GPIO_Pin = PIN0 | PIN1 |PIN2  | PIN6 | PIN7 | PIN8 | PIN9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, PIN0 | PIN1| PIN2| PIN6 | PIN7 | PIN8 | PIN9);	//初始化为高电平
	//浮空输入
	//INPUT1,INPUT5,INPUT6,OUTPUT8(PIN2)
	GPIO_InitStructure.GPIO_Pin =  PIN3 | PIN4 | PIN5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//GPIOD端口引脚配置
	//推挽输出
	GPIO_InitStructure.GPIO_Pin = PIN6|PIN7|PIN8|PIN9;			//LED灯控制端口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD, PIN6 | PIN7 | PIN8 | PIN9);				//初始为高电平
	//浮空输入
	GPIO_InitStructure.GPIO_Pin = PIN6|PIN7|PIN8|PIN9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//GPIOE端口引脚配置
	//推挽输出
	//OUTPUT1,OUTPUT2,OUTPUT3,OUTPUT4,OUTPUT5
	GPIO_InitStructure.GPIO_Pin =PIN1| PIN2| PIN3 | PIN4 | PIN5 | PIN6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_SetBits(GPIOE, PIN1| PIN2 | PIN3 | PIN4 | PIN5 | PIN6);				//初始为高电平
	//浮空输入
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
