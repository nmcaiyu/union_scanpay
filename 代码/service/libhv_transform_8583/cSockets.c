#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "cSockets.h"

#define MAX_SOCKET_PORT_LEN 10

// get sockaddr, IPv4 or IPv6
void *vGetInAddr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    else
    {
        return &(((struct sockaddr_in6 *)sa)->sin6_addr);
    }
}

int Ei_CheckSocketStates(int fd, int waittimeout)
{
    int iRet, iMax, i;
    fd_set fdr;
    struct timeval sTime;
    //将指定的文件描述符集清空
    FD_ZERO(&fdr);
    //用于在文件描述符集合中增加一个新的文件描述符。
    FD_SET(fd, &fdr);
    iMax = fd + 1;
    for (i = 0; i < 1; i++)
    {
        FD_SET(fd, &fdr);
        sTime.tv_sec = waittimeout;
        sTime.tv_usec = 0;  // 100毫秒
        iRet = select(iMax, &fdr, NULL, NULL, &sTime);
        if (iRet == -1)
        {
            return -1;
        }
        else if (iRet > 0)
        {
            return 0;
        }
    }
    return -2;
}

int EI_iCreateServerSocket(const char *pcIP, const int iPort, const int iProtocalFamily, const int iSocktype, const int iNonblocking,
                           const int iBacklog)
{
    if (iPort <= 0)
    {
        return -1;
    }

    struct addrinfo hints, *res, *p;
    int sockfd;
    int yes = 1;
    char *bindip;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = iProtocalFamily;
    hints.ai_socktype = iSocktype;

    if (pcIP == NULL || strlen(pcIP) == 0)
    {
        bindip = NULL;
        hints.ai_flags = AI_PASSIVE;
    }
    else
    {
        bindip = (char *)pcIP;
    }

    char listenPort[MAX_SOCKET_PORT_LEN] = {'\0'};
    sprintf(listenPort, "%d", iPort);

    int status = getaddrinfo(bindip, listenPort, &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo error:%s\n", gai_strerror(status));
        return -1;
    }

    for (p = res; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd <= 0)
        {
            perror("failed to create socket\n");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        {
            close(sockfd);
            perror("failed setsockopt SO_REUSEADDR");
            continue;
        }

        status = bind(sockfd, p->ai_addr, p->ai_addrlen);
        if (status == -1)
        {
            close(sockfd);
            perror("failed to bind socket");
            continue;
        }

        if (iNonblocking == 1)
        {
            if (EI_iSetNonblocking(sockfd) != 1)
            {
                close(sockfd);
                continue;
            }
        }

        if (listen(sockfd, iBacklog) == -1)
        {
            close(sockfd);
            perror("failed to listen");
            continue;
        }
        break;
    }

    freeaddrinfo(res);

    if (p == NULL)
    {
        return -1;
    }
    else
    {
        return sockfd;
    }
}

int EI_iCreateClientSocket(const char *pcTargetIP, const int iPort, const int iProtocalFamily, const int iSocktype)
{
    if (pcTargetIP == NULL || strlen(pcTargetIP) == 0)
    {
        return -1;
    }

    struct addrinfo hints, *serverinfo, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = iProtocalFamily;
    hints.ai_socktype = iSocktype;
    int sockfd;

    char serverport[MAX_SOCKET_PORT_LEN] = {'\0'};
    sprintf(serverport, "%d", iPort);

    int status = getaddrinfo(pcTargetIP, serverport, &hints, &serverinfo);
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo error:%s\n", gai_strerror(status));
        return -1;
    }

    for (p = serverinfo; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd <= 0)
        {
            perror("failed to create socket\n");
            continue;
        }

        // return value of connect is 0  for success or -1 for error
        int rConnect = connect(sockfd, p->ai_addr, p->ai_addrlen);
        if (rConnect == 0)
        {
            freeaddrinfo(serverinfo);
            return sockfd;
        }
        else
        {
            close(sockfd);
            break;
        }
    }

    freeaddrinfo(serverinfo);
    return -1;
}

int EI_iCreateClientSocketWithTimeout(const char *pcTargetIP, const int iPort, const int iProtocalFamily, const int iSocktype, long lSeconds,
                                      long lMicroSeconds)
{
    if (pcTargetIP == NULL || strlen(pcTargetIP) == 0)
    {
        return -1;
    }

    struct addrinfo hints, *serverinfo, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = iProtocalFamily;
    hints.ai_socktype = iSocktype;
    int sockfd;

    char serverport[MAX_SOCKET_PORT_LEN] = {'\0'};
    sprintf(serverport, "%d", iPort);

    int status = getaddrinfo(pcTargetIP, serverport, &hints, &serverinfo);
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo error:%s\n", gai_strerror(status));
        return -1;
    }

    for (p = serverinfo; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd <= 0)
        {
            perror("failed to create socket\n");
            continue;
        }

        if (EI_iSetSocketSendTimeout(sockfd, lSeconds, lMicroSeconds) == -1)
        {
            fprintf(stderr, "failed to set send timeout for connect, sockfd=%d, error is:%s\n", sockfd, strerror(errno));
            close(sockfd);
            break;
        }
        // return value of connect is 0  for success or -1 for error
        int rConnect = connect(sockfd, p->ai_addr, p->ai_addrlen);
        if (rConnect == 0)
        {
            freeaddrinfo(serverinfo);
            return sockfd;
        }
        else
        {
            close(sockfd);
            break;
        }
    }

    freeaddrinfo(serverinfo);
    return -1;
}

int EI_iSetSocketSendTimeout(int iSockfd, long lSeconds, long lMicroSeconds)
{
    struct timeval tv = {0};
    tv.tv_sec = lSeconds;
    tv.tv_usec = lMicroSeconds;

    if (setsockopt(iSockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == 0)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

int EI_iSetSocketRecvTimeout(int iSockfd, long lSeconds, long lMicroSeconds)
{
    struct timeval tv = {0};
    tv.tv_sec = lSeconds;
    tv.tv_usec = lMicroSeconds;

    if (setsockopt(iSockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == 0)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

int EI_iSendBuf(int iSockfd, const char *pbBuffer, const int iBufferLen)
{
    if (iSockfd <= 0 || pbBuffer == NULL || iBufferLen <= 0)
    {
        return -1;
    }

    int iCode = send(iSockfd, pbBuffer, iBufferLen, 0);
    if (iCode == -1)
    {
        fprintf(stderr, "failed to send message to socketfd %d, error no=%d\n", iSockfd, errno);
        return -1;
    }

    return iCode;
}

int EI_iSendBufLoop(int iSockfd, const char *pbBuffer, const int iBufferLen, const int iCountOnce)
{
    if (iSockfd <= 0 || pbBuffer == NULL || iBufferLen <= 0 || iCountOnce <= 0)
    {
        return -1;
    }

    int iHasSent = 0;
    int iSent = 0;
    int iNeedToSend = 0;
    int iRemaining = 0;

    while (iHasSent < iBufferLen)
    {
        iRemaining = iBufferLen - iHasSent;                               //剩余未发送的字节数
        iNeedToSend = iCountOnce < iRemaining ? iCountOnce : iRemaining;  //需要发送的字节数

        iSent = send(iSockfd, pbBuffer + iHasSent, iNeedToSend, 0);

        if (iSent == -1)
        {
            fprintf(stderr, "failed to send message to socketfd %d, error no=%d\n", iSockfd, errno);
            break;
        }
        else
        {
            iHasSent += iSent;
        }
    }

    return iHasSent;
}

int EI_iRecvBuf(int iSockfd, char *pbOutBuffer, const int iBufferLen)
{
    if (iSockfd <= 0 || pbOutBuffer == NULL || iBufferLen <= 0)
    {
        return -1;
    }

    int iCode = recv(iSockfd, (void *)pbOutBuffer, iBufferLen, 0);
    if (iCode == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)  //接收超时
        {
            return -2;
        }

        fprintf(stderr, "failed to receive message from socketfd %d, error =%s\n", iSockfd, strerror(errno));
        return -1;
    }

    return iCode;
}

int EI_iPeekNBytes(int iSockfd, char *pbOutBuffer, const int iExpectedLen, long lSeconds, long lNanoSeconds)
{
    struct timespec ts;

    if (iSockfd <= 0 || pbOutBuffer == NULL || iExpectedLen <= 0)
    {
        return -1;
    }

    int iCode = recv(iSockfd, (void *)pbOutBuffer, iExpectedLen, MSG_PEEK);
    if (iCode != iExpectedLen)
    {
        // sleep for a while and try again
        if (lSeconds > 0 || lNanoSeconds > 0)
        {
            ts.tv_sec = lSeconds;
            ts.tv_nsec = lNanoSeconds;
            nanosleep(&ts, NULL);

            iCode = recv(iSockfd, (void *)pbOutBuffer, iExpectedLen, MSG_PEEK);
        }
    }

    return iCode;
}

int EI_iRecvNBytes(int iSockfd, char *pbOutBuffer, const int iExpectedLen)
{
    int iHasRecv = 0;
    int irecv = 0;
    char *buf = (char *)malloc(sizeof(char) * iExpectedLen);
    if (buf == NULL)
    {
        return 0;
    }
    char *p = pbOutBuffer;

    while (iHasRecv < iExpectedLen)
    {
        irecv = recv(iSockfd, buf, iExpectedLen - iHasRecv, 0);

        if (irecv <= 0)
        {
            break;
        }
        else
        {
            memcpy(p, buf, irecv);
            p += irecv;
            iHasRecv += irecv;
        }
    }

    free(buf);
    return iHasRecv;
}

int EI_iSetNonblocking(int iSockfd)
{
    int opts;
    opts = fcntl(iSockfd, F_GETFL);
    if (opts < 0)
    {
        fprintf(stderr, "failed to set nonblocking for %d, error no=%d", iSockfd, errno);
        return -1;
    }
    opts |= O_NONBLOCK;
    if (fcntl(iSockfd, F_SETFL, opts) < 0)
    {
        fprintf(stderr, "failed to set nonblocking for %d, error no=%d", iSockfd, errno);
        return -1;
    }

    return 1;
}

int EI_iSetBlocking(int iSockfd)
{
    int opts;
    opts = fcntl(iSockfd, F_GETFL);
    if (opts < 0)
    {
        fprintf(stderr, "failed to set blocking for %d, error no=%d", iSockfd, errno);
        return -1;
    }
    opts &= (~O_NONBLOCK);
    if (fcntl(iSockfd, F_SETFL, opts) < 0)
    {
        fprintf(stderr, "failed to set blocking for %d, error no=%d", iSockfd, errno);
        return -1;
    }

    return 1;
}
