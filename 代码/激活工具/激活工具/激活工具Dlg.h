
// 激活工具Dlg.h : 头文件
//

#pragma once


#include "xpack/xpack.h"
#include "xpack/xml.h"

#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "openssl/bn.h"
#include "afxwin.h"
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")

using namespace std;
// C激活工具Dlg 对话框
class C激活工具Dlg : public CDialogEx
{
// 构造
public:
	C激活工具Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CString m_sn;
	CBitmap m_Bmp;
	afx_msg void OnBnClickedButton3();
	CEdit m_editSN;
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButton2();
	CString m_tips;
};
#define MAX_IP_LEN	1024
#define URL_GETINIT	"/spay/device/epos/getinit"
#define URL_QUERYINIT	"/spay/device/epos/queryinit"

typedef struct
{
	char szConfigPath[MAX_PATH + 1];
	char szLogProp[MAX_PATH + 1];
	char szDLLPath[MAX_PATH + 1];

	char szActiveUrl[1024];
	char szVN[100 + 1];
	char szSN[100 + 1];

	char szDevicePubKeyPath[MAX_PATH + 1];
	char szDevicePriKeyPath[MAX_PATH + 1];

	char szGETINITURL[MAX_IP_LEN + 1];
	char szQUERYINITURL[MAX_IP_LEN + 1];
	int iTimeout;

	char szQRPath[MAX_PATH + 1];
	char SN_QR_INIT_TOKEN[100 + 1];
}CFG;

typedef struct  REQ_GETINIT
{
	string VN, SN_ID, DEVICE_PUB_KEY, DEVICE_INFO;
	XPACK(O(VN, SN_ID, DEVICE_PUB_KEY, DEVICE_INFO));
};

typedef struct  RES_GETINIT
{
	string STATUS, MESSAGE, SN_QR_INIT_TOKEN, SN_QR_INIT_URL, SN_QR_INIT_TIMEOUT;
	XPACK(O(STATUS, MESSAGE, SN_QR_INIT_TOKEN, SN_QR_INIT_URL, SN_QR_INIT_TIMEOUT));
};

typedef struct  REQ_QUERYINIT
{
	string SN_ID, SN_QR_INIT_TOKEN;
	XPACK(O(SN_ID, SN_QR_INIT_TOKEN));
};

typedef struct  RES_QUERYINIT
{
	int SN_QR_INIT_STATUS;
	string STATUS, MESSAGE, SN_QR_INIT_URL, SN_QR_INIT_MESSAGE, ORG_PUB_KEY, MCH_NAME, MCH_ID, SN_INIT_TIME;
	XPACK(O(SN_QR_INIT_STATUS, STATUS, MESSAGE, SN_QR_INIT_URL, SN_QR_INIT_MESSAGE, ORG_PUB_KEY, MCH_NAME, MCH_ID, SN_INIT_TIME));
};