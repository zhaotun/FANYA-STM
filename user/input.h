#ifndef __INPUT_H
#define __INPUT_H

#include "stm32f10x.h"
#include "define.h"
#include "stdio.h"

void Input_InputStateCollect(void);		//�������ݲɼ���1ms�жϵ���
void Input_Edge(void);		//���������ء��½��زɼ�������Input_State��Old_Input_State��
							//���Input_Rising_Edge�����ء�Input_falling_Edge�½��أ�
							//Input_InputStateCollect()�ϵ���
void MJ_KMCounter(void);	//��ģ����ͳ��

#endif
