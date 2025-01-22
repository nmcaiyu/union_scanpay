// YLScan.h : YLScan DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "xpack/xpack.h"
#include "xpack/json.h"
#include "xpack/xml.h"
#include "openssl/sha.h"
#include "openssl/err.h"
#include "openssl/pem.h"
#include "openssl/md5.h"
#include "openssl/ssl.h"
#include "string"
#include "cSocket.h"
#include "Serial.h"	
#include "vector"
using namespace std;

#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")

// #define DEBUG_MD5		
#define COM_HEAD			8
#define SOCKETDATELEN		1024 * 10
#define COMDATELEN			1024 * 2
#define ACK					0x06 	//确认
#define NAK					0x15 	//拒绝
#define FS 					0x1C 	//域分隔符
#define COM_MERNO			"111111111111111"
#define COM_TERNO			"22222222"

#define RSA_SIGNTYPE		"RSA2"


#define SCAN_PAY			"unified.trade.micropay"
#define SCAN_PAYQUERY		"unified.trade.query"
#define SCAN_REFUND			"unified.trade.refund"
#define SCAN_REFUNDQUERY	"unified.trade.refundquery"
#define SCAN_CLOSE			"unified.trade.close"
#define SCAN_REVERSE		"unified.micropay.reverse"


// CYLScanApp
// 有关此类实现的信息，请参阅 YLScan.cpp
//

class CYLScanApp : public CWinApp
{
public:
	CYLScanApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

typedef struct 
{
	string service, mch_id, out_trade_no, device_info, body, goods_detail, sub_appid, attach, pnrInsIdCd, need_receipt, total_fee, mch_create_ip, auth_code, time_start, time_expire, op_user_id, op_shop_id, op_device_id, goods_tag, terminal_info, nonce_str, sign, sign_agentno, groupno, sign_type, sn_id;

	XPACK(X(F(OE), service, mch_id, out_trade_no, device_info, body, goods_detail, sub_appid, attach, pnrInsIdCd, need_receipt, total_fee, mch_create_ip, auth_code, time_start, time_expire, op_user_id, op_shop_id, op_device_id, goods_tag, terminal_info, nonce_str, sign, sign_agentno, groupno, sign_type, sn_id));
}trade_micropay_request;


typedef struct
{
	string sign_agentno, groupno, status, message, code, need_query, result_code, mch_id, device_info, err_code, err_msg, appid, openid, user_id, sub_openid, trade_type, is_subscribe, pay_result, pay_info, transaction_id, out_transaction_id, sub_is_subscribe, sub_appid, out_trade_no, total_fee, cash_fee, mdiscount, invoice_amount, coupon_fee, promotion_detail, buyer_logon_id, buyer_pay_amount, buyer_user_id, point_amount, receipt_amount, discount_goods_detail, fee_type, attach, bank_type, uuid, bank_billno, time_end;
	XPACK(X(F(0), sign_agentno, groupno, status, message, code, need_query, result_code, mch_id, device_info, err_code, err_msg, appid, openid, user_id, sub_openid, trade_type, is_subscribe, pay_result, pay_info, transaction_id, out_transaction_id, sub_is_subscribe, sub_appid, out_trade_no, total_fee, cash_fee, mdiscount, invoice_amount, coupon_fee, promotion_detail, buyer_logon_id, buyer_pay_amount, buyer_user_id, point_amount, receipt_amount, discount_goods_detail, fee_type, attach, bank_type, uuid, bank_billno, time_end));
}trade_micropay_response;

typedef struct
{
	string service, mch_id, out_trade_no, transaction_id, sign_agentno, groupno, nonce_str, sign, sign_type, sn_id, device_info;
	XPACK(X(F(OE), service, mch_id, out_trade_no, transaction_id, sign_agentno, groupno, nonce_str, sign, sign_type, sn_id, device_info));
}trade_query_request;

typedef struct
{
	string sign_agentno, groupno, status, message, code, result_code, mch_id, device_info, err_code, err_msg, trade_state, trade_state_desc, trade_type, appid, user_id, sub_appid, openid, sub_openid, is_subscribe, sub_is_subscribe, transaction_id, out_transaction_id, out_trade_no, total_fee, cash_fee, promotion_detail, unionpay_discount, invoice_amount, buyer_logon_id, buyer_pay_amount, buyer_user_id, point_amount, receipt_amount, fund_bill_list, discount_goods_detail, coupon_fee, mdiscount, fee_type, attach, bank_type, bank_billno, time_end, settle_key;
	XPACK(X(F(0), sign_agentno, groupno, status, message, code, result_code, mch_id, device_info, err_code, err_msg, trade_state, trade_state_desc, trade_type, appid, user_id, sub_appid, openid, sub_openid, is_subscribe, sub_is_subscribe, transaction_id, out_transaction_id, out_trade_no, total_fee, cash_fee, promotion_detail, unionpay_discount, invoice_amount, buyer_logon_id, buyer_pay_amount, buyer_user_id, point_amount, receipt_amount, fund_bill_list, discount_goods_detail, coupon_fee, mdiscount, fee_type, attach, bank_type, bank_billno, time_end, settle_key));
}trade_query_response;


typedef struct
{
// 	XPACK(O());
}trade_reverse_request;

typedef struct
{
// 	XPACK(O());
}trade_reverse_response;


typedef struct
{
	string service, mch_id, out_trade_no, transaction_id, out_refund_no, total_fee, goods_detail, refund_fee, op_user_id, refund_channel, nonce_str, sign, sign_agentno, groupno, sign_type, sn_id, device_info;
	XPACK(X(F(OE), service, mch_id, out_trade_no, transaction_id, out_refund_no, total_fee, goods_detail, refund_fee, op_user_id, refund_channel, nonce_str, sign, sign_agentno, groupno, sign_type, sn_id, device_info));
}trade_refund_request;

typedef struct
{
	string sign_agentno, groupno, status, message, code, result_code, mch_id, device_info, err_code, err_msg, transaction_id, out_trade_no, out_refund_no, refund_id, refund_channel, refund_fee, coupon_refund_fee, trade_type;
	XPACK(X(F(0), sign_agentno, groupno, status, message, code, result_code, mch_id, device_info, err_code, err_msg, transaction_id, out_trade_no, out_refund_no, refund_id, refund_channel, refund_fee, coupon_refund_fee, trade_type));
}trade_refund_response;


typedef struct
{
	string service, mch_id, out_trade_no, transaction_id, out_refund_no, refund_id, nonce_str, sign, sign_agentno, groupno, sign_type, sn_id, device_info;
	XPACK(X(F(OE), service, mch_id, out_trade_no, transaction_id, out_refund_no, refund_id, nonce_str, sign, sign_agentno, groupno, sign_type, sn_id, device_info));
}trade_refundquery_request;

typedef struct
{
	string sign_agentno, groupno, status, message, code, result_code, mch_id, device_info, err_code, transaction_id, out_trade_no, refund_count, out_refund_no_0, refund_id_0, refund_channel, refund_fee_0, coupon_refund_fee, mdiscount, refund_time_0, trade_type, refund_status_0, settle_key;
	XPACK(X(F(0), sign_agentno, groupno, status, message, code, result_code, mch_id, device_info, err_code, transaction_id, out_trade_no, refund_count, out_refund_no_0, refund_id_0, refund_channel, refund_fee_0, coupon_refund_fee, mdiscount, refund_time_0, trade_type, refund_status_0, settle_key));
}trade_refundquery_response;

// start	[2023/5/12]
// 新增关闭订单
typedef struct
{
	string service, mch_id, out_trade_no, nonce_str, sign, sign_agentno, groupno, sign_type, sn_id, device_info;
	XPACK(X(F(OE), service, mch_id, out_trade_no, nonce_str, sign, sign_agentno, groupno, sign_type, sn_id, device_info));
}trade_close_request;

typedef struct
{
	string sign_agentno, groupno, status, message, code, result_code, mch_id, device_info, err_code, err_msg;
	XPACK(X(F(0), sign_agentno, groupno, status, message, code, result_code, mch_id, device_info, err_code, err_msg));
}trade_close_response;
// end		[2023/5/12]

typedef struct  
{
	string transName, authCode, amt, outTradeNo, body, transactionId, outRefundNo, refundAmt, refundId, outTransactionId;
	XPACK(X(F(OE), transName, authCode, amt, outTradeNo, body, transactionId, outRefundNo, refundAmt, refundId, outTransactionId));
}SCANTRANS_REQUEST;

typedef struct  
{
	string respCode, respMsg, needQuery, outTradeNo, transactionId, refundId, outRefundNo, tradeType, bankBillNo, dateTime, amt, refundAmt, outTransactionId;
	XPACK(X(F(OE), respCode, respMsg, needQuery, outTradeNo, transactionId, refundId, outRefundNo, tradeType, bankBillNo, dateTime, amt, refundAmt, outTransactionId));
}SCANTRANS_RESPONSE;

typedef struct  
{
	int timeout;
	string transId, amt, oldTrace, oldRefNo, cardNo, expr, oldDate, oldAuthNo;
	XPACK(O(timeout, transId, amt, oldTrace, oldRefNo, cardNo, expr, oldDate, oldAuthNo));
}POSTRANS_REQUEST;

typedef struct  
{
	string respCode, respMsg, amt, cardNo, expr, trace, batch, refNo, authNo, date, time, merchantNo, merchantName, terminalNo, sn, qrCode;
	XPACK(O(respCode, respMsg, amt, cardNo, expr, trace, batch, refNo, authNo, date, time, merchantNo, merchantName, terminalNo, sn, qrCode));
}POSTRANS_RESPONSE;


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
	TRANSID_GETQRCODE
};