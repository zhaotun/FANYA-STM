
#define RXBF_LEN			100			//接收缓冲区定为100个字节
#define STX					':'			//接收开始
#define RXCR				'\r'		//回车
#define RXLF				'\n'		//换行
#define RECEIVING			0X01		//正在接收
#define RECEIVE_SUCCESS		0X02		//接收成功
#define RECEIVE_WRONG		0X04		//接受错误
#define RECEIVE_OVER		0X08		//接收完毕


void USART1_Init(void);	//串口1函数初始化
void USART1_Receive(void);	//串口1接收函数

void USART3_Init(void);	//串口2函数初始化
void USART3_Receive(u8 buffer);	//串口1接收函数
void USART3_DW(void);			//发送完毕处理



