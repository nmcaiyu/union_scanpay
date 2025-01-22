#ifndef __CSOCKETS_H__
#define __CSOCKETS_H__

#include <sys/socket.h>
#include <unistd.h>

#define SOCKET_BUFFER_SIZE 4096

#ifdef __cplusplus
extern "C" {
#endif

int EI_iCreateServerSocket(const char *pcIP, const int iPort, const int iProtocalFamily, const int iSocktype, const int iNonblocking,
                           const int iBacklog);

int EI_iCreateClientSocket(const char *pcTargetIP, const int iPort, const int iProtocalFamily, const int iSocktype);

int EI_iCreateClientSocketWithTimeout(const char *pcTargetIP, const int iPort, const int iProtocalFamily, const int iSocktype, long lSeconds,
                                      long lMicroSeconds);

int EI_iSetSocketSendTimeout(int iSockfd, long lSeconds, long lMicroSeconds);

int EI_iSetSocketRecvTimeout(int iSockfd, long lSeconds, long lMicroSeconds);

int EI_iSetNonblocking(int iSockfd);

int EI_iSetBlocking(int iSockfd);

int EI_iSendBuf(int iSockfd, const char *pbBuffer, const int iBufferLen);

int EI_iRecvBuf(int iSockfd, char *pbOutBuffer, const int iBufferLen);

int EI_iRecvNBytes(int iSockfd, char *pbOutBuffer, const int iExpectedLen);

int EI_iPeekNBytes(int iSockfd, char *pbOutBuffer, const int iExpectedLen, long lSeconds, long lNanoSeconds);

int EI_iSendBufLoop(int iSockfd, const char *pbBuffer, const int iBufferLen, const int iCountOnce);

int Ei_CheckSocketStates(int fd, int waittimeout);

#ifdef __cplusplus
}
#endif

#endif
