/*
�û�������
�û���������������
*/
#include "user.h"

struct machine_structure Machine_Structure;		//�����ṹ��
struct input_structure Input_Structure ={120,0};			//����ṹ��

struct led_struct{
	u16 timer;
	u8 flag;	//b1=1,����˸;b1=0ֹͣ��˸,b2=1��ÿ��2����˸һ��
	u8 Blue_LED_on;
	} LED_Struct;

/*
	�������� void User_Timer1S(void)
	������	 �û�1�붨ʱ�����붨ʱ������
*/
void User_Timer1s(void)
{
	Input_Structure.Timer++;
	LED_Struct.timer++;
// 	if((LED_Struct.timer % 2) == 0)
// 	{
// 		if(LED_Struct.flag & b2)
// 		{
// 			LED_Struct.flag &= ~b2;
// 		}
// 		else 
// 		{
// 			LED_Struct.flag |= b2;
// 		}
// 	}
}

/*
	�������� void User_Timer1SClear(void)
	������	 �û��붨ʱ��ʱ������
*/
void User_Timer1sClear(void)
{
	Input_Structure.Timer=0;
}

/*
	�������� u32 User_GetTimer1sValue(void)
	������	 �������ο�ģ��ʱ��������λ����
*/
u32 User_GetTimer1sValue(void)
{
	return Input_Structure.Timer;
}

void User_SetTimer1sValue(u32 buf)
{
	Input_Structure.Timer=buf;
}

//����ָʾ�ƿ���
void LED_main(void)
{
	if(LED_Struct.Blue_LED_on & 1)
	{
	//����
		if(LED_Struct.flag & b1)
		{
			GPIO_ResetBits(OUTPUT4);
		}
		else if(LED_Struct.flag &b2)
		{
			GPIO_SetBits(OUTPUT4);
		}
		if(User_GetTimer1sValue() > 300 && LED_Struct.timer > 300)
		{//���������û�п�ģ����ֹͣ��˸
			LED_Struct.timer=0;
			LED_Struct.Blue_LED_on = 0;
			GPIO_SetBits(OUTPUT4);
		}
	}
}

//����ָʾ����˸
void LED_ON(void)
{
	LED_Struct.flag &= ~b2;
	LED_Struct.flag |= b1;
	LED_Struct.Blue_LED_on =1;
}

//����ָʾ��ֹͣ��˸
void LED_OFF(void)
{
	LED_Struct.flag &= ~b1;
	LED_Struct.flag |= b2;
}

/*
	�������� void Communictae_Check(void)
	������	 �ú������ڼ����̫��ͨѶ�Ƿ�ͨ��������ͨ����������̫��ģ�飬ע�⣬Ϊ�˵ȴ���̫��
			 ģ������������ģ����Ҫ����3S�Ժ������
			 �㷨����������⵽��̫���˿������ݷ��ͺ󣬿�ʼ��ʱ����3�����Ȼû���յ���������
			 �����·������ݣ�������3��û���յ���λ�����ص����ݣ�����Ϊ��̫��ģ��������⣬��
			 ��������̫��ģ�飬����������3�Σ�����Ϊ��ǰ������ϣ���Ҫ�˹���Ԥ
*/
struct Communicate_struct
{
	u8 flag;	//b1(�������ݷ���),b2(��ǰͨѶ״̬,0������1�쳣),b3(ģ��������5S)
	u16 timer;
	u32 timer1;
	u8	count1;	//���·������ݴ���
	u8	count2;	//��������
	u8  TxBuffer[255];		//���ڱ�����Ҫ���͵�����
	u8  TxLen;	//���ڱ��浱ǰ�������ݵĳ���
} comm_str;

u32 Ethernet_Time_Count;
extern u8 USART3_TxBuffer[255];
extern u8 USART3_TxLen;
u8 Ethernet_Reset_Count;

void Communictae_Check(void)
{
	if(comm_str.flag & b3)
	{//ģ���Ѿ�����
		if(comm_str.timer >=12000)
		{//���ճ�ʱ
			comm_str.timer=0;
			comm_str.count1++;
			if((comm_str.count1>=3) && (Ethernet_Time_Count>3600))		//1Сʱ��û���յ��ظ����ڵ���3�Σ���������
			{//��������ģ��
				//����ģ��
//				printf("��̫��ģ��������\r\n");
				comm_str.flag=0;
				Ethernet_Time_Count=0;
				comm_str.count1=0;
				comm_str.timer=0;
				comm_str.TxLen=0;
				Ethernet_Reset_Count++;	//��������ͳ��
				GPIO_SetBits(OUTPUT8);		//����ģ��
//				GPIO_ResetBits(RESET_PIN);		//����ģ��

			}
			else if(comm_str.count1>=3)
			{
//				printf("����������6��\r\n");
//				printf("timer1=%u\r\n",Ethernet_Time_Count);
				Communicate_SetOff();
			}
			else
			{//û���յ����أ�������һ�η��͵�����
				u8 i;
				for(i=0;i<comm_str.TxLen;i++)
				{
					USART3_TxBuffer[i]=comm_str.TxBuffer[i];
				}
				USART3_TxLen=comm_str.TxLen;
				USART3_SendDW();
//				printf("timer1=%u\r\n",Ethernet_Time_Count);
			}
		}
	}
	else if(comm_str.timer >=3000)
	{//��̫������
		Communicate_SetOff();
		comm_str.flag |= b3;		//����ģ��
		if(comm_str.TxLen>0)
		{
			u8 i;
			for(i=0;i<comm_str.TxLen;i++)
			{
				USART3_TxBuffer[i]=comm_str.TxBuffer[i];
			}
			USART3_TxLen=comm_str.TxLen;
			USART3_SendDW();
		}
	}
	else if(comm_str.timer >=500)
	{//������̫��ģ��
		GPIO_ResetBits(OUTPUT8);
//		GPIO_SetBits(RESET_PIN);

	}
}

/*
	�������� void Communicate_timer(void)
	������	 ͨѶ��ⶨʱ�����ú�����1ms��ʱ������
			 ����̫��ģ����������ʱ����Ҫ�ȴ�3S
*/
void Communicate_timer(void)
{
	if((comm_str.flag & b3) == 0)
	{//ģ���Լ�����
		comm_str.timer++;
	}
	else if(comm_str.flag & b1)
	{//�����ݷ��ͣ�������ʱ
		comm_str.timer++;
	}
}

/*
	�������� void Communicate_SetIn(void)
	������	 ����comm_str.flag��b1λ��������ʱ,��������ʱ����
*/
void Communicate_SetOn(void)
{
	comm_str.flag |= b1;
	comm_str.timer=0;
}
/*
	���δ�յ����ݼ�ʱ
*/
void Clera_Timer1(void)
{
	Ethernet_Time_Count=0;
}

/*
	�������� void Communicate_SetIn(void)
	������	 ��comm_str.flag��b1λ��ֹͣ��ʱ��ͬʱ�����ǰ�ļ�����
*/
void Communicate_SetOff(void)
{
	comm_str.flag &= ~(b1 | b2);
	comm_str.timer=0;
	comm_str.count1=0;
	comm_str.count2=0;
}

/*
	�������� void Communicate_SaveData(void)
	������   ���浱ǰ��Ҫ���͵�����
	���룺	 u8 *p(��Ҫ�������ݵ��׵�ַ),u8 len(��Ҫ�������ݵĳ���)
*/
void Communicate_SaveData(u8 *p,u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		comm_str.TxBuffer[i]=*p++;
	}
	comm_str.TxLen=len;
}

/*
	�������� void Communicate_Star(void)
	������	 ��������ʱ���ã�����ʱ����ʱ3S��Ȼ���ټ������Ա�����̫��ģ������
*/
void Communicate_Star(void)
{
	GPIO_ResetBits(OUTPUT8);
//	GPIO_SetBits(RESET_PIN);
	while(comm_str.timer<1500);
	comm_str.timer=0;
}

void Communicate_StarON(void)
{
	while(comm_str.timer<1000);
	comm_str.timer=0;
	comm_str.flag =0;
	comm_str.flag |= b3;		//����ģ��
	comm_str.TxLen=0;
}




