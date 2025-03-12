// YLScan.h : YLScan DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
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
#include <iomanip>
#include <algorithm>
using namespace std;

#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")

// #define DEBUG_MD5		
#define COM_HEAD			8
#define SOCKETDATELEN		1024 * 10
#define COMDATELEN			1024 * 2
#define ACK					0x06 	//ȷ��
#define NAK					0x15 	//�ܾ�
#define FS 					0x1C 	//��ָ���
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
// �йش���ʵ�ֵ���Ϣ������� YLScan.cpp
//

class CYLScanApp : public CWinApp
{
public:
	CYLScanApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

typedef struct 
{
	string service, mch_id, out_trade_no, device_info, body, goods_detail, sub_appid, attach, pnrInsIdCd, need_receipt, total_fee, mch_create_ip, auth_code, time_start, time_expire, op_user_id, op_shop_id, op_device_id, goods_tag, terminal_info, nonce_str, sign, sign_agentno, groupno, sign_type, sn_id;

	XPACK(X(F(OE), service, mch_id, out_trade_no, device_info, body, goods_detail, sub_appid, attach, pnrInsIdCd, need_receipt, total_fee, mch_create_ip, auth_code, time_start, time_expire, op_user_id, op_shop_id, op_device_id, goods_tag, terminal_info, nonce_str, sign, sign_agentno, groupno, sign_type, sn_id));
}trade_micropay_request;

struct goods_detail_item {
	std::string goods_id;     // 商品编号
	std::string goods_name;   // 商品名称
	int quantity;             // 商品数量
	float price;        // 商品价格

	// 使用 XPACK 宏定义序列化字段
	XPACK(O(goods_id, goods_name, quantity, price));
};

typedef struct {
	std::vector<goods_detail_item> goods_detail;  // 商品明细数组

	// 使用 XPACK 宏定义序列化字段
	XPACK(O(goods_detail));
} detail_item;

typedef struct {
	std::string service;      			// 服务名称
	std::string channel;      			// 支付渠道
    std::string agetId;        			// 机构号
	std::string appConfigId;
    std::string custId;        			// 商户编号
    std::string orderNo;       			// 服务商订单号
    std::string txamt;         			// 订单金额
    std::string code;          			// 扫码支付授权码
    std::string tradingIp;     			// 交易IP
    std::string type;          			// 设备类型
    std::string timeStamp;     			// 当前时间
    std::string version;       			// 版本号
	std::string sign;        			// 签名
    detail_item detail;  				// 商品明细
	XPACK(O(service, channel, agetId, appConfigId, custId, orderNo, txamt, code, tradingIp, type, timeStamp, version, sign, detail))
} gt_trade_micropay_request;

typedef struct
{
	string sign_agentno, groupno, status, message, code, need_query, result_code, mch_id, device_info, err_code, err_msg, appid, openid, user_id, sub_openid, trade_type, is_subscribe, pay_result, pay_info, transaction_id, out_transaction_id, sub_is_subscribe, sub_appid, out_trade_no, total_fee, cash_fee, mdiscount, invoice_amount, coupon_fee, promotion_detail, buyer_logon_id, buyer_pay_amount, buyer_user_id, point_amount, receipt_amount, discount_goods_detail, fee_type, attach, bank_type, uuid, bank_billno, time_end;
	XPACK(X(F(0), sign_agentno, groupno, status, message, code, need_query, result_code, mch_id, device_info, err_code, err_msg, appid, openid, user_id, sub_openid, trade_type, is_subscribe, pay_result, pay_info, transaction_id, out_transaction_id, sub_is_subscribe, sub_appid, out_trade_no, total_fee, cash_fee, mdiscount, invoice_amount, coupon_fee, promotion_detail, buyer_logon_id, buyer_pay_amount, buyer_user_id, point_amount, receipt_amount, discount_goods_detail, fee_type, attach, bank_type, uuid, bank_billno, time_end));
}trade_micropay_response;

typedef struct {
	std::string code;              // 支付结果码
	std::string msg;               // 消息
	struct {
		std::string agetId;        // 代理商ID
		std::string threeOrderNo;  // 三方订单号
		std::string netrAmt;               // 净金额（单位：分）
		std::string txamt;                 // 交易金额（单位：分）
		std::string custFee;               // 客户手续费（单位：分）
		std::string custAmt;               // 客户优惠金额（单位：分）
		std::string orderNo;       // 订单号
		std::string torderNo;      // 目标通道流水号（微信/支付宝等）
		std::string openId;        // 用户OpenID
		std::string tradingIp;     // 交易IP
		std::string orderTime;     // 订单时间（yyyyMMddHHmmss）
		std::string cardType;      // 卡类型（01：借记卡、02：贷记卡、03：其他）
		std::string discountFlag;  // 补贴标志（0：无，1：机构补贴，2：公司补贴，11：落地机构补贴）
		std::string serverThreeOrderNo; // 国通设备订单号
		std::string bankCode;      // 银行代码
		std::string bankName;      // 银行名称
		XPACK(O(agetId, threeOrderNo, netrAmt, txamt, custFee, custAmt, orderNo, torderNo, openId, tradingIp, orderTime, cardType, discountFlag, serverThreeOrderNo, bankCode, bankName))
	} data;
	XPACK(O(code, msg, data))
} gt_trade_micropay_response;

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

typedef struct {
	std::string service;
	std::string channel;
	std::string orderNo;
	std::string agetId;
	std::string custId;
	std::string appConfigId;
	std::string orderTime;
	std::string timeStamp;
	std::string version;
	std::string sign;
	XPACK(O(service, channel, orderNo, agetId, custId, appConfigId, orderTime, timeStamp, version, sign))
} gt_trade_query_request;

typedef struct
{
	string code, msg;
	struct {
		std::string threeOrderNo;  // 三方订单号
		std::string txamt;                 // 交易金额（单位：分）
		std::string torderNo;      // 目标通道流水号（微信/支付宝等）
		std::string orderTime;     // 订单时间（yyyyMMddHHmmss）
		std::string payChannel;
		std::string payNo;
		XPACK(X(F(OE), threeOrderNo, txamt, torderNo, orderTime, payChannel, payNo));
	} data;
	XPACK(X(F(0), code, msg, data));
} gt_trade_query_response;

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
	std::string service;
	std::string channel;
	std::string orderNo;
	std::string agetId;
	std::string custId;
	std::string appConfigId;
	std::string oldTOrderNo;
	std::string refundAmount;
	std::string tag;
	std::string timeStamp;
	std::string version;
	std::string sign;
	XPACK(O(service, channel, orderNo, agetId, custId, appConfigId, oldTOrderNo, refundAmount, tag, timeStamp, version, sign))
}gt_trade_refund_request;

typedef struct
{
	string code, msg;
	struct {
		std::string orderFlowNo;
		std::string threeOrderNo;
		std::string refundAmt;
		std::string orderStatus;
		std::string orderTime;
		XPACK(X(F(OE), orderFlowNo, threeOrderNo, refundAmt, orderStatus, orderTime));
	} data;
	XPACK(X(F(0), code, msg, data));
}gt_trade_refund_response;

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
// �����رն���
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

typedef struct
{
	std::string service;
	std::string channel;
	std::string threeOrderNO;
	std::string agetId;
	std::string custId;
	std::string appConfigId;
	std::string timeStamp;
	std::string version;
	std::string sign;
	XPACK(O(service, channel, threeOrderNO, agetId, custId, appConfigId, timeStamp, version, sign))
}gt_trade_close_request;

typedef struct
{
	string code, msg;
	struct {
		std::string orderFlowNo;
		std::string threeOrderNo;
		std::string refundAmt;
		std::string orderStatus;
		std::string orderTime;
		XPACK(X(F(OE), orderFlowNo, threeOrderNo, refundAmt, orderStatus, orderTime));
	} data;
	XPACK(X(F(0), code, msg, data));
}gt_trade_close_response;

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