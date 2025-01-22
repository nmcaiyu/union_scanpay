/*
Navicat MySQL Data Transfer

Source Server         : 10.25.26.129
Source Server Version : 50738
Source Host           : 10.25.26.129:3306
Source Database       : test

Target Server Type    : MYSQL
Target Server Version : 50738
File Encoding         : 65001

Date: 2022-07-28 10:29:36
*/
DROP SCHEMA IF EXISTS `mispos` ;
CREATE SCHEMA IF NOT EXISTS `mispos` DEFAULT CHARACTER SET utf8 ;
USE `mispos` ;
SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for t_pos_recv
-- ----------------------------
DROP TABLE IF EXISTS `t_pos_recv`;
CREATE TABLE `t_pos_recv` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `MERCHANT_NO` char(15) DEFAULT NULL,
  `TERMINAL_NO` char(8) DEFAULT NULL,
  `TRANS` int(11) DEFAULT NULL,
  `TRACE` char(6) DEFAULT NULL,
  `BATCH` char(6) DEFAULT NULL,
  `CARD_NO` varchar(30) DEFAULT NULL,
  `AMT` double DEFAULT NULL,
  `REF_NO` char(12) DEFAULT NULL,
  `TRANS_DATE` char(8) DEFAULT NULL,
  `TRANS_TIME` char(6) DEFAULT NULL,
  `RESP_CODE` char(2) DEFAULT NULL,
  `RESP_MSG` varchar(100) DEFAULT NULL,
  `CREATE_TIME` timestamp DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for t_pos_send
-- ----------------------------
DROP TABLE IF EXISTS `t_pos_send`;
CREATE TABLE `t_pos_send` (
  `ID` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `MERCHANT_NO` char(15) DEFAULT NULL,
  `TERMINAL_NO` char(8) DEFAULT NULL,
  `TRANS` int(11) DEFAULT NULL,
  `TRACE` char(6) DEFAULT NULL,
  `CARD_NO` varchar(30) DEFAULT NULL,
  `BATCH` char(6) DEFAULT NULL,
  `AMT` double DEFAULT NULL,
  `CREATE_TIME` timestamp DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for t_scan_recv
-- ----------------------------
DROP TABLE IF EXISTS `t_scan_recv`;
CREATE TABLE `t_scan_recv` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `SERVICE` varchar(20) DEFAULT NULL,
  `MERCHANT_ID` varchar(20) DEFAULT NULL,
  `OUT_TRADE_NO` varchar(50) DEFAULT NULL,
  `MESSAGE` varchar(100) DEFAULT NULL,
  `CODE` varchar(10) DEFAULT NULL,
  `APPID` varchar(50) DEFAULT NULL,
  `OPENID` varchar(50) DEFAULT NULL,
  `USER_ID` varchar(50) DEFAULT NULL,
  `SUB_OPENID` varchar(50) DEFAULT NULL,
  `TRADE_TYPE` varchar(30) DEFAULT NULL,
  `TRANSACTION_ID` varchar(50) DEFAULT NULL,
  `OUT_TRANSACTION_ID` varchar(50) DEFAULT NULL,
  `SUB_APPID` varchar(50) DEFAULT NULL,
  `TOTAL_FEE` double DEFAULT NULL,
  `DISCOUNT_FEE` double DEFAULT NULL,
  `REAL_FEE` double DEFAULT NULL,
  `BUYER_LOGON_ID` varchar(50) DEFAULT NULL,
  `BUYER_USER_ID` varchar(50) DEFAULT NULL,
  `BANK_BILLNO` varchar(50) DEFAULT NULL,
  `TIME_END` varchar(20) DEFAULT NULL,
  `OUT_REFUND_NO` varchar(50) DEFAULT NULL,
  `REFUND_ID` varchar(50) DEFAULT NULL,
  `REFUND_FEE` double DEFAULT NULL,
  `CREATE_TIME` timestamp DEFAULT CURRENT_TIMESTAMP,
  `MERCHANT_ORDER_NO` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `UNI` (`MERCHANT_ID`,`MERCHANT_ORDER_NO`)
) ENGINE=InnoDB AUTO_INCREMENT=26 DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Table structure for t_scan_send
-- ----------------------------
DROP TABLE IF EXISTS `t_scan_send`;
CREATE TABLE `t_scan_send` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `MERCHANT_ID` varchar(20) DEFAULT NULL,
  `OUT_TRADE_NO` varchar(50) DEFAULT NULL,
  `OUT_REFUND_NO` varchar(50) DEFAULT NULL,
  `CREATE_TIME` timestamp DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=18 DEFAULT CHARSET=utf8mb4;
