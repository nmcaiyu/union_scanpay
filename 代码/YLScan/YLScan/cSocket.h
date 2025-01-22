
#include "StdAfx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
int InitSocket();
int EI_iRecvBuf(int Socket, char *vTcpData, int iBufferLen);
int EI_iRecvNBytes(int Socket, char *vTcpData, int iExpectedLen);
int EI_iSendBuf(int Socket, char *SendBuf, int iCount);
int EI_iCreateClientSocket(char *pcTargetIP, int iPort, int iProtocalFamily, 
                           int iSocktype, char* pcBindIP);
int EI_iCreateClientSocketWithTimeout(char *pcTargetIP, int iPort, int iProtocalFamily, 
                                      int iSocktype, long lSeconds, long lMicroSeconds, 
                                      char *pcBindIP);
int EI_iSetBlocking(int Socket);
int EI_iSetNonBlocking(int Socket);
int EI_iSetSocketSendTimeout(int Socket, long lSeconds, long lMicroSeconds);
int EI_iSetSocketRecvTimeout(int Socket, long lSeconds, long lMicroSeconds);
int EI_iSendBufLoop(int Socket, char *pbBuffer, int iBufferLen, int iCountOnce);
int EI_iCreateServerSocket(char *pcIP, int iPort, int iProtocalFamily, 
                          int iSocktype, int iNonblocking, int iBacklog);
int Ei_CloseSocket(int iSocket);
