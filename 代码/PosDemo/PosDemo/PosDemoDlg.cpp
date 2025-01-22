
// PosDemoDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CPosDemoDlg �Ի���



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


// CPosDemoDlg ��Ϣ�������

BOOL CPosDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPosDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
