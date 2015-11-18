/*
	�汾��		V1.0
	��д���ڣ�	2013��9��7��
	�༭�ˣ�	����
	��ģ�����ã���ģ��Ϊ����RTCʵʱʱ�ӣ�������32λ��ת��Ϊ��ǰ���ں���
	ʹ�÷�����	 
	
	�޸����ڣ�
	�޸�˵����
*/
#include "rtc.h"
#include "stm32f10x.h"
#include <stdio.h>

/*
	�������� RTC_Init(void)
	������	 RTCʵʱʱ�ӳ�ʼ��
	���룺	 ��
	�����	 ��
	����;	 ��
*/
void RTC_Init(void)
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
		printf("RTC NO POWER");
// 		RTC_Blank=1; /*�����־����RTC��û��Ԥ���(����˵��û����Ŧ�۵��) �ô���ѽɶ���������*/
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

