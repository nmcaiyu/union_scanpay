#pragma once
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "ISO8583Engine/iso8583engine.h"
#include "cSockets.h"
#include "config.h"
#include "curl/curl.h"
#include "mysql.h"

#define HTTPUSRTAGENT "User-Agent: Donjin Http 0.1"
#define HTTPCACHECONTROL "Cache-Control: no-cache"
#define HTTPCONTENTTYPE "Content-Type:x-ISO-TPDU/x-auth"
#define HTTPACCEPT "Accept: */*"
#define ISO8583_TOTAL_LEN 2
#define ISO8583_TPDU_LEN 5
#define ISO8583_HEAD_LEN 6

using namespace std;
#ifdef __cplusplus
extern "C" {
#endif
void action(char *recvbuf, int recvLen, char *sendbuf, int *sendLen);
#ifdef __cplusplus
}
#endif
