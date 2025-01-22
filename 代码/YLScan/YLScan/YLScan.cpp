// YLScan.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "YLScan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define INITCHAR(VARNAME, LEN) char VARNAME[LEN]; memset(VARNAME, 0x00, LEN);

//
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CYLScanApp

BEGIN_MESSAGE_MAP(CYLScanApp, CWinApp)
END_MESSAGE_MAP()


// CYLScanApp 构造

CYLScanApp::CYLScanApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CYLScanApp 对象

CYLScanApp theApp;


// CYLScanApp 初始化

BOOL CYLScanApp::InitInstance()
{
	CWinApp::InitInstance();


	return TRUE;
}
#include "Tools.h"
#include "slog.h"
using namespace Tool;

typedef struct
{
	char szConfigPath[MAX_PATH + 1];
	char szLogProp[MAX_PATH + 1];
	char szDLLPath[MAX_PATH + 1];

	// 扫码
	char szScanIP[1024 + 1];
	char szDevice[100 + 1];
	int iScanPort, iSSL;
	int iScanTimeout;
	int iScanType;
	char szScanKey[1024 + 1];
	// 银行卡
	char szCOM[20 + 1];
	int iBPS;
	int iCOMTimeout, iPOSSSL;
	char szPOSIP[1024 + 1];
	int iPOSPort;
	int iPOSTimeout;
	// 设备信息
	char szVN[100 + 1];
	char szDevicePubKeyPath[MAX_PATH + 1];
	char szDevicePriKeyPath[MAX_PATH + 1];
	// 商户信息
	char szSN[100 + 1];
	char szMERNO[100 + 1];
	char szORGPUBKEY[1024 + 1];
	
}CFG;

CFG config;
void InitConfig()
{
	char *pIndex = NULL;
	memset(&config, 0x00, sizeof(config));
	GetModuleFileName(Tools::GetSelfModuleHandle(), config.szConfigPath, MAX_PATH);
	if ((pIndex = strrchr(config.szConfigPath, '\\')) != NULL)
	{
		strcpy(pIndex + 1, "POSConfig.ini");
	}
	GetModuleFileName(Tools::GetSelfModuleHandle(), config.szDLLPath, MAX_PATH);
	if (config.szDLLPath != NULL)
	{
		pIndex = strrchr(config.szDLLPath, '\\');
		if (pIndex != NULL)
		{
			*pIndex = '\0';
		}
	}

	sprintf(config.szLogProp, "%s\\%s", config.szDLLPath, "log.properties");
	// 扫码
	GetPrivateProfileString("SCAN", "IP", "", config.szScanIP, sizeof(config.szScanIP), config.szConfigPath);
	config.iScanPort = GetPrivateProfileInt("SCAN", "PORT", 0, config.szConfigPath);
	config.iScanTimeout = GetPrivateProfileInt("SCAN", "TIMEOUT", 60, config.szConfigPath);
	config.iScanType = GetPrivateProfileInt("SCAN", "TYPE", 1, config.szConfigPath);
	GetPrivateProfileString("SCAN", "DEVICE", "", config.szDevice, sizeof(config.szDevice), config.szConfigPath);


	// 银行卡
	GetPrivateProfileString("POS", "NAME", "", config.szCOM, sizeof(config.szCOM), config.szConfigPath);
	config.iBPS = GetPrivateProfileInt("POS", "BAUDRATE", 9600, config.szConfigPath);
	config.iCOMTimeout = GetPrivateProfileInt("POS", "COMTIMEOUT", 120, config.szConfigPath);
	GetPrivateProfileString("POS", "IP", "", config.szPOSIP, sizeof(config.szPOSIP), config.szConfigPath);
	config.iPOSPort = GetPrivateProfileInt("POS", "PORT", 0, config.szConfigPath);
	config.iPOSTimeout = GetPrivateProfileInt("POS", "SOCKETTIMEOUT", 120, config.szConfigPath);


	GetPrivateProfileString("ACTIVE", "VN", "", config.szVN, sizeof(config.szVN), config.szConfigPath);
	GetPrivateProfileString("MERINFO", "SN", "", config.szSN, sizeof(config.szSN), config.szConfigPath);
	GetPrivateProfileString("MERINFO", "MERNO", "", config.szMERNO, sizeof(config.szMERNO), config.szConfigPath);
	GetPrivateProfileString("MERINFO", "ORGPUBKEY", "", config.szORGPUBKEY, sizeof(config.szORGPUBKEY), config.szConfigPath);
	GetPrivateProfileString("MERINFO", "KEY", "", config.szScanKey, sizeof(config.szScanKey), config.szConfigPath);
	sprintf(config.szDevicePubKeyPath, "%s\\%s_pub.pem", config.szDLLPath, config.szSN);
	sprintf(config.szDevicePriKeyPath, "%s\\%s_pri.pem", config.szDLLPath, config.szSN);

	Log.init(config.szLogProp);
	char szVer[20 + 1];
	BUFCLR(szVer);
	Tools::GetCurrFileVersion(szVer);
	LOG_INFO("动态库版本号[%s]", szVer);

	string errmsg;
	string strDumpPath = config.szDLLPath;
	strDumpPath.append("\\");
// 	if (!CrashEngine::getInstance().init(strDumpPath.c_str(), errmsg))
// 	{
// 		LOG_ERROR(errmsg.c_str());
// 	}
}

int TI_iCmpStruct(const void *a, const void *b)
{
	CString *strA = (CString*)a;
	CString *strB = (CString*)b;
	return strA->Compare(*strB);
}

void findJson_UTF8(cJSON *root, char *szName, char *szValue)
{
	cJSON *node;
	if (szName == NULL)
	{
		return;
	}

	for (node = root->child; node != NULL; node = node->next)
	{
		if (strcmp(szName, node->string) == 0)
		{
			char szReturn_GBK[1024];
			BUFCLR(szReturn_GBK);
			Tools::strlcpy(szReturn_GBK, node->valuestring, sizeof(szReturn_GBK));
			Tools::GBKToUTF8(szReturn_GBK, szValue);
		}
	}
	return;
}

int base64Encode(const char *buffer, int length, bool newLine, char *buff)
{
	BIO *bmem = NULL;
	BIO *b64 = NULL;
	BUF_MEM *bptr;

	b64 = BIO_new(BIO_f_base64());
	if (!newLine) {
		BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	}
	bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);
	BIO_write(b64, buffer, length);
	BIO_flush(b64);
	BIO_get_mem_ptr(b64, &bptr);
	BIO_set_close(b64, BIO_NOCLOSE);

	memcpy(buff, bptr->data, bptr->length);
	buff[bptr->length] = 0;
	BIO_free_all(b64);
	return bptr->length;
}

void makeSign_wft(string &strData, string &sign)
{
	MD5_CTX c;
	// 	string sign = "";
	BIO *key = NULL;
	RSA* rsa = NULL;
	char szErrMsg[1024];
	BUFCLR(szErrMsg);
	string strContent = "";
	unsigned int iSigLen;
	unsigned char szSigOut[128];
	unsigned char szDataOut[1024];
	char pvDataOut[1024];
	CString strTemp[100 + 1];
	cJSON *root = cJSON_Parse(strData.c_str());
	if (root == NULL)
	{
		goto cleanUP;
	}
	int i = 0;
	cJSON *node;
	node = root->child;
	for (; node != NULL; node = node->next)
	{
		strTemp[i] = node->string;
		i++;
	}
	qsort(strTemp, i, sizeof(CString), TI_iCmpStruct);
	for (int j = 0; j < i; j++)
	{
		char szValue[1024];
		BUFCLR(szValue);
		findJson_UTF8(root, strTemp[j].GetBuffer(), szValue);
		if (strlen(szValue) != 0)
		{
			strContent.append(strTemp[j]);
			strContent.append("=");
			strContent.append(szValue);
			strContent.append("&");
		}
	}
	strContent.erase(strContent.end() - 1);
	strContent.append("&key=");
	strContent.append(config.szScanKey);
	LOG_DEBUG("计算加密前字符串 [%s]", strContent.c_str());
	if (1 != MD5_Init(&c))
	{
		BUFCLR(szErrMsg);
		ERR_error_string(ERR_get_error(), szErrMsg);
		LOG_ERROR("MD5_Init MD5初始化失败 [%ld][%s]", ERR_get_error(), szErrMsg);
		goto cleanUP;
	}

	if (1 != MD5_Update(&c, (const void *)strContent.c_str(), strContent.length()))
	{
		BUFCLR(szErrMsg);
		ERR_error_string(ERR_get_error(), szErrMsg);
		LOG_ERROR("MD5_Update MD5添加失败 [%ld][%s]", ERR_get_error(), szErrMsg);
		goto cleanUP;
	}

	BUFCLR(szSigOut);
	if (1 != MD5_Final(szSigOut, &c))
	{
		BUFCLR(szErrMsg);
		ERR_error_string(ERR_get_error(), szErrMsg);
		LOG_ERROR("MD5_Final MD5计算失败 [%ld][%s]", ERR_get_error(), szErrMsg);
		goto cleanUP;
	}
	BUFCLR(szDataOut);
	Tools::bcd_to_asc(szDataOut, szSigOut, MD5_DIGEST_LENGTH * 2, 0);
	sign = (char *)szDataOut;
cleanUP:
	LOG_DEBUG("计算结果 [%s]", sign.c_str());
}

void makeSign(string &strData, string &sign)
{
	MD5_CTX c;
// 	string sign = "";
	BIO *key = NULL;
	RSA* rsa = NULL;
	char szErrMsg[1024];
	BUFCLR(szErrMsg);
	string strContent = "";
	unsigned int iSigLen;
	unsigned char szSigOut[128];
	unsigned char szDataOut[1024];
	char pvDataOut[1024];
	CString strTemp[100 + 1];
	cJSON *root = cJSON_Parse(strData.c_str());
	if (root == NULL)
	{
		goto cleanUP;
	}
	int i = 0;
	cJSON *node;
	node = root->child;
	for (; node != NULL; node = node->next)
	{
		strTemp[i] = node->string;
		i++;
	}
	qsort(strTemp, i, sizeof(CString), TI_iCmpStruct);
	for (int j = 0; j < i; j++)
	{
		char szValue[1024];
		BUFCLR(szValue);
		findJson_UTF8(root, strTemp[j].GetBuffer(), szValue);
		if (strlen(szValue) != 0)
		{
			strContent.append(strTemp[j]);
			strContent.append("=");
			strContent.append(szValue);
			strContent.append("&");
		}
	}
	strContent.erase(strContent.end() - 1);
#ifdef DEBUG_MD5
	AfxMessageBox("测试版本计算MD5KEY");
	strContent.append("&key=979da4cfccbae7923641daa5dd7047c2");
	LOG_DEBUG("计算加密前字符串 [%s]", strContent.c_str());
	if (1 != MD5_Init(&c))
	{
		BUFCLR(szErrMsg);
		ERR_error_string(ERR_get_error(), szErrMsg);
		LOG_ERROR("MD5_Init MD5初始化失败 [%ld][%s]", ERR_get_error(), szErrMsg);
		goto cleanUP;
	}

	if (1 != MD5_Update(&c, (const void *)strContent.c_str(), strContent.length()))
	{
		BUFCLR(szErrMsg);
		ERR_error_string(ERR_get_error(), szErrMsg);
		LOG_ERROR("MD5_Update MD5添加失败 [%ld][%s]", ERR_get_error(), szErrMsg);
		goto cleanUP;
	}

	BUFCLR(szSigOut);
	if (1 != MD5_Final(szSigOut, &c))
	{
		BUFCLR(szErrMsg);
		ERR_error_string(ERR_get_error(), szErrMsg);
		LOG_ERROR("MD5_Final MD5计算失败 [%ld][%s]", ERR_get_error(), szErrMsg);
		goto cleanUP;
	}
	BUFCLR(szDataOut);
	Tools::bcd_to_asc(szDataOut, szSigOut, MD5_DIGEST_LENGTH * 2, 0);
	sign = (char *)szDataOut;
#else 
	LOG_DEBUG("计算加密前字符串 [%s]", strContent.c_str());
	BUFCLR(szDataOut);

	SHA256((unsigned char*)strContent.c_str(), strContent.length(), szDataOut);
	BUFCLR(szSigOut);
	Tools::bcd_to_asc(szSigOut, szDataOut, SHA256_DIGEST_LENGTH * 2, true);
	BUFCLR(szDataOut);
	strcpy((char *)szDataOut, (char *)szSigOut);

	strContent = (char *)szDataOut;
	BUFCLR(szDataOut);
	SHA256((unsigned char*)strContent.c_str(), strContent.length(), szDataOut);

	LOG_DEBUGSB((char *)szDataOut, SHA256_DIGEST_LENGTH, "2次 SHA256 计算结果 [%d]", SHA256_DIGEST_LENGTH);


	key = BIO_new(BIO_s_file());
	BIO_read_filename(key, config.szDevicePriKeyPath);
	if (key == NULL)
	{
		BUFCLR(szErrMsg);
		ERR_error_string(ERR_get_error(), szErrMsg);
		LOG_ERROR("BIO_read_filename 密钥文件读取失败 [%ld][%s]", ERR_get_error(), szErrMsg);
		goto cleanUP;
	}
// 	if (NULL == (fp = fopen(config.szDevicePriKeyPath, "r")))
// 	{
// 		LOG_ERROR("密钥文件读取失败 [%s]", config.szDevicePriKeyPath);
// 		goto cleanUP;
// 	}
// 	rsa = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
	rsa = PEM_read_bio_RSAPrivateKey(key, &rsa, NULL, NULL);
	if (rsa == NULL)
	{
		BUFCLR(szErrMsg);
		ERR_error_string(ERR_get_error(), szErrMsg);
		LOG_ERROR("PEM_read_bio_RSAPrivateKey 密钥内容读取失败 [%ld][%s]", ERR_get_error(), szErrMsg);
		goto cleanUP;
	}
	BUFCLR(szSigOut);

	int iRet = RSA_sign(NID_sha256, szDataOut, SHA256_DIGEST_LENGTH, (unsigned char*)szSigOut, &iSigLen, rsa);
	if (iRet >= 0)
	{
		LOG_DEBUGSB((char *)szSigOut, iSigLen, "RSA计算结果 [%d]", iSigLen);
		BUFCLR(pvDataOut);
		base64Encode((char *)szSigOut, iSigLen, false, pvDataOut);
		sign = pvDataOut;
		goto cleanUP;
	}
	else 
	{
		BUFCLR(szErrMsg);
		ERR_error_string(ERR_get_error(), szErrMsg);
		LOG_ERROR("RSA_sign 密钥签名失败 [%ld][%s]", ERR_get_error(), szErrMsg);
		goto cleanUP;
	}
#endif // DEBUG_MD5

cleanUP:
	if (key != NULL)
	{
		LOG_DEBUG("BIO_free_all 1");
		BIO_free_all(key);
		LOG_DEBUG("BIO_free_all 2");
	}
	if (rsa != NULL)
	{
		LOG_DEBUG("RSA_free 1");
		RSA_free(rsa);
		LOG_DEBUG("RSA_free 2");
	}
	if (root != NULL)
	{
		cJSON_Delete(root);
	}
	LOG_DEBUG("计算结果 [%s]", sign.c_str());
}

int SendRecvSocket(unsigned char *szSendBuf, int iSendLen, unsigned char *szRecvBuf, int *iRecvLen, CString &strErrMsg, CString &strNeed)
{
	int iRet = -1;
	int fdSocket = 0;
	fdSocket = EI_iCreateClientSocketWithTimeout(config.szScanIP, config.iScanPort, AF_INET, SOCK_STREAM, config.iScanTimeout, 0, NULL);
	if (fdSocket <= 0)
	{
		strNeed = "N";
		strErrMsg.Format("链接 [%s:%d] 失败 [%d]", config.szScanIP, config.iScanPort, fdSocket);
		iRet = -1;
		goto cleanUP;
	}
	if (EI_iSetSocketRecvTimeout(fdSocket, config.iScanTimeout, 0) == -1)
	{
		strNeed = "N";
		strErrMsg.Format("链接 [%s:%d] 设置超时失败 [%d]", config.szScanIP, config.iScanPort, fdSocket);
		iRet = -1;
		goto cleanUP;
	}

	if (EI_iSetBlocking(fdSocket) == -1)
	{
		strNeed = "N";
		strErrMsg.Format("链接 [%s:%d] 设置模式失败 [%d]", config.szScanIP, config.iScanPort, fdSocket);
		iRet = -1;
		goto cleanUP;
	}
	iRet = EI_iSendBuf(fdSocket, (char *)szSendBuf, iSendLen);
	if (iRet != 0)
	{
		strNeed = "N";
		strErrMsg.Format("发送报文 [%s:%d] 失败 [%d]", config.szScanIP, config.iScanPort, iRet);
		iRet = -1;
		goto cleanUP;
	}
	iRet = EI_iRecvNBytes(fdSocket, (char *)szRecvBuf, 2);
	if (iRet != 2)
	{
		strNeed = "Y";
		strErrMsg.Format("接收报文长度[%s:%d] 失败[%d]", config.szScanIP, config.iScanPort, iRet);
		iRet = -1;
		goto cleanUP;
	}
	int recvLen = szRecvBuf[0] * 256 + szRecvBuf[1];
	LOG_DEBUG("计算长度为[%d]", recvLen);
	iRet = EI_iRecvNBytes(fdSocket, (char *)szRecvBuf + 2, recvLen);
	if (iRet != recvLen)
	{
		strNeed = "Y";
		LOG_ERRORSB((char *)szRecvBuf, recvLen + 2, "接收报文内容[%s:%d] 失败[%d]", config.szScanIP, config.iScanPort, iRet);
		strErrMsg.Format("接收报文内容[%s:%d] 失败[%d]", config.szScanIP, config.iScanPort, iRet);
		iRet = -1;
		goto cleanUP;
	}
	iRet = 0;
	strNeed = "";
cleanUP:
	if (strErrMsg.GetLength() > 0)
	{
		LOG_ERROR("%s", strErrMsg);
	}
	if (fdSocket > 0)
	{
		Ei_CloseSocket(fdSocket);
	}
	return iRet;
}

string trade_micropay_getmsg(trade_micropay_response res)
{
	if (res.pay_info.length() > 0)
	{
		return res.pay_info;
	}
	else if (res.err_msg.length() > 0)
	{
		return res.err_msg;
	}
	else if (res.message.length() > 0)
	{
		return res.message;
	}
	else
	{
		return "未知返回信息";
	}
}

void trade_micropay(SCANTRANS_REQUEST req, SCANTRANS_RESPONSE &res)
{
	CString cstrData, cstrErrMsg, cstrNeedQuery;
	int fdSocket = 0;
	char szDateTime[30 + 1];
	BUFCLR(szDateTime);
	trade_micropay_request request;
	trade_micropay_response response;
	unsigned char szSendBuf[SOCKETDATELEN], szRecvBuf[SOCKETDATELEN];
	string strData;
	int iRecvLen;

	request.service = SCAN_PAY;
	request.mch_create_ip = "127.0.0.1";
#ifdef DEBUG_MD5
	request.sign_type = "MD5";
#else 
// 	request.op_device_id = config.szSN;
	request.device_info = "EPOS";
	request.sign_type = RSA_SIGNTYPE;
	request.sn_id = config.szVN;
	request.sn_id.append(config.szSN);
#endif // DEBUG_MD5

	request.mch_id = config.szMERNO;
	Tools::GetDateTime(szDateTime);
	request.nonce_str = szDateTime;

	request.out_trade_no = req.outTradeNo;
	request.body = req.body;
	request.total_fee = req.amt;
	request.auth_code = req.authCode;

	strData = xpack::json::encode(request);
	makeSign(strData, request.sign);

	
	BUFCLR(szSendBuf);
	BUFCLR(szRecvBuf);
	strData = xpack::xml::encode(request, "XML", 0, 2, ' ');
	cstrData = Tools::GBKToUTF8(strData.c_str());
	szSendBuf[0] = cstrData.GetLength() / 256;
	szSendBuf[1] = cstrData.GetLength() % 256;
	strcpy((char *)szSendBuf + 2, cstrData.GetBuffer(0));
	LOG_INFO("发送报文 [%s]", strData.c_str());
	
	if (SendRecvSocket(szSendBuf, cstrData.GetLength() + 2, szRecvBuf, &iRecvLen, cstrErrMsg, cstrNeedQuery) != 0)
	{
		res.respCode = "XX";
		res.respMsg = cstrErrMsg.GetBuffer(0);
		res.needQuery = cstrNeedQuery.GetBuffer(0);
		goto cleanUP;
	}

	try
	{
		BUFCLR(szSendBuf);
		Tools::DeleteCDATA((char *)szSendBuf, (char *)szRecvBuf + 2);
		cstrData = Tools::UTF8ToGBK((char *)szSendBuf);
		LOG_INFO("后台返回字符串 [%s]", cstrData.GetBuffer(0));
		xpack::xml::decode(cstrData.GetBuffer(0), response);
	}
	catch (exception &e)
	{
		LOG_ERROR("解析返回XML失败 %s", e.what());
		res.respCode = "XX";
		res.respMsg = "解析返回XML失败";
		res.needQuery = "Y";
		goto cleanUP;
	}

	if (response.need_query.compare("Y") == 0)
	{
		res.respCode = "EE";
		res.needQuery = "Y";
		res.respMsg = trade_micropay_getmsg(response);
		goto cleanUP;
	}
	else if(response.status.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_micropay_getmsg(response);
		goto cleanUP;
	}
	else if (response.result_code.compare("0") != 0)
	{
		if (response.err_code.compare("SYSTEMERROR") == 0 || 
			response.err_code.compare("Internal error") == 0 ||
			response.err_code.compare("BANKERROR") == 0 ||
			response.err_code.compare("10003") == 0 ||
			response.err_code.compare("USERPAYING") == 0 ||
			response.err_code.compare("System error") == 0 ||
			response.err_code.compare("aop.ACQ.SYSTEM_ERROR") == 0 ||
			response.err_code.compare("ACQ.SYSTEM_ERROR") == 0
			)
		{
			res.respCode = "EE";
			res.needQuery = "Y";
			res.respMsg = trade_micropay_getmsg(response);
			goto cleanUP;
		}
		else if (response.err_code.compare("04") == 0 ||
			response.err_code.compare("06") == 0
			)
		{
			res.respCode = "EE";
			res.needQuery = "Y";
			res.respMsg = trade_micropay_getmsg(response);
			goto cleanUP;
		}
		else if (response.err_code.compare("00") == 0 && response.trade_type.compare("pay.unionpay.micropay") == 0)
		{
			res.respCode = "00";
			res.needQuery = "N";
			res.outTradeNo = response.out_trade_no;
			res.transactionId = response.transaction_id;
			res.outTransactionId = response.out_transaction_id;
			res.tradeType = response.trade_type;
			res.bankBillNo = response.bank_billno;
			res.dateTime = response.time_end;
			res.amt = response.total_fee;
			goto cleanUP;
		}
		else
		{
			res.respCode = "EE";
			res.needQuery = "N";
			res.respMsg = trade_micropay_getmsg(response);
			goto cleanUP;
		}
	}
	else if (response.pay_result.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_micropay_getmsg(response);
		goto cleanUP;
	}
	else
	{
		res.respCode = "00";
		res.needQuery = "N";
		res.outTradeNo = response.out_trade_no;
		res.transactionId = response.transaction_id;
		res.outTransactionId = response.out_transaction_id;
		res.tradeType = response.trade_type;
		res.bankBillNo = response.bank_billno;
		res.dateTime = response.time_end;
		res.amt = response.total_fee;
		goto cleanUP;
	}



cleanUP:
	if (res.outTradeNo.length() == 0)
	{
		res.outTradeNo = req.outTradeNo;
	}
}

string trade_query_getmsg(trade_query_response res)
{
	if (res.trade_state_desc.length() > 0)
	{
		return res.trade_state_desc;
	}
	if (res.err_msg.length() > 0)
	{
		return res.err_msg;
	}
	else if (res.message.length() > 0)
	{
		return res.message;
	}
	else
	{
		if (res.trade_state.compare("REFUND") == 0)
		{
			return "[REFUND]-转入退款";
		}
		else if (res.trade_state.compare("NOTPAY") == 0)
		{
			return "[NOTPAY]-未支付";
		}
		else if (res.trade_state.compare("CLOSED") == 0)
		{
			return "[CLOSED]-已关闭";
		}
		else if (res.trade_state.compare("REFUND") == 0 || res.trade_state.compare("REVERSE") == 0)
		{
			return "[REVOKED/REVERSE]-转入退款";
		}
		else if (res.trade_state.compare("USERPAYING") == 0)
		{
			return "[USERPAYING]-用户支付中";
		}
		else if (res.trade_state.compare("PAYERROR") == 0)
		{
			return "[PAYERROR]-支付失败";
		}
		else
		{
			return "未知返回信息";
		}
	}
}

void trade_query(SCANTRANS_REQUEST req, SCANTRANS_RESPONSE &res)
{
	CString cstrData, cstrErrMsg, cstrNeedQuery;
	int fdSocket = 0;
	char szDateTime[30 + 1];
	BUFCLR(szDateTime);
	trade_query_request request;
	trade_query_response response;
	unsigned char szSendBuf[SOCKETDATELEN], szRecvBuf[SOCKETDATELEN];
	string strData;
	int iRecvLen;

	request.service = SCAN_PAYQUERY;
#ifdef DEBUG_MD5
	request.sign_type = "MD5";
#else 
	request.device_info = "EPOS";
	request.sign_type = RSA_SIGNTYPE;
	request.sn_id = config.szVN;
	request.sn_id.append(config.szSN);
#endif // DEBUG_MD5

	request.mch_id = config.szMERNO;
	Tools::GetDateTime(szDateTime);
	request.nonce_str = szDateTime;

	request.out_trade_no = req.outTradeNo;
	request.transaction_id = req.transactionId;

	strData = xpack::json::encode(request);
	makeSign(strData, request.sign);
	BUFCLR(szSendBuf);
	BUFCLR(szRecvBuf);
	strData = xpack::xml::encode(request, "XML", 0, 2, ' ');
	cstrData = Tools::GBKToUTF8(strData.c_str());
	szSendBuf[0] = cstrData.GetLength() / 256;
	szSendBuf[1] = cstrData.GetLength() % 256;
	strcpy((char *)szSendBuf + 2, cstrData.GetBuffer(0));
	LOG_INFO("发送报文 [%s]", strData.c_str());

	if (SendRecvSocket(szSendBuf, cstrData.GetLength() + 2, szRecvBuf, &iRecvLen, cstrErrMsg, cstrNeedQuery) != 0)
	{
		res.respCode = "XX";
		res.respMsg = cstrErrMsg.GetBuffer(0);
		res.needQuery = cstrNeedQuery.GetBuffer(0);
		goto cleanUP;
	}

	try
	{
		BUFCLR(szSendBuf);
		Tools::DeleteCDATA((char *)szSendBuf, (char *)szRecvBuf + 2);
		cstrData = Tools::UTF8ToGBK((char *)szSendBuf);
		LOG_INFO("后台返回字符串 [%s]", cstrData.GetBuffer(0));
		xpack::xml::decode(cstrData.GetBuffer(0), response);
	}
	catch (exception &e)
	{
		LOG_ERROR("解析返回XML失败 %s", e.what());
		res.respCode = "XX";
		res.respMsg = "解析返回XML失败";
		res.needQuery = "Y";
		goto cleanUP;
	}
	

	if (response.status.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_query_getmsg(response);
		goto cleanUP;
	}
	else if (response.result_code.length() > 0 && response.result_code.compare("0") != 0)
	{
		if (response.err_code.compare("SYSTEMERROR") == 0 ||
			response.err_code.compare("Internal error") == 0 ||
			response.err_code.compare("BANKERROR") == 0 ||
			response.err_code.compare("10003") == 0 ||
			response.err_code.compare("USERPAYING") == 0 ||
			response.err_code.compare("System error") == 0 ||
			response.err_code.compare("aop.ACQ.SYSTEM_ERROR") == 0 ||
			response.err_code.compare("ACQ.SYSTEM_ERROR") == 0
			)
		{
			res.respCode = "EE";
			res.needQuery = "Y";
			res.respMsg = trade_query_getmsg(response);
			goto cleanUP;
		}
		else if (response.err_code.compare("04") == 0 ||
			response.err_code.compare("06") == 0
			)
		{
			res.respCode = "EE";
			res.needQuery = "Y";
			res.respMsg = trade_query_getmsg(response);
			goto cleanUP;
		}
		else if (response.err_code.compare("00") == 0 && response.trade_type.compare("pay.unionpay.micropay") == 0)
		{
			res.respCode = "00";
			res.needQuery = "N";
			res.outTradeNo = response.out_trade_no;
			res.transactionId = response.transaction_id;
			res.outTransactionId = response.out_transaction_id;
			res.tradeType = response.trade_type;
			res.bankBillNo = response.bank_billno;
			res.dateTime = response.time_end;
			res.amt = response.total_fee;
			goto cleanUP;
		}
		else
		{
			res.respCode = "EE";
			res.needQuery = "N";
			res.respMsg = trade_query_getmsg(response);
			goto cleanUP;
		}
	}
	else if (response.trade_state.compare("USERPAYING") == 0)
	{
		res.respCode = "EE";
		res.needQuery = "Y";
		res.respMsg = trade_query_getmsg(response);
		goto cleanUP;
	}
	else if (response.trade_state.compare("SUCCESS") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_query_getmsg(response);
		goto cleanUP;
	}
	else
	{
		res.respCode = "00";
		res.needQuery = "N";
		res.outTradeNo = response.out_trade_no;
		res.transactionId = response.transaction_id;
		res.outTransactionId = response.out_transaction_id;
		res.tradeType = response.trade_type;
		res.bankBillNo = response.bank_billno;
		res.dateTime = response.time_end;
		res.amt = response.total_fee;
		goto cleanUP;
	}

cleanUP:
	if (res.outTradeNo.length() == 0)
	{
		res.outTradeNo = req.outTradeNo;
	}
	if (res.transactionId.length() == 0)
	{
		res.transactionId = req.transactionId;
	}
	return;
}

string trade_refund_getmsg(trade_refund_response res)
{
	if (res.err_msg.length() > 0)
	{
		return res.err_msg;
	}
	else if (res.message.length() > 0)
	{
		return res.message;
	}
	else
	{
		return "未知返回信息";
	}
}

void trade_refund(SCANTRANS_REQUEST req, SCANTRANS_RESPONSE &res)
{
	CString cstrData, cstrErrMsg, cstrNeedQuery;
	int fdSocket = 0;
	char szDateTime[30 + 1];
	BUFCLR(szDateTime);
	trade_refund_request request;
	trade_refund_response response;
	unsigned char szSendBuf[SOCKETDATELEN], szRecvBuf[SOCKETDATELEN];
	string strData;
	int iRecvLen;

	request.service = SCAN_REFUND;
#ifdef DEBUG_MD5
	request.sign_type = "MD5";
#else 
	request.device_info = "EPOS";
	request.sign_type = RSA_SIGNTYPE;
	request.sn_id = config.szVN;
	request.sn_id.append(config.szSN);
#endif // DEBUG_MD5

	request.mch_id = config.szMERNO;
	Tools::GetDateTime(szDateTime);
	request.nonce_str = szDateTime;

	request.total_fee = req.amt;
	request.out_trade_no = req.outTradeNo;
	request.transaction_id = req.transactionId;
	request.out_refund_no = req.outRefundNo;
	request.refund_fee = req.refundAmt;
	request.op_user_id = config.szMERNO;

	strData = xpack::json::encode(request);
	makeSign(strData, request.sign);
	BUFCLR(szSendBuf);
	BUFCLR(szRecvBuf);
	strData = xpack::xml::encode(request, "XML", 0, 2, ' ');
	cstrData = Tools::GBKToUTF8(strData.c_str());
	szSendBuf[0] = cstrData.GetLength() / 256;
	szSendBuf[1] = cstrData.GetLength() % 256;
	strcpy((char *)szSendBuf + 2, cstrData.GetBuffer(0));
	LOG_INFO("发送报文 [%s]", strData.c_str());

	if (SendRecvSocket(szSendBuf, cstrData.GetLength() + 2, szRecvBuf, &iRecvLen, cstrErrMsg, cstrNeedQuery) != 0)
	{
		res.respCode = "XX";
		res.respMsg = cstrErrMsg.GetBuffer(0);
		res.needQuery = cstrNeedQuery.GetBuffer(0);
		goto cleanUP;
	}

	try
	{
		BUFCLR(szSendBuf);
		Tools::DeleteCDATA((char *)szSendBuf, (char *)szRecvBuf + 2);
		cstrData = Tools::UTF8ToGBK((char *)szSendBuf);
		LOG_INFO("后台返回字符串 [%s]", cstrData.GetBuffer(0));
		xpack::xml::decode(cstrData.GetBuffer(0), response);
	}
	catch (exception &e)
	{
		LOG_ERROR("解析返回XML失败 %s", e.what());
		res.respCode = "XX";
		res.respMsg = "解析返回XML失败";
		res.needQuery = "Y";
		goto cleanUP;
	}


	if (response.status.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_refund_getmsg(response);
		goto cleanUP;
	}
	else if (response.result_code.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_refund_getmsg(response);
		goto cleanUP;
	}
	else 
	{
		res.respCode = "00";
		res.needQuery = "N";
		res.outTradeNo = response.out_trade_no;
		res.transactionId = response.transaction_id;
		res.outRefundNo = response.out_refund_no;
		res.refundId = response.refund_id;
		res.refundAmt = response.refund_fee;
		res.tradeType = response.trade_type;
		goto cleanUP;
	}

cleanUP:
	return;
}

string trade_refundquery_getmsg(trade_refundquery_response res)
{
	if (res.message.length() > 0)
	{
		return res.message;
	}
	else 
	{
		if (res.refund_status_0.compare("SUCCESS") == 0)
		{
			return "[SUCCESS]-退款成功";
		}
		else if (res.refund_status_0.compare("FAIL") == 0)
		{
			return "[FAIL]-退款失败";
		}
		else if (res.refund_status_0.compare("PROCESSING") == 0)
		{
			return "[PROCESSING]-退款处理中";
		}
		else if (res.refund_status_0.compare("NOTSURE") == 0)
		{
			return "[NOTSURE]-未确定, 需要商户原退款单号重新发起";
		}
		else if (res.refund_status_0.compare("CHANGE") == 0)
		{
			return "[CHANGE]-转入代发, 请通过线下或者平台转账的方式进行退款";
		}
		else
		{
			return "未知返回信息";
		}
	}
}

void trade_refundquery(SCANTRANS_REQUEST req, SCANTRANS_RESPONSE &res)
{
	CString cstrData, cstrErrMsg, cstrNeedQuery;
	int fdSocket = 0;
	char szDateTime[30 + 1];
	BUFCLR(szDateTime);
	trade_refundquery_request request;
	trade_refundquery_response response;
	unsigned char szSendBuf[SOCKETDATELEN], szRecvBuf[SOCKETDATELEN];
	string strData;
	int iRecvLen;

	request.service = SCAN_REFUNDQUERY;
#ifdef DEBUG_MD5
	request.sign_type = "MD5";
#else 
	request.device_info = "EPOS";
	request.sign_type = RSA_SIGNTYPE;
	request.sn_id = config.szVN;
	request.sn_id.append(config.szSN);
#endif // DEBUG_MD5

	request.mch_id = config.szMERNO;
	Tools::GetDateTime(szDateTime);
	request.nonce_str = szDateTime;

// 	request.out_trade_no = req.outTradeNo;
// 	request.transaction_id = req.transactionId;
	request.out_refund_no = req.outRefundNo;
	request.refund_id = req.refundId;

	strData = xpack::json::encode(request);
	makeSign(strData, request.sign);
	BUFCLR(szSendBuf);
	BUFCLR(szRecvBuf);
	strData = xpack::xml::encode(request, "XML", 0, 2, ' ');
	cstrData = Tools::GBKToUTF8(strData.c_str());
	szSendBuf[0] = cstrData.GetLength() / 256;
	szSendBuf[1] = cstrData.GetLength() % 256;
	strcpy((char *)szSendBuf + 2, cstrData.GetBuffer(0));
	LOG_INFO("发送报文 [%s]", strData.c_str());

	if (SendRecvSocket(szSendBuf, cstrData.GetLength() + 2, szRecvBuf, &iRecvLen, cstrErrMsg, cstrNeedQuery) != 0)
	{
		res.respCode = "XX";
		res.respMsg = cstrErrMsg.GetBuffer(0);
		res.needQuery = cstrNeedQuery.GetBuffer(0);
		goto cleanUP;
	}

	try
	{
		BUFCLR(szSendBuf);
		Tools::DeleteCDATA((char *)szSendBuf, (char *)szRecvBuf + 2);
		cstrData = Tools::UTF8ToGBK((char *)szSendBuf);
		LOG_INFO("后台返回字符串 [%s]", cstrData.GetBuffer(0));
		xpack::xml::decode(cstrData.GetBuffer(0), response);
	}
	catch (exception &e)
	{
		LOG_ERROR("解析返回XML失败 %s", e.what());
		res.respCode = "XX";
		res.respMsg = "解析返回XML失败";
		res.needQuery = "Y";
		goto cleanUP;
	}


	if (response.status.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_refundquery_getmsg(response);
		goto cleanUP;
	}
	else if (response.code.length() > 0 && response.code.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_refundquery_getmsg(response);
		goto cleanUP;
	}
	else if (response.result_code.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_refundquery_getmsg(response);
		goto cleanUP;
	}
	else if (response.refund_status_0.compare("SUCCESS") != 0 &&
		response.refund_status_0.compare("PROCESSING") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_refundquery_getmsg(response);
		goto cleanUP;
	}
	else
	{
		res.respCode = "00";
		res.needQuery = "N";
		res.respMsg = trade_refundquery_getmsg(response);
		res.outTradeNo = response.out_trade_no;
		res.transactionId = response.transaction_id;
		res.outRefundNo = response.out_refund_no_0;
		res.refundId = response.refund_id_0;
		res.refundAmt = response.refund_fee_0;
		res.tradeType = response.trade_type;
		res.dateTime = response.refund_time_0;
		goto cleanUP;
	}

cleanUP:
	return;
}

string trade_close_getmsg(trade_close_response res)
{
	if (res.err_msg.length() > 0)
	{
		return res.err_msg;
	}
	else if (res.message.length() > 0)
	{
		return res.message;
	}
	else
	{
		return "";
	}
}

// start	[2023/4/4]
// 威富通渠道
void trade_micropay_wft(SCANTRANS_REQUEST req, SCANTRANS_RESPONSE &res)
{
	CString cstrData, cstrErrMsg, cstrNeedQuery;
	int fdSocket = 0;
	char szDateTime[30 + 1];
	BUFCLR(szDateTime);
	trade_micropay_request request;
	trade_micropay_response response;
	unsigned char szSendBuf[SOCKETDATELEN], szRecvBuf[SOCKETDATELEN];
	string strData;
	int iRecvLen;

	request.service = SCAN_PAY;
	request.mch_create_ip = "127.0.0.1";
	request.sign_type = "MD5";
	// #ifdef DEBUG_MD5
	// 	request.sign_type = "MD5";
	// #else 
	// 	// 	request.op_device_id = config.szSN;
	// 	request.device_info = "EPOS";
	// 	request.sign_type = RSA_SIGNTYPE;
	// 	request.sn_id = config.szVN;
	// 	request.sn_id.append(config.szSN);
	// #endif // DEBUG_MD5

	request.mch_id = config.szMERNO;
	if (strlen(config.szDevice) > 0)
	{
		request.op_user_id = config.szDevice;
	}
	Tools::GetDateTime(szDateTime);
	request.nonce_str = szDateTime;

	request.out_trade_no = req.outTradeNo;
	request.body = req.body;
	request.total_fee = req.amt;
	request.auth_code = req.authCode;

	strData = xpack::json::encode(request);
	makeSign_wft(strData, request.sign);


	BUFCLR(szSendBuf);
	BUFCLR(szRecvBuf);
	strData = xpack::xml::encode(request, "XML", 0, 2, ' ');
	cstrData = Tools::GBKToUTF8(strData.c_str());
	szSendBuf[0] = cstrData.GetLength() / 256;
	szSendBuf[1] = cstrData.GetLength() % 256;
	strcpy((char *)szSendBuf + 2, cstrData.GetBuffer(0));
	LOG_INFO("发送报文 [%s]", strData.c_str());

	if (SendRecvSocket(szSendBuf, cstrData.GetLength() + 2, szRecvBuf, &iRecvLen, cstrErrMsg, cstrNeedQuery) != 0)
	{
		res.respCode = "XX";
		res.respMsg = cstrErrMsg.GetBuffer(0);
		res.needQuery = cstrNeedQuery.GetBuffer(0);
		goto cleanUP;
	}

	try
	{
		BUFCLR(szSendBuf);
		Tools::DeleteCDATA((char *)szSendBuf, (char *)szRecvBuf + 2);
		cstrData = Tools::UTF8ToGBK((char *)szSendBuf);
		LOG_INFO("后台返回字符串 [%s]", cstrData.GetBuffer(0));
		xpack::xml::decode(cstrData.GetBuffer(0), response);
	}
	catch (exception &e)
	{
		LOG_ERROR("解析返回XML失败 %s", e.what());
		res.respCode = "XX";
		res.respMsg = "解析返回XML失败";
		res.needQuery = "Y";
		goto cleanUP;
	}

	if (response.need_query.compare("Y") == 0)
	{
		res.respCode = "EE";
		res.needQuery = "Y";
		res.respMsg = trade_micropay_getmsg(response);
		goto cleanUP;
	}
	else if (response.status.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_micropay_getmsg(response);
		goto cleanUP;
	}
	else if (response.result_code.compare("0") != 0)
	{
		if (response.err_code.compare("SYSTEMERROR") == 0 ||
			response.err_code.compare("Internal error") == 0 ||
			response.err_code.compare("BANKERROR") == 0 ||
			response.err_code.compare("10003") == 0 ||
			response.err_code.compare("USERPAYING") == 0 ||
			response.err_code.compare("System error") == 0 ||
			response.err_code.compare("aop.ACQ.SYSTEM_ERROR") == 0 ||
			response.err_code.compare("ACQ.SYSTEM_ERROR") == 0 ||
			response.err_code.compare("Unknow error") == 0 ||
			response.err_code.compare("UNKNOWN_TRANSACTION_STATUS") == 0 ||
			response.err_code.compare("ORDER_PAY_PROCESSING") == 0
			)
		{
			res.respCode = "EE";
			res.needQuery = "Y";
			res.respMsg = trade_micropay_getmsg(response);
			goto cleanUP;
		}
		else if (response.err_code.compare("04") == 0 ||
			response.err_code.compare("06") == 0
			)
		{
			res.respCode = "EE";
			res.needQuery = "Y";
			res.respMsg = trade_micropay_getmsg(response);
			goto cleanUP;
		}
		else if (response.err_code.compare("00") == 0 && response.trade_type.compare("pay.unionpay.micropay") == 0)
		{
			res.respCode = "00";
			res.needQuery = "N";
			res.outTradeNo = response.out_trade_no;
			res.transactionId = response.transaction_id;
			res.outTransactionId = response.out_transaction_id;
			res.tradeType = response.trade_type;
			res.bankBillNo = response.bank_billno;
			res.dateTime = response.time_end;
			res.amt = response.total_fee;
			goto cleanUP;
		}
		else
		{
			res.respCode = "EE";
			res.needQuery = "N";
			res.respMsg = trade_micropay_getmsg(response);
			goto cleanUP;
		}
	}
	else if (response.pay_result.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_micropay_getmsg(response);
		goto cleanUP;
	}
	else
	{
		res.respCode = "00";
		res.needQuery = "N";
		res.outTradeNo = response.out_trade_no;
		res.transactionId = response.transaction_id;
		res.outTransactionId = response.out_transaction_id;
		res.tradeType = response.trade_type;
		res.bankBillNo = response.bank_billno;
		res.dateTime = response.time_end;
		res.amt = response.total_fee;
		goto cleanUP;
	}



cleanUP:
	if (res.outTradeNo.length() == 0)
	{
		res.outTradeNo = req.outTradeNo;
	}
}
void trade_query_wft(SCANTRANS_REQUEST req, SCANTRANS_RESPONSE &res)
{
	CString cstrData, cstrErrMsg, cstrNeedQuery;
	int fdSocket = 0;
	char szDateTime[30 + 1];
	BUFCLR(szDateTime);
	trade_query_request request;
	trade_query_response response;
	unsigned char szSendBuf[SOCKETDATELEN], szRecvBuf[SOCKETDATELEN];
	string strData;
	int iRecvLen;

	request.service = SCAN_PAYQUERY;
	request.sign_type = "MD5";
	request.mch_id = config.szMERNO;
	Tools::GetDateTime(szDateTime);
	request.nonce_str = szDateTime;

	request.out_trade_no = req.outTradeNo;
	request.transaction_id = req.transactionId;

	strData = xpack::json::encode(request);
	makeSign_wft(strData, request.sign);
	BUFCLR(szSendBuf);
	BUFCLR(szRecvBuf);
	strData = xpack::xml::encode(request, "XML", 0, 2, ' ');
	cstrData = Tools::GBKToUTF8(strData.c_str());
	szSendBuf[0] = cstrData.GetLength() / 256;
	szSendBuf[1] = cstrData.GetLength() % 256;
	strcpy((char *)szSendBuf + 2, cstrData.GetBuffer(0));
	LOG_INFO("发送报文 [%s]", strData.c_str());

	if (SendRecvSocket(szSendBuf, cstrData.GetLength() + 2, szRecvBuf, &iRecvLen, cstrErrMsg, cstrNeedQuery) != 0)
	{
		res.respCode = "XX";
		res.respMsg = cstrErrMsg.GetBuffer(0);
		res.needQuery = cstrNeedQuery.GetBuffer(0);
		goto cleanUP;
	}

	try
	{
		BUFCLR(szSendBuf);
		Tools::DeleteCDATA((char *)szSendBuf, (char *)szRecvBuf + 2);
		cstrData = Tools::UTF8ToGBK((char *)szSendBuf);
		LOG_INFO("后台返回字符串 [%s]", cstrData.GetBuffer(0));
		xpack::xml::decode(cstrData.GetBuffer(0), response);
	}
	catch (exception &e)
	{
		LOG_ERROR("解析返回XML失败 %s", e.what());
		res.respCode = "XX";
		res.respMsg = "解析返回XML失败";
		res.needQuery = "Y";
		goto cleanUP;
	}


	if (response.status.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_query_getmsg(response);
		goto cleanUP;
	}
	else if (response.result_code.length() > 0 && response.result_code.compare("0") != 0)
	{
		if (response.err_code.compare("SYSTEMERROR") == 0 ||
			response.err_code.compare("Internal error") == 0 ||
			response.err_code.compare("BANKERROR") == 0 ||
			response.err_code.compare("10003") == 0 ||
			response.err_code.compare("USERPAYING") == 0 ||
			response.err_code.compare("System error") == 0 ||
			response.err_code.compare("aop.ACQ.SYSTEM_ERROR") == 0 ||
			response.err_code.compare("ACQ.SYSTEM_ERROR") == 0
			)
		{
			res.respCode = "EE";
			res.needQuery = "Y";
			res.respMsg = trade_query_getmsg(response);
			goto cleanUP;
		}
		else if (response.err_code.compare("04") == 0 ||
			response.err_code.compare("06") == 0
			)
		{
			res.respCode = "EE";
			res.needQuery = "Y";
			res.respMsg = trade_query_getmsg(response);
			goto cleanUP;
		}
		else if (response.err_code.compare("00") == 0 && response.trade_type.compare("pay.unionpay.micropay") == 0)
		{
			res.respCode = "00";
			res.needQuery = "N";
			res.outTradeNo = response.out_trade_no;
			res.transactionId = response.transaction_id;
			res.outTransactionId = response.out_transaction_id;
			res.tradeType = response.trade_type;
			res.bankBillNo = response.bank_billno;
			res.dateTime = response.time_end;
			res.amt = response.total_fee;
			goto cleanUP;
		}
		else
		{
			res.respCode = "EE";
			res.needQuery = "N";
			res.respMsg = trade_query_getmsg(response);
			goto cleanUP;
		}
	}
	else if (response.trade_state.compare("USERPAYING") == 0)
	{
		res.respCode = "EE";
		res.needQuery = "Y";
		res.respMsg = trade_query_getmsg(response);
		goto cleanUP;
	}
	else if (response.trade_state.compare("SUCCESS") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_query_getmsg(response);
		goto cleanUP;
	}
	else
	{
		res.respCode = "00";
		res.needQuery = "N";
		res.outTradeNo = response.out_trade_no;
		res.transactionId = response.transaction_id;
		res.outTransactionId = response.out_transaction_id;
		res.tradeType = response.trade_type;
		res.bankBillNo = response.bank_billno;
		res.dateTime = response.time_end;
		res.amt = response.total_fee;
		goto cleanUP;
	}

cleanUP:
	if (res.outTradeNo.length() == 0)
	{
		res.outTradeNo = req.outTradeNo;
	}
	if (res.transactionId.length() == 0)
	{
		res.transactionId = req.transactionId;
	}
	return;
}
void trade_refund_wft(SCANTRANS_REQUEST req, SCANTRANS_RESPONSE &res)
{
	CString cstrData, cstrErrMsg, cstrNeedQuery;
	int fdSocket = 0;
	char szDateTime[30 + 1];
	BUFCLR(szDateTime);
	trade_refund_request request;
	trade_refund_response response;
	unsigned char szSendBuf[SOCKETDATELEN], szRecvBuf[SOCKETDATELEN];
	string strData;
	int iRecvLen;

	request.service = SCAN_REFUND;
	request.sign_type = "MD5";
	request.mch_id = config.szMERNO;
	if (strlen(config.szDevice) > 0)
	{
// 		request.device_info = config.szDevice;
		request.op_user_id = config.szDevice;
	}
	Tools::GetDateTime(szDateTime);
	request.nonce_str = szDateTime;

	request.total_fee = req.amt;
	request.out_trade_no = req.outTradeNo;
	request.transaction_id = req.transactionId;
	request.out_refund_no = req.outRefundNo;
	request.refund_fee = req.refundAmt;

	strData = xpack::json::encode(request);
	makeSign_wft(strData, request.sign);
	BUFCLR(szSendBuf);
	BUFCLR(szRecvBuf);
	strData = xpack::xml::encode(request, "XML", 0, 2, ' ');
	cstrData = Tools::GBKToUTF8(strData.c_str());
	szSendBuf[0] = cstrData.GetLength() / 256;
	szSendBuf[1] = cstrData.GetLength() % 256;
	strcpy((char *)szSendBuf + 2, cstrData.GetBuffer(0));
	LOG_INFO("发送报文 [%s]", strData.c_str());

	if (SendRecvSocket(szSendBuf, cstrData.GetLength() + 2, szRecvBuf, &iRecvLen, cstrErrMsg, cstrNeedQuery) != 0)
	{
		res.respCode = "XX";
		res.respMsg = cstrErrMsg.GetBuffer(0);
		res.needQuery = cstrNeedQuery.GetBuffer(0);
		goto cleanUP;
	}

	try
	{
		BUFCLR(szSendBuf);
		Tools::DeleteCDATA((char *)szSendBuf, (char *)szRecvBuf + 2);
		cstrData = Tools::UTF8ToGBK((char *)szSendBuf);
		LOG_INFO("后台返回字符串 [%s]", cstrData.GetBuffer(0));
		xpack::xml::decode(cstrData.GetBuffer(0), response);
	}
	catch (exception &e)
	{
		LOG_ERROR("解析返回XML失败 %s", e.what());
		res.respCode = "XX";
		res.respMsg = "解析返回XML失败";
		res.needQuery = "Y";
		goto cleanUP;
	}


	if (response.status.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_refund_getmsg(response);
		goto cleanUP;
	}
	else if (response.result_code.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_refund_getmsg(response);
		goto cleanUP;
	}
	else
	{
		res.respCode = "00";
		res.needQuery = "N";
		res.outTradeNo = response.out_trade_no;
		res.transactionId = response.transaction_id;
		res.outRefundNo = response.out_refund_no;
		res.refundId = response.refund_id;
		res.refundAmt = response.refund_fee;
		res.tradeType = response.trade_type;
		goto cleanUP;
	}

cleanUP:
	return;
}
void trade_refundquery_wft(SCANTRANS_REQUEST req, SCANTRANS_RESPONSE &res)
{
	CString cstrData, cstrErrMsg, cstrNeedQuery;
	int fdSocket = 0;
	char szDateTime[30 + 1];
	BUFCLR(szDateTime);
	trade_refundquery_request request;
	trade_refundquery_response response;
	unsigned char szSendBuf[SOCKETDATELEN], szRecvBuf[SOCKETDATELEN];
	string strData;
	int iRecvLen;

	request.service = SCAN_REFUNDQUERY;
	request.sign_type = "MD5";
	request.mch_id = config.szMERNO;
	Tools::GetDateTime(szDateTime);
	request.nonce_str = szDateTime;

	// 	request.out_trade_no = req.outTradeNo;
	// 	request.transaction_id = req.transactionId;
	request.out_refund_no = req.outRefundNo;
	request.refund_id = req.refundId;

	strData = xpack::json::encode(request);
	makeSign_wft(strData, request.sign);
	BUFCLR(szSendBuf);
	BUFCLR(szRecvBuf);
	strData = xpack::xml::encode(request, "XML", 0, 2, ' ');
	cstrData = Tools::GBKToUTF8(strData.c_str());
	szSendBuf[0] = cstrData.GetLength() / 256;
	szSendBuf[1] = cstrData.GetLength() % 256;
	strcpy((char *)szSendBuf + 2, cstrData.GetBuffer(0));
	LOG_INFO("发送报文 [%s]", strData.c_str());

	if (SendRecvSocket(szSendBuf, cstrData.GetLength() + 2, szRecvBuf, &iRecvLen, cstrErrMsg, cstrNeedQuery) != 0)
	{
		res.respCode = "XX";
		res.respMsg = cstrErrMsg.GetBuffer(0);
		res.needQuery = cstrNeedQuery.GetBuffer(0);
		goto cleanUP;
	}

	try
	{
		BUFCLR(szSendBuf);
		Tools::DeleteCDATA((char *)szSendBuf, (char *)szRecvBuf + 2);
		cstrData = Tools::UTF8ToGBK((char *)szSendBuf);
		LOG_INFO("后台返回字符串 [%s]", cstrData.GetBuffer(0));
		xpack::xml::decode(cstrData.GetBuffer(0), response);
	}
	catch (exception &e)
	{
		LOG_ERROR("解析返回XML失败 %s", e.what());
		res.respCode = "XX";
		res.respMsg = "解析返回XML失败";
		res.needQuery = "Y";
		goto cleanUP;
	}


	if (response.status.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_refundquery_getmsg(response);
		goto cleanUP;
	}
	else if (response.code.length() > 0 && response.code.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_refundquery_getmsg(response);
		goto cleanUP;
	}
	else if (response.result_code.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_refundquery_getmsg(response);
		goto cleanUP;
	}
	else if (response.refund_status_0.compare("SUCCESS") != 0 &&
		response.refund_status_0.compare("PROCESSING") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_refundquery_getmsg(response);
		goto cleanUP;
	}
	else
	{
		res.respCode = "00";
		res.needQuery = "N";
		res.respMsg = trade_refundquery_getmsg(response);
		res.outTradeNo = response.out_trade_no;
		res.transactionId = response.transaction_id;
		res.outRefundNo = response.out_refund_no_0;
		res.refundId = response.refund_id_0;
		res.refundAmt = response.refund_fee_0;
		res.tradeType = response.trade_type;
		res.dateTime = response.refund_time_0;
		goto cleanUP;
	}

cleanUP:
	return;
}
void trade_close_wft(SCANTRANS_REQUEST req, SCANTRANS_RESPONSE &res)
{
	CString cstrData, cstrErrMsg, cstrNeedQuery;
	int fdSocket = 0;
	char szDateTime[30 + 1];
	BUFCLR(szDateTime);
	trade_close_request request;
	trade_close_response response;
	unsigned char szSendBuf[SOCKETDATELEN], szRecvBuf[SOCKETDATELEN];
	string strData;
	int iRecvLen;

	request.service = SCAN_CLOSE;
	request.sign_type = "MD5";
	request.mch_id = config.szMERNO;
	Tools::GetDateTime(szDateTime);
	request.nonce_str = szDateTime;

	// 	request.out_trade_no = req.outTradeNo;
	// 	request.transaction_id = req.transactionId;
	request.out_trade_no = req.outTradeNo;

	strData = xpack::json::encode(request);
	makeSign_wft(strData, request.sign);
	BUFCLR(szSendBuf);
	BUFCLR(szRecvBuf);
	strData = xpack::xml::encode(request, "XML", 0, 2, ' ');
	cstrData = Tools::GBKToUTF8(strData.c_str());
	szSendBuf[0] = cstrData.GetLength() / 256;
	szSendBuf[1] = cstrData.GetLength() % 256;
	strcpy((char *)szSendBuf + 2, cstrData.GetBuffer(0));
	LOG_INFO("发送报文 [%s]", strData.c_str());

	if (SendRecvSocket(szSendBuf, cstrData.GetLength() + 2, szRecvBuf, &iRecvLen, cstrErrMsg, cstrNeedQuery) != 0)
	{
		res.respCode = "XX";
		res.respMsg = cstrErrMsg.GetBuffer(0);
		res.needQuery = cstrNeedQuery.GetBuffer(0);
		goto cleanUP;
	}

	try
	{
		BUFCLR(szSendBuf);
		Tools::DeleteCDATA((char *)szSendBuf, (char *)szRecvBuf + 2);
		cstrData = Tools::UTF8ToGBK((char *)szSendBuf);
		LOG_INFO("后台返回字符串 [%s]", cstrData.GetBuffer(0));
		xpack::xml::decode(cstrData.GetBuffer(0), response);
	}
	catch (exception &e)
	{
		LOG_ERROR("解析返回XML失败 %s", e.what());
		res.respCode = "XX";
		res.respMsg = "解析返回XML失败";
		res.needQuery = "Y";
		goto cleanUP;
	}


	if (response.status.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_close_getmsg(response);
		goto cleanUP;
	}
	else if (response.code.length() > 0 && response.code.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_close_getmsg(response);
		goto cleanUP;
	}
	else if (response.result_code.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_close_getmsg(response);
		goto cleanUP;
	}
	else 
	{
		res.respCode = "00";
		res.needQuery = "N";
		res.respMsg = trade_close_getmsg(response);
		goto cleanUP;
	}

cleanUP:
	return;
}
void trade_reverse_wft(SCANTRANS_REQUEST req, SCANTRANS_RESPONSE &res)
{
	CString cstrData, cstrErrMsg, cstrNeedQuery;
	int fdSocket = 0;
	char szDateTime[30 + 1];
	BUFCLR(szDateTime);
	trade_close_request request;
	trade_close_response response;
	unsigned char szSendBuf[SOCKETDATELEN], szRecvBuf[SOCKETDATELEN];
	string strData;
	int iRecvLen;

	request.service = SCAN_REVERSE;
	request.sign_type = "MD5";
	request.mch_id = config.szMERNO;
	Tools::GetDateTime(szDateTime);
	request.nonce_str = szDateTime;

	// 	request.out_trade_no = req.outTradeNo;
	// 	request.transaction_id = req.transactionId;
	request.out_trade_no = req.outTradeNo;

	strData = xpack::json::encode(request);
	makeSign_wft(strData, request.sign);
	BUFCLR(szSendBuf);
	BUFCLR(szRecvBuf);
	strData = xpack::xml::encode(request, "XML", 0, 2, ' ');
	cstrData = Tools::GBKToUTF8(strData.c_str());
	szSendBuf[0] = cstrData.GetLength() / 256;
	szSendBuf[1] = cstrData.GetLength() % 256;
	strcpy((char *)szSendBuf + 2, cstrData.GetBuffer(0));
	LOG_INFO("发送报文 [%s]", strData.c_str());

	if (SendRecvSocket(szSendBuf, cstrData.GetLength() + 2, szRecvBuf, &iRecvLen, cstrErrMsg, cstrNeedQuery) != 0)
	{
		res.respCode = "XX";
		res.respMsg = cstrErrMsg.GetBuffer(0);
		res.needQuery = cstrNeedQuery.GetBuffer(0);
		goto cleanUP;
	}

	try
	{
		BUFCLR(szSendBuf);
		Tools::DeleteCDATA((char *)szSendBuf, (char *)szRecvBuf + 2);
		cstrData = Tools::UTF8ToGBK((char *)szSendBuf);
		LOG_INFO("后台返回字符串 [%s]", cstrData.GetBuffer(0));
		xpack::xml::decode(cstrData.GetBuffer(0), response);
	}
	catch (exception &e)
	{
		LOG_ERROR("解析返回XML失败 %s", e.what());
		res.respCode = "XX";
		res.respMsg = "解析返回XML失败";
		res.needQuery = "Y";
		goto cleanUP;
	}


	if (response.status.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_close_getmsg(response);
		goto cleanUP;
	}
	else if (response.code.length() > 0 && response.code.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_close_getmsg(response);
		goto cleanUP;
	}
	else if (response.result_code.compare("0") != 0)
	{
		res.respCode = "EE";
		res.needQuery = "N";
		res.respMsg = trade_close_getmsg(response);
		goto cleanUP;
	}
	else
	{
		res.respCode = "00";
		res.needQuery = "N";
		res.respMsg = trade_close_getmsg(response);
		goto cleanUP;
	}

cleanUP:
	return;
}
// end		[2023/4/4]
void checkRequest(SCANTRANS_REQUEST req)
{
	if (config.iScanType != 1 && config.iScanType != 2)
	{
		throw std::runtime_error("配置文件扫码渠道有误");
	}
	if (req.transName.compare(SCAN_PAY) == 0)
	{
		if (req.authCode.length() == 0)
		{
			throw std::runtime_error("未传入付款码");
		}
		else if (req.amt.length() == 0)
		{
			throw std::runtime_error("未传入金额");
		}
		else if (!Tools::IsDigit2(req.amt))
		{
			throw std::runtime_error("金额格式有误");
		}
		else if (req.outTradeNo.length() == 0)
		{
			throw std::runtime_error("未传入商户订单号");
		}
		else if (req.body.length() == 0)
		{
			throw std::runtime_error("未传入商品信息");
		}
	}
	else if (req.transName.compare(SCAN_PAYQUERY) == 0)
	{
		if (req.outTradeNo.length() == 0 && req.transactionId.length() == 0)
		{
			throw std::runtime_error("商户订单号与平台订单号必须传入其中之一");
		}
	}
	else if (req.transName.compare(SCAN_REFUND) == 0)
	{
		if (req.outTradeNo.length() == 0 && req.transactionId.length() == 0)
		{
			throw std::runtime_error("商户订单号与平台订单号必须传入其中之一");
		}
		else if (req.outRefundNo.length() == 0)
		{
			throw std::runtime_error("未传入商户退货订单号");
		}
		else if (req.refundAmt.length() == 0)
		{
			throw std::runtime_error("未传入退货金额");
		}
		else if (!Tools::IsDigit2(req.refundAmt))
		{
			throw std::runtime_error("退货金额格式有误");
		}
		else if (req.amt.length() == 0)
		{
			throw std::runtime_error("未传入金额");
		}
		else if (!Tools::IsDigit2(req.amt))
		{
			throw std::runtime_error("金额格式有误");
		}
	}
	else if (req.transName.compare(SCAN_REFUNDQUERY) == 0)
	{
		if (req.outRefundNo.length() == 0 && req.refundId.length() == 0)
		{
			throw std::runtime_error("商户退货订单号与平台退款订单号必须传入其中之一");
		}
	}
	else if (req.transName.compare(SCAN_CLOSE) == 0 || req.transName.compare(SCAN_REVERSE) == 0)
	{
		if (req.outTradeNo.length() == 0)
		{
			throw std::runtime_error("未传入商户订单号");
		}
	}
	else
	{
		throw std::runtime_error("暂时不支持该交易");
	}
}

int __stdcall ScanTrans(char *inputData, char *outputData)
{
	SCANTRANS_REQUEST req;
	SCANTRANS_RESPONSE res;
	InitConfig();
	LOG_INFO("ScanTrans 传入参数[%s]", inputData);
	try
	{
		xpack::json::decode(inputData, req);
	}
	catch (exception &e)
	{
		LOG_ERROR("传入参数JSON解析失败 %s", e.what());
		res.respCode = "XX";
		res.respMsg = "传入参数JSON解析失败";
		res.needQuery = "N";
		goto cleanUP;
	}

	try
	{
		checkRequest(req);
	}
	catch (exception &e)
	{
		LOG_ERROR("传入参数检查有误 %s", e.what());
		res.respCode = "XX";
		res.respMsg = e.what();
		res.needQuery = "N";
		goto cleanUP;
	}

	if (req.transName.compare(SCAN_PAY) == 0)
	{
		if (config.iScanType == 1)
		{
			trade_micropay(req, res);
		}
		else if (config.iScanType == 2)
		{
			trade_micropay_wft(req, res);
		}
	}
	else if (req.transName.compare(SCAN_PAYQUERY) == 0)
	{
		if (config.iScanType == 1)
		{
			trade_query(req, res);
		}
		else if (config.iScanType == 2)
		{
			trade_query_wft(req, res);
		}
	}
	else if (req.transName.compare(SCAN_REFUND) == 0)
	{
		if (config.iScanType == 1)
		{
			trade_refund(req, res);
		}
		else if (config.iScanType == 2)
		{
			trade_refund_wft(req, res);
		}
	}
	else if (req.transName.compare(SCAN_REFUNDQUERY) == 0)
	{
		if (config.iScanType == 1)
		{
			trade_refundquery(req, res);
		}
		else if (config.iScanType == 2)
		{
			trade_refundquery_wft(req, res);
		}
	}
	else if (req.transName.compare(SCAN_CLOSE) == 0)
	{
		if (config.iScanType == 1)
		{
			res.respCode = "XX";
			res.respMsg = "暂时不支持扫码关单";
			res.needQuery = "N";
			goto cleanUP;
		}
		else
		{
			trade_close_wft(req, res);
		}
	}
	else if (req.transName.compare(SCAN_REVERSE) == 0)
	{
		if (config.iScanType == 1)
		{
			res.respCode = "XX";
			res.respMsg = "暂时不支持扫码关单";
			res.needQuery = "N";
			goto cleanUP;
		}
		else
		{
			trade_reverse_wft(req, res);
		}
	}
	else
	{
		LOG_ERROR("暂时不支持该交易 [%s]", req.transName.c_str());
		res.respCode = "XX";
		res.respMsg = "暂时不支持该交易";
		res.needQuery = "N";
		goto cleanUP;
	}

cleanUP:
	string strData = xpack::json::encode(res);
	LOG_INFO("ScanTrans 传出参数[%s]", strData.c_str());
	strcpy(outputData, strData.c_str());
	return 0;
}


bool TestPos()
{
	unsigned char CmdStr[100];
	unsigned char recvbuf[1024];// 解决同步超时异常的错误
	int recvlen;
	BUFCLR(CmdStr);
	sprintf((char *)CmdStr, "%c%c%6.6s%s", 0x01, 0x01, "      ", "99");
	BUFCLR(recvbuf);
	recvlen = 0;
	sendtocom_RS232(CmdStr, 10);
	if (readfromcom_RS232(recvbuf, &recvlen, 2) == 0)
	{
		if (recvbuf[COM_HEAD] == ACK)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

int Read_Field(int iCount, char* str, int iStrLen, char *FieldData)
{
	int i, j;
	int iBeginPos, iEndPos;
	j = 1; iBeginPos = 0; iEndPos = 0;
	for (i = 0; i < iStrLen; i++)
	{
		if (str[i] == FS)
		{
			iBeginPos = iEndPos;
			iEndPos = i;
			if (j == iCount)
			{
				if (iBeginPos == 0)
				{
					memcpy(FieldData, str, iEndPos - iBeginPos - 1);
				}
				else
				{
					memcpy(FieldData, str + iBeginPos + 1, iEndPos - iBeginPos - 1);
				}
				return 0;
			}
			j++;
		}
	}

	return 0;
}

int SendRecvPosSocket(unsigned char *szSendBuf, int iSendLen, unsigned char *szRecvBuf, int *iRecvLen)
{
	CString strErrMsg;
	int iRet = -1;
	int fdSocket = 0;
	SSL_CTX* ctx = nullptr;
	SSL* ssl = nullptr;
	int recvLen;
	if (config.iPOSSSL == 1)
	{
		fdSocket = EI_iCreateClientSocketWithTimeout(config.szPOSIP, config.iPOSPort, AF_INET, SOCK_STREAM, config.iScanTimeout, 0, NULL);
		if (fdSocket <= 0)
		{
			strErrMsg.Format("链接 [%s:%d] 失败 [%d]", config.szPOSIP, config.iPOSPort, fdSocket);
			iRet = -1;
			goto cleanUP;
		}
		if (EI_iSetSocketRecvTimeout(fdSocket, config.iScanTimeout, 0) == -1)
		{
			strErrMsg.Format("链接 [%s:%d] 设置超时失败 [%d]", config.szPOSIP, config.iPOSPort, fdSocket);
			iRet = -1;
			goto cleanUP;
		}

		if (EI_iSetBlocking(fdSocket) == -1)
		{
			strErrMsg.Format("链接 [%s:%d] 设置模式失败 [%d]", config.szPOSIP, config.iPOSPort, fdSocket);
			iRet = -1;
			goto cleanUP;
		}

		ctx = SSL_CTX_new(SSLv23_client_method());
		ssl = SSL_new(ctx);
		SSL_set_fd(ssl, fdSocket);
		if (SSL_connect(ssl) != 1) {
			strErrMsg.Format("链接 [%s:%d] SSL模式链接失败 [%d]", config.szPOSIP, config.iPOSPort, fdSocket);
			iRet = -1;
			goto cleanUP;
		}

		iRet = SSL_write(ssl, (char *)szSendBuf, iSendLen);
		if (iRet != 0)
		{
			strErrMsg.Format("发送报文 [%s:%d] 失败 [%d]", config.szPOSIP, config.iPOSPort, iRet);
			iRet = -1;
			goto cleanUP;
		}

		iRet = SSL_read(ssl, szRecvBuf, 2);
		if (iRet != 2)
		{
			strErrMsg.Format("接收报文长度[%s:%d] 失败[%d]", config.szPOSIP, config.iPOSPort, iRet);
			iRet = -2;
			goto cleanUP;
		}
		recvLen = szRecvBuf[0] * 256 + szRecvBuf[1];
		LOG_DEBUG("计算长度为[%d]", recvLen);
		iRet = SSL_read(ssl, (char *)szRecvBuf + 2, recvLen);
		if (iRet != recvLen)
		{
			LOG_ERRORSB((char *)szRecvBuf, recvLen + 2, "接收报文内容[%s:%d] 失败[%d]", config.szPOSIP, config.iPOSPort, iRet);
			strErrMsg.Format("接收报文内容[%s:%d] 失败[%d]", config.szPOSIP, config.iPOSPort, iRet);
			iRet = -2;
			goto cleanUP;
		}
		iRet = 0;
		*iRecvLen = recvLen + 2;
	}
	else
	{
		fdSocket = EI_iCreateClientSocketWithTimeout(config.szPOSIP, config.iPOSPort, AF_INET, SOCK_STREAM, config.iScanTimeout, 0, NULL);
		if (fdSocket <= 0)
		{
			strErrMsg.Format("链接 [%s:%d] 失败 [%d]", config.szPOSIP, config.iPOSPort, fdSocket);
			iRet = -1;
			goto cleanUP;
		}
		if (EI_iSetSocketRecvTimeout(fdSocket, config.iScanTimeout, 0) == -1)
		{
			strErrMsg.Format("链接 [%s:%d] 设置超时失败 [%d]", config.szPOSIP, config.iPOSPort, fdSocket);
			iRet = -1;
			goto cleanUP;
		}

		if (EI_iSetBlocking(fdSocket) == -1)
		{
			strErrMsg.Format("链接 [%s:%d] 设置模式失败 [%d]", config.szPOSIP, config.iPOSPort, fdSocket);
			iRet = -1;
			goto cleanUP;
		}
		iRet = EI_iSendBuf(fdSocket, (char *)szSendBuf, iSendLen);
		if (iRet != 0)
		{
			strErrMsg.Format("发送报文 [%s:%d] 失败 [%d]", config.szPOSIP, config.iPOSPort, iRet);
			iRet = -1;
			goto cleanUP;
		}
		iRet = EI_iRecvNBytes(fdSocket, (char *)szRecvBuf, 2);
		if (iRet != 2)
		{
			strErrMsg.Format("接收报文长度[%s:%d] 失败[%d]", config.szPOSIP, config.iPOSPort, iRet);
			iRet = -2;
			goto cleanUP;
		}
		recvLen = szRecvBuf[0] * 256 + szRecvBuf[1];
		LOG_DEBUG("计算长度为[%d]", recvLen);
		iRet = EI_iRecvNBytes(fdSocket, (char *)szRecvBuf + 2, recvLen);
		if (iRet != recvLen)
		{
			LOG_ERRORSB((char *)szRecvBuf, recvLen + 2, "接收报文内容[%s:%d] 失败[%d]", config.szPOSIP, config.iPOSPort, iRet);
			strErrMsg.Format("接收报文内容[%s:%d] 失败[%d]", config.szPOSIP, config.iPOSPort, iRet);
			iRet = -2;
			goto cleanUP;
		}
		iRet = 0;
		*iRecvLen = recvLen + 2;
	}
	
cleanUP:
	if (ssl != nullptr)
	{
		SSL_shutdown(ssl);
		SSL_free(ssl);
	}
	if (ctx != nullptr)
	{
		SSL_CTX_free(ctx);
	}
	if (strErrMsg.GetLength() > 0)
	{
		LOG_ERROR("%s", strErrMsg);
	}
	if (fdSocket > 0)
	{
		Ei_CloseSocket(fdSocket);
	}
	return iRet;
}

void GetTransInfo(POSTRANS_REQUEST req, POSTRANS_RESPONSE &res, int iReprintFlag)
{
	char szTemp[1024];
	int i;
	int iRet;
	int iRecvLen;
	char szCommand[COMDATELEN];
	char sendbuf[COMDATELEN];
	char recvbuf[COMDATELEN];
	vector<string> vPrintData;
	BUFCLR(szCommand);
	if (iReprintFlag == 2)
	{
		sprintf(szCommand, "%s%c%s%c%s%c%s%c%s%c%s%c%d%c", "61", FS, COM_MERNO, FS, COM_TERNO, FS, "", FS, "", FS, req.oldTrace.c_str(), FS, iReprintFlag, FS);
	}
	else
	{
		sprintf(szCommand, "%s%c%s%c%s%c%s%c%s%c%s%c%d%c", "69", FS, COM_MERNO, FS, COM_TERNO, FS, "", FS, "", FS, req.oldTrace.c_str(), FS, iReprintFlag, FS);
	}

	BUFCLR(sendbuf);
	sprintf((char*)sendbuf, "%c%c%6.6s", 0x03, 0x01, "      ");
	memcpy(sendbuf + COM_HEAD, szCommand, strlen(szCommand));
	sendtocom_RS232((unsigned char *)sendbuf, strlen(szCommand) + COM_HEAD);

	BUFCLR(recvbuf);
	iRet = readfromcom_RS232((unsigned char *)recvbuf, &iRecvLen, config.iCOMTimeout);
	if (iRet != 0)
	{
		LOG_ERROR("串口接收失败 %d", iRet);
		res.respCode = "XX";
		res.respMsg = "取打印失败";
		goto cleanUP;
	}
	if (recvbuf[COM_HEAD] == NAK)
	{
		BUFCLR(szTemp);
		Read_Field(2, (char*)recvbuf + COM_HEAD, iRecvLen - COM_HEAD, szTemp);
		res.respCode = szTemp;
		BUFCLR(szTemp);
		Read_Field(3, (char*)recvbuf + COM_HEAD, iRecvLen - COM_HEAD, szTemp);
		res.respMsg = szTemp;
		goto cleanUP;
	}
	i = 0;
	res.respCode = "00";
	while (true)
	{	
		i++;
		if (i > 39) //信息长度，跟着规范改动。
		{
			break;
		}
		BUFCLR(szTemp);
		Read_Field(i, (char*)recvbuf + COM_HEAD, iRecvLen - COM_HEAD, szTemp);
		LOG_INFO("取打印第[%d]域为:[%s]", i, szTemp);
		vPrintData.emplace_back(szTemp);
	}

	res.amt = vPrintData[17];
	res.authNo = vPrintData[15];
	res.batch = vPrintData[12];
	res.cardNo = vPrintData[8];
	res.expr = vPrintData[11];
	res.merchantName = vPrintData[3];
	res.merchantNo = vPrintData[1];
	res.refNo = vPrintData[16];
	res.terminalNo = vPrintData[2];
	res.trace = vPrintData[13];

	char szDateTime[20 + 1];
	BUFCLR(szDateTime);
	Tools::GetDateTime(szDateTime);
	memcpy(szDateTime + 4, vPrintData[14].c_str(), vPrintData[14].length());
	BUFCLR(szTemp);
	memcpy(szTemp, szDateTime, 8);
	res.date = szTemp;
	BUFCLR(szTemp);
	memcpy(szTemp, szDateTime + 8, 6);
	res.time = szTemp;

	
cleanUP:
	return;
}

int __stdcall PosTrans(char *inputData, char *outputData)
{
	POSTRANS_REQUEST req;
	POSTRANS_RESPONSE res;
	CString strCOM;
	char szTemp[1024];
	char szCommand[COMDATELEN];
	char szCom[100 + 1];
	int iRet;
	char sendbuf[COMDATELEN];
	char recvbuf[COMDATELEN];
	unsigned char socketSendBuf[SOCKETDATELEN];
	unsigned char socketRecvBuf[SOCKETDATELEN];
	int iRecvLen;

	InitConfig();
	LOG_INFO("PosTrans 传入参数[%s]", inputData);
	try
	{
		xpack::json::decode(inputData, req);
		if (req.timeout <= 0)
		{
			req.timeout = 15;
		}
	}
	catch (exception &e)
	{
		LOG_ERROR("传入参数JSON解析失败 %s", e.what());
		res.respCode = "XX";
		res.respMsg = "传入参数JSON解析失败";
		goto cleanUP;
	}

	BUFCLR(szCom);
	sprintf(szCom, "\\\\.\\%s", config.szCOM);
	iRet = SerialOpen(szCom, config.iBPS); //初始化设备
	if (iRet != 0)
	{
		LOG_ERROR("打开串口失败 %s", szCom);
		res.respCode = "XX";
		res.respMsg = "打开串口失败";
		goto cleanUP;
	}
	if (!TestPos())
	{
		LOG_ERROR("检测串口失败 %s", szCom);
		res.respCode = "XX";
		res.respMsg = "检测串口失败";
		goto cleanUP;
	}

	BUFCLR(szCommand);
	switch (atoi(req.transId.c_str()))
	{
	case TRANSID::TRANSID_SIGN:
		sprintf(szCommand, "%s%c", "51", FS);
		break;// TRANSID_SIGN
	case TRANSID::TRANSID_CONSUME:
		sprintf(szCommand, "%s%c%s%c%s%c%s%c%s%c%s%c", "01", FS, COM_MERNO, FS, COM_TERNO, FS, "", FS, "", FS, req.amt.c_str(), FS);
		break;// TRANSID_CONSUME
	case TRANSID::TRANSID_CANCEL:
		sprintf(szCommand, "%s%c%s%c%s%c%s%c%s%c%s%c%s%c", "02", FS, COM_MERNO, FS, COM_TERNO, FS, "", FS, "", FS, req.amt.c_str(), FS, req.oldTrace.c_str(), FS);
		break;// TRANSID_CANCEL
	case TRANSID::TRANSID_REFUND:
		sprintf(szCommand, "%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c", "03", FS, COM_MERNO, FS, COM_TERNO, FS, "", FS, "", FS, req.amt.c_str(), FS, req.oldRefNo.c_str(), FS, req.oldDate.c_str() + 4, FS, "", FS);
		break;// TRANSID_REFUND
	case TRANSID::TRANSID_QUERY:
		sprintf(szCommand, "%s%c", "04", FS);
		break;// TRANSID_QUERY
	case TRANSID::TRANSID_REPRINT:
		GetTransInfo(req, res, 2);
		goto cleanUP;
		break;
	case TRANSID::TRANSID_SETTLE:
		sprintf(szCommand, "%s%c", "52", FS);
		break;// TRANSID_SETTLE
	case TRANSID::TRANSID_GETPOSINFO:
		sprintf(szCommand, "%s%c", "11", FS);
		break;// TRANSID_GETPOSINFO
	case TRANSID::TRANSID_GETQRCODE:
		sprintf(szCommand, "%s%c%d%c", "12", FS, req.timeout, FS);
		break;// TRANSID_GETQRCODE
	default:
		LOG_ERROR("暂时不支持该交易 %s", req.transId.c_str());
		res.respCode = "XX";
		res.respMsg = "暂时不支持该交易";
		goto cleanUP;
		break;
	}

	

	BUFCLR(sendbuf);
	sprintf((char*)sendbuf, "%c%c%6.6s", 0x03, 0x01, "      ");
	memcpy(sendbuf + COM_HEAD, szCommand, strlen(szCommand));

	sendtocom_RS232((unsigned char *)sendbuf, strlen(szCommand) + COM_HEAD);

	while (true)
	{
		BUFCLR(recvbuf);
		iRet = readfromcom_RS232((unsigned char *)recvbuf, &iRecvLen, config.iCOMTimeout);
		if (iRet != 0)
		{
			LOG_ERROR("串口接收失败 %d", iRet);
			res.respCode = "XX";
			res.respMsg = "串口接收失败";
			goto cleanUP;
		}

		if (recvbuf[COM_HEAD] == ACK)
		{
			res.respCode = "00";
			BUFCLR(szTemp);
			Read_Field(3, (char*)recvbuf + COM_HEAD, iRecvLen - COM_HEAD, szTemp);
			res.respMsg = szTemp;
			switch (atoi(req.transId.c_str()))
			{
			case TRANSID::TRANSID_CONSUME:
			case TRANSID::TRANSID_CANCEL:
			case TRANSID::TRANSID_REFUND:
				req.oldTrace = "000000";
				GetTransInfo(req, res, 1);
				break;
			case TRANSID::TRANSID_SETTLE:
				break;
			case TRANSID::TRANSID_GETPOSINFO:
				res.respMsg = "取设备信息成功";
				BUFCLR(szTemp);
				Read_Field(3, (char*)recvbuf + COM_HEAD, iRecvLen - COM_HEAD, szTemp);
				res.merchantNo = szTemp;
				BUFCLR(szTemp);
				Read_Field(4, (char*)recvbuf + COM_HEAD, iRecvLen - COM_HEAD, szTemp);
				res.terminalNo = szTemp;
				BUFCLR(szTemp);
				Read_Field(5, (char*)recvbuf + COM_HEAD, iRecvLen - COM_HEAD, szTemp);
				res.sn = szTemp;
				break;
			case TRANSID::TRANSID_GETQRCODE:
				res.respMsg = "取扫码信息成功";
				BUFCLR(szTemp);
				Read_Field(3, (char*)recvbuf + COM_HEAD, iRecvLen - COM_HEAD, szTemp);
				res.qrCode = szTemp;
				break;
			}
			goto cleanUP;
		}
		else if (recvbuf[COM_HEAD] == NAK)
		{
			BUFCLR(szTemp);
			Read_Field(2, (char*)recvbuf + COM_HEAD, iRecvLen - COM_HEAD, szTemp);
			res.respCode = szTemp;
			BUFCLR(szTemp);
			Read_Field(3, (char*)recvbuf + COM_HEAD, iRecvLen - COM_HEAD, szTemp);
			res.respMsg = szTemp;
			goto cleanUP;
		}
		else
		{
			BUFCLR(socketSendBuf);
			BUFCLR(socketRecvBuf);

			socketSendBuf[0] = (iRecvLen - COM_HEAD) / 256;
			socketSendBuf[1] = (iRecvLen - COM_HEAD) % 256;
			memcpy(socketSendBuf + 2, recvbuf + COM_HEAD, iRecvLen - COM_HEAD);

			iRet = SendRecvPosSocket(socketSendBuf, iRecvLen - COM_HEAD + 2, socketRecvBuf, &iRecvLen);
			if (iRet == -2)
			{
				LOG_ERROR("网络接收失败 %d", iRet);
				res.respCode = "XX";
				res.respMsg = "网络接收失败";
				goto cleanUP;
			}
			else if (iRet != 0)
			{
				LOG_ERROR("网络发送失败 %d", iRet);
				res.respCode = "XX";
				res.respMsg = "网络发送失败";
				goto cleanUP;
			}
			else
			{
				BUFCLR(sendbuf);
				sprintf((char*)sendbuf, "%c%c%6.6s", 0x06, 0x01, "      ");
				memcpy(sendbuf + COM_HEAD, socketRecvBuf + 2, iRecvLen - 2);
				sendtocom_RS232((unsigned char *)sendbuf, iRecvLen - 2 + COM_HEAD);
			}
		}
	}
	

cleanUP:
	SerialClose();
	string strData = xpack::json::encode(res);
	LOG_INFO("PosTrans 传出参数[%s]", strData.c_str());
	strcpy(outputData, strData.c_str());
	return 0;
}


