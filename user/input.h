#ifndef __INPUT_H
#define __INPUT_H

#include "stm32f10x.h"
#include "define.h"
#include "stdio.h"

void Input_InputStateCollect(void);		//输入数据采集，1ms中断调用
void Input_Edge(void);		//输入上升沿、下降沿采集，输入Input_State、Old_Input_State，
							//输出Input_Rising_Edge上升沿、Input_falling_Edge下降沿，
							//Input_InputStateCollect()上调用
void MJ_KMCounter(void);	//开模数据统计

#endif
