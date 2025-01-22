#include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include "Serial.h"
#include "Tools.h"
#include "slog.h"

static HANDLE CommPort=INVALID_HANDLE_VALUE;

/****************************************************************
 * ��������: ��ָ���Ĵ��ڶ˿�
 * ���뺯��: 
 				DevName: ָ�����ں�
 				baud : 	  ����ͨѶ������
 				databit: 	  �����ֽ���
 				parity:	  ������żλ
 				stopbit:	  ����ֹͣλ
 * �������: 
 * �������أ����� ʧ�ܣ�0 �ɹ�
 * ��ʷ��¼���޸���		   ����			  �汾��
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
 * ��������: ��ָ���Ĵ��ڶ˿�д����
 * ���뺯��: 
 				*lpByte : ��Ҫд�˿�����
 				dwBytesToWrite : д�˿������ֽ���
 				TimeOut: д�˿������ӳ�ʱ��
 * �������: 
 * �������أ����� ʧ�ܣ�0 �ɹ�
 * ��ʷ��¼���޸���		   ����			  �汾��
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
 * ��������: ��ָ���Ĵ��ڶ˿ڶ�����
 * ���뺯��: 
 				*lpByte :���˿�����
 				dwLength : ���˿������ֽ���
 				TimeOut: ���˿������ӳ�ʱ��
 * �������: 
 * �������أ����� ʧ�ܣ�0 �ɹ�
 * ��ʷ��¼���޸���		   ����			  �汾��
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
 * ��������: ���ͽ�������
 * ���뺯��: 
 				*Cmd:     ��������
 				CmdLen : ���������ֽ���
 * �������: 
 * �������أ����� ʧ�ܣ�0 �ɹ�
 * ��ʷ��¼���޸���		   ����			  �汾��
 ***************************************************************/
static uchar SendCmd(uchar *Cmd, uint CmdLen)
{
	if(SerialWrite(Cmd,CmdLen,3)) return COMM_ERR;
	return 0;
}

/****************************************************************
 * ��������: ��ָ������
 * ���뺯��: 
 				*pDevName:��������
 * �������: 
 * �������أ�0xff ʧ�ܣ�0 �ɹ�
 * ��ʷ��¼���޸���		   ����			  �汾��
 ***************************************************************/
uchar SerialOpen(char *pDevName, DWORD baud)
{
	if (OpenSerialPort(pDevName,baud,8,NOPARITY,ONESTOPBIT) ==FALSE)
	{
		LOG_ERROR("�򿪴���ʧ��");
		return 0xff;
	}

	LOG_INFO("�򿪴��ڳɹ�");
	return 0;
}


/****************************************************************
 * ��������:  �ر�ָ������
 * ���뺯��: 
 * �������: 
 * ��������:
 * ��ʷ��¼���޸���		   ����			  �汾��
 ***************************************************************/
void SerialClose(void)
{
	if (CommPort !=INVALID_HANDLE_VALUE)
	{
		CloseHandle(CommPort);
		CommPort = INVALID_HANDLE_VALUE;
		LOG_INFO("�رմ��ڳɹ�");
	}
}



/****************************************************************
 * ��������: ���ͽ�������
 * ���뺯��: 
 				*Cmd:     ��������
 				CmdLen : ���������ֽ���
 * �������: 
 * �������أ����� ʧ�ܣ�0 �ɹ�
 * ��ʷ��¼���޸���		   ����			  �汾��
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
	sbuf[0] = STX; //	02h  ������ʼ
	sbuf[1] = len / 256;
	sbuf[2] = len % 256;
	memcpy(sbuf + 3, buf, len);
	sbuf[len + 3] = ETX; //03h  ������ֹ

	lrc = 0x00;
	for (i = 0; i < len + 2 + 1; i++)
		lrc ^= sbuf[i + 1];
	sbuf[len + 4] = lrc;

	PurgeCommBuf(PURGE_RXCLEAR);

	if (SerialWrite(sbuf, len + 5, 3))
	{
		LOG_ERROR("���ʹ�������ʧ��");
		return COMM_ERR;
	}
	LOG_INFOSB((char *)sbuf, len + 5, "���ʹ������ݳɹ�[%d]", len + 5);
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
			if (sbuf[0] == STX) //	02h  ������ʼ
				break;
		}
		i++;
		if ((ulong)i > TimeOut * 2)
		{
			LOG_ERROR("���ڽ�����ʼ�ַ�����");
			return -1;//����STX��ʱ
		}
	}
	if (SerialRead(sbuf + 1, 2, 100) != 0)
	{
		LOG_ERROR("���ڽ��ճ����ַ�����");
		return -2;//����LEN��ʱ
	}
	len =  sbuf[1]*256 + sbuf[2];

	for (i = 3; i < len + 3 + 2; i++)
	{
		if (SerialRead(sbuf + i, 1, 500) != 0)
		{
			LOG_ERROR("���ڽ��ճ��ȵ�[%d]���ַ�ʱ����", i);
			return -3;//����LEN=2���ֽڴ���
		}
	}

	if (sbuf[3 + len] != ETX)
	{
		LOG_ERROR("���ڽ��ս����ַ�����");
		return -4;//ETX�����д���
	}
	lrc = 0x00;
	for (i=0; i<len+2+1 ; i ++)//����ETX LEN
		lrc ^= sbuf[i+1];
	if (sbuf[len + 4] != lrc)
	{
		LOG_ERROR("���ڽ���У���ַ��������� [%02X] [%02X]", lrc, sbuf[len + 4]);
		return -5;//lrcУ���
	}

	memcpy(buf, sbuf+3,len);
	*RecvLen = len;

	LOG_INFOSB((char *)sbuf, len + 5, "���մ������ݳɹ�[%d]", len + 5);
	return 0;
}
