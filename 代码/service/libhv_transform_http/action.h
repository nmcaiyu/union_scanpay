#pragma once
#include "config.h"
#include "xpack/xpack.h"
#include "xpack/json.h"
#include "xpack/xml.h"
#include "string"
#include "curl/curl.h"
#include "mysql.h"
#include "vector"

using namespace std;

typedef struct
{
    string service, mch_id, out_trade_no, out_refund_no, channel, agetId, orderNo;
    XPACK(O(service, mch_id, out_trade_no, out_refund_no, channel, agetId, orderNo));
}trade_content_request;

typedef struct 
{
    string service, mch_id, out_trade_no, device_info, body, goods_detail, sub_appid, attach, pnrInsIdCd, need_receipt, total_fee, mch_create_ip, auth_code, time_start, time_expire, op_user_id, op_shop_id, op_device_id, goods_tag, terminal_info, nonce_str, sign, sign_agentno, groupno, sign_type, sn_id;

    XPACK(X(F(OE), service, mch_id, out_trade_no, device_info, body, goods_detail, sub_appid, attach, pnrInsIdCd, need_receipt, total_fee, mch_create_ip, auth_code, time_start, time_expire, op_user_id, op_shop_id, op_device_id, goods_tag, terminal_info, nonce_str, sign, sign_agentno, groupno, sign_type, sn_id));
}trade_micropay_request;


typedef struct
{
    string sign_agentno, groupno, status, message, code, need_query, result_code, mch_id, device_info, err_code, err_msg, appid, openid, user_id, sub_openid, trade_type, is_subscribe, pay_result, pay_info, transaction_id, out_transaction_id, sub_is_subscribe, sub_appid, out_trade_no, total_fee, cash_fee, mdiscount, invoice_amount, coupon_fee, promotion_detail, buyer_logon_id, buyer_pay_amount, buyer_user_id, point_amount, receipt_amount, discount_goods_detail, fee_type, attach, bank_type, uuid, bank_billno, time_end;
    XPACK(X(F(OE), sign_agentno, groupno, status, message, code, need_query, result_code, mch_id, device_info, err_code, err_msg, appid, openid, user_id, sub_openid, trade_type, is_subscribe, pay_result, pay_info, transaction_id, out_transaction_id, sub_is_subscribe, sub_appid, out_trade_no, total_fee, cash_fee, mdiscount, invoice_amount, coupon_fee, promotion_detail, buyer_logon_id, buyer_pay_amount, buyer_user_id, point_amount, receipt_amount, discount_goods_detail, fee_type, attach, bank_type, uuid, bank_billno, time_end));
}trade_micropay_response;

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
	XPACK(O(agetId, appConfigId, custId, orderNo, txamt, code, tradingIp, type, timeStamp, version, sign, detail))
} gt_trade_micropay_request;

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
        XPACK(X(F(OE), agetId, threeOrderNo, netrAmt, txamt, custFee, custAmt, orderNo, torderNo, openId, tradingIp, orderTime, cardType, discountFlag, serverThreeOrderNo, bankCode, bankName));
    } data;
    XPACK(X(F(OE), code, msg, data));
} gt_trade_micropay_response;

typedef struct
{
    string service, mch_id, out_trade_no, transaction_id, sign_agentno, groupno, nonce_str, sign, sign_type, sn_id;
    XPACK(X(F(OE), service, mch_id, out_trade_no, transaction_id, sign_agentno, groupno, nonce_str, sign, sign_type, sn_id));
}trade_query_request;

typedef struct
{
    string sign_agentno, groupno, status, message, code, result_code, mch_id, device_info, err_code, err_msg, trade_state, trade_state_desc, trade_type, appid, user_id, sub_appid, openid, sub_openid, is_subscribe, sub_is_subscribe, transaction_id, out_transaction_id, out_trade_no, total_fee, cash_fee, promotion_detail, unionpay_discount, invoice_amount, buyer_logon_id, buyer_pay_amount, buyer_user_id, point_amount, receipt_amount, fund_bill_list, discount_goods_detail, coupon_fee, mdiscount, fee_type, attach, bank_type, bank_billno, time_end, settle_key;
    XPACK(X(F(OE), sign_agentno, groupno, status, message, code, result_code, mch_id, device_info, err_code, err_msg, trade_state, trade_state_desc, trade_type, appid, user_id, sub_appid, openid, sub_openid, is_subscribe, sub_is_subscribe, transaction_id, out_transaction_id, out_trade_no, total_fee, cash_fee, promotion_detail, unionpay_discount, invoice_amount, buyer_logon_id, buyer_pay_amount, buyer_user_id, point_amount, receipt_amount, fund_bill_list, discount_goods_detail, coupon_fee, mdiscount, fee_type, attach, bank_type, bank_billno, time_end, settle_key));
}trade_query_response;

typedef struct {
	std::string orderNo;
	std::string agetId;
	std::string custId;
	std::string appConfigId;
	std::string orderTime;
	std::string timeStamp;
	std::string version;
	std::string sign;
	XPACK(O(orderNo, agetId, custId, appConfigId, orderTime, timeStamp, version, sign))
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
    string service, mch_id, out_trade_no, transaction_id, out_refund_no, total_fee, goods_detail, refund_fee, op_user_id, refund_channel, nonce_str, sign, sign_agentno, groupno, sign_type, sn_id;
    XPACK(X(F(OE), service, mch_id, out_trade_no, transaction_id, out_refund_no, total_fee, goods_detail, refund_fee, op_user_id, refund_channel, nonce_str, sign, sign_agentno, groupno, sign_type, sn_id));
}trade_refund_request;

typedef struct
{
    string sign_agentno, groupno, status, message, code, result_code, mch_id, device_info, err_code, err_msg, transaction_id, out_trade_no, out_refund_no, refund_id, refund_channel, refund_fee, coupon_refund_fee, trade_type;
    XPACK(X(F(OE), sign_agentno, groupno, status, message, code, result_code, mch_id, device_info, err_code, err_msg, transaction_id, out_trade_no, out_refund_no, refund_id, refund_channel, refund_fee, coupon_refund_fee, trade_type));
}trade_refund_response;

typedef struct
{
    string sign_agentno, groupno, status, message, code, result_code, mch_id, device_info, err_code, transaction_id, out_trade_no, refund_count,
        out_refund_no_0, refund_id_0, refund_channel, refund_fee_0, coupon_refund_fee, mdiscount, refund_time_0, trade_type, refund_status_0,
        settle_key;
    XPACK(O(sign_agentno, groupno, status, message, code, result_code, mch_id, device_info, err_code, transaction_id, out_trade_no, refund_count,
            out_refund_no_0, refund_id_0, refund_channel, refund_fee_0, coupon_refund_fee, mdiscount, refund_time_0, trade_type, refund_status_0,
            settle_key));
} trade_refundquery_response;

typedef struct
{
    std::string orderNo;
    std::string oldTOrderNo;
	std::string agetId;
	std::string custId;
	std::string appConfigId;
	std::string refundAmount;
	std::string tag;
	std::string timeStamp;
	std::string version;
	std::string sign;
	XPACK(O(orderNo, oldTOrderNo, agetId, custId, appConfigId, refundAmount, tag, timeStamp, version, sign))
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
	std::string threeOrderNO;
	std::string agetId;
	std::string custId;
	std::string appConfigId;
	std::string timeStamp;
	std::string version;
	std::string sign;
	XPACK(O(threeOrderNO, agetId, custId, appConfigId, timeStamp, version, sign))
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

#ifdef __cplusplus
extern "C" {
#endif
    void action(char *recvbuf, int recvLen, char *sendbuf, int *sendLen);
#ifdef __cplusplus
}
#endif
