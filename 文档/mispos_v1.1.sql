/*
Navicat MySQL Data Transfer

Source Server         : localhost
Source Server Version : 110200
Source Host           : localhost:3306
Source Database       : eladmin

Target Server Type    : MYSQL
Target Server Version : 110200
File Encoding         : 65001

Date: 2023-10-10 13:54:18
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for t_pos_recv
-- ----------------------------
DROP TABLE IF EXISTS `t_pos_recv`;
CREATE TABLE `t_pos_recv` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `MERCHANT_NO` char(15) DEFAULT '' COMMENT '商户号',
  `TERMINAL_NO` char(8) DEFAULT '' COMMENT '终端号',
  `TRANS` int(11) DEFAULT NULL COMMENT '交易类型',
  `TRACE` char(6) DEFAULT '' COMMENT '流水号',
  `BATCH` char(6) DEFAULT '' COMMENT '批次号',
  `CARD_NO` varchar(30) DEFAULT '' COMMENT '卡号',
  `AMT` double DEFAULT NULL COMMENT '金额',
  `REF_NO` char(12) DEFAULT '' COMMENT '参考号',
  `TRANS_DATE` char(8) DEFAULT '' COMMENT '银联交易日期',
  `TRANS_TIME` char(6) DEFAULT '' COMMENT '银联交易时间',
  `RESP_CODE` char(2) DEFAULT '' COMMENT '返回码',
  `RESP_MSG` varchar(100) DEFAULT '' COMMENT '返回中文信息',
  `CREATE_TIME` timestamp NULL DEFAULT current_timestamp() COMMENT '创建时间',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- ----------------------------
-- Table structure for t_pos_send
-- ----------------------------
DROP TABLE IF EXISTS `t_pos_send`;
CREATE TABLE `t_pos_send` (
  `ID` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `MERCHANT_NO` char(15) DEFAULT '' COMMENT '商户号',
  `TERMINAL_NO` char(8) DEFAULT '' COMMENT '终端号',
  `TRANS` int(11) DEFAULT NULL COMMENT '交易类型',
  `TRACE` char(6) DEFAULT '' COMMENT '流水号',
  `CARD_NO` varchar(30) DEFAULT '' COMMENT '卡号',
  `BATCH` char(6) DEFAULT '' COMMENT '批次号',
  `AMT` double DEFAULT NULL COMMENT '金额',
  `CREATE_TIME` timestamp NULL DEFAULT current_timestamp() COMMENT '创建时间',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- ----------------------------
-- Table structure for t_scan_recv
-- ----------------------------
DROP TABLE IF EXISTS `t_scan_recv`;
CREATE TABLE `t_scan_recv` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `SERVICE` int(20) DEFAULT NULL COMMENT '交易类型',
  `MERCHANT_ID` varchar(20) DEFAULT '' COMMENT '商户号',
  `OUT_TRADE_NO` varchar(50) DEFAULT '' COMMENT '商户交易订单号',
  `MESSAGE` varchar(100) DEFAULT '' COMMENT '返回信息',
  `CODE` varchar(10) DEFAULT '' COMMENT '返回码',
  `APPID` varchar(50) DEFAULT NULL,
  `OPENID` varchar(50) DEFAULT NULL,
  `USER_ID` varchar(50) DEFAULT '' COMMENT '用户ID',
  `SUB_OPENID` varchar(50) DEFAULT NULL,
  `TRADE_TYPE` varchar(30) DEFAULT '' COMMENT '交易渠道类型',
  `TRANSACTION_ID` varchar(50) DEFAULT NULL,
  `OUT_TRANSACTION_ID` varchar(50) DEFAULT '' COMMENT '平台订单号',
  `SUB_APPID` varchar(50) DEFAULT NULL,
  `TOTAL_FEE` double DEFAULT NULL COMMENT '总金额',
  `DISCOUNT_FEE` double DEFAULT NULL COMMENT '优惠金额',
  `REAL_FEE` double DEFAULT NULL COMMENT '实际付款金额',
  `BUYER_LOGON_ID` varchar(50) DEFAULT NULL,
  `BUYER_USER_ID` varchar(50) DEFAULT NULL,
  `BANK_BILLNO` varchar(50) DEFAULT '' COMMENT '银行流水号',
  `TIME_END` varchar(20) DEFAULT '' COMMENT '交易时间',
  `OUT_REFUND_NO` varchar(50) DEFAULT '' COMMENT '商户退款订单号',
  `REFUND_ID` varchar(50) DEFAULT '' COMMENT '退款平台订单号',
  `REFUND_FEE` double DEFAULT NULL COMMENT '退款金额',
  `CREATE_TIME` timestamp NULL DEFAULT current_timestamp(),
  `MERCHANT_ORDER_NO` varchar(50) DEFAULT '' COMMENT '当前商户交易订单号',
  PRIMARY KEY (`ID`),
  UNIQUE KEY `UNI` (`MERCHANT_ID`,`MERCHANT_ORDER_NO`)
) ENGINE=InnoDB AUTO_INCREMENT=26 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- ----------------------------
-- Table structure for t_scan_send
-- ----------------------------
DROP TABLE IF EXISTS `t_scan_send`;
CREATE TABLE `t_scan_send` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `MERCHANT_ID` varchar(20) DEFAULT '' COMMENT '商户号',
  `OUT_TRADE_NO` varchar(50) DEFAULT '' COMMENT '商户交易订单号',
  `OUT_REFUND_NO` varchar(50) DEFAULT '' COMMENT '商户退款订单号',
  `CREATE_TIME` timestamp NULL DEFAULT current_timestamp(),
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=18 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
