#include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include "Serial.h"
#include "Tools.h"
#include "slog.h"

static HANDLE CommPort=INVALID_HANDLE_VALUE;

/****************************************************************
 * 函数介绍: 打开指定的串口端口
 * 输入函数: 
 				DevName: 指定串口号
 				baud : 	  串口通讯波特率
 				databit: 	  数据字节数
 				parity:	  数据奇偶位
 				stopbit:	  数据停止位
 * 输出函数: 
 * 函数返回：其他 失败，0 成功
 * 历史纪录：修改人		   日期			  版本号
 ***************************************************************/
static BOOL OpenSerialPort(LPCTSTR DevName,DWORD baud,BYTE databit,BYTE parity,BYTE stopbit)
{
	DCB 		dcb;
	BOOL		fRetVal ;
	BYTE		bSet ;
	DWORD		dwError;
	COMMTIMEOUTS  to;
	
	if ((CommPort=CreateFile(DevName,
		GENERIC_READ | GENERIC_WRITE,
		0,					
		NULL,				
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 
		NULL )) == (HANDLE) -1 )
	{
		CommPort =INVALID_HANDLE_VALUE;
		return FALSE;
	}
	fRetVal = SetupComm(CommPort, 65536, 65536 ) ;
	if (fRetVal == 0)
	{
		CloseHandle(CommPort);
		CommPort = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	
	PurgeComm(CommPort, PURGE_TXABORT | PURGE_RXABORT |
		PURGE_TXCLEAR | PURGE_RXCLEAR );
	
	
	GetCommTimeouts(CommPort, &to ) ;
	to.ReadIntervalTimeout = 0xFFFFFFFF;
	to.ReadTotalTimeoutMultiplier = 0 ;
	to.ReadTotalTimeoutConstant = 0 ;
	to.WriteTotalTimeoutMultiplier = 10;
	to.WriteTotalTimeoutConstant = 5000 ;
	SetCommTimeouts(CommPort, &to ) ;
	
	dcb.DCBlength = sizeof( DCB ) ;
	GetCommState(CommPort, &dcb ) ;
	dcb.BaudRate=baud;
	dcb.ByteSize=databit;
	dcb.Parity = parity;
	dcb.StopBits =stopbit;
	
	bSet = FALSE;
	dcb.fOutxDsrFlow = bSet ;
	if (bSet)		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE ;
	else			dcb.fDtrControl = DTR_CONTROL_ENABLE ;
	
	dcb.fOutxCtsFlow = bSet ;
	if (bSet)		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE ;
	else			dcb.fRtsControl = RTS_CONTROL_ENABLE ;
	
	bSet = FALSE;
	dcb.fInX = dcb.fOutX = bSet ;
	dcb.XonLim = 100 ;
	dcb.XoffLim = 100 ;
	dcb.XonChar = XON;
	dcb.XoffChar = XOFF;
	
	dcb.fBinary = TRUE;
	if (dcb.Parity == 0)
		dcb.fParity = FALSE;
	else
		dcb.fParity = TRUE;

	dcb.DCBlength = sizeof( DCB );
	fRetVal = SetCommState(CommPort, &dcb );
	if (fRetVal == 0)
	{
		dwError = GetLastError();
		CloseHandle(CommPort);
		CommPort = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	
	GetCommState(CommPort, &dcb ) ;
	PurgeComm(CommPort , PURGE_TXABORT | PURGE_RXABORT |
		PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	return TRUE;
}


/****************************************************************
 * 函数介绍: 向指定的串口端口写数据
 * 输入函数: 
 				*lpByte : 需要写端口数据
 				dwBytesToWrite : 写端口数据字节数
 				TimeOut: 写端口数据延迟时间
 * 输出函数: 
 * 函数返回：其他 失败，0 成功
 * 历史纪录：修改人		   日期			  版本号
 ***************************************************************/
static BYTE SerialWrite(BYTE *lpByte , WORD dwBytesToWrite , WORD TimeOut)
{
	BOOL		fWriteStat, fRetVal ;
	DWORD		dwBytesWritten ;
	DWORD		dwErrorFlags;
	DWORD		dwError;
	DWORD		dwBytesSent=0;
	COMSTAT 	ComStat;
	
	if(dwBytesToWrite==0) return TRUE;
	fWriteStat = WriteFile(CommPort, lpByte, 1,
		&dwBytesWritten, NULL); 
	if (!fWriteStat)
	{
		dwError = GetLastError();
		fRetVal = ClearCommError(CommPort, &dwErrorFlags, &ComStat );
		
		return 0xff;
	}
	if(dwBytesToWrite>1)
	{
		fWriteStat = WriteFile(CommPort, lpByte+1, dwBytesToWrite-1,
			&dwBytesWritten, NULL); 
		if (!fWriteStat)
		{
			dwError = GetLastError();
			fRetVal = ClearCommError(CommPort, &dwErrorFlags, &ComStat );			
			return 0xff;
		}
	} 
	return 0;
}


/****************************************************************
 * 函数介绍: 向指定的串口端口读数据
 * 输入函数: 
 				*lpByte :读端口数据
 				dwLength : 读端口数据字节数
 				TimeOut: 读端口数据延迟时间
 * 输出函数: 
 * 函数返回：其他 失败，0 成功
 * 历史纪录：修改人		   日期			  版本号
 ***************************************************************/
static BYTE SerialRead(BYTE *lpByte , WORD dwLength , DWORD timeout)
{
	BOOL		fReadStat;
	DWORD		dwReaded, dwValue;
	DWORD		dwErrorFlags;
	COMSTAT 	ComStat;
	SYSTEMTIME	tStartTime, tCurTime;
	time_t		begin_time, cur_time;
	BYTE		i=0;
	
	time(&begin_time);
	GetLocalTime(&tStartTime);
	while (1)
	{
		ClearCommError(CommPort, &dwErrorFlags, &ComStat);
		fReadStat = ReadFile(CommPort, lpByte+i, dwLength-i,
			&dwReaded, NULL);
		
		if (!fReadStat)
		{
			return 0xff;
		}
		
		if(dwReaded ==0)
		{
			time(&cur_time);
			GetLocalTime(&tCurTime);
			dwValue =(cur_time-begin_time)*1000+tCurTime.wMilliseconds;
			dwValue -=tStartTime.wMilliseconds;
			if (dwValue >=timeout)
				return 0xff;
			
			Sleep(10);
		}
		else
		{
			i +=(BYTE)dwReaded; 
			if (i >= dwLength)
				return 0;
		}
	} 
}

static void PurgeCommBuf(DWORD mode)
{
	PurgeComm(CommPort, mode);
}

static void GetEDC(uchar *str, uint len)
{
	uint i;
	
	str[len]=str[0];
	for(i=1;i<len;i++)
		str[len] ^= str[i];
}


/****************************************************************
 * 函数介绍: 发送交易数据
 * 输入函数: 
 				*Cmd:     交易数据
 				CmdLen : 交易数据字节数
 * 输出函数: 
 * 函数返回：其他 失败，0 成功
 * 历史纪录：修改人		   日期			  版本号
 ***************************************************************/
static uchar SendCmd(uchar *Cmd, uint CmdLen)
{
	if(SerialWrite(Cmd,CmdLen,3)) return COMM_ERR;
	return 0;
}

/****************************************************************
 * 函数介绍: 打开指定串口
 * 输入函数: 
 				*pDevName:串口名称
 * 输出函数: 
 * 函数返回：0xff 失败，0 成功
 * 历史纪录：修改人		   日期			  版本号
 ***************************************************************/
uchar SerialOpen(char *pDevName, DWORD baud)
{
	if (OpenSerialPort(pDevName,baud,8,NOPARITY,ONESTOPBIT) ==FALSE)
	{
		LOG_ERROR("打开串口失败");
		return 0xff;
	}

	LOG_INFO("打开串口成功");
	return 0;
}


/****************************************************************
 * 函数介绍:  关闭指定串口
 * 输入函数: 
 * 输出函数: 
 * 函数返回:
 * 历史纪录：修改人		   日期			  版本号
 ***************************************************************/
void SerialClose(void)
{
	if (CommPort !=INVALID_HANDLE_VALUE)
	{
		CloseHandle(CommPort);
		CommPort = INVALID_HANDLE_VALUE;
		LOG_INFO("关闭串口成功");
	}
}



/****************************************************************
 * 函数介绍: 发送交易数据
 * 输入函数: 
 				*Cmd:     交易数据
 				CmdLen : 交易数据字节数
 * 输出函数: 
 * 函数返回：其他 失败，0 成功
 * 历史纪录：修改人		   日期			  版本号
 ***************************************************************/
int writetocom(uchar *Cmd, uint CmdLen)
{
	SendCmd(Cmd,CmdLen);
	return 0;
}


int sendtocom_RS232(uchar *buf,uint len)
{
	uchar sbuf[50*1024];
	uchar lrc;
	int i ;
	
	BUFCLR(sbuf);
	sbuf[0] = STX; //	02h  报文起始
	sbuf[1] = len / 256;
	sbuf[2] = len % 256;
	memcpy(sbuf + 3, buf, len);
	sbuf[len + 3] = ETX; //03h  报文终止

	lrc = 0x00;
	for (i = 0; i < len + 2 + 1; i++)
		lrc ^= sbuf[i + 1];
	sbuf[len + 4] = lrc;

	PurgeCommBuf(PURGE_RXCLEAR);

	if (SerialWrite(sbuf, len + 5, 3))
	{
		LOG_ERROR("发送串口数据失败");
		return COMM_ERR;
	}
	LOG_INFOSB((char *)sbuf, len + 5, "发送串口数据成功[%d]", len + 5);
	return 0;

}

int readfromcom_RS232(uchar *buf, int *RecvLen, ulong TimeOut)
{
	uchar sbuf[50 * 1024], buffer[50];
	uchar lrc;
	int i, len;
	BUFCLR(sbuf);
	i = 0;
	while (1)
	{
		BUFCLR(buffer);
		if (SerialRead(sbuf, 1, 500) == 0)
		{
			if (sbuf[0] == STX) //	02h  报文起始
				break;
		}
		i++;
		if ((ulong)i > TimeOut * 2)
		{
			LOG_ERROR("串口接收起始字符有误");
			return -1;//接收STX超时
		}
	}
	if (SerialRead(sbuf + 1, 2, 100) != 0)
	{
		LOG_ERROR("串口接收长度字符有误");
		return -2;//接收LEN超时
	}
	len =  sbuf[1]*256 + sbuf[2];

	for (i = 3; i < len + 3 + 2; i++)
	{
		if (SerialRead(sbuf + i, 1, 500) != 0)
		{
			LOG_ERROR("串口接收长度到[%d]个字符时有误", i);
			return -3;//接收LEN=2个字节错误
		}
	}

	if (sbuf[3 + len] != ETX)
	{
		LOG_ERROR("串口接收结束字符有误");
		return -4;//ETX接收有错误
	}
	lrc = 0x00;
	for (i=0; i<len+2+1 ; i ++)//包括ETX LEN
		lrc ^= sbuf[i+1];
	if (sbuf[len + 4] != lrc)
	{
		LOG_ERROR("串口接收校验字符计算有误 [%02X] [%02X]", lrc, sbuf[len + 4]);
		return -5;//lrc校验错
	}

	memcpy(buf, sbuf+3,len);
	*RecvLen = len;

	LOG_INFOSB((char *)sbuf, len + 5, "接收串口数据成功[%d]", len + 5);
	return 0;
}
