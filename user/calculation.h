void U32_To_Char(u32 a);
void Data_CRC(unsigned char *puchMsg, u8 *p2, unsigned short usDataLen);
void Cu8_To_Char(u8 *p, u8 a);
void Cu32_To_Char(u32 a,u8 *p);		//ֻȡ����λ
u8 Check_Data(u8 *pointer, u8 len);
void u16_char(u16 tmp, u8 *p);		//��u16ת��Ϊchar,ֻȡ��3λ
u8 Char_Compare(u8 *p1,u8 *p2, u8 len);//�Ƚ�������ͬ���ȵ������Ƿ���ͬ������ͬ�򷵻�1������ͬ�򷵻�0��
u8 IsChar(u8 *P,u8 Len);		//�ж�һ�������Ƿ��ǿ���ʾ�ַ���

void u32_to_decimalist(u32 buf,u8 *p);
u8 u8_to_char(u8 buf);
unsigned int uiCrc16Cal(unsigned char const *pucY, unsigned char ucX);	//����Ƶ������У�����
unsigned short CRC16(unsigned char *puchMsg,unsigned short usDataLen);	//��׼CRC�㷨

