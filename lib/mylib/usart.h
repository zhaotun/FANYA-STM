
#define RXBF_LEN			100			//���ջ�������Ϊ100���ֽ�
#define STX					':'			//���տ�ʼ
#define RXCR				'\r'		//�س�
#define RXLF				'\n'		//����
#define RECEIVING			0X01		//���ڽ���
#define RECEIVE_SUCCESS		0X02		//���ճɹ�
#define RECEIVE_WRONG		0X04		//���ܴ���
#define RECEIVE_OVER		0X08		//�������


void USART1_Init(void);	//����1������ʼ��
void USART1_Receive(void);	//����1���պ���

void USART3_Init(void);	//����2������ʼ��
void USART3_Receive(u8 buffer);	//����1���պ���
void USART3_DW(void);			//������ϴ���



