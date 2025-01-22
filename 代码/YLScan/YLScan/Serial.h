

#ifndef _SERIAL_H
#define _SERIAL_H

#define STX 				0x02 	//报文起始
#define ETX 				0x03 	//报文终止
#define TMSETX				0x05	//TMS数据结束
#define XON					0x11
#define XOFF				0x13
#define MAX_CMD_LEN  		300
#define MAX_RSP_LEN  		300
#define COMM_ERR     		0xff

#define uchar unsigned char
#define uint  unsigned short
#define ulong unsigned long

uchar SerialOpen(char *pDevName,DWORD baud);
void  SerialClose(void);
int writetocom(uchar *Cmd, uint CmdLen);
int sendtocom_RS232(uchar *buf,uint len);
int sendtocom8583_RS232(uchar *buf,uint len);
int sendtocomTMS_RS232(uchar *buf,uint len);
int readfromcom_RS232(uchar *buf, int *RecvLen, ulong TimeOut);

#endif

