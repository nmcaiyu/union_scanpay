// ScanDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ScanDemo.h"

#include "xpack/xpack.h"
#include "xpack/json.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "YLScan.lib")
int __stdcall ScanTrans(char *inputData, char *outputData);

// 唯一的应用程序对象

CWinApp theApp;

using namespace std;
	

typedef struct
{
	string transName, authCode, amt, outTradeNo, body, transactionId, outRefundNo, refundAmt, refundId, outTransactionId;
	XPACK(X(F(OE), transName, authCode, amt, outTradeNo, body, transactionId, outRefundNo, refundAmt, refundId, outTransactionId));
}SCANTRANS_REQUEST;

#define INPUTDATA(name, value, defaultValue)		cout << "请输入 " << name << "[默认值:"<< defaultValue << "]" << endl;\
	cin.clear(); cin.sync(); \
	getline(cin, value); \
if (value.length() == 0)\
{value = defaultValue; }\


void GetDateTime(char *szDateTime)
{
	struct tm *ptm = NULL;
	time_t currtime;
	memset(&currtime, 0x00, sizeof(currtime));
	time(&currtime);
	ptm = localtime(&currtime);
	sprintf(szDateTime, "%04d%02d%02d%02d%02d%02d",
		ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
		ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	return;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// 初始化 MFC 并在失败时显示错误
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO:  更改错误代码以符合您的需要
			_tprintf(_T("错误:  MFC 初始化失败\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO:  在此处为应用程序的行为编写代码。
		}
	}
	else
	{
		// TODO:  更改错误代码以符合您的需要
		_tprintf(_T("错误:  GetModuleHandle 失败\n"));
		nRetCode = 1;
	}
	char szMerchantOrderNo[100 + 1];
	SCANTRANS_REQUEST req;
	string type;
	while (true)
	{
	restart:
		cout << "请输入交易类型" << endl;
		cout << "输入1为 | unified.trade.micropay | 用户被扫支付 |" << endl;
		cout << "输入2为 |  unified.trade.query   | 被扫支付查询 |" << endl;
		cout << "输入3为 |  unified.trade.refund  |   扫码退货   |" << endl;
		cout << "输入4为 |  unified.trade.close  |   扫码关单   |" << endl;
		cout << "输入5为 |  unified.trade.reverse |   扫码撤销   |" << endl;
		cout << "|     JSON节点     |  类型  |               说明                |" << endl;
		cout << "| :--------------: | :----: | :-------------------------------: |" << endl;
		cout << "|    transName     | String |           [扫码交易类型]           |" << endl;
		cout << "|     authCode     | String |              授权码               |" << endl;
		cout << "|       amt        | String |             消费金额              |" << endl;
		cout << "|    outTradeNo    | String |            商户订单号             |" << endl;
		cout << "|       body       | String |             商品信息              |" << endl;
		cout << "|  transactionId   | String |            平台订单号             |" << endl;
		cout << "|   outRefundNo    | String |          退货商户订单号           |" << endl;
		cout << "|    refundAmt     | String |             退货金额              |" << endl;
		cout << "|     refundId     | String |          退货平台订单号           |" << endl;
		cout << "| outTransactionId | String |            第3方订单号            |" << endl;
		cout << "0 为退出测试" << endl;
		cin.clear(); cin.sync();
		getline(cin, type);
		switch (atoi(type.c_str()))
		{
		case 1:
			req.transName = "unified.trade.micropay";
			INPUTDATA("金额", req.amt, "000000000001");
			INPUTDATA("商品信息", req.body, "测试");
			INPUTDATA("授权码", req.authCode, "");
			memset(szMerchantOrderNo, 0x00, sizeof(szMerchantOrderNo));
			GetDateTime(szMerchantOrderNo);
			INPUTDATA("商户订单号", req.outTradeNo, szMerchantOrderNo);
			break;
		case 2:
			req.transName = "unified.trade.query";
			INPUTDATA("商户订单号", req.outTradeNo, "");
			INPUTDATA("平台订单号", req.transactionId, "");
			break;
		case 3:
			req.transName = "unified.trade.refund";
			INPUTDATA("总金额", req.amt, "000000000001");
			INPUTDATA("退货金额", req.refundAmt, "000000000001");
			INPUTDATA("原商户订单号", req.outTradeNo, "");
			INPUTDATA("平台订单号", req.transactionId, "");
			memset(szMerchantOrderNo, 0x00, sizeof(szMerchantOrderNo));
			GetDateTime(szMerchantOrderNo);
			INPUTDATA("退货商户订单号", req.outRefundNo, szMerchantOrderNo);
			break;
		case 4:
			req.transName = "unified.trade.close";
			INPUTDATA("商户订单号", req.outTradeNo, "");
			break;
		case 5:
			req.transName = "unified.micropay.reverse";
			INPUTDATA("商户订单号", req.outTradeNo, "");
			break;
		case 0:
			goto cleanUP;
			break;
		default:
			goto restart;
			break;
		}
		char szOut[1024 + 1];
		memset(szOut, 0x00, sizeof(szOut));
		cout << "请求字符串\n" << xpack::json::encode(req, 0, 2, ' ') << endl;
		ScanTrans((char *)xpack::json::encode(req).c_str(), szOut);
		cout << "返回字符串\n" << szOut << endl;
	}

cleanUP:
	return nRetCode;
}
