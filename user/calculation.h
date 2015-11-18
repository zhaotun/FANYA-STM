void U32_To_Char(u32 a);
void Data_CRC(unsigned char *puchMsg, u8 *p2, unsigned short usDataLen);
void Cu8_To_Char(u8 *p, u8 a);
void Cu32_To_Char(u32 a,u8 *p);		//只取低六位
u8 Check_Data(u8 *pointer, u8 len);
void u16_char(u16 tmp, u8 *p);		//将u16转换为char,只取低3位
u8 Char_Compare(u8 *p1,u8 *p2, u8 len);//比较两个相同长度的数组是否相同，若相同则返回1，若不同则返回0；
u8 IsChar(u8 *P,u8 Len);		//判断一串数组是否都是可显示字符串

void u32_to_decimalist(u32 buf,u8 *p);
u8 u8_to_char(u8 buf);
unsigned int uiCrc16Cal(unsigned char const *pucY, unsigned char ucX);	//超高频读卡器校验计算
unsigned short CRC16(unsigned char *puchMsg,unsigned short usDataLen);	//标准CRC算法

