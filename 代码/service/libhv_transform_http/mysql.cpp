#include "mysql.h"
#include <zdb.h>
URL_T gDBURL;
ConnectionPool_T gDBPool;

void strFen2Yuan(string fen, string& yuan)
{
    char szAmt[20 + 1];
    BUFCLR(szAmt);
    double dYuan = 0;
    int iFen = atoi(fen.c_str());
    dYuan = iFen / 100.0;

    sprintf(szAmt, "%.2f", dYuan);
    yuan = szAmt;
}

int initMysql()
{
    char szMysqlUrl[1024 * 2];
    BUFCLR(szMysqlUrl);
    sprintf(szMysqlUrl, "mysql://%s:%d/%s?user=%s&password=%s&charset=utf8",
        globalCFG.szMysqlUrl, globalCFG.iMysqlPort, globalCFG.szMysqlDB, globalCFG.szMysqlUser, globalCFG.szMysqlPass);
    LOG_DEBUG(szMysqlUrl);
    gDBURL = URL_new(szMysqlUrl);
    if (gDBURL == NULL)
    {
        LOG_ERROR("MYSQLURL组织失败");
        return -1;
    }
    gDBPool = ConnectionPool_new(gDBURL);
    if (gDBPool == NULL)
    {
        LOG_ERROR("获取线程池时出错");
        return -1;
    }

    if (globalCFG.iMysqlMin < 0 || globalCFG.iMysqlMax < 0 || globalCFG.iMysqlMax < globalCFG.iMysqlMin)
    {
        LOG_ERROR("线程最小个数和最大个数设置有误");
        return -1;
    }
    
    ConnectionPool_setInitialConnections(gDBPool, globalCFG.iMysqlMin);
	ConnectionPool_setMaxConnections(gDBPool, globalCFG.iMysqlMax);
    ConnectionPool_start(gDBPool);
    return 0;
}

int InsertScanRecv(SCAN_RECV obj)
{
    string strTemp;
    ostringstream ostr;
    switch (obj.SERVICE)
    {
        case SERVICE_PAY:
            obj.MERCHANT_ORDER_NO = obj.OUT_TRADE_NO;
            break;
        case SERVICE_REF:
            obj.MERCHANT_ORDER_NO = obj.OUT_REFUND_NO;
            break;
        default:
            return 0;
            break;
    }
    Connection_T con = NULL;
    int iRet;
    char szSQL[1024 * 2];
    BUFCLR(szSQL);
    sprintf(szSQL,
            " INSERT INTO T_SCAN_RECV ("
            " `SERVICE`,"
            " `MERCHANT_ID`,"
            " `OUT_TRADE_NO`,"
            " `MESSAGE`,"
            " `CODE`,"
            " `APPID`,"
            " `OPENID`,"
            " `USER_ID`,"
            " `SUB_OPENID`,"
            " `TRADE_TYPE`,"
            " `TRANSACTION_ID`,"
            " `OUT_TRANSACTION_ID`,"
            " `SUB_APPID`,"
            " `TOTAL_FEE`,"
            " `DISCOUNT_FEE`,"
            " `REAL_FEE`,"
            " `BUYER_LOGON_ID`,"
            " `BUYER_USER_ID`,"
            " `BANK_BILLNO`,"
            " `TIME_END`,"
            " `OUT_REFUND_NO`,"
            " `REFUND_ID`,"
            " `REFUND_FEE`,"
            " `MERCHANT_ORDER_NO`"
            " )"
            " VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");

    TRY
    {
        LOG_DEBUG("执行sql语句 [%s]", szSQL);
        con = ConnectionPool_getConnection(gDBPool);
        if (con == NULL)
        {
            LOG_ERROR("获取MYSQL线程池链接失败");
            iRet = -1;
            return iRet;
        }

        PreparedStatement_T p = Connection_prepareStatement(con, szSQL);
        ostr.str("");
        PreparedStatement_setInt(p, 1, obj.SERVICE);  // SERVICE
        ostr << obj.SERVICE << ", ";
        PreparedStatement_setString(p, 2, obj.MERCHANT_ID.c_str());
        ostr << obj.MERCHANT_ID << ", ";
        PreparedStatement_setString(p, 3, obj.OUT_TRADE_NO.c_str());
        ostr << obj.OUT_TRADE_NO << ", ";
        PreparedStatement_setString(p, 4, obj.MESSAGE.c_str());
        ostr << obj.MESSAGE << ", ";
        PreparedStatement_setString(p, 5, obj.CODE.c_str());
        ostr << obj.CODE << ", ";
        PreparedStatement_setString(p, 6, obj.APPID.c_str());
        ostr << obj.APPID << ", ";
        PreparedStatement_setString(p, 7, obj.OPENID.c_str());
        ostr << obj.OPENID << ", ";
        PreparedStatement_setString(p, 8, obj.USER_ID.c_str());
        ostr << obj.USER_ID << ", ";
        PreparedStatement_setString(p, 9, obj.SUB_OPENID.c_str());
        ostr << obj.SUB_OPENID << ", ";
        PreparedStatement_setString(p, 10, obj.TRADE_TYPE.c_str());
        ostr << obj.TRADE_TYPE << ", ";
        PreparedStatement_setString(p, 11, obj.TRANSACTION_ID.c_str());
        ostr << obj.TRANSACTION_ID << ", ";
        PreparedStatement_setString(p, 12, obj.OUT_TRANSACTION_ID.c_str());
        ostr << obj.OUT_TRANSACTION_ID << ", ";
        PreparedStatement_setString(p, 13, obj.SUB_APPID.c_str());
        ostr << obj.SUB_APPID << ", ";
        strFen2Yuan(obj.TOTAL_FEE, strTemp);
        PreparedStatement_setDouble(p, 14, atof(strTemp.c_str()));
        ostr << atof(strTemp.c_str()) << ", ";
        strFen2Yuan(obj.DISCOUNT_FEE, strTemp);
        PreparedStatement_setDouble(p, 15, atof(strTemp.c_str()));
        ostr << atof(strTemp.c_str()) << ", ";
        strFen2Yuan(obj.REAL_FEE, strTemp);
        PreparedStatement_setDouble(p, 16, atof(strTemp.c_str()));
        ostr << atof(strTemp.c_str()) << ", ";
        PreparedStatement_setString(p, 17, obj.BUYER_LOGON_ID.c_str());
        ostr << obj.BUYER_LOGON_ID << ", ";
        PreparedStatement_setString(p, 18, obj.BUYER_USER_ID.c_str());
        ostr << obj.BUYER_USER_ID << ", ";
        PreparedStatement_setString(p, 19, obj.BANK_BILLNO.c_str());
        ostr << obj.BANK_BILLNO << ", ";
        PreparedStatement_setString(p, 20, obj.TIME_END.c_str());
        ostr << obj.TIME_END << ", ";
        PreparedStatement_setString(p, 21, obj.OUT_REFUND_NO.c_str());
        ostr << obj.OUT_REFUND_NO << ", ";
        PreparedStatement_setString(p, 22, obj.REFUND_ID.c_str());
        ostr << obj.REFUND_ID << ", ";
        strFen2Yuan(obj.REFUND_FEE, strTemp);
        PreparedStatement_setDouble(p, 23, atof(strTemp.c_str()));
        ostr << atof(strTemp.c_str()) << ", ";
        PreparedStatement_setString(p, 24, obj.MERCHANT_ORDER_NO.c_str());
        ostr << obj.MERCHANT_ORDER_NO << ", ";

        LOG_DEBUG("sql参数 [%s]", ostr.str().c_str());
        PreparedStatement_execute(p);

        iRet = 0;
    }
    CATCH(SQLException)
    {
        LOG_ERROR("执行SQL语句失败 %s", Exception_frame.message);
        iRet = -1;
    }
    FINALLY { Connection_close(con); }
    END_TRY;
    return iRet;
}

int InsertScanSend(SCAN_SEND obj)
{
    ostringstream ostr;
    if (obj.SERVICE != SERVICE_PAY && obj.SERVICE != SERVICE_REF)
    {
        return 0;
    }
    Connection_T con = NULL;
    int iRet;
    char szSQL[1024 * 2];
    BUFCLR(szSQL);
    sprintf(szSQL, "INSERT INTO T_SCAN_SEND (`MERCHANT_ID`, `OUT_TRADE_NO`, `OUT_REFUND_NO`) VALUES (?, ?, ?)");
    TRY
    {
        LOG_DEBUG("执行sql语句 [%s]", szSQL);
        con = ConnectionPool_getConnection(gDBPool);
        if (con == NULL)
        {
            LOG_ERROR("获取MYSQL线程池链接失败");
            iRet = -1;
            return iRet;
        }

        PreparedStatement_T p = Connection_prepareStatement(con, szSQL);
        ostr.str("");
        PreparedStatement_setString(p, 1, obj.MERCHANT_ID.c_str());
        ostr << obj.MERCHANT_ID << ", ";
        PreparedStatement_setString(p, 2, obj.OUT_TRADE_NO.c_str());
        ostr << obj.OUT_TRADE_NO << ", ";
        PreparedStatement_setString(p, 3, obj.OUT_REFUND_NO.c_str());
        ostr << obj.OUT_REFUND_NO << ", ";
        LOG_DEBUG("sql参数 [%s]", ostr.str().c_str());
        PreparedStatement_execute(p);

        iRet = 0;
    }
    CATCH(SQLException)
    {
        LOG_ERROR("执行SQL语句失败 %s", Exception_frame.message);
        iRet = -1;
    }
    FINALLY { Connection_close(con); }
    END_TRY;
    return iRet;
}

int UpdateScanRecv(SCAN_RECV obj)
{
    if (obj.CODE.compare("00") != 0)
    {
        return 0;
    }
    switch (obj.SERVICE)
    {
        case SERVICE_PAY:
            obj.MERCHANT_ORDER_NO = obj.OUT_TRADE_NO;
            break;
        case SERVICE_REF:
            obj.MERCHANT_ORDER_NO = obj.OUT_REFUND_NO;
            break;
        default:
            return 0;
            break;
    }
    string strTemp;
    ostringstream ostr;
    Connection_T con = NULL;
    int iRet;
    char szSQL[1024 * 2];
    BUFCLR(szSQL);
    sprintf(szSQL,
            " REPLACE INTO T_SCAN_RECV ("
            " `SERVICE`,"
            " `MERCHANT_ID`,"
            " `OUT_TRADE_NO`,"
            " `MESSAGE`,"
            " `CODE`,"
            " `APPID`,"
            " `OPENID`,"
            " `USER_ID`,"
            " `SUB_OPENID`,"
            " `TRADE_TYPE`,"
            " `TRANSACTION_ID`,"
            " `OUT_TRANSACTION_ID`,"
            " `SUB_APPID`,"
            " `TOTAL_FEE`,"
            " `DISCOUNT_FEE`,"
            " `REAL_FEE`,"
            " `BUYER_LOGON_ID`,"
            " `BUYER_USER_ID`,"
            " `BANK_BILLNO`,"
            " `TIME_END`,"
            " `OUT_REFUND_NO`,"
            " `REFUND_ID`,"
            " `REFUND_FEE`,"
            " `MERCHANT_ORDER_NO`"
            " )"
            " VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");

    TRY
    {
        LOG_DEBUG("执行sql语句 [%s]", szSQL);
        con = ConnectionPool_getConnection(gDBPool);
        if (con == NULL)
        {
            LOG_ERROR("获取MYSQL线程池链接失败");
            iRet = -1;
            return iRet;
        }

        PreparedStatement_T p = Connection_prepareStatement(con, szSQL);
        ostr.str("");
        PreparedStatement_setInt(p, 1, obj.SERVICE);  // SERVICE
        ostr << obj.SERVICE << ", ";
        PreparedStatement_setString(p, 2, obj.MERCHANT_ID.c_str());
        ostr << obj.MERCHANT_ID << ", ";
        PreparedStatement_setString(p, 3, obj.OUT_TRADE_NO.c_str());
        ostr << obj.OUT_TRADE_NO << ", ";
        PreparedStatement_setString(p, 4, obj.MESSAGE.c_str());
        ostr << obj.MESSAGE << ", ";
        PreparedStatement_setString(p, 5, obj.CODE.c_str());
        ostr << obj.CODE << ", ";
        PreparedStatement_setString(p, 6, obj.APPID.c_str());
        ostr << obj.APPID << ", ";
        PreparedStatement_setString(p, 7, obj.OPENID.c_str());
        ostr << obj.OPENID << ", ";
        PreparedStatement_setString(p, 8, obj.USER_ID.c_str());
        ostr << obj.USER_ID << ", ";
        PreparedStatement_setString(p, 9, obj.SUB_OPENID.c_str());
        ostr << obj.SUB_OPENID << ", ";
        PreparedStatement_setString(p, 10, obj.TRADE_TYPE.c_str());
        ostr << obj.TRADE_TYPE << ", ";
        PreparedStatement_setString(p, 11, obj.TRANSACTION_ID.c_str());
        ostr << obj.TRANSACTION_ID << ", ";
        PreparedStatement_setString(p, 12, obj.OUT_TRANSACTION_ID.c_str());
        ostr << obj.OUT_TRANSACTION_ID << ", ";
        PreparedStatement_setString(p, 13, obj.SUB_APPID.c_str());
        ostr << obj.SUB_APPID << ", ";
        strFen2Yuan(obj.TOTAL_FEE, strTemp);
        PreparedStatement_setDouble(p, 14, atof(strTemp.c_str()));
        ostr << atof(strTemp.c_str()) << ", ";
        strFen2Yuan(obj.DISCOUNT_FEE, strTemp);
        PreparedStatement_setDouble(p, 15, atof(strTemp.c_str()));
        ostr << atof(strTemp.c_str()) << ", ";
        strFen2Yuan(obj.REAL_FEE, strTemp);
        PreparedStatement_setDouble(p, 16, atof(strTemp.c_str()));
        ostr << atof(strTemp.c_str()) << ", ";
        PreparedStatement_setString(p, 17, obj.BUYER_LOGON_ID.c_str());
        ostr << obj.BUYER_LOGON_ID << ", ";
        PreparedStatement_setString(p, 18, obj.BUYER_USER_ID.c_str());
        ostr << obj.BUYER_USER_ID << ", ";
        PreparedStatement_setString(p, 19, obj.BANK_BILLNO.c_str());
        ostr << obj.BANK_BILLNO << ", ";
        PreparedStatement_setString(p, 20, obj.TIME_END.c_str());
        ostr << obj.TIME_END << ", ";
        PreparedStatement_setString(p, 21, obj.OUT_REFUND_NO.c_str());
        ostr << obj.OUT_REFUND_NO << ", ";
        PreparedStatement_setString(p, 22, obj.REFUND_ID.c_str());
        ostr << obj.REFUND_ID << ", ";
        strFen2Yuan(obj.REFUND_FEE, strTemp);
        PreparedStatement_setDouble(p, 23, atof(strTemp.c_str()));
        ostr << atof(strTemp.c_str()) << ", ";
        PreparedStatement_setString(p, 24, obj.MERCHANT_ORDER_NO.c_str());
        ostr << obj.MERCHANT_ORDER_NO << ", ";

        LOG_DEBUG("sql参数 [%s]", ostr.str().c_str());
        PreparedStatement_execute(p);

        iRet = 0;
    }
    CATCH(SQLException)
    {
        LOG_ERROR("执行SQL语句失败 %s", Exception_frame.message);
        iRet = -1;
    }
    FINALLY { Connection_close(con); }
    END_TRY;
    return iRet;
}

void ClearScanSend(SCAN_SEND& obj)
{
    obj.MERCHANT_ID = "";
    obj.OUT_TRADE_NO = "";
    obj.OUT_REFUND_NO = "";
}

void ClearScanRecv(SCAN_RECV& obj)
{
    BUFCLR(obj.szServiceData);
    obj.MERCHANT_ID = "";
    obj.OUT_TRADE_NO = "";
    obj.MESSAGE = "";
    obj.APPID = "";
    obj.OPENID = "";
    obj.USER_ID = "";
    obj.SUB_OPENID = "";
    obj.TRADE_TYPE = "";
    obj.TRANSACTION_ID = "";
    obj.OUT_TRANSACTION_ID = "";
    obj.SUB_APPID = "";
    obj.TOTAL_FEE = "";
    obj.DISCOUNT_FEE = "";
    obj.REAL_FEE = "";
    obj.BUYER_LOGON_ID = "";
    obj.BUYER_USER_ID = "";
    obj.BANK_BILLNO = "";
    obj.TIME_END = "";
    obj.CODE = "";
    obj.OUT_REFUND_NO = "";
    obj.REFUND_ID = "";
    obj.REFUND_FEE = "";
    obj.MERCHANT_ORDER_NO = "";
}