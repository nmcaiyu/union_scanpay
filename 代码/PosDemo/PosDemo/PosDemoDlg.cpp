
// PosDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PosDemo.h"
#include "PosDemoDlg.h"
#include "afxdialogex.h"
#include "xpack/xpack.h"
#include "xpack/json.h"
#include "string"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

#pragma comment(lib, "YLScan.lib")
int __stdcall PosTrans(char *inputData, char *outputData);

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPosDemoDlg 对话框



CPosDemoDlg::CPosDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPosDemoDlg::IDD, pParent)
	, m_1(_T("00"))
	, m_2(_T("000000000001"))
	, m_3(_T("000000"))
	, m_4(_T("121212121212"))
	, m_5(_T("20220804"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPosDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_1);
	DDX_Text(pDX, IDC_EDIT2, m_2);
	DDX_Text(pDX, IDC_EDIT3, m_3);
	DDX_Text(pDX, IDC_EDIT4, m_4);
	DDX_Text(pDX, IDC_EDIT5, m_5);
}

BEGIN_MESSAGE_MAP(CPosDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CPosDemoDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CPosDemoDlg 消息处理程序

BOOL CPosDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPosDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPosDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPosDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

typedef struct
{
	string transId, amt, oldTrace, oldRefNo, cardNo, expr, oldDate, oldAuthNo;
	XPACK(O(transId, amt, oldTrace, oldRefNo, cardNo, expr, oldDate, oldAuthNo));
}POSTRANS_REQUEST;

typedef struct
{
	string respCode, respMsg, amt, cardNo, expr, trace, batch, refNo, authNo, date, time, merchantNo, merchantName, terminalNo;
	XPACK(O(respCode, respMsg, amt, cardNo, expr, trace, batch, refNo, authNo, date, time, merchantNo, merchantName, terminalNo));
}POSTRANS_RESPONSE;

void CPosDemoDlg::OnBnClickedButton1()
{
	char szData[1024 * 2];
	POSTRANS_REQUEST request;
	POSTRANS_RESPONSE response;
	UpdateData();

	request.transId = m_1;
	request.amt = m_2;
	request.oldTrace = m_3;
	request.oldRefNo = m_4;
	request.oldDate = m_5;

	string strInput = xpack::json::encode(request);
	memset(szData, 0x00, sizeof(szData));
	PosTrans((char *)strInput.c_str(), szData);
	AfxMessageBox(szData);
}
