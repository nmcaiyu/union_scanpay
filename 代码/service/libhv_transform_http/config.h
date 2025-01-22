#pragma once
#include "INIReader.h"
#include "slog.h"
#ifndef MAXPACKLEN
#define MAXPACKLEN 1024 * 5
#endif // !MAXPACKLEN

#ifndef BUFCLR
#define BUFCLR(x) memset(x, 0x00, sizeof(x))
#endif // !BUFCLR

#ifndef MAX_URLLEN
#define MAX_URLLEN 1024
#endif // !MAX_URLLEN

typedef struct _CFG
{
    char host[100];
    int port;
    int process_num;
    int thread_num;

    char url[MAX_URLLEN];
    char iConnTimeout;
    char iRecvTimeout;

    // mysql
    char szMysqlUrl[MAX_URLLEN];
    char szMysqlDB[100 + 1];
    int iMysqlPort;
    char szMysqlUser[100 + 1];
    char szMysqlPass[100 + 1];
    int iMysqlMin, iMysqlMax;
} CFG;

#ifndef SERVICE_PAY
#define SERVICE_PAY 1
#endif

#ifndef SERVICE_REF
#define SERVICE_REF 2
#endif

#ifdef __cplusplus
extern "C" {
#endif

    extern CFG globalCFG;
    int readIni();

#ifdef __cplusplus
}
#endif