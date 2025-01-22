#include "action.h"

static size_t http_response(void *contents, size_t size, size_t nmemb, void *userp)
{
	int isize = 0;
	char strSize[4];
	size_t realsize = size * nmemb;
	char **response_ptr = (char **)userp;

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
		*response_ptr = (char *)realloc(*response_ptr, isize + realsize + 1 + 4);
	}

	if (*response_ptr == NULL)
	{
		free(*response_ptr);
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	printf("indata==%s\n", contents);
	memcpy((*response_ptr) + isize + 4, contents, realsize);
	isize += realsize;
	memcpy(*response_ptr, &isize, 4);
	(*response_ptr)[isize + 4] = 0;

	return realsize;
}

void DeleteCDATA(char* pDesxml, char* pSrcxml)
{
    int i = 0;
    int iBit = 0;
    char szStartstr[10];
    char szEndstr[4];

    memset(szStartstr, 0, sizeof(szStartstr));
    strcpy(szStartstr, "<![CDATA[");
    memset(szEndstr, 0, sizeof(szEndstr));
    strcpy(szEndstr, "]]>");

    for (i = 0; i < strlen(pSrcxml); i++)
    {
        if (memcmp(pSrcxml + i, szStartstr, strlen(szStartstr)) != 0)
        {
            pDesxml[iBit++] = pSrcxml[i];
        }
        else
        {
            i += strlen(szStartstr);
            for (; i < strlen(pSrcxml); i++)
            {
                if (memcmp(pSrcxml + i, szEndstr, strlen(szEndstr)) != 0)
                {
                    pDesxml[iBit++] = pSrcxml[i];
                }
                else
                {
                    i += strlen(szEndstr) - 1;
                    break;
                }
            }
        }
    }
}

string trade_micropay_getmsg(trade_micropay_response res)
{
    if (res.pay_info.length() > 0)
    {
        return res.pay_info;
    }
    else if (res.err_msg.length() > 0)
    {
        return res.err_msg;
    }
    else if (res.message.length() > 0)
    {
        return res.message;
    }
    else
    {
        return "未知返回信息";
    }
}

string trade_query_getmsg(trade_query_response res)
{
    if (res.trade_state_desc.length() > 0)
    {
        return res.trade_state_desc;
    }
    if (res.err_msg.length() > 0)
    {
        return res.err_msg;
    }
    else if (res.message.length() > 0)
    {
        return res.message;
    }
    else
    {
        if (res.trade_state.compare("REFUND") == 0)
        {
            return "[REFUND]-转入退款";
        }
        else if (res.trade_state.compare("NOTPAY") == 0)
        {
            return "[NOTPAY]-未支付";
        }
        else if (res.trade_state.compare("CLOSED") == 0)
        {
            return "[CLOSED]-已关闭";
        }
        else if (res.trade_state.compare("REFUND") == 0 || res.trade_state.compare("REVERSE") == 0)
        {
            return "[REVOKED/REVERSE]-转入退款";
        }
        else if (res.trade_state.compare("USERPAYING") == 0)
        {
            return "[USERPAYING]-用户支付中";
        }
        else if (res.trade_state.compare("PAYERROR") == 0)
        {
            return "[PAYERROR]-支付失败";
        }
        else
        {
            return "未知返回信息";
        }
    }
}

string trade_refund_getmsg(trade_refund_response res)
{
    if (res.err_msg.length() > 0)
    {
        return res.err_msg;
    }
    else if (res.message.length() > 0)
    {
        return res.message;
    }
    else
    {
        return "未知返回信息";
    }
}

string trade_refundquery_getmsg(trade_refundquery_response res)
{
    if (res.message.length() > 0)
    {
        return res.message;
    }
    else
    {
        if (res.refund_status_0.compare("SUCCESS") == 0)
        {
            return "[SUCCESS]-退款成功";
        }
        else if (res.refund_status_0.compare("FAIL") == 0)
        {
            return "[FAIL]-退款失败";
        }
        else if (res.refund_status_0.compare("PROCESSING") == 0)
        {
            return "[PROCESSING]-退款处理中";
        }
        else if (res.refund_status_0.compare("NOTSURE") == 0)
        {
            return "[NOTSURE]-未确定, 需要商户原退款单号重新发起";
        }
        else if (res.refund_status_0.compare("CHANGE") == 0)
        {
            return "[CHANGE]-转入代发, 请通过线下或者平台转账的方式进行退款";
        }
        else
        {
            return "未知返回信息";
        }
    }
}

void analyzeXML(trade_content_request req, char* xmlReturn)
{
    SCAN_RECV scanRecv;
    ClearScanRecv(scanRecv);
    char szXML[1024 * 10];
    BUFCLR(szXML);
    DeleteCDATA((char*)szXML, (char*)xmlReturn);
    if (req.service.compare("unified.trade.micropay") == 0)
    {
        trade_micropay_response res;
        try
        {
            scanRecv.SERVICE = SERVICE_PAY;
            xpack::xml::decode(szXML, res);
            scanRecv.MERCHANT_ID = res.mch_id;
            scanRecv.OUT_TRADE_NO = res.out_trade_no;
            if (res.out_trade_no.length() == 0)
            {
                scanRecv.OUT_TRADE_NO = req.out_trade_no;
            }
            scanRecv.APPID = res.appid;
            scanRecv.OPENID = res.openid;
            scanRecv.USER_ID = res.user_id;
            scanRecv.SUB_OPENID = res.sub_openid;
            scanRecv.TRADE_TYPE = res.trade_type;
            scanRecv.TRANSACTION_ID = res.transaction_id;
            scanRecv.OUT_TRANSACTION_ID = res.out_transaction_id;
            scanRecv.SUB_APPID = res.sub_appid;
            scanRecv.TOTAL_FEE = res.total_fee;
            // scanRecv.DISCOUNT_FEE = ;
            // scanRecv.REAL_FEE = ;
            scanRecv.BUYER_LOGON_ID = res.buyer_logon_id;
            scanRecv.BUYER_USER_ID = res.buyer_user_id;
            scanRecv.BANK_BILLNO = res.bank_billno;
            scanRecv.TIME_END = res.time_end;
            if (res.need_query.compare("Y") == 0)
            {
                scanRecv.CODE = "01";
                scanRecv.MESSAGE = trade_micropay_getmsg(res);
            }
            else if (res.status.compare("0") != 0)
            {
                scanRecv.CODE = "02";
                scanRecv.MESSAGE = trade_micropay_getmsg(res);
            }
            else if (res.result_code.compare("0") != 0)
            {
                if (res.err_code.compare("SYSTEMERROR") == 0 || res.err_code.compare("Internal error") == 0 ||
                    res.err_code.compare("BANKERROR") == 0 || res.err_code.compare("10003") == 0 || res.err_code.compare("USERPAYING") == 0 ||
                    res.err_code.compare("System error") == 0 || res.err_code.compare("aop.ACQ.SYSTEM_ERROR") == 0 ||
                    res.err_code.compare("ACQ.SYSTEM_ERROR") == 0)
                {
                    scanRecv.CODE = "01";
                    scanRecv.MESSAGE = trade_micropay_getmsg(res);
                }
                else if (res.err_code.compare("04") == 0 || res.err_code.compare("06") == 0)
                {
                    scanRecv.CODE = "01";
                    scanRecv.MESSAGE = trade_micropay_getmsg(res);
                }
                else if (res.err_code.compare("00") == 0 && res.trade_type.compare("pay.unionpay.micropay") == 0)
                {
                    scanRecv.CODE = "00";
                    scanRecv.MESSAGE = "";
                }
                else
                {
                    scanRecv.CODE = "02";
                    scanRecv.MESSAGE = trade_micropay_getmsg(res);
                }
            }
            else if (res.pay_result.compare("0") != 0)
            {
                scanRecv.CODE = "02";
                scanRecv.MESSAGE = trade_micropay_getmsg(res);
            }
            else
            {
                scanRecv.CODE = "00";
                scanRecv.MESSAGE = "";
            }
        } catch (const std::exception& e)
        {
            LOG_ERROR("返回xml报文解析失败");
            return;
        }
        InsertScanRecv(scanRecv);
    }
    else if (req.service.compare("unified.trade.refund") == 0)
    {
        trade_refund_response res;
        try
        {
            scanRecv.SERVICE = SERVICE_REF;
            xpack::xml::decode(szXML, res);
            scanRecv.MERCHANT_ID = res.mch_id;
            scanRecv.OUT_TRADE_NO = res.out_trade_no;
            if (res.out_trade_no.length() == 0)
            {
                scanRecv.OUT_TRADE_NO = req.out_trade_no;
            }
            scanRecv.TRADE_TYPE = res.trade_type;
            scanRecv.TRANSACTION_ID = res.transaction_id;
            scanRecv.OUT_REFUND_NO = res.out_refund_no;
            if (res.out_refund_no.length() == 0)
            {
                scanRecv.OUT_REFUND_NO = req.out_refund_no;
            }
            scanRecv.REFUND_ID = res.refund_id;
            scanRecv.REFUND_FEE = res.refund_fee;
            if (res.status.compare("0") != 0)
            {
                scanRecv.CODE = "02";
                scanRecv.MESSAGE = trade_refund_getmsg(res);
            }
            else if (res.result_code.compare("0") != 0)
            {
                scanRecv.CODE = "02";
                scanRecv.MESSAGE = trade_refund_getmsg(res);
            }
            else
            {
                scanRecv.CODE = "00";
                scanRecv.MESSAGE = "";
            }
        } catch (const std::exception& e)
        {
            LOG_ERROR("返回xml报文解析失败");
            return;
        }
        InsertScanRecv(scanRecv);
    }
    else if (req.service.compare("unified.trade.query") == 0)
    {
        trade_query_response res;
        try
        {
            scanRecv.SERVICE = SERVICE_PAY;
            xpack::xml::decode(szXML, res);
            scanRecv.MERCHANT_ID = res.mch_id;
            scanRecv.OUT_TRADE_NO = res.out_trade_no;
            if (res.out_trade_no.length() == 0)
            {
                scanRecv.OUT_TRADE_NO = req.out_trade_no;
            }
            scanRecv.APPID = res.appid;
            scanRecv.OPENID = res.openid;
            scanRecv.USER_ID = res.user_id;
            scanRecv.SUB_OPENID = res.sub_openid;
            scanRecv.TRADE_TYPE = res.trade_type;
            scanRecv.TRANSACTION_ID = res.transaction_id;
            scanRecv.OUT_TRANSACTION_ID = res.out_transaction_id;
            scanRecv.SUB_APPID = res.sub_appid;
            scanRecv.TOTAL_FEE = res.total_fee;
            // scanRecv.DISCOUNT_FEE = ;
            // scanRecv.REAL_FEE = ;
            scanRecv.BUYER_LOGON_ID = res.buyer_logon_id;
            scanRecv.BUYER_USER_ID = res.buyer_user_id;
            scanRecv.BANK_BILLNO = res.bank_billno;
            scanRecv.TIME_END = res.time_end;

            if (res.status.compare("0") != 0)
            {
                scanRecv.CODE = "02";
                scanRecv.MESSAGE = trade_query_getmsg(res);
            }
            else if (res.result_code.length() > 0 && res.result_code.compare("0") != 0)
            {
                if (res.err_code.compare("SYSTEMERROR") == 0 || res.err_code.compare("Internal error") == 0 ||
                    res.err_code.compare("BANKERROR") == 0 || res.err_code.compare("10003") == 0 || res.err_code.compare("USERPAYING") == 0 ||
                    res.err_code.compare("System error") == 0 || res.err_code.compare("aop.ACQ.SYSTEM_ERROR") == 0 ||
                    res.err_code.compare("ACQ.SYSTEM_ERROR") == 0)
                {
                    scanRecv.CODE = "01";
                    scanRecv.MESSAGE = trade_query_getmsg(res);
                }
                else if (res.err_code.compare("04") == 0 || res.err_code.compare("06") == 0)
                {
                    scanRecv.CODE = "01";
                    scanRecv.MESSAGE = trade_query_getmsg(res);
                }
                else if (res.err_code.compare("00") == 0 && res.trade_type.compare("pay.unionpay.micropay") == 0)
                {
                    scanRecv.CODE = "00";
                    scanRecv.MESSAGE = "";
                }
                else
                {
                    scanRecv.CODE = "02";
                    scanRecv.MESSAGE = trade_query_getmsg(res);
                }
            }
            else if (res.trade_state.compare("USERPAYING") == 0)
            {
                scanRecv.CODE = "01";
                scanRecv.MESSAGE = trade_query_getmsg(res);
            }
            else if (res.trade_state.compare("SUCCESS") != 0)
            {
                scanRecv.CODE = "02";
                scanRecv.MESSAGE = trade_query_getmsg(res);
            }
            else
            {
                scanRecv.CODE = "00";
                scanRecv.MESSAGE = "";
            }
        } catch (const std::exception& e)
        {
            LOG_ERROR("返回xml报文解析失败");
            return;
        }
        UpdateScanRecv(scanRecv);
    }
    else if (req.service.compare("unified.trade.refundquery") == 0)
    {
        trade_refundquery_response res;
        try
        {
            scanRecv.SERVICE = SERVICE_REF;
            xpack::xml::decode(szXML, res);
            scanRecv.MERCHANT_ID = res.mch_id;
            scanRecv.OUT_TRADE_NO = res.out_trade_no;
            if (res.out_trade_no.length() == 0)
            {
                scanRecv.OUT_TRADE_NO = req.out_trade_no;
            }
            scanRecv.OUT_REFUND_NO = res.out_refund_no_0;
            if (res.out_refund_no_0.length() == 0)
            {
                scanRecv.OUT_REFUND_NO = req.out_refund_no;
            }
            scanRecv.TRADE_TYPE = res.trade_type;
            scanRecv.TRANSACTION_ID = res.transaction_id;
            scanRecv.REFUND_ID = res.refund_id_0;
            scanRecv.REFUND_FEE = res.refund_fee_0;
            scanRecv.TIME_END = res.refund_time_0;

            if (res.status.compare("0") != 0)
            {
                scanRecv.CODE = "02";
                scanRecv.MESSAGE = trade_refundquery_getmsg(res);
            }
            else if (res.code.length() > 0 && res.code.compare("0") != 0)
            {
                scanRecv.CODE = "02";
                scanRecv.MESSAGE = trade_refundquery_getmsg(res);
            }
            else if (res.result_code.compare("0") != 0)
            {
                scanRecv.CODE = "02";
                scanRecv.MESSAGE = trade_refundquery_getmsg(res);
            }
            else if (res.refund_status_0.compare("SUCCESS") != 0 && res.refund_status_0.compare("PROCESSING") != 0)
            {
                scanRecv.CODE = "02";
                scanRecv.MESSAGE = trade_refundquery_getmsg(res);
            }
            else
            {
                scanRecv.CODE = "00";
            }
        } catch (const std::exception& e)
        {
            LOG_ERROR("返回xml报文解析失败");
            return;
        }
        UpdateScanRecv(scanRecv);
    }
    else
    {
    }
}

void action(char* recvbuf, int recvLen, char* sendbuf, int* sendLen)
{
    SCAN_SEND scanSend;
    ClearScanSend(scanSend);
    trade_content_request contentReq;
    int iRet;
    CURLcode res;
    CURL* curlHandle;
    char* pRes = NULL;
    struct curl_slist* pHeaders = NULL;
    LOG_INFOSB(recvbuf, recvLen, "接收到请求报文[%d]", recvLen);

    try
    {
        xpack::xml::decode(recvbuf + 2, contentReq);
        if (contentReq.service.compare("unified.trade.micropay") == 0)
        {
            scanSend.SERVICE = SERVICE_PAY;
            scanSend.MERCHANT_ID = contentReq.mch_id;
            scanSend.OUT_TRADE_NO = contentReq.out_trade_no;
        }
        else if (contentReq.service.compare("unified.trade.refund") == 0)
        {
            scanSend.SERVICE = SERVICE_REF;
            scanSend.MERCHANT_ID = contentReq.mch_id;
            scanSend.OUT_REFUND_NO = contentReq.out_refund_no;
        }
        InsertScanSend(scanSend);
    } catch (exception& e)
    {
        LOG_ERROR("请求xml报文解析失败");
        goto cleanUP;
    }

    curlHandle = curl_easy_init();
    if (curlHandle)
    {
        curl_easy_setopt(curlHandle, CURLOPT_HEADER, 0L);
        curl_easy_setopt(curlHandle, CURLOPT_URL, globalCFG.url);
        curl_easy_setopt(curlHandle, CURLOPT_HTTPPOST, 1);
        curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYHOST, 0);
        curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, recvbuf + 2);
        curl_easy_setopt(curlHandle, CURLOPT_FORBID_REUSE, 1);
        curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT, globalCFG.iRecvTimeout);
        curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curlHandle, CURLOPT_CONNECTTIMEOUT, globalCFG.iConnTimeout);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, http_response);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &pRes);
        
        pHeaders = curl_slist_append(pHeaders, "Content-Type:application/xml");
        curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, pHeaders);
        res = curl_easy_perform(curlHandle);
        if (res == CURLE_OK && pRes != NULL)
        {
            LOG_INFO("LIBCURL通讯成功[%d][%s]", strlen(pRes + 4), pRes + 4);
            sendbuf[0] = strlen(pRes + 4) / 256;
            sendbuf[1] = strlen(pRes + 4) % 256;
            memcpy(sendbuf + 2, pRes + 4, strlen(pRes + 4));
            *sendLen = strlen(pRes + 4) + 2;

            analyzeXML(contentReq, pRes + 4);
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
    
cleanUP:
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
    return;
}