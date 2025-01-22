#include "cSocket.h"
#include "slog.h"

int InitSocket()
{
    int iRet;
    
    WORD dwVersion = MAKEWORD(2, 2);//获取套接字版本号
    WSADATA WsaData;
    iRet = WSAStartup(dwVersion, &WsaData);
    if( iRet != 0 )
    {
        return -1;
    }
    return 0;
}

int EI_iRecvBuf(int iSocket , char *pbOutBuffer , int iOutBufferLen)
{
    int iRet;
    char *pData;
    
    pData = (char *)pbOutBuffer;
    iRet = recv(iSocket, pData, iOutBufferLen, 0);
    if(iRet == -1)
    {
		LOG_ERROR("Error:%d", WSAGetLastError());
        return -1;
    }
    else if(iRet == 0)
    {
        return 0;
    }
    else
    {
		LOG_DEBUGSB(pData, iOutBufferLen, "接收Socket[%d]", iOutBufferLen);
        return iRet;
    }
}

int EI_iSendBuf(int Socket, char * pbBuffer, int iBufferLen)
{
    int iRet = 0;
	LOG_DEBUGSB(pbBuffer, iBufferLen, "发送Socket[%d]", iBufferLen);
    if(iRet = send(Socket, pbBuffer, iBufferLen, 0 ) == -1)
    {
        Ei_CloseSocket(Socket);
		LOG_ERROR("SOCKET 发送数据失败,返回码[-1]");
        return -1;
    }
    return iRet;
}

int EI_iCreateClientSocket(char *pcTargetIP , int iPort, int iProtocalFamily, int iSocktype, char* pcBindIP)
{
    int iRet;
    SOCKET Socket;
    struct sockaddr_in client_addr;	
    InitSocket();
    if((Socket = socket(iProtocalFamily, iSocktype, 0)) == INVALID_SOCKET )
    {
		LOG_ERROR("创建SOCKET失败,返回码[-1]");
        return -1;
    }
    if((pcBindIP != NULL) && (strlen(pcBindIP) > 0))
    {
        memset(&client_addr, 0, sizeof(struct sockaddr_in));
        client_addr.sin_family = AF_INET;
        client_addr.sin_addr.s_addr = inet_addr(pcBindIP);
        client_addr.sin_port = htons(0);
        
        if(bind(Socket, (struct sockaddr *)(&client_addr), sizeof(struct sockaddr)) == -1)
        {
            Ei_CloseSocket(Socket);
			LOG_ERROR("Socket绑定到IP%s失败,返回码[-1]", pcBindIP);
            return -1;
        }
    }
    //连接Socket
    struct sockaddr_in sa;
    sa.sin_family = iProtocalFamily;
    sa.sin_addr.s_addr = inet_addr(pcTargetIP);
    sa.sin_port = htons(iPort);
    iRet=connect(Socket, (struct sockaddr *)&sa, sizeof(sa));
    if (iRet == SOCKET_ERROR)
    {
        Ei_CloseSocket(Socket);
		LOG_ERROR("SOCKET connect 次数超过5次失败,返回码[-1]");
        return -1;	
    }
    //查看Socket是否可读写
    struct timeval timeout;
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(Socket, &fd);
    timeout.tv_sec = 2;
    timeout.tv_usec =0;
    iRet = select(0, 0, &fd, 0, &timeout);
    if (iRet <= 0)
    {
        Ei_CloseSocket(Socket);
		LOG_ERROR("SOCKET 连接不上,返回码[-1]");
        return -1;
    } 	
    return Socket;
}

int EI_iSetBlocking(int Socket)
{
    int iRet;
    unsigned long ulFlag = 0;
    iRet = ioctlsocket(Socket, FIONBIO, (unsigned long*)&ulFlag);//set blocking
    if (iRet != 0)
    {
        Ei_CloseSocket(Socket);
		LOG_ERROR("SOCKET 设置阻塞模式失败,返回码[-1]");
        return -1;	
    }
    return 1;
}

int EI_iSetNonBlocking(int Socket)
{
    int iRet;
    unsigned long ulFlag = 1;
    iRet = ioctlsocket(Socket, FIONBIO, (unsigned long*)&ulFlag);//set not blocking
    if (iRet != 0)
    {
        Ei_CloseSocket(Socket);
		LOG_ERROR("SOCKET 设置非阻塞模式失败,返回码[-1]");
        return -1;
    }
    return 1;
}

int EI_iSetSocketSendTimeout(int Socket, long lSeconds, long lMilliSeconds)
{
    int timeout = lSeconds * 1000 + lMilliSeconds;
    if (setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) == SOCKET_ERROR)
    {
        Ei_CloseSocket(Socket);
		LOG_ERROR("SOCKET设置发送超时失败,返回码[-1]");
        return -1;
    }
    return 1;
}

int EI_iSetSocketRecvTimeout(int Socket, long lSeconds, long lMilliSeconds)
{
    int timeout = lSeconds * 1000 + lMilliSeconds;
    if (setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) == SOCKET_ERROR)
    {
        Ei_CloseSocket(Socket);
		LOG_ERROR("SOCKET设置接收超时失败,返回码[-1]");
        return -1;
    }
    return 1;
}

int EI_iRecvNBytes(int Socket,  char *pbOutBuffer, int iExpectedLen)
{
    int iRet;
    char  *pData;
    pData = (char *)pbOutBuffer;
    
    iRet = recv(Socket, pData, iExpectedLen, 0);
    if( iRet == -1)
    {
        return -1;
    }
    else if(iRet == 0)
    {
        return 0;
    }
    else
    {
		LOG_DEBUGSB(pData, iExpectedLen, "接收Socket[%d]", iExpectedLen);
        return iRet;
    }
}

int EI_iCreateClientSocketWithTimeout(char *pcTargetIP , int iPort, int iProtocalFamily, 
                                      int iSocktype, long lSeconds, long lMicroSeconds, 
                                      char *pcBindIP)
{
    int iRet;
    SOCKET Socket;
    struct sockaddr_in client_addr;	
    InitSocket();
    if((Socket = socket(iProtocalFamily, iSocktype, 0)) == INVALID_SOCKET )
    {
		LOG_ERROR("创建SOCKET失败,返回码[-1]");
        return -1;
    }
    if(EI_iSetSocketSendTimeout(Socket, lSeconds, lMicroSeconds) == -1)
    {
        Ei_CloseSocket(Socket);
		LOG_ERROR("SOCKET设置接收超时失败,返回码[-7]");
        return -1;
    }

    if((pcBindIP != NULL) && (strlen(pcBindIP) > 0))
    {
        memset(&client_addr, 0, sizeof(struct sockaddr_in));
        client_addr.sin_family = AF_INET;
        client_addr.sin_addr.s_addr = inet_addr(pcBindIP);
        client_addr.sin_port = htons(0);
        
        if(bind(Socket, (struct sockaddr *)(&client_addr), sizeof(struct sockaddr)) == -1)
        {
			LOG_ERROR("Socket绑定到IP%s失败,返回码[-1]", pcBindIP);
            Ei_CloseSocket(Socket);
            return -1;
        }
    }

    //连接Socket
    struct sockaddr_in sa;
    sa.sin_family = iProtocalFamily;
    sa.sin_addr.s_addr = inet_addr(pcTargetIP);
    sa.sin_port = htons(iPort);
    iRet = connect(Socket, (struct sockaddr *)&sa, sizeof(sa));
    if (iRet == SOCKET_ERROR)
    {
        Ei_CloseSocket(Socket);
		LOG_ERROR("SOCKET connect 次数超过5次失败,返回码[-11]");
        return -1;	
    }
    

    //查看Socket是否可读写
    struct timeval timeout;
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(Socket, &fd);
    timeout.tv_sec = 2;
    timeout.tv_usec =0;
    iRet = select(0, 0, &fd, 0, &timeout);
    if (iRet <= 0)
    {
        Ei_CloseSocket(Socket);
		LOG_ERROR("SOCKET 连接不上,返回码[-1]");
        return -1;
    }
    return Socket;
}

int EI_iSendBufLoop(int iSockfd, char *pbBuffer, int iBufferLen, int iCountOnce)
{
    int iHasSent = 0;
    int iSent = 0;
    int iNeedToSend = 0;
    int iRemaining = 0;
	
	LOG_DEBUGSB(pbBuffer, iBufferLen, "发送Socket[%d]", iBufferLen);
    
    while(iHasSent < iBufferLen) 
    {
        iRemaining = iBufferLen - iHasSent;//剩余未发送的字节数
        iNeedToSend = iCountOnce < iRemaining ? iCountOnce : iRemaining;//需要发送的字节数
        
        iSent = send(iSockfd, pbBuffer+iHasSent, iNeedToSend, 0);
        
        if(iSent == -1)
        {
			LOG_ERROR("Socket发送错误,返回码[-1]");
            return -1;
        }
        else
        {
            iHasSent += iSent;
        }
    }
    return iBufferLen;
}

int EI_iCreateServerSocket(char *pcIP, int iPort, int iProtocalFamily, 
                          int iSocktype, int iNonblocking, int iBacklog)
{
    int iRet;
    SOCKET Socket;
    bool bFlag = false;
    InitSocket();

    Socket = socket(iProtocalFamily, iSocktype, 0);
    if (Socket == INVALID_SOCKET)
    {
        return -1;
    }
    if(setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, (char *)&bFlag, sizeof(bFlag)) == -1)
    {
        Ei_CloseSocket(Socket);
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(iPort);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(Socket, (SOCKADDR *)&server_addr, sizeof(server_addr)) == -1)
    {
        Ei_CloseSocket(Socket);
        return -1;
    } 

    if (iNonblocking == 1)
    {
        if(EI_iSetNonBlocking(Socket) != 1)
        {
            Ei_CloseSocket(Socket);
            return -1;
        }
    }
    else
    {
        if(EI_iSetBlocking(Socket) != 1)
        {
            Ei_CloseSocket(Socket);
            return -1;
        }
    }

    if (listen(Socket, iBacklog) == -1)
    {
        Ei_CloseSocket(Socket);
        return -1;
    }

    return Socket;
}

int Ei_CloseSocket(int iSocket)
{
	closesocket(iSocket);
	WSACleanup();
	return 0;
}

