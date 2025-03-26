#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <fstream>

#include "xpack/xpack.h"
#include "xpack/json.h"
#include "xpack/xml.h"

#include <iostream>
#include <cstring>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <openssl/des.h>
#include <openssl/md5.h>

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

typedef struct {
    std::string RSPCOD;
    XPACK(O(RSPCOD));
} POSTRANS_GT_COMMON_RESPONSE;

typedef struct {
    std::string RSPCOD, RSPMSG, POS_TOKEN;
    XPACK(O(RSPCOD, RSPMSG, POS_TOKEN));
} POSTRANS_SIGN_RESPONSE;
typedef struct
{
    string THIRD_ACCESS_CODE, TERM_SN, POS_IMEI, CUST_ID, POS_TOKEN, SYSCOD, U_GPS_ADDREES, TRADING_IP, TXNDAT, TXNTIM, MOBMODEL, APP_VERSION, APP_TERMVERSION, ICC_ID, PINKEY;
    XPACK(O(THIRD_ACCESS_CODE, TERM_SN, POS_IMEI, CUST_ID, POS_TOKEN, SYSCOD, U_GPS_ADDREES, TRADING_IP, TXNDAT, TXNTIM, MOBMODEL, APP_VERSION, APP_TERMVERSION, ICC_ID, PINKEY));
} POSTRANS_GT_COMMON;

typedef struct : public POSTRANS_GT_COMMON
{
	string respCode, respMsg, transId;
	XPACK(O(THIRD_ACCESS_CODE, TERM_SN, POS_IMEI, CUST_ID, POS_TOKEN, SYSCOD, U_GPS_ADDREES, TRADING_IP, TXNDAT, TXNTIM, MOBMODEL, 
		respCode, respMsg, transId));
} POSTRANS_RESPONSE;

typedef struct : public POSTRANS_GT_COMMON{
    XPACK(O(THIRD_ACCESS_CODE, TERM_SN, POS_IMEI, CUST_ID, POS_TOKEN, SYSCOD, U_GPS_ADDREES, TRADING_IP, TXNDAT, TXNTIM, MOBMODEL, APP_VERSION, APP_TERMVERSION, ICC_ID));
} POSTRANS_GT_SIGN_REQUEST;

typedef struct {
    string THREE_ORDER_NO, PINBLK, PAY_WAY, MEDIATYPE, CRDNO, TRACK2, AMOUNT, DCDATA, TRAN_TYPE, DEVICE_TYPE, ENC_RANDOM, TERM_SERIAL_ENCNO, AP_VERSION_NO, ICNUMBER, SIGE_TYPE, BUY_PRO_FLAG, REMARK, SIGN_FLAG, UNI_STRCODE, U_GPS_ADDREES;
    XPACK(O(THREE_ORDER_NO, PINBLK, PAY_WAY, MEDIATYPE, CRDNO, TRACK2, AMOUNT, DCDATA, TRAN_TYPE, DEVICE_TYPE, ENC_RANDOM, TERM_SERIAL_ENCNO, AP_VERSION_NO, ICNUMBER, SIGE_TYPE, BUY_PRO_FLAG, REMARK, SIGN_FLAG, UNI_STRCODE, U_GPS_ADDREES));
} POSTRANS_GT_CONSUME_REQUEST;

typedef struct {
    string THREE_ORDER_NO, ORDER_NO, CRDNO, TRACK2, PAY_WAY, SIGN_FLAG;
    XPACK(O(THREE_ORDER_NO, ORDER_NO, CRDNO, TRACK2, PAY_WAY, SIGN_FLAG));
} POSTRANS_GT_CANCEL_REQUEST;

typedef struct {
    string THREE_ORDER_NO, ORDER_NO, REFUND_AMT, PAY_WAY, CRDNO, TRACK2, SIGN_FLAG, DEVICE_TYPE, ENC_RANDOM, TERM_SERIAL_ENCNO, AP_VERSION_NO, DCDATA, ICNUMBER;
    XPACK(O(THREE_ORDER_NO, ORDER_NO, REFUND_AMT, PAY_WAY, CRDNO, TRACK2, SIGN_FLAG, DEVICE_TYPE, ENC_RANDOM, TERM_SERIAL_ENCNO, AP_VERSION_NO, DCDATA, ICNUMBER));
} POSTRANS_GT_REFUND_REQUEST;

typedef struct {
    string PINBLK, PAY_WAY, MEDIATYPE, CRDNO, TRACK2, DCDATA, DEVICE_TYPE, ENC_RANDOM, TERM_SERIAL_ENCNO, AP_VERSION_NO, ICNUMBER;
    XPACK(O(PINBLK, PAY_WAY, MEDIATYPE, CRDNO, TRACK2, DCDATA, DEVICE_TYPE, ENC_RANDOM, TERM_SERIAL_ENCNO, AP_VERSION_NO, ICNUMBER));
} POSTRANS_GT_QUERY_REQUEST;

typedef struct {
    string OPER_TYPE, TXNLOGID;
    XPACK(O(OPER_TYPE, TXNLOGID));
} POSTRANS_GT_REPRINT_REQUEST;

enum TRANSID
{
	TRANSID_SIGN = 0,
	TRANSID_CONSUME,
	TRANSID_CANCEL,
	TRANSID_REFUND,
	TRANSID_QUERY,
	TRANSID_REPRINT,
	TRANSID_SETTLE,
	TRANSID_GETPOSINFO,
	TRANSID_GETQRCODE,
    TRANSID_DOWNLOADKEY,
    TRANSID_ACTIVE
};

using namespace std;
#ifdef __cplusplus
extern "C" {
#endif
void action(char *recvbuf, int recvLen, char *sendbuf, int *sendLen);
void process8583(char* recvbuf, int recvLen, char* sendbuf, int* sendLen);
void processGT(char* recvbuf, int recvLen, char* sendbuf, int* sendLen, const POSTRANS_RESPONSE& response);
#ifdef __cplusplus
}
#endif