
#ifndef __SERIAL3_H
#define __SERIAL3_H

#include "stm32f10x.h"

//USART3相关宏定义
#define USART1_RxBufferSizeMax	42		//定义最大的接收缓冲区
#define USART3_RxBufferSizeMax  255		//定义串口3最大接收缓冲区

//USART1相关宏定义
#define STX		0x3A		//":"定义接收字符开始标志位
#define EOR		0x0D		//回车,结束第一个字符
#define EOX		0X0A		//换行,结束第二个字符
#define ID		"AB"		//设备编号

//USART3相关函数
void USART3_Configuration(void);			//USART3初始化
void Send_ETH(void);						//向以太网发送数据
void USART3_SendDA(void);						//串口3发送一个字节数据
void Cu32_To_Char(u32 a,u8 *p);				//将计数值转为字符发送到串口显示
//void Data_CRC(u8 *p1, u8 *p2, u8 len);		//CRC计算，计算以指针*p1开始，长度为len的CRC值，求和后取反，在转换为四个字节的ASCII码到指针*p2上
void Data_CRC(unsigned char *puchMsg, u8 *p2, unsigned short usDataLen);
void Cu8_To_Char(u8 *p, u8 a);				//将一个u8<100的数转换为ASCII码，并用指针*p输出
void USART3_SendDW(void);					//发送数据处理
void Send_AMR(void);						//发送机器编号、类型，编号，状态、指令、命令，输入无，输出无，返回无
void Receive_ETH(void);							//接收数据处理
void USART3_ReceiveDA(u8 buffer);			//串口3接收一帧数据
void USART3_ReceiveDW(void);				//串口3接收一帧数据后处理

void Send_MJ_Code(void);		//模具更新或被移开，发送模具计时,tmp=1,发送旧模具信息，tmp=2发送新模具信息
void Send_XTRL_Time(void);		//发送系统上电、断电信息，熔炉上电断电信息
void Change_TxBuffer(void);		//更改TxBuffer寄存器相关信息

void Send_RGXX_Time(void);		 //发送员工上岗、结束上岗信息
void Send_QC_Input_Data(void);	 //发送QC输入的合格率
void Send_Mould_Defect(void);	 //发送模具缺陷

void Send_QC_Name(void);
void Send_QC_PassWord(void);

void Send_QCBad_QianZhu(void);
void Send_QCBad_LaMo(void);
void Send_QCBad_ShaKong(void);
void Send_QCBad_LengGe(void);
void Send_QCBad_BianXing(void);
void Send_QCBad_BengQue(void);
void Send_QCBad_QiPao(void);
void Send_QCBad_LuGuan(void);
void Send_QCBad_JiTan(void);
void Send_QCBad_ChiChen(void);
void Send_QCBad_HeiYin(void);
void Send_QCBad_QiTa(void);
void Send_QC_ChengFen(void);

void Send_QC_ZhuXiao(void);

void Send_Error(void);		//向以太网发送错误命令提示
void Send_Address(void);	//向以太网发本机地址
void Send_MACHINE_TYPE(void);	//向以太网发本机机器类型
void SendMJCode(void);	//向以太网发本机模具条码
void SendYGCode(void);	//向以太网发本机员工条码
void Send_MMYY(u32 tem);//向以太网发本机模具计数、模具计时、员工上岗开模计数、员工上岗计时，输入模具计数/模具计时/员工计数/员工计时
void Send_MoJuYiKai(void);			//发送模具移开信息

void Communicate_Computer(void);	//通讯标准协议处理

//================================
void Send_MJCode(void);
void Reset_Mag(void);
void Send_MuJuYuanGong(void);
void Clear_RxBuffer(void);

#endif
