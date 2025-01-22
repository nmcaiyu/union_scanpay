#include "config.h"

CFG globalCFG;

int readIni() {
    Log.init("log.properties");
    INIReader reader("config.ini");
    if (reader.ParseError() < 0)
    {
        return 1;
    }

    memset(&globalCFG, 0x00, sizeof(CFG));
    globalCFG.port = reader.GetInteger("listen", "port", 1000);
    strcpy(globalCFG.host, reader.Get("listen", "host", "0.0.0.0").c_str());
    globalCFG.process_num = reader.GetInteger("listen", "process_num", 2);
    globalCFG.thread_num = reader.GetInteger("listen", "thread_num", 100);
    
    strcpy(globalCFG.url, reader.Get("info", "url", "").c_str());
    globalCFG.iConnTimeout = reader.GetInteger("info", "connTimeout", 10);
    globalCFG.iConnTimeout = reader.GetInteger("info", "recvTimeout", 50);

    // mysql
    strcpy(globalCFG.szMysqlUrl, reader.Get("mysql", "url", "").c_str());
    strcpy(globalCFG.szMysqlDB, reader.Get("mysql", "db", "").c_str());
    strcpy(globalCFG.szMysqlUser, reader.Get("mysql", "user", "").c_str());
    strcpy(globalCFG.szMysqlPass, reader.Get("mysql", "pass", "").c_str());
    globalCFG.iMysqlPort = reader.GetInteger("mysql", "port", 3306);
    globalCFG.iMysqlMin = reader.GetInteger("mysql", "min", 10);
    globalCFG.iMysqlMax = reader.GetInteger("mysql", "max", 50);
    
    return 0;
}