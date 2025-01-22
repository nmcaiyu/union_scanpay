#include "action.h"

ISO8583_FieldFormat SampleFldFmt[ISO8583_MAXFIELD] = {
    {ISO8583TYPE_BIN, 64},                     //  1
    {ISO8583TYPE_BCD | ISO8583TYPE_VAR, 19},   //  2 PAN
    {ISO8583TYPE_BCD, 6},                      //  3 Processing Code
    {ISO8583TYPE_BCD, 12},                     //  4 Amount
    {ISO8583TYPE_BCD, 12},                     //  5
    {ISO8583TYPE_BCD, 12},                     //  6
    {ISO8583TYPE_BCD, 10},                     //  7
    {ISO8583TYPE_ASC, 1},                      //  8
    {ISO8583TYPE_BCD, 8},                      //  9
    {ISO8583TYPE_BCD, 8},                      // 10
    {ISO8583TYPE_BCD, 6},                      // 11 System trace
    {ISO8583TYPE_BCD, 6},                      // 12 Time
    {ISO8583TYPE_BCD, 4},                      // 13 Date
    {ISO8583TYPE_BCD, 4},                      // 14 ExpDate
    {ISO8583TYPE_BCD, 4},                      // 15 Settlement date
    {ISO8583TYPE_ASC, 1},                      // 16
    {ISO8583TYPE_BCD, 4},                      // 17
    {ISO8583TYPE_BCD, 5},                      // 18
    {ISO8583TYPE_BCD, 3},                      // 19
    {ISO8583TYPE_BCD, 3},                      // 20
    {ISO8583TYPE_ASC, 7},                      // 21
    {ISO8583TYPE_BCD, 3},                      // 22 POS entry mode
    {ISO8583TYPE_BCD, 3},                      // 23 IC Application PAN
    {ISO8583TYPE_ASC, 2},                      // 24 NII
    {ISO8583TYPE_BCD, 2},                      // 25
    {ISO8583TYPE_BCD, 2},                      // 26
    {ISO8583TYPE_BCD, 1},                      // 27
    {ISO8583TYPE_BCD, 8},                      // 28
    {ISO8583TYPE_BCD, 8},                      // 29
    {ISO8583TYPE_BCD, 8},                      // 30
    {ISO8583TYPE_BCD, 8},                      // 31
    {ISO8583TYPE_BCD | ISO8583TYPE_VAR, 11},   // 32
    {ISO8583TYPE_BCD | ISO8583TYPE_VAR, 11},   // 33
    {ISO8583TYPE_BCD | ISO8583TYPE_VAR, 28},   // 34
    {ISO8583TYPE_BCD | ISO8583TYPE_VAR, 37},   // 35 Track2
    {ISO8583TYPE_BCD | ISO8583TYPE_VAR, 104},  // 36 Track3
    {ISO8583TYPE_ASC, 12},                     // 37 System Reference No
    {ISO8583TYPE_ASC, 6},                      // 38 System AuthID
    {ISO8583TYPE_ASC, 2},                      // 39 Response Code
    {ISO8583TYPE_ASC, 3},                      // 40
    {ISO8583TYPE_ASC, 8},                      // 41 TID
    {ISO8583TYPE_ASC, 15},                     // 42 CustomID
    {ISO8583TYPE_ASC, 40},                     // 43 Custom Name
    {ISO8583TYPE_ASC | ISO8583TYPE_VAR, 25},   // 44
    {ISO8583TYPE_ASC | ISO8583TYPE_VAR, 76},   // 45 Track1
    {ISO8583TYPE_ASC | ISO8583TYPE_VAR, 999},  // 46
    {ISO8583TYPE_ASC | ISO8583TYPE_VAR, 999},  // 47
    {ISO8583TYPE_BCD | ISO8583TYPE_VAR, 999},  // 48
    {ISO8583TYPE_ASC, 3},                      // 49 Currency Code  Transaction
    {ISO8583TYPE_ASC, 3},                      // 50
    {ISO8583TYPE_ASC, 3},                      // 51
    {ISO8583TYPE_BIN, 64},                     // 52 PIN block Data
    {ISO8583TYPE_BCD, 16},                     // 53 Security Data
    {ISO8583TYPE_ASC | ISO8583TYPE_VAR, 320},  // 54
    {ISO8583TYPE_ASC | ISO8583TYPE_VAR, 999},  // 55 ICC information
    {ISO8583TYPE_ASC | ISO8583TYPE_VAR, 999},  // 56
    {ISO8583TYPE_ASC | ISO8583TYPE_VAR, 999},  // 57
    {ISO8583TYPE_ASC | ISO8583TYPE_VAR, 999},  // 58
    {ISO8583TYPE_ASC | ISO8583TYPE_VAR, 999},  // 59
    {ISO8583TYPE_BCD | ISO8583TYPE_VAR, 999},  // 60 Additional Data
    {ISO8583TYPE_BCD | ISO8583TYPE_VAR, 999},  // 61 Additional Data
    {ISO8583TYPE_ASC | ISO8583TYPE_VAR, 999},  // 62 Additional Data
    {ISO8583TYPE_ASC | ISO8583TYPE_VAR, 999},  // 63 Additional Data
    {ISO8583TYPE_BIN, 64},                     // 64 MAC data
};

static size_t http_response(void* contents, size_t size, size_t nmemb, void* userp)
{
    int isize = 0;
    char strSize[4];
    size_t realsize = size * nmemb;
    char** response_ptr = (char**)userp;

    if (*response_ptr == NULL)
    {
        printf("*response_ptr NULL\n");
        *response_ptr = (char*)calloc(1, realsize + 1 + 4);
        isize = 0;
        memcpy(*response_ptr, &isize, 4);
    }
    else
    {
        memset(strSize, 0, sizeof(strSize));
        memcpy(strSize, *response_ptr, 4);
        isize = *(int*)strSize;
        printf("size==%d\n", isize);
        *response_ptr = (char*)realloc(*response_ptr, isize + realsize + 1 + 4);
    }

    if (*response_ptr == NULL)
    {
        free(*response_ptr);
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy((*response_ptr) + isize + 4, contents, realsize);
    isize += realsize;
    memcpy(*response_ptr, &isize, 4);
    (*response_ptr)[isize + 4] = 0;

    return realsize;
}

void action(char* recvbuf, int recvLen, char* sendbuf, int* sendLen)
{
    POS_SEND posSend;
    POS_RECV posRecv;
    string strPrint8583Data;
    ostringstream ostr;
    unsigned char szData[1024 * 3];
    char szSize[4 + 1];
    int iRet;
    CURLcode res;
    CURL* curlHandle = NULL;
    char* pRes = NULL;
    struct curl_slist* pHeaders = NULL;
    ISO8583_Rec isoReq, isoRes;
    int iFDServer = 0;
    int nLen = 0;

    ClearPosSend(posSend);
    ClearPosRecv(posRecv);

    // Initiate structure RequestIso8583
    ISO8583Engine_ClearAllFields(&isoReq);
    ISO8583Engine_ClearAllFields(&isoRes);

    // Initiate field format
    ISO8583Engine_InitFieldFormat(ISO8583_BITMAP64, &SampleFldFmt[0]);
    ISO8583Engine_HexbufToIso8583(&isoReq, (unsigned char*)recvbuf + ISO8583_TOTAL_LEN + ISO8583_TPDU_LEN + ISO8583_HEAD_LEN);

    strPrint8583Data = "请求8583内容解析为\n";
    ostr.str("");
    for (int i = 0; i <= 64; i++)
    {
        BUFCLR(szData);
        iRet = ISO8583Engine_GetField(&isoReq, i, szData, sizeof(szData));
        if (iRet > 0)
        {
            ostr << "第[" << setw(2) << setfill('0') << i << "]域[" << setw(3) << setfill('0') << iRet << "][" << szData << "]\n";
        }
    }
    strPrint8583Data.append(ostr.str());
    LOG_DEBUG(strPrint8583Data.c_str());

    {
        LOG_TRACE("解析8583发起报文");
        BUFCLR(szData);
        ISO8583Engine_GetField(&isoReq, 0, szData, sizeof(szData));
        if (strcmp((char*)szData, "0200") == 0)
        {
            BUFCLR(szData);
            ISO8583Engine_GetField(&isoReq, 3, szData, sizeof(szData));
            if (memcmp(szData, "00", 2) == 0)
            {
                posSend.TRANS = POSTRANS_CONSUME;
            }
            else if (memcmp(szData, "20", 2) == 0)
            {
                posSend.TRANS = POSTRANS_CANCEL;
            }
        }
        else if (strcmp((char*)szData, "0220") == 0)
        {
            BUFCLR(szData);
            ISO8583Engine_GetField(&isoReq, 3, szData, sizeof(szData));
            if (memcmp(szData, "20", 2) == 0)
            {
                posSend.TRANS = POSTRANS_REFUND;
            }
        }
        else if (strcmp((char*)szData, "0400") == 0)
        {
            BUFCLR(szData);
            ISO8583Engine_GetField(&isoReq, 3, szData, sizeof(szData));
            if (memcmp(szData, "00", 2) == 0)
            {
                posSend.TRANS = POSTRANS_CONSUME_REPAIR;
            }
            else if (memcmp(szData, "20", 2) == 0)
            {
                posSend.TRANS = POSTRANS_CANCEL_REPAIR;
            }
        }

        if (posSend.TRANS != POSTRANS_OTHER)
        {
            BUFCLR(szData);
            ISO8583Engine_GetField(&isoReq, 4, szData, sizeof(szData));
            posSend.AMT = (char*)szData;

            BUFCLR(szData);
            ISO8583Engine_GetField(&isoReq, 11, szData, sizeof(szData));
            posSend.TRACE = (char*)szData;

            BUFCLR(szData);
            ISO8583Engine_GetField(&isoReq, 42, szData, sizeof(szData));
            posSend.MERCHANT_NO = (char*)szData;

            BUFCLR(szData);
            ISO8583Engine_GetField(&isoReq, 41, szData, sizeof(szData));
            posSend.TERMINAL_NO = (char*)szData;

            BUFCLR(szData);
            ISO8583Engine_GetField(&isoReq, 2, szData, sizeof(szData));
            posSend.CARD_NO = (char*)szData;
            if (posSend.CARD_NO.length() == 0)
            {
                BUFCLR(szData);
                ISO8583Engine_GetField(&isoReq, 35, szData, sizeof(szData));
                char* p = strstr((char*)szData, (const char*)"D");
                if (p != NULL)
                {
                    *p = 0x00;
                }
                p = strstr((char*)szData, (const char*)"=");
                if (p != NULL)
                {
                    *p = 0x00;
                }
                posSend.CARD_NO = (char*)szData;
            }

            BUFCLR(szData);
            ISO8583Engine_GetField(&isoReq, 60, szData, sizeof(szData));
            szData[2 + 6] = 0x00;
            posSend.BATCH = (char*)szData + 2;
        }

        InsertPosSend(posSend);

        posRecv.TRANS = posSend.TRANS;
        posRecv.MERCHANT_NO = posSend.MERCHANT_NO;
        posRecv.TERMINAL_NO = posSend.TERMINAL_NO;
        posRecv.TRACE = posSend.TRACE;
        posRecv.BATCH = posSend.BATCH;
        posRecv.CARD_NO = posSend.CARD_NO;
        posRecv.CARD_NO = posSend.AMT;
    }

    if (globalCFG.iHttpEnable == 1)
    {
        curlHandle = curl_easy_init();
        if (curlHandle)
        {
            curl_easy_setopt(curlHandle, CURLOPT_HEADER, 0L);
            curl_easy_setopt(curlHandle, CURLOPT_URL, globalCFG.url);
            curl_easy_setopt(curlHandle, CURLOPT_HTTPPOST, 1);
            curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYHOST, 0);
            curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, recvbuf);
            curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDSIZE, recvLen);
            curl_easy_setopt(curlHandle, CURLOPT_FORBID_REUSE, 1);
            curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT, globalCFG.iRecvTimeout);
            curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1L);
            curl_easy_setopt(curlHandle, CURLOPT_CONNECTTIMEOUT, globalCFG.iConnTimeout);
            curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, http_response);
            curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &pRes);

            pHeaders = curl_slist_append(pHeaders, HTTPUSRTAGENT);
            pHeaders = curl_slist_append(pHeaders, HTTPCACHECONTROL);
            pHeaders = curl_slist_append(pHeaders, HTTPCONTENTTYPE);
            pHeaders = curl_slist_append(pHeaders, HTTPACCEPT);
            curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, pHeaders);
            res = curl_easy_perform(curlHandle);
            if (res == CURLE_OK && pRes != NULL)
            {
                BUFCLR(szSize);
                memcpy(szSize, pRes, 4);
                *sendLen = *(int*)szSize;
                memcpy(sendbuf, pRes + 4, *sendLen);
                LOG_INFOSB(sendbuf, *sendLen, "LIBCURL通讯成功[%d]", *sendLen);
            }
            else
            {
                LOG_ERROR("LIBCURl通讯失败 [%s] [%s]", globalCFG.url, curl_easy_strerror(res));

                goto cleanUP;
            }
        }
        else
        {
            LOG_ERROR("LIBCURl申请失败");
            goto cleanUP;
        }
    }
    else
    {
        iFDServer = EI_iCreateClientSocketWithTimeout(globalCFG.szSocketIp, globalCFG.iSocketPort, AF_INET, SOCK_STREAM, globalCFG.iConnTimeout, 0);
        if (iFDServer <= 0)
        {
            posRecv.RESP_CODE = "XX";
            posRecv.RESP_MSG = "SOCKET链接失败";
            LOG_ERROR("SOCKET链接失败 [%s:%d] [%d]", globalCFG.szSocketIp, globalCFG.iSocketPort, iFDServer);
            goto cleanUP;
        }
        if (EI_iSetSocketRecvTimeout(iFDServer, globalCFG.iRecvTimeout, 0) == -1)
        {
            LOG_ERROR("SOCKET设置超时时间失败 [%s:%d]", globalCFG.szSocketIp, globalCFG.iSocketPort);
        }
        nLen = EI_iSendBuf(iFDServer, recvbuf, recvLen);
        if (nLen < 0)
        {
            posRecv.RESP_CODE = "XX";
            posRecv.RESP_MSG = "SOCKET发送报文失败";
            LOG_ERRORSB(recvbuf, recvLen, "SOCKET发送报文失败 [%s:%d] [%d]", globalCFG.szSocketIp, globalCFG.iSocketPort, nLen);
            goto cleanUP;
        }
        nLen = EI_iRecvNBytes(iFDServer, sendbuf, ISO8583_TOTAL_LEN);
        if (nLen != ISO8583_TOTAL_LEN)
        {
            posRecv.RESP_CODE = "XX";
            posRecv.RESP_MSG = "SOCKET接收报文头失败";
            LOG_ERRORSB(sendbuf, ISO8583_TOTAL_LEN, "SOCKET接收报文头失败 [%s:%d] [%d]", globalCFG.szSocketIp, globalCFG.iSocketPort, nLen);
            goto cleanUP;
        }

        *sendLen = ((unsigned char)(sendbuf[0])) * 256 + (unsigned char)sendbuf[1] + ISO8583_TOTAL_LEN;
        LOG_DEBUG("准备接收报文[%d]", *sendLen);
        nLen = EI_iRecvNBytes(iFDServer, sendbuf + ISO8583_TOTAL_LEN, *sendLen - ISO8583_TOTAL_LEN);
        if (nLen != *sendLen - ISO8583_TOTAL_LEN)
        {
            posRecv.RESP_CODE = "XX";
            posRecv.RESP_MSG = "SOCKET接收报文内容失败";
            LOG_ERRORSB(sendbuf, *sendLen, "SOCKET接收报文内容失败 [%s:%d] [%d]", globalCFG.szSocketIp, globalCFG.iSocketPort, nLen);
            goto cleanUP;
        }
    }

    ISO8583Engine_HexbufToIso8583(&isoRes, (unsigned char*)sendbuf + ISO8583_TOTAL_LEN + ISO8583_TPDU_LEN + ISO8583_HEAD_LEN);

    strPrint8583Data = "返回8583内容解析为\n";
    ostr.str("");
    for (int i = 0; i <= 64; i++)
    {
        BUFCLR(szData);
        iRet = ISO8583Engine_GetField(&isoRes, i, szData, sizeof(szData));
        if (iRet > 0)
        {
            ostr << "第[" << setw(2) << setfill('0') << i << "]域[" << setw(3) << setfill('0') << iRet << "][" << szData << "]\n";
        }
    }
    strPrint8583Data.append(ostr.str());
    LOG_DEBUG(strPrint8583Data.c_str());

    {
        ClearPosRecv(posRecv);
        LOG_TRACE("解析8583返回报文");
        BUFCLR(szData);
        ISO8583Engine_GetField(&isoRes, 0, szData, sizeof(szData));
        if (strcmp((char*)szData, "0210") == 0)
        {
            BUFCLR(szData);
            ISO8583Engine_GetField(&isoRes, 3, szData, sizeof(szData));
            if (memcmp(szData, "00", 2) == 0)
            {
                posRecv.TRANS = POSTRANS_CONSUME;
            }
            else if (memcmp(szData, "20", 2) == 0)
            {
                posRecv.TRANS = POSTRANS_CANCEL;
            }
        }
        else if (strcmp((char*)szData, "0230") == 0)
        {
            BUFCLR(szData);
            ISO8583Engine_GetField(&isoRes, 3, szData, sizeof(szData));
            if (memcmp(szData, "20", 2) == 0)
            {
                posRecv.TRANS = POSTRANS_REFUND;
            }
        }
        if (strcmp((char*)szData, "0410") == 0)
        {
            BUFCLR(szData);
            ISO8583Engine_GetField(&isoRes, 3, szData, sizeof(szData));
            if (memcmp(szData, "00", 2) == 0)
            {
                posRecv.TRANS = POSTRANS_CONSUME_REPAIR;
            }
            else if (memcmp(szData, "20", 2) == 0)
            {
                posRecv.TRANS = POSTRANS_CANCEL_REPAIR;
            }
        }
        if (posRecv.TRANS != POSTRANS_OTHER)
        {
            BUFCLR(szData);
            ISO8583Engine_GetField(&isoRes, 4, szData, sizeof(szData));
            posRecv.AMT = (char*)szData;

            BUFCLR(szData);
            ISO8583Engine_GetField(&isoRes, 12, szData, sizeof(szData));
            posRecv.TRANS_TIME = (char*)szData;

            BUFCLR(szData);
            ISO8583Engine_GetField(&isoRes, 13, szData, sizeof(szData));
            posRecv.TRANS_DATE = (char*)szData;

            BUFCLR(szData);
            ISO8583Engine_GetField(&isoRes, 37, szData, sizeof(szData));
            posRecv.REF_NO = (char*)szData;

            BUFCLR(szData);
            ISO8583Engine_GetField(&isoRes, 39, szData, sizeof(szData));
            posRecv.RESP_CODE = (char*)szData;

            BUFCLR(szData);
            ISO8583Engine_GetField(&isoRes, 11, szData, sizeof(szData));
            posRecv.TRACE = (char*)szData;

            BUFCLR(szData);
            ISO8583Engine_GetField(&isoRes, 42, szData, sizeof(szData));
            posRecv.MERCHANT_NO = (char*)szData;

            BUFCLR(szData);
            ISO8583Engine_GetField(&isoRes, 41, szData, sizeof(szData));
            posRecv.TERMINAL_NO = (char*)szData;

            BUFCLR(szData);
            ISO8583Engine_GetField(&isoRes, 2, szData, sizeof(szData));
            posRecv.CARD_NO = (char*)szData;
            if (posRecv.CARD_NO.length() == 0)
            {
                BUFCLR(szData);
                ISO8583Engine_GetField(&isoReq, 35, szData, sizeof(szData));
                char* p = strstr((char*)szData, (const char*)"D");
                if (p != NULL)
                {
                    *p = 0x00;
                }
                p = strstr((char*)szData, (const char*)"=");
                if (p != NULL)
                {
                    *p = 0x00;
                }
                posRecv.CARD_NO = (char*)szData;
            }
            BUFCLR(szData);
            ISO8583Engine_GetField(&isoRes, 60, szData, sizeof(szData));
            szData[2 + 6] = 0x00;
            posRecv.BATCH = (char*)szData + 2;
        }
    }

cleanUP:
    InsertPosRecv(posRecv);
    if (pRes != NULL)
    {
        free(pRes);
        pRes = NULL;
    }
    if (pHeaders != NULL)
    {
        curl_slist_free_all(pHeaders);
    }
    if (curlHandle != NULL)
    {
        curl_easy_cleanup(curlHandle);
    }
    if (iFDServer > 0)
    {
        close(iFDServer);
    }
    return;
}