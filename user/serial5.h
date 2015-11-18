#include "stm32f10x.h"
#include "calculation.h" 

#define RX5_LEN 		100
#define RX5_TIMER 		20
#define USART5_BaudRate 9600

void Init_Serial5(void);		//串口5初始化
void USART5_Send(void);			//串口5发送一帧数据
void USART5_Receive(u8 buf);		//串口5接收一帧数据
void USART5_RxDW(void);			//串口5接收处理，定时器6常调用
void Serial5_DW(u8 command);	//串口5接收到数据后命令处理
void HMI_Touch_Data(void);		//向触摸屏发送一串数据

