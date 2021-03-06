/************************************************************************
3-13 20:42 调试OK
可计数
可切换四种模式
******************************************************************************/

#include "input.h"

extern u32 MJ_Cnt;	//开模计数
extern u32 YG_Cnt;	//员工开模计数
extern u8  USART3_SendWorld;
extern u8 RUN_STATE;
extern u8 Mode_STATE;

u8  Input_Flag=0;			//输入相关标志位
u8  Input_Timer=0;			//输入采集计时
u8  Input1_Cnt=0;			//1管脚输入
u8  Input13_Cnt=0;			//2管脚输入
u8  Input14_Cnt=0;			//3管脚输入
u8  Input4_Cnt=0;			//4管脚输入
u16 Input_State=0;			//当前输入状态
u16 Old_Input_State=0;		//上一时刻输入状态
u16 Input_Rising_Edge=0;	//输入上升沿
u16 Input_Falling_Edge=0;	//输入下降沿

u8  Old_Flag = 0;
u8  Flag = 4;

u8  Input2_LowLevel_Flag = 0;
u8  Input2_HighLevel_Flag = 0;

u8  Input3_LowLevel_Flag = 0;
u8  Input3_HighLevel_Flag = 0;

u8  Input4_LowLevel_Flag = 0;
u8  Input4_HighLevel_Flag = 0;

u8  Auto_Flag=0;

u8  AutoMode_Flag;
u8  SemiAutoMode_Flag;
u8  ManualMode_Flag;

u8  doorOpenTimer = 0;
u8  doorCloseTimer = 0;
u8  doorOpenFlag = 0;
u8  doorCloseFlag = 0;

u32  doorCloseCounter = 0;
u32  doorOpenCounter = 0;
u8  disturbDoorOpen = 0;
u8  doorOpenCS = 0;
/********************************************************************************************************
	函数名：Input_InputStateCollect()
	说  明：
	功  能：开模数据的24V输入引脚（CN1的1、2脚24V输入端）的上升、下降沿脉冲信号的采集
	        压铸机AC_220V电源通断信号（即CN4的1、2脚IN1_AC220V输入）的采集
			熔炉AC_220V电源通断信号（即CN4的1、3脚IN1_AC220V输入）的采集
	输  入：无
	输  出：无

	INPUT1--开模计数
	INPUT2--自动、手动模式检测    需要自动模式时INPUT2始终为低
	INPUT4--系统电源检测
**********************************************************************************************************/
void Input_InputStateCollect()
{
	if(GPIO_ReadInputDataBit(INPUT1)==0)  	//当扫描到INPUT1，即GPIOC, GPIO_Pin_3引脚为低电平，扫描频率1次/ms
		Input1_Cnt++;						//扫描到低电平的次数加1
	
//通过前门信号判断模式，前门一直关时为自动模式，此时控制控制器输入24V，INPUT2应为0
//前门有开时为手动模式，此时控制输入0V，INPUT2应为1
//自动手动判断逻辑：
//当检测到持续达4秒的开门信号时，说明是有人打开了前门，由此判断为手动模式
//当检测到持续达100秒的关门信号时，说明前门始终处于关闭状态，由此判断为自动模式
//在检测100秒的关门信号期间，可能会受到持续时间小于4秒的开门信号干扰，此时将干扰忽略，继续计数，直至达到100秒
	if(GPIO_ReadInputDataBit(INPUT2) == 1) 	//当扫描到INPUT2为高（开门信号），即GPIOA, GPIO_Pin_1引脚为低电平，扫描频率1次/ms
	{//开门,手动
         doorOpenTimer++;      //开门计数器开始计数
    }
	else//关门,自动�
    { 
         doorCloseTimer++;     //关门计数器开始计数
    }

	if(GPIO_ReadInputDataBit(INPUT4)==0) 	//当扫描到INPUT13，即GPIOE, GPIO_Pin_11引脚为低电平，扫描频率1次/ms
		{ Input3_LowLevel_Flag = 1; }
	else
		{ Input3_HighLevel_Flag = 1;}

	if(GPIO_ReadInputDataBit(INPUT3)==0) 	//系统电源扫描
		Input4_Cnt++;						//扫描到低电平的次数加1

	Old_Input_State=Input_State;	        //保存上一扫描时刻的输入电平状态
	
	Input_Timer++;							//扫描计数器加1，因扫描频率为1次/ms，故此计数器每毫秒加1

	if(Input_Timer >= 50)						//若扫描次数达到30，即30ms以内
	{
//-----------------------INPUT1-----------------------
		if(Input1_Cnt >= 35)					//在30ms以内，若扫描到低电平的次数达到15，则证明该引脚输入了有效的低电平
		{
			Input_State |= 0x0001;	        //将输入状态置0x0001，表明该引脚输了有效的低电平
		}
		else							    //在30ms以内，若扫描到低电平的次数小于15，则证明该引脚输入了有效的高电平
		{
			Input_State &= 0xFFFE;	        //将输入状态置为非低，即高电平
		}
		
        if(doorOpenTimer >= 35)
        {//检测到一次有效的开门信号
             doorOpenTimer = 0;
             doorCloseTimer = 0;

             doorOpenCounter++;

//             printf("doorOpenCounter = %d\r\n",doorOpenCounter);

             if( doorOpenCounter >= 4 * 20 )  //检测到开门的时间达到4秒钟
             {
                doorOpenCounter = 0;
                doorOpenFlag = 1 ;           //置开门标志
                doorCloseFlag = 0;           //清除关门标志
                doorCloseCounter = 0;                
             }
        }
         if(doorCloseTimer >= 35)
        {//检测到一次有效的关门信号
             doorCloseTimer = 0;
             doorOpenTimer = 0;

             doorCloseCounter ++ ;    //检测到有效关门信号的次数

//             printf("&&&&doorCloseCounter = %d\r\n",doorCloseCounter);

             doorOpenCounter = 0;     //清除开门计数，防止其累加，可滤除关门期间的开门信号干扰
             
             if( doorCloseCounter >= 100 * 20 )  //检测到关门的时间达到100秒
             {
                 doorOpenFlag = 0 ; 
                 doorCloseFlag = 1;

                 doorCloseCounter = 0; 
//               doorOpenCounter = 0;                                   
             }
        }
       
    
		if(Input4_Cnt >= 35)				//在30ms以内，若扫描到低电平的次数达到8，则证明该引脚输入了有效的低电平
		{
 			//printf("输入管脚13在e20ms采集到的次数为:%u\r\n",Input13_Cnt);
			Input_State |= 0x1000;	        //将输入状态置0x1000，表明该引脚输入了有效的低电平
		}
		else								//在30ms以内，若扫描到低电平的次数小于8，则证明该引脚输入了有效的高电平
		{//输入管脚13无效
			Input_State &= 0xEFFF;	        //将输入状态置为非低，即高电平
		}

		Input_Edge();	                    //输入上升沿、下降沿的判断
		
		Input1_Cnt=0;
		Input4_Cnt=0;	                    //统计数据清零
		Input_Timer=0; 
	}

}

/**************************************************************************************************
	函数名： Input_Edge(void)
	说  明： 通过Old_Input_State与Input_State二者的差异比较来进行判断
		 
	功  能： 判断24V开模数据、压铸机通断电信号、熔炉通断电信号的上升沿、下降沿，此处的上升下降沿
	         为实际状态，反应到程序中则是实际状态通过光耦取反后得到的状态
***************************************************************************************************/
void Input_Edge(void)
{
/************开模数据--INPUT1--上升沿、下降沿的判断*********************/
	if(((Old_Input_State & 0x0001)==0) && (Input_State & 0x0001))     //当Old_Input_State为0xFFFE（即高电平）且Input_State为0x0001（即低电平）时	
	{
		Input_Rising_Edge |=0x0001;							          //24V开模数据产生了一个上升沿
	}
	else if((Old_Input_State & 0x0001) && ((Input_State & 0x0001)==0))//当Old_Input_State为0x0001（即低电平）且Input_State为0xFFFE（即高电平）时		
	{
		Input_Falling_Edge |=0x0001;								  //24V开模数据产生了一个下降沿
	}
/************压铸机电源通断信号上升沿、下降沿的判断**************/
	if(((Old_Input_State & 0x1000)==0) && (Input_State & 0x1000))     //当Old_Input_State为0xEFFF（即高电平）且Input_State为0x1000（即低电平）时	
	{
		Input_Rising_Edge |=0x1000;									  //压铸机电源产生了一个上升沿，表明此刻压铸机正在上电
	}
	else if((Old_Input_State & 0x1000) && ((Input_State & 0x1000)==0))//当Old_Input_State为0x1000（即低电平）且Input_State为0xEFFF（即高电平）时	
	{
		Input_Falling_Edge |=0x1000;								  //压铸机电源产生了一个下降沿，表明此刻压铸机正在断电
	}

//只要检测到开门信号就是手动，开门为高电平，即控制器输入为高，单片机IO口检测到低，即GPIO_ReadInputDataBit(INPUT2)==0
//////////////--单片机IO口为低电平，实际INPUT2为高电平，---判断为手动模式////////////	
	if( doorOpenFlag == 1 )	  //检测到开门标志，代表手动模式
	{
		doorOpenFlag = 0;
		
		Flag = 2;
		if(Old_Flag != Flag)
		{
    		Mode_STATE = 'M';
//            printf("控制器输入低电平，手动模式\r\n");
    		USART3_SendWorld=26;  //发送命令301
			Old_Flag = Flag;
		}				   
	}
    else if(doorCloseFlag == 1)    
    {
        doorCloseFlag = 0 ;

        Flag = 3;
		if(Old_Flag != Flag)
		{
    		Mode_STATE = 'A';
//            printf("控制器输入高电平，自动模式\r\n");
    		USART3_SendWorld=28;  //发送命令303
			Old_Flag = Flag;
		}				      
    }
}
/************************************************************************************
	函数名：MJ_KMCounter()
	功  能：开模数据统计  ：模具生产的工件数量统计
	输  入：无
	输  出：无
************************************************************************************/
void MJ_KMCounter(void)
{
/****************开模数据统计*****************/
	if (USART3_SendWorld|= 0) return; //若USART3的发送标志字为0，则不发送数据，继续等待
	
	if(Input_Rising_Edge & 0x0001)	  //若出现开模输入上升沿，则进行一次计数	
	{
		Input_Rising_Edge &=0xFFFE;	  //首先将开模输入上升沿标志位清零，等待下一次置位
		
		MJ_Cnt++;				      //模具计数加1
		YG_Cnt++;				      //员工计数加1
	
		USART3_SendWorld=1;		      //将USART3的发送标志字置1，进入以太网数据发送模式
									  //每采集一次开模数据，即向以太网发送一次
		
		User_Timer1sClear();		  //用户秒定时器时钟清零
		
		Nul_MuJuCount();		      //模具为空时开模计数？？？？？？？？？？？？？？？？？？？？？
		
		return;
	}	
/*******************系统上电断电发送********************/
	if(Input_Rising_Edge & 0x1000) 	  //若出现压铸机AC220电源开启
	{
		Input_Rising_Edge &=0xEFFF;	  //首先将压铸机电源开启信号清零，等待下一次置位
		Input_Flag |= 0x01;		      //压铸机上电标志位置位，开始压铸机上电时间统计
		USART3_SendWorld=9;			  //若USART3的发送标志字为9，则发送系统上电信息112
		return;
	}
	if(Input_Falling_Edge & 0x1000)	  //若出现压铸机AC220电源关闭时
	{
		Input_Falling_Edge &=0xEFFF;  //首先将压铸机电源关闭信号清零，等待下一次置位
		Input_Flag &= 0xFE;		      //压铸机断电标志位置位，结束压铸机上电时间统计
		USART3_SendWorld=10;		  //若USART3的发送标志字为10，发送系统断电信息113及其累计上电时间
		return;
	}

}




