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
    string service, mch_id, out_trade_no, out_refund_no;
    XPACK(O(service, mch_id, out_trade_no, out_refund_no));
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

#ifdef __cplusplus
extern "C" {
#endif
    void action(char *recvbuf, int recvLen, char *sendbuf, int *sendLen);
#ifdef __cplusplus
}
#endif
