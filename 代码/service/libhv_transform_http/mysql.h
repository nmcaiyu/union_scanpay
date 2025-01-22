#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#include "config.h"
#include "string"

using namespace std;
typedef struct
{
    int SERVICE;
    std::string MERCHANT_ID, OUT_TRADE_NO, OUT_REFUND_NO;
} SCAN_SEND;

typedef struct
{
    int SERVICE;
    char szServiceData[10 + 1];
    std::string MERCHANT_ID, OUT_TRADE_NO, MESSAGE, APPID, OPENID, USER_ID, SUB_OPENID, TRADE_TYPE, TRANSACTION_ID, OUT_TRANSACTION_ID, SUB_APPID,
        TOTAL_FEE, DISCOUNT_FEE, REAL_FEE, BUYER_LOGON_ID, BUYER_USER_ID, BANK_BILLNO, TIME_END, CODE, OUT_REFUND_NO, REFUND_ID, REFUND_FEE,
        MERCHANT_ORDER_NO;
} SCAN_RECV;

int initMysql();
int InsertScanSend(SCAN_SEND obj);
int InsertScanRecv(SCAN_RECV obj);
int UpdateScanRecv(SCAN_RECV obj);
void ClearScanSend(SCAN_SEND& obj);
void ClearScanRecv(SCAN_RECV& obj);