
#ifndef __USER_H
#define __USER_H

#include "stm32f10x.h"

struct machine_structure	//����״̬�ṹ
{
	u16 Address;	//������ַ
	u8 Type[2];		//��������
	u8 State;		//��������״̬
	u32 Timer;		//����ʱ��
	u32 Count;		//�������߼�����
};

struct input_structure
{
	u32 Timer;		//���ο�ģ��ʱ����������һ�ο�ģ��ʼ��ʱ������һ�ο�ģ�������¼�ʱ
	u16 Count;		//һ��û��ģ�����뿪ģ����
};

void User_Timer1s(void);			//�û���ʱ����1�붨ʱ��
void User_Timer1sClear(void);		//�û�1�붨ʱ������
u32 User_GetTimer1sValue(void);		//���ض�ʱ����ֵ
void User_SetTimer1sValue(u32 buf);	//���ö�ʱ����ֵ

void LED_main(void);	//��˸��������ϵͳ���������
void LED_ON(void);		//����˸��
void LED_OFF(void);		//����˸��


void Communictae_Check(void);
void Communicate_timer(void);
void Communicate_SetOn(void);
void Communicate_SetOff(void);
void Clera_Timer1(void);
void Communicate_SaveData(u8 *p,u8 len);
void Communicate_Star(void);
void Communicate_StarON(void);

void OneSecond_Handler(void); //1S��ʱ�жϺ���


#endif

