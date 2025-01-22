#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#include "config.h"
#include "string"

using namespace std;

#define POSTRANS_OTHER 0
#define POSTRANS_CONSUME 1
#define POSTRANS_CANCEL 2
#define POSTRANS_REFUND 3
#define POSTRANS_CONSUME_REPAIR 4
#define POSTRANS_CANCEL_REPAIR 5

typedef struct
{
    int TRANS;
    string MERCHANT_NO, TERMINAL_NO, TRACE, CARD_NO, BATCH, AMT;
} POS_SEND;

typedef struct
{
    int TRANS;
    string MERCHANT_NO, TERMINAL_NO, TRACE, BATCH, CARD_NO, AMT, REF_NO, TRANS_DATE, TRANS_TIME, RESP_CODE, RESP_MSG;
} POS_RECV;

int initMysql();
int InsertPosRecv(POS_RECV obj);
int InsertPosSend(POS_SEND obj);
void ClearPosSend(POS_SEND& obj);
void ClearPosRecv(POS_RECV& obj);
