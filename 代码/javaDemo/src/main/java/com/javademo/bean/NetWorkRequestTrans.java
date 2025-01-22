package com.javademo.bean;

import lombok.Data;

@Data
public class NetWorkRequestTrans {
    private String attach;
    private String auth_code;
    private String body;
    private String device_info;
    private String mch_create_ip;
    private String mch_id;
    private String nonce_str;
    private String notify_url;
    private String op_device_id;
    private String op_user_id;
    private String out_refund_no;
    private String out_trade_no;
    private long refund_fee;
    private String service;
    private String sign;
    private String sign_type;
    private String sn_id;
    private long total_fee;
    private String transaction_id;
}
