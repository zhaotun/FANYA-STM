
#ifndef __SERIAL3_H
#define __SERIAL3_H

#include "stm32f10x.h"

//USART3��غ궨��
#define USART1_RxBufferSizeMax	42		//�������Ľ��ջ�����
#define USART3_RxBufferSizeMax  255		//���崮��3�����ջ�����

//USART1��غ궨��
#define STX		0x3A		//":"��������ַ���ʼ��־λ
#define EOR		0x0D		//�س�,������һ���ַ�
#define EOX		0X0A		//����,�����ڶ����ַ�
#define ID		"AB"		//�豸���

//USART3��غ���
void USART3_Configuration(void);			//USART3��ʼ��
void Send_ETH(void);						//����̫����������
void USART3_SendDA(void);						//����3����һ���ֽ�����
void Cu32_To_Char(u32 a,u8 *p);				//������ֵתΪ�ַ����͵�������ʾ
//void Data_CRC(u8 *p1, u8 *p2, u8 len);		//CRC���㣬������ָ��*p1��ʼ������Ϊlen��CRCֵ����ͺ�ȡ������ת��Ϊ�ĸ��ֽڵ�ASCII�뵽ָ��*p2��
void Data_CRC(unsigned char *puchMsg, u8 *p2, unsigned short usDataLen);
void Cu8_To_Char(u8 *p, u8 a);				//��һ��u8<100����ת��ΪASCII�룬����ָ��*p���
void USART3_SendDW(void);					//�������ݴ���
void Send_AMR(void);						//���ͻ�����š����ͣ���ţ�״̬��ָ���������ޣ�����ޣ�������
void Receive_ETH(void);							//�������ݴ���
void USART3_ReceiveDA(u8 buffer);			//����3����һ֡����
void USART3_ReceiveDW(void);				//����3����һ֡���ݺ���

void Send_MJ_Code(void);		//ģ�߸��»��ƿ�������ģ�߼�ʱ,tmp=1,���;�ģ����Ϣ��tmp=2������ģ����Ϣ
void Send_XTRL_Time(void);		//����ϵͳ�ϵ硢�ϵ���Ϣ����¯�ϵ�ϵ���Ϣ
void Change_TxBuffer(void);		//����TxBuffer�Ĵ��������Ϣ

void Send_RGXX_Time(void);		 //����Ա���ϸڡ������ϸ���Ϣ
void Send_QC_Input_Data(void);	 //����QC����ĺϸ���
void Send_Mould_Defect(void);	 //����ģ��ȱ��

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

void Send_Error(void);		//����̫�����ʹ���������ʾ
void Send_Address(void);	//����̫����������ַ
void Send_MACHINE_TYPE(void);	//����̫����������������
void SendMJCode(void);	//����̫��������ģ������
void SendYGCode(void);	//����̫��������Ա������
void Send_MMYY(u32 tem);//����̫��������ģ�߼�����ģ�߼�ʱ��Ա���ϸڿ�ģ������Ա���ϸڼ�ʱ������ģ�߼���/ģ�߼�ʱ/Ա������/Ա����ʱ
void Send_MoJuYiKai(void);			//����ģ���ƿ���Ϣ

void Communicate_Computer(void);	//ͨѶ��׼Э�鴦��

//================================
void Send_MJCode(void);
void Reset_Mag(void);
void Send_MuJuYuanGong(void);
void Clear_RxBuffer(void);

#endif
