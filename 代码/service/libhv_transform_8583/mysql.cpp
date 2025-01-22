#include <zdb.h>

#include "mysql.h"

URL_T gDBURL;
ConnectionPool_T gDBPool;

int initMysql()
{
    char szMysqlUrl[1024 * 2];
    BUFCLR(szMysqlUrl);
    sprintf(szMysqlUrl, "mysql://%s:%d/%s?user=%s&password=%s&charset=utf8", globalCFG.szMysqlUrl, globalCFG.iMysqlPort, globalCFG.szMysqlDB,
            globalCFG.szMysqlUser, globalCFG.szMysqlPass);
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

int InsertPosRecv(POS_RECV obj)
{
    ostringstream ostr;
    if (obj.TRANS == POSTRANS_OTHER)
    {
        return 0;
    }
    Connection_T con = NULL;
    int iRet;
    char szSQL[1024 * 2];
    BUFCLR(szSQL);
    sprintf(szSQL,
            " INSERT INTO T_POS_RECV ("
            " `MERCHANT_NO`, "
            " `TERMINAL_NO`, "
            " `TRANS`, "
            " `TRACE`, "
            " `BATCH`, "
            " `CARD_NO`, "
            " `AMT`, "
            " `REF_NO`, "
            " `TRANS_DATE`, "
            " `TRANS_TIME`, "
            " `RESP_CODE`, "
            " `RESP_MSG`"
            " )"
            " VALUES (?,?,?,?,?,?,?,?,?,?,?,?)");

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
        PreparedStatement_setString(p, 1, obj.MERCHANT_NO.c_str());
        ostr << obj.MERCHANT_NO << ", ";
        PreparedStatement_setString(p, 2, obj.TERMINAL_NO.c_str());
        ostr << obj.TERMINAL_NO << ", ";
        PreparedStatement_setInt(p, 3, obj.TRANS);
        ostr << obj.TRANS << ", ";
        PreparedStatement_setString(p, 4, obj.TRACE.c_str());
        ostr << obj.TRACE << ", ";
        PreparedStatement_setString(p, 5, obj.BATCH.c_str());
        ostr << obj.BATCH << ", ";
        PreparedStatement_setString(p, 6, obj.CARD_NO.c_str());
        ostr << obj.CARD_NO << ", ";
        PreparedStatement_setDouble(p, 7, atof(obj.AMT.insert(10, 1, '.').c_str()));
        ostr << atof(obj.AMT.insert(10, 1, '.').c_str()) << ", ";
        PreparedStatement_setString(p, 8, obj.REF_NO.c_str());
        ostr << obj.REF_NO << ", ";
        PreparedStatement_setString(p, 9, obj.TRANS_DATE.c_str());
        ostr << obj.TRANS_DATE << ", ";
        PreparedStatement_setString(p, 10, obj.TRANS_TIME.c_str());
        ostr << obj.TRANS_TIME << ", ";
        PreparedStatement_setString(p, 11, obj.RESP_CODE.c_str());
        ostr << obj.RESP_CODE << ", ";
        PreparedStatement_setString(p, 12, obj.RESP_MSG.c_str());
        ostr << obj.RESP_MSG << ", ";
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

int InsertPosSend(POS_SEND obj)
{
    ostringstream ostr;
    if (obj.TRANS == POSTRANS_OTHER)
    {
        return 0;
    }
    Connection_T con = NULL;
    int iRet;
    char szSQL[1024 * 2];
    BUFCLR(szSQL);
    sprintf(szSQL, "INSERT INTO T_POS_SEND (`MERCHANT_NO`, `TERMINAL_NO`, `TRANS`, `TRACE`, `CARD_NO`, `BATCH`, `AMT`) VALUES (?, ?, ?, ?, ?, ?, ?)");
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
        PreparedStatement_setString(p, 1, obj.MERCHANT_NO.c_str());
        ostr << obj.MERCHANT_NO << ", ";
        PreparedStatement_setString(p, 2, obj.TERMINAL_NO.c_str());
        ostr << obj.TERMINAL_NO << ", ";
        PreparedStatement_setInt(p, 3, obj.TRANS);
        ostr << obj.TRANS << ", ";
        PreparedStatement_setString(p, 4, obj.TRACE.c_str());
        ostr << obj.TRACE << ", ";
        PreparedStatement_setString(p, 5, obj.CARD_NO.c_str());
        ostr << obj.CARD_NO << ", ";
        PreparedStatement_setString(p, 6, obj.BATCH.c_str());
        ostr << obj.BATCH << ", ";
        PreparedStatement_setDouble(p, 7, atof(obj.AMT.insert(10, 1, '.').c_str()));
        ostr << atof(obj.AMT.insert(10, 1, '.').c_str()) << ", ";
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

void ClearPosSend(POS_SEND& obj)
{
    obj.TRANS = POSTRANS_OTHER;
    obj.MERCHANT_NO = "";
    obj.TERMINAL_NO = "";
    obj.TRACE = "";
    obj.CARD_NO = "";
    obj.BATCH = "";
}

void ClearPosRecv(POS_RECV& obj)
{
    obj.TRANS = POSTRANS_OTHER;
    obj.MERCHANT_NO = "";
    obj.TERMINAL_NO = "";
    obj.TRACE = "";
    obj.BATCH = "";
    obj.CARD_NO = "";
    obj.AMT = "";
    obj.REF_NO = "";
    obj.TRANS_DATE = "";
    obj.TRANS_TIME = "";
    obj.RESP_CODE = "";
    obj.RESP_MSG = "";
}