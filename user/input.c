/************************************************************************
3-13 20:42 ����OK
�ɼ���
���л�����ģʽ
******************************************************************************/

#include "input.h"

extern u32 MJ_Cnt;	//��ģ����
extern u32 YG_Cnt;	//Ա����ģ����
extern u8  USART3_SendWorld;
extern u8 RUN_STATE;
extern u8 Mode_STATE;

u8  Input_Flag=0;			//������ر�־λ
u8  Input_Timer=0;			//����ɼ���ʱ
u8  Input1_Cnt=0;			//1�ܽ�����
u8  Input13_Cnt=0;			//2�ܽ�����
u8  Input14_Cnt=0;			//3�ܽ�����
u8  Input4_Cnt=0;			//4�ܽ�����
u16 Input_State=0;			//��ǰ����״̬
u16 Old_Input_State=0;		//��һʱ������״̬
u16 Input_Rising_Edge=0;	//����������
u16 Input_Falling_Edge=0;	//�����½���

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
u8  Timer = 0;
/********************************************************************************************************
	��������Input_InputStateCollect()
	˵  ����
	��  �ܣ���ģ���ݵ�24V�������ţ�CN1��1��2��24V����ˣ����������½��������źŵĲɼ�
	        ѹ����AC_220V��Դͨ���źţ���CN4��1��2��IN1_AC220V���룩�Ĳɼ�
			��¯AC_220V��Դͨ���źţ���CN4��1��3��IN1_AC220V���룩�Ĳɼ�
	��  �룺��
	��  ������

	INPUT1--��ģ����
	INPUT2--�Զ����ֶ�ģʽ���    ��Ҫ�Զ�ģʽʱINPUT2ʼ��Ϊ��
	INPUT4--ϵͳ��Դ���
**********************************************************************************************************/
void Input_InputStateCollect()
{
	if(GPIO_ReadInputDataBit(INPUT1)==0)  	//��ɨ�赽INPUT1����GPIOC, GPIO_Pin_3����Ϊ�͵�ƽ��ɨ��Ƶ��1��/ms
		Input1_Cnt++;						//ɨ�赽�͵�ƽ�Ĵ�����1
	
//ͨ��ǰ���ź��ж�ģʽ��ǰ��һֱ��ʱΪ�Զ�ģʽ����ʱ���ƿ���������24V��INPUT2ӦΪ0
//ǰ���п�ʱΪ�ֶ�ģʽ����ʱ��������0V��INPUT2ӦΪ1
//�Զ��ֶ��ж��߼���
//����⵽������4��Ŀ����ź�ʱ��˵�������˴���ǰ�ţ��ɴ��ж�Ϊ�ֶ�ģʽ
//����⵽������100��Ĺ����ź�ʱ��˵��ǰ��ʼ�մ��ڹر�״̬���ɴ��ж�Ϊ�Զ�ģʽ
//�ڼ��100��Ĺ����ź��ڼ䣬���ܻ��ܵ�����ʱ��С��4��Ŀ����źŸ��ţ���ʱ�����ź��ԣ�����������ֱ���ﵽ100��
	if(GPIO_ReadInputDataBit(INPUT2) == 1) 	//��ɨ�赽INPUT2Ϊ�ߣ������źţ�����GPIOA, GPIO_Pin_1����Ϊ�͵�ƽ��ɨ��Ƶ��1��/ms
	{//����,�ֶ�
         doorOpenTimer++;      //���ż�������ʼ����
    }
	else//����,�Զ��
    { 
         doorCloseTimer++;     //���ż�������ʼ����
    }

	if(GPIO_ReadInputDataBit(INPUT4)==0) 	//��ɨ�赽INPUT13����GPIOE, GPIO_Pin_11����Ϊ�͵�ƽ��ɨ��Ƶ��1��/ms
		{ Input3_LowLevel_Flag = 1; }
	else
		{ Input3_HighLevel_Flag = 1;}

	if(GPIO_ReadInputDataBit(INPUT3)==0) 	//ϵͳ��Դɨ��
		Input4_Cnt++;						//ɨ�赽�͵�ƽ�Ĵ�����1

	Old_Input_State=Input_State;	        //������һɨ��ʱ�̵������ƽ״̬
	
	Input_Timer++;							//ɨ���������1����ɨ��Ƶ��Ϊ1��/ms���ʴ˼�����ÿ�����1
//    Timer++;

	if(Input_Timer >= 50)						//��ɨ������ﵽ30����30ms����
	{
//-----------------------INPUT1-----------------------
		if(Input1_Cnt >= 35)					//��30ms���ڣ���ɨ�赽�͵�ƽ�Ĵ����ﵽ15����֤����������������Ч�ĵ͵�ƽ
		{
			Input_State |= 0x0001;	        //������״̬��0x0001������������������Ч�ĵ͵�ƽ
		}
		else							    //��30ms���ڣ���ɨ�赽�͵�ƽ�Ĵ���С��15����֤����������������Ч�ĸߵ�ƽ
		{
			Input_State &= 0xFFFE;	        //������״̬��Ϊ�ǵͣ����ߵ�ƽ
		}
		
        if(doorOpenTimer >= 35)
        {//��⵽һ����Ч�Ŀ����ź�
             doorOpenTimer = 0;
             doorCloseTimer = 0;

             doorOpenCounter++;

             printf("doorOpenCounter = %d\r\n",doorOpenCounter);

             if( doorOpenCounter >= 4 * 20 )  //��⵽���ŵ�ʱ��ﵽ4����
             {
                doorOpenCounter = 0;
                doorOpenFlag = 1 ;           //�ÿ��ű�־
                doorCloseFlag = 0;           //������ű�־
                doorCloseCounter = 0;                
             }
        }
         if(doorCloseTimer >= 35)
        {//��⵽һ����Ч�Ĺ����ź�
             doorCloseTimer = 0;
             doorOpenTimer = 0;

             doorCloseCounter ++ ;    //��⵽��Ч�����źŵĴ���

             printf("&&&&doorCloseCounter = %d\r\n",doorCloseCounter);

             doorOpenCounter = 0;     //������ż�������ֹ���ۼӣ����˳������ڼ�Ŀ����źŸ���
             
             if( doorCloseCounter >= 100 * 20 )  //��⵽���ŵ�ʱ��ﵽ100��
             {
                 doorOpenFlag = 0 ; 
                 doorCloseFlag = 1;

                 doorCloseCounter = 0; 
//               doorOpenCounter = 0;                                   
             }
        }
       
    
		if(Input4_Cnt >= 35)				//��30ms���ڣ���ɨ�赽�͵�ƽ�Ĵ����ﵽ8����֤����������������Ч�ĵ͵�ƽ
		{
 			//printf("����ܽ�13��e20ms�ɼ����Ĵ���Ϊ:%u\r\n",Input13_Cnt);
			Input_State |= 0x1000;	        //������״̬��0x1000��������������������Ч�ĵ͵�ƽ
		}
		else								//��30ms���ڣ���ɨ�赽�͵�ƽ�Ĵ���С��8����֤����������������Ч�ĸߵ�ƽ
		{//����ܽ�13��Ч
			Input_State &= 0xEFFF;	        //������״̬��Ϊ�ǵͣ����ߵ�ƽ
		}

		Input_Edge();	                    //���������ء��½��ص��ж�
		
		Input1_Cnt=0;
		Input4_Cnt=0;	                    //ͳ����������
		Input_Timer=0; 
	}

}

/**************************************************************************************************
	�������� Input_Edge(void)
	˵  ���� ͨ��Old_Input_State��Input_State���ߵĲ���Ƚ��������ж�
		 
	��  �ܣ� �ж�24V��ģ���ݡ�ѹ����ͨ�ϵ��źš���¯ͨ�ϵ��źŵ������ء��½��أ��˴��������½���
	         Ϊʵ��״̬����Ӧ������������ʵ��״̬ͨ������ȡ����õ���״̬
***************************************************************************************************/
void Input_Edge(void)
{
/************��ģ����--INPUT1--�����ء��½��ص��ж�*********************/
	if(((Old_Input_State & 0x0001)==0) && (Input_State & 0x0001))     //��Old_Input_StateΪ0xFFFE�����ߵ�ƽ����Input_StateΪ0x0001�����͵�ƽ��ʱ	
	{
		Input_Rising_Edge |=0x0001;							          //24V��ģ���ݲ�����һ��������
	}
	else if((Old_Input_State & 0x0001) && ((Input_State & 0x0001)==0))//��Old_Input_StateΪ0x0001�����͵�ƽ����Input_StateΪ0xFFFE�����ߵ�ƽ��ʱ		
	{
		Input_Falling_Edge |=0x0001;								  //24V��ģ���ݲ�����һ���½���
	}
/************ѹ������Դͨ���ź������ء��½��ص��ж�**************/
	if(((Old_Input_State & 0x1000)==0) && (Input_State & 0x1000))     //��Old_Input_StateΪ0xEFFF�����ߵ�ƽ����Input_StateΪ0x1000�����͵�ƽ��ʱ	
	{
		Input_Rising_Edge |=0x1000;									  //ѹ������Դ������һ�������أ������˿�ѹ���������ϵ�
	}
	else if((Old_Input_State & 0x1000) && ((Input_State & 0x1000)==0))//��Old_Input_StateΪ0x1000�����͵�ƽ����Input_StateΪ0xEFFF�����ߵ�ƽ��ʱ	
	{
		Input_Falling_Edge |=0x1000;								  //ѹ������Դ������һ���½��أ������˿�ѹ�������ڶϵ�
	}

//ֻҪ��⵽�����źž����ֶ�������Ϊ�ߵ�ƽ��������������Ϊ�ߣ���Ƭ��IO�ڼ�⵽�ͣ���GPIO_ReadInputDataBit(INPUT2)==0
//////////////--��Ƭ��IO��Ϊ�͵�ƽ��ʵ��INPUT2Ϊ�ߵ�ƽ��---�ж�Ϊ�ֶ�ģʽ////////////	
	if( doorOpenFlag == 1 )	  //��⵽���ű�־�������ֶ�ģʽ
	{
		doorOpenFlag = 0;
		
		Flag = 2;
		if(Old_Flag != Flag)
		{
    		Mode_STATE = 'M';
            printf("����������͵�ƽ���ֶ�ģʽ\r\n");
    		USART3_SendWorld=26;  //��������301
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
            printf("����������ߵ�ƽ���Զ�ģʽ\r\n");
    		USART3_SendWorld=28;  //��������303
			Old_Flag = Flag;
		}				      
    }
}
/************************************************************************************
	��������MJ_KMCounter()
	��  �ܣ���ģ����ͳ��  ��ģ�������Ĺ�������ͳ��
	��  �룺��
	��  ������
************************************************************************************/
void MJ_KMCounter(void)
{
/****************��ģ����ͳ��*****************/
	if (USART3_SendWorld|= 0) return; //��USART3�ķ��ͱ�־��Ϊ0���򲻷������ݣ������ȴ�
	
	if(Input_Rising_Edge & 0x0001)	  //�����ֿ�ģ���������أ������һ�μ���	
	{
		Input_Rising_Edge &=0xFFFE;	  //���Ƚ���ģ���������ر�־λ���㣬�ȴ���һ����λ
		
		MJ_Cnt++;				      //ģ�߼�����1
		YG_Cnt++;				      //Ա��������1
	
		USART3_SendWorld=1;		      //��USART3�ķ��ͱ�־����1��������̫�����ݷ���ģʽ
									  //ÿ�ɼ�һ�ο�ģ���ݣ�������̫������һ��
		
		User_Timer1sClear();		  //�û��붨ʱ��ʱ������
		
		Nul_MuJuCount();		      //ģ��Ϊ��ʱ��ģ����������������������������������������������
		
		return;
	}	
/*******************ϵͳ�ϵ�ϵ緢��********************/
	if(Input_Rising_Edge & 0x1000) 	  //������ѹ����AC220��Դ����
	{
		Input_Rising_Edge &=0xEFFF;	  //���Ƚ�ѹ������Դ�����ź����㣬�ȴ���һ����λ
		Input_Flag |= 0x01;		      //ѹ�����ϵ��־λ��λ����ʼѹ�����ϵ�ʱ��ͳ��
		USART3_SendWorld=9;			  //��USART3�ķ��ͱ�־��Ϊ9������ϵͳ�ϵ���Ϣ112
		return;
	}
	if(Input_Falling_Edge & 0x1000)	  //������ѹ����AC220��Դ�ر�ʱ
	{
		Input_Falling_Edge &=0xEFFF;  //���Ƚ�ѹ������Դ�ر��ź����㣬�ȴ���һ����λ
		Input_Flag &= 0xFE;		      //ѹ�����ϵ��־λ��λ������ѹ�����ϵ�ʱ��ͳ��
		USART3_SendWorld=10;		  //��USART3�ķ��ͱ�־��Ϊ10������ϵͳ�ϵ���Ϣ113�����ۼ��ϵ�ʱ��
		return;
	}

}




