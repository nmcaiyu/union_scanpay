[TOC]
# 扫码后台接口
## 调用方式
    采用HTTP POST调用方式
    字符串编码格式为UTF-8
    请求报文与返回报文采用JSON格式
## 请求JSON内容

|   JSON节点    |  类型  | 长度  |                 说明                 |
| :-----------: | :----: | :---: | :----------------------------------: |
|    transId    | String |   2   |               交易类型               |
|      amt      | String |  12   |       交易金额, 12位以分为单位       |
|   authCode    | String |  128  |         微信, 支付宝的授权码         |
|  outTradeNo   | String |  48   |            商户消费订单号            |
|     title     | String |  64   | 订单标题<br>支付宝必输，微信无需上送 |
| transactionId | String |  32   |              平台订单号              |
|   tradeDate   | String |   8   |               交易日期               |
|  outRefundNo  | String |  48   |            商户退货订单号            |
|  outCancelNo  | String |  48   |            商户撤销订单号            |

## 返回内容

|     JSON节点     |  类型  | 长度  |                                                     说明                                                     |
| :--------------: | :----: | :---: | :----------------------------------------------------------------------------------------------------------: |
|     respCode     | String |   2   |                                        返回码 <br>00-成功<br>其他失败                                        |
|     respMsg      | String |  256  |                                                 返回中文信息                                                 |
|    needQuery     | String |   1   |                                      是否需要执行查询<br> "Y"为需要查询                                      |
|       amt        | String |  12   |                                           交易金额, 12位以分为单位                                           |
|    tradeDate     | String |   8   |                                                   交易日期                                                   |
|    tradeTime     | String |   6   |                                                   交易时间                                                   |
|    outTradeNo    | String |  48   |                                                  商户订单号                                                  |
|  transactionId   | String |  32   |                                                  平台订单号                                                  |
|  oldOutTradeNo   | String |  48   |                                      原商户订单号, 如果没有原订单，为空                                      |
| oldTransactionId | String |  32   |                                      原平台订单号, 如果没有原订单，为空                                      |
|    transType     | String |  10   | 交易渠道类型<br>WX-微信<br>AL-支付宝<br>UPWX-银联微信<br>UPAL-银联支付宝<br>NUWX-网联微信<br>NUAL-网联支付宝 |

## 扫码交易类型说明
| 交易类型 |     说明     |                                  需要参数                                   |
| :------: | :----------: | :-------------------------------------------------------------------------: |
|    01    | 用户被扫消费 |              transId<br>amt<br>authCode<br>outTradeNo<br>title              |
|    02    |   订单查询   |            transId<br>tradeDate<br>outTradeNo 或者 transactionId            |
|    03    |   订单退货   | transId<br>amt<br>outRefundNo<br>tradeDate<br>outTradeNo 或者 transactionId |
|    04    |   订单撤销   |           transId<br>outCancelNo<br>outTradeNo 或者 transactionId           |