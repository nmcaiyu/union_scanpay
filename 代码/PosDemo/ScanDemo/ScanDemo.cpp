// ScanDemo.cpp : �������̨Ӧ�ó������ڵ㡣
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

// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;
	

typedef struct
{
	string transName, authCode, amt, outTradeNo, body, transactionId, outRefundNo, refundAmt, refundId, outTransactionId;
	XPACK(X(F(OE), transName, authCode, amt, outTradeNo, body, transactionId, outRefundNo, refundAmt, refundId, outTransactionId));
}SCANTRANS_REQUEST;

#define INPUTDATA(name, value, defaultValue)		cout << "������ " << name << "[Ĭ��ֵ:"<< defaultValue << "]" << endl;\
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
		// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO:  ���Ĵ�������Է���������Ҫ
			_tprintf(_T("����:  MFC ��ʼ��ʧ��\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO:  �ڴ˴�ΪӦ�ó������Ϊ��д���롣
		}
	}
	else
	{
		// TODO:  ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����:  GetModuleHandle ʧ��\n"));
		nRetCode = 1;
	}
	char szMerchantOrderNo[100 + 1];
	SCANTRANS_REQUEST req;
	string type;
	while (true)
	{
	restart:
		cout << "�����뽻������" << endl;
		cout << "����1Ϊ | unified.trade.micropay | �û���ɨ֧�� |" << endl;
		cout << "����2Ϊ |  unified.trade.query   | ��ɨ֧����ѯ |" << endl;
		cout << "����3Ϊ |  unified.trade.refund  |   ɨ���˻�   |" << endl;
		cout << "����4Ϊ |  unified.trade.close  |   ɨ��ص�   |" << endl;
		cout << "����5Ϊ |  unified.trade.reverse |   ɨ�볷��   |" << endl;
		cout << "|     JSON�ڵ�     |  ����  |               ˵��                |" << endl;
		cout << "| :--------------: | :----: | :-------------------------------: |" << endl;
		cout << "|    transName     | String |           [ɨ�뽻������]           |" << endl;
		cout << "|     authCode     | String |              ��Ȩ��               |" << endl;
		cout << "|       amt        | String |             ���ѽ��              |" << endl;
		cout << "|    outTradeNo    | String |            �̻�������             |" << endl;
		cout << "|       body       | String |             ��Ʒ��Ϣ              |" << endl;
		cout << "|  transactionId   | String |            ƽ̨������             |" << endl;
		cout << "|   outRefundNo    | String |          �˻��̻�������           |" << endl;
		cout << "|    refundAmt     | String |             �˻����              |" << endl;
		cout << "|     refundId     | String |          �˻�ƽ̨������           |" << endl;
		cout << "| outTransactionId | String |            ��3��������            |" << endl;
		cout << "0 Ϊ�˳�����" << endl;
		cin.clear(); cin.sync();
		getline(cin, type);
		switch (atoi(type.c_str()))
		{
		case 1:
			req.transName = "unified.trade.micropay";
			INPUTDATA("���", req.amt, "000000000001");
			INPUTDATA("��Ʒ��Ϣ", req.body, "����");
			INPUTDATA("��Ȩ��", req.authCode, "");
			memset(szMerchantOrderNo, 0x00, sizeof(szMerchantOrderNo));
			GetDateTime(szMerchantOrderNo);
			INPUTDATA("�̻�������", req.outTradeNo, szMerchantOrderNo);
			break;
		case 2:
			req.transName = "unified.trade.query";
			INPUTDATA("�̻�������", req.outTradeNo, "");
			INPUTDATA("ƽ̨������", req.transactionId, "");
			break;
		case 3:
			req.transName = "unified.trade.refund";
			INPUTDATA("�ܽ��", req.amt, "000000000001");
			INPUTDATA("�˻����", req.refundAmt, "000000000001");
			INPUTDATA("ԭ�̻�������", req.outTradeNo, "");
			INPUTDATA("ƽ̨������", req.transactionId, "");
			memset(szMerchantOrderNo, 0x00, sizeof(szMerchantOrderNo));
			GetDateTime(szMerchantOrderNo);
			INPUTDATA("�˻��̻�������", req.outRefundNo, szMerchantOrderNo);
			break;
		case 4:
			req.transName = "unified.trade.close";
			INPUTDATA("�̻�������", req.outTradeNo, "");
			break;
		case 5:
			req.transName = "unified.micropay.reverse";
			INPUTDATA("�̻�������", req.outTradeNo, "");
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
		cout << "�����ַ���\n" << xpack::json::encode(req, 0, 2, ' ') << endl;
		ScanTrans((char *)xpack::json::encode(req).c_str(), szOut);
		cout << "�����ַ���\n" << szOut << endl;
	}

cleanUP:
	return nRetCode;
}
