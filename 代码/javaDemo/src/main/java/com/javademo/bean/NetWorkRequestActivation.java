package com.javademo.bean;

import lombok.Data;

@Data
public class NetWorkRequestActivation {
    private String DEVICE_INFO;
    private String DEVICE_PUB_KEY;
    private String SN_ID;
    private String SN_QR_INIT_TOKEN;
    private String VN;
}
