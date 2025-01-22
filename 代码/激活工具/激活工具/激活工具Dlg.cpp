
// 激活工具Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "激活工具.h"
#include "激活工具Dlg.h"
#include "afxdialogex.h"
#include "io.h"
#include "xpack/xpack.h"
#include "xpack/json.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "YLScan.lib")
int __stdcall PosTrans(char *inputData, char *outputData);

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
// #define EXEDEBUG
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


// C激活工具Dlg 对话框



C激活工具Dlg::C激活工具Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(C激活工具Dlg::IDD, pParent)
	, m_sn(_T(""))
	, m_tips(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void C激活工具Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SN, m_sn);
	DDX_Control(pDX, IDC_EDIT_SN, m_editSN);
	DDX_Text(pDX, IDC_STATIC_TIP, m_tips);
}

BEGIN_MESSAGE_MAP(C激活工具Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &C激活工具Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &C激活工具Dlg::OnBnClickedButton3)
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BUTTON2, &C激活工具Dlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// C激活工具Dlg 消息处理程序

#include "Tools.h"
#include "slog.h"
using namespace Tool;

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
	sprintf(config.szQRPath, "%s\\%s", config.szDLLPath, "QR.bmp");


	GetPrivateProfileString("ACTIVE", "URL", "", config.szActiveUrl, sizeof(config.szActiveUrl), config.szConfigPath);
	GetPrivateProfileString("ACTIVE", "VN", "", config.szVN, sizeof(config.szVN), config.szConfigPath);
	config.iTimeout = GetPrivateProfileInt("ACTIVE", "TIMEOUT", 10, config.szConfigPath);

	sprintf(config.szGETINITURL, "%s%s", config.szActiveUrl, URL_GETINIT);
	sprintf(config.szQUERYINITURL, "%s%s", config.szActiveUrl, URL_QUERYINIT);

	Log.init(config.szLogProp);
	char szVer[20 + 1];
	BUFCLR(szVer);
	Tools::GetCurrFileVersion(szVer);
	LOG_INFO("动态库版本号[%s]", szVer);
}
BOOL C激活工具Dlg::OnInitDialog()
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

	InitConfig();
	m_tips = "请获取POS终端序列号";
	UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void C激活工具Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void C激活工具Dlg::OnPaint()
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

	CPaintDC dc1(GetDlgItem(IDC_STATIC_BMP));
	CRect rcclient;
	GetDlgItem(IDC_STATIC_BMP)->GetClientRect(&rcclient);
	CDC memdc;
	memdc.CreateCompatibleDC(&dc1);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc1, rcclient.Width(), rcclient.Height());
	memdc.SelectObject(&bitmap);
	CWnd::DefWindowProc(WM_PAINT, (WPARAM)memdc.m_hDC, 0);
	CDC maskdc;
	maskdc.CreateCompatibleDC(&dc1);
	CBitmap maskbitmap;
	maskbitmap.CreateBitmap(rcclient.Width(), rcclient.Height(), 1, 1, NULL);
	maskdc.SelectObject(&maskbitmap);
	maskdc.BitBlt(0, 0, rcclient.Width(), rcclient.Height(), &memdc,
		rcclient.left, rcclient.top, SRCCOPY);
	CBrush brush;
	brush.CreatePatternBrush(&m_Bmp);
	dc1.FillRect(rcclient, &brush);
	dc1.BitBlt(rcclient.left, rcclient.top, rcclient.Width(), rcclient.Height(),
		&memdc, rcclient.left, rcclient.top, SRCPAINT);
	brush.DeleteObject();
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR C激活工具Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool MakeRsaKeySSL(const char *savePrivateKeyFilePath, const  char *savePublicKeyFilePath) {
	int             ret = 0;
	RSA             *r = NULL;
	BIGNUM          *bne = NULL;
	BIO             *bp_public = NULL, *bp_private = NULL, *bp_public_pkcs8 = NULL;
	int             bits = 2048;
	unsigned long   e = RSA_F4;

	char savePublicKeyFilePath_pkcs8[MAX_PATH + 1];
	BUFCLR(savePublicKeyFilePath_pkcs8);

	// 1. generate rsa key
	bne = BN_new();
	ret = BN_set_word(bne, e);
	if (ret != 1) {
		fprintf(stderr, "MakeLocalKeySSL BN_set_word err \n");
		goto free_all;
	}

	r = RSA_new();
	ret = RSA_generate_key_ex(r, bits, bne, NULL);
	if (ret != 1) {
		fprintf(stderr, "MakeLocalKeySSL RSA_generate_key_ex err \n");
		goto free_all;
	}

	// 2. save public key
	if (savePublicKeyFilePath != NULL) {
		bp_public = BIO_new_file(savePublicKeyFilePath, "w+");
// 		ret = PEM_write_bio_RSAPublicKey(bp_public, r);
// 		if (ret != 1) {
// 			fprintf(stderr, "MakeLocalKeySSL PEM_write_bio_RSAPublicKey err \n");
// 			goto free_all;
// 		}
// 		bp_public_pkcs8 = BIO_new_file(savePublicKeyFilePath_pkcs8, "w+");
// 		sprintf(savePublicKeyFilePath_pkcs8, "%s_pkcs8", savePublicKeyFilePath);
		ret = PEM_write_bio_RSA_PUBKEY(bp_public, r);
		if (ret != 1) {
			fprintf(stderr, "MakeLocalKeySSL PEM_write_bio_RSA_PUBKEY err \n");
			goto free_all;
		}
	}

	// 3. save private key
	if (savePrivateKeyFilePath != NULL) {
		bp_private = BIO_new_file(savePrivateKeyFilePath, "w+");
		ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);
	}

	// 4. free
free_all:

	BIO_free_all(bp_public);
	BIO_free_all(bp_private);
	RSA_free(r);
	BN_free(bne);

	return (ret == 1);
}

string strReadPub(char *path)
{
// 	char szNewPath[MAX_PATH + 1];
// 	BUFCLR(szNewPath);
// 	sprintf(savePublicKeyFilePath_pkcs8, "%s_pkcs8", savePublicKeyFilePath);
	string strData;
	char szData[1024 * 10];
	BUFCLR(szData);
	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
	{
		return "";
	}
	else
	{
		fread(szData, sizeof(char), sizeof(szData), fp);
		CString cstrData = szData;
		fclose(fp);
		cstrData.Replace("\r", "");
		cstrData.Replace("\n", "");
		cstrData.Replace("-----BEGIN PUBLIC KEY-----", "");
		cstrData.Replace("-----END PUBLIC KEY-----", "");
		strData = cstrData.GetBuffer(0);
		return strData;
	}
}

#include "QR-ENCODE-3.2/qrencode.h"
int GenQRBMP(char *szSourceSring, char *BMPFilePath)
{
#define OUT_FILE_PIXEL_PRESCALER	8											// Prescaler (number of pixels in bmp file for each QRCode pixel, on each dimension)

#define PIXEL_COLOR_R				0x00										// Color of bmp pixels
#define PIXEL_COLOR_G				0x00
#define PIXEL_COLOR_B				0x00

	unsigned int	unWidth, x, y, l, n, unWidthAdjusted, unDataBytes;
	unsigned char*	pRGBData, *pSourceData, *pDestData;
	QRcode*			pQRC;
	FILE*			f;


	// Compute QRCode

	if (pQRC = QRcode_encodeString(szSourceSring, 0, QR_ECLEVEL_M, QR_MODE_8, 1))
	{
		unWidth = pQRC->width;
		unWidthAdjusted = unWidth * OUT_FILE_PIXEL_PRESCALER * 3;
		if (unWidthAdjusted % 4)
			unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
		unDataBytes = unWidthAdjusted * unWidth * OUT_FILE_PIXEL_PRESCALER;

		// Allocate pixels buffer

		if (!(pRGBData = (unsigned char*)malloc(unDataBytes)))
		{
			printf("Out of memory");
			exit(-1);
		}

		// Preset to white

		memset(pRGBData, 0xff, unDataBytes);


		// Prepare bmp headers

		BITMAPFILEHEADER kFileHeader;
		kFileHeader.bfType = 0x4d42;  // "BM"
		kFileHeader.bfSize = sizeof(BITMAPFILEHEADER)+
			sizeof(BITMAPINFOHEADER)+
			unDataBytes;
		kFileHeader.bfReserved1 = 0;
		kFileHeader.bfReserved2 = 0;
		kFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER)+
			sizeof(BITMAPINFOHEADER);

		BITMAPINFOHEADER kInfoHeader;
		kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		kInfoHeader.biWidth = unWidth  * OUT_FILE_PIXEL_PRESCALER;
		kInfoHeader.biHeight = unWidth * OUT_FILE_PIXEL_PRESCALER;
		kInfoHeader.biPlanes = 1;
		kInfoHeader.biBitCount = 24;
		kInfoHeader.biCompression = 0;
		kInfoHeader.biSizeImage = unDataBytes;
		kInfoHeader.biXPelsPerMeter = 0;
		kInfoHeader.biYPelsPerMeter = 0;
		kInfoHeader.biClrUsed = 0;
		kInfoHeader.biClrImportant = 0;


		// Convert QrCode bits to bmp pixels

		pSourceData = pQRC->data;
		for (y = 0; y < unWidth; y++)
		{
			pDestData = pRGBData + unWidthAdjusted * y * OUT_FILE_PIXEL_PRESCALER;
			for (x = 0; x < unWidth; x++)
			{
				if (*pSourceData & 1)
				{
					for (l = 0; l < OUT_FILE_PIXEL_PRESCALER; l++)
					{
						for (n = 0; n < OUT_FILE_PIXEL_PRESCALER; n++)
						{
							*(pDestData + n * 3 + unWidthAdjusted * l) = PIXEL_COLOR_B;
							*(pDestData + 1 + n * 3 + unWidthAdjusted * l) = PIXEL_COLOR_G;
							*(pDestData + 2 + n * 3 + unWidthAdjusted * l) = PIXEL_COLOR_R;
						}
					}
				}
				pDestData += 3 * OUT_FILE_PIXEL_PRESCALER;
				pSourceData++;
			}
		}

		// Output the bmp file
		f = fopen(BMPFilePath, "wb");
		if (f != NULL)
		{
			fwrite(&kFileHeader, sizeof(BITMAPFILEHEADER), 1, f);
			fwrite(&kInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);
			// 			fwrite(rgbquad, sizeof(nQuadLen), 1, f);
			fwrite(pRGBData, sizeof(unsigned char), unDataBytes, f);

			fclose(f);
		}
		else
		{
			printf("Unable to open file");
			exit(-1);
		}

		// Free data

		free(pRGBData);
		QRcode_free(pQRC);
	}
	else
	{
		printf("NULL returned");
		exit(-1);
	}
	return 0;
}

void C激活工具Dlg::OnBnClickedButton1()
{
	UpdateData();
	if (m_sn.GetLength() == 0)
	{
		AfxMessageBox("请输入SN号");
		return;
	}
	LOG_INFO("开始激活");
	REQ_GETINIT getinitRequest;
	RES_GETINIT getinitResponse;

	getinitRequest.VN = config.szVN;
	getinitRequest.SN_ID = config.szVN;
	getinitRequest.SN_ID.append(m_sn.GetBuffer(0));
	getinitRequest.DEVICE_INFO = "EPOS";

	sprintf(config.szDevicePubKeyPath, "%s\\%s_pub.pem", config.szDLLPath, m_sn);
	sprintf(config.szDevicePriKeyPath, "%s\\%s_pri.pem", config.szDLLPath, m_sn);
	if (_access(config.szDevicePubKeyPath, 0) == 0 || _access(config.szDevicePriKeyPath, 0) == 0)
	{

	}
	else
	{
		MakeRsaKeySSL(config.szDevicePriKeyPath, config.szDevicePubKeyPath);
	}

	getinitRequest.DEVICE_PUB_KEY = strReadPub(config.szDevicePubKeyPath);

	string strSend = xpack::xml::encode(getinitRequest, "XML");
	char *pRes = NULL;
	int iRet = Tools::HttpPostXml(config.szGETINITURL, (char *)strSend.c_str(), &pRes, config.iTimeout);
	if (iRet != 0 || pRes == NULL)
	{
		AfxMessageBox("连接后台失败, 请重试");
		return;
	}
	CString cstrRecv = Tools::UTF8ToGBK(pRes + 4);
	LOG_INFO("后台返回 [%s]", cstrRecv.GetBuffer(0));
	try
	{
		xpack::xml::decode(cstrRecv.GetBuffer(0), getinitResponse);
	}
	catch (exception &e)
	{
		LOG_ERROR("XML解析失败 %s", e.what());
	}
#ifdef EXEDEBUG
	AfxMessageBox("测试模式");
	getinitResponse.STATUS = "0";
	getinitResponse.SN_QR_INIT_URL = "https://ac.95516.com/spay/device/epos/bind?snId=000003123";
	getinitResponse.SN_QR_INIT_TOKEN = "123";
#endif // EXEDEBUG

	if (getinitResponse.STATUS.compare("0") != 0)
	{
		AfxMessageBox(getinitResponse.MESSAGE.c_str());
		return;
	}

	Tools::strlcpy(config.SN_QR_INIT_TOKEN, getinitResponse.SN_QR_INIT_TOKEN.c_str(), sizeof(config.SN_QR_INIT_TOKEN));
	m_editSN.SetReadOnly(TRUE);

	GenQRBMP((char *)getinitResponse.SN_QR_INIT_URL.c_str(), config.szQRPath);
	HBITMAP hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), config.szQRPath, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	if (hBmp == NULL)
	{
		return;
	}

	if (NULL != m_Bmp.m_hObject)
	{
		m_Bmp.DeleteObject();
	}
	m_Bmp.Attach(hBmp);
	DIBSECTION ds;
	BITMAPINFOHEADER &bmInfo = ds.dsBmih;
	m_Bmp.GetObject(sizeof(ds), &ds);
	int cx = bmInfo.biWidth;
	int cy = bmInfo.biHeight;
	CRect rect;
	GetDlgItem(IDC_STATIC_BMP)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	GetDlgItem(IDC_STATIC_BMP)->MoveWindow(rect.left, rect.top, cx, cy, true);

	m_tips = "请扫码授权";
	UpdateData(FALSE);

	Invalidate();
}

void C激活工具Dlg::OnBnClickedButton3()
{
	if (strlen(config.SN_QR_INIT_TOKEN) == 0)
	{
		AfxMessageBox("TOKEN 为空");
		return;
	}
	REQ_QUERYINIT queryRequest;
	RES_QUERYINIT queryResponse;
	queryRequest.SN_ID = config.szVN;
	queryRequest.SN_ID.append(m_sn.GetBuffer(0));
	queryRequest.SN_QR_INIT_TOKEN = config.SN_QR_INIT_TOKEN;

	string strSend = xpack::xml::encode(queryRequest, "XML");
	char *pRes = NULL;
	int iRet = Tools::HttpPostXml(config.szQUERYINITURL, (char *)strSend.c_str(), &pRes, config.iTimeout);
	if (iRet != 0 || pRes == NULL)
	{
		AfxMessageBox("连接后台失败, 请重试");
		return;
	}
	CString cstrRecv = Tools::UTF8ToGBK(pRes + 4);
	LOG_INFO("后台返回 [%s]", cstrRecv.GetBuffer(0));
	try
	{
		xpack::xml::decode(cstrRecv.GetBuffer(0), queryResponse);
	}
	catch (exception &e)
	{
		LOG_ERROR("XML解析失败 %s", e.what());
	}
#ifdef EXEDEBUG
	AfxMessageBox("测试模式");
	queryResponse.STATUS = "0";
	queryResponse.SN_QR_INIT_STATUS = 0;
	queryResponse.ORG_PUB_KEY = "ORG_PUB_KEY";
	queryResponse.MCH_NAME = "MCH_NAME";
	queryResponse.MCH_ID = "MCH_ID";
	queryResponse.SN_INIT_TIME = "SN_INIT_TIME";
#endif // EXEDEBUG

	if (queryResponse.STATUS.compare("0") != 0)
	{
		AfxMessageBox(queryResponse.MESSAGE.c_str());
		return;
	}

	if (queryResponse.SN_QR_INIT_STATUS != 0)
	{
		AfxMessageBox(queryResponse.SN_QR_INIT_MESSAGE.c_str());
		return;
	}

	WritePrivateProfileString("MERINFO", "SN", m_sn.GetBuffer(0), config.szConfigPath);
	WritePrivateProfileString("MERINFO", "NAME", queryResponse.MCH_NAME.c_str(), config.szConfigPath);
	WritePrivateProfileString("MERINFO", "MERNO", queryResponse.MCH_ID.c_str(), config.szConfigPath);
	WritePrivateProfileString("MERINFO", "ORGPUBKEY", queryResponse.ORG_PUB_KEY.c_str(), config.szConfigPath);
	AfxMessageBox("激活成功");
}

void C激活工具Dlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CDialogEx::OnLButtonDblClk(nFlags, point);
}

typedef struct
{
	string transId, amt, oldTrace, oldRefNo, cardNo, expr, oldDate, oldAuthNo;
	XPACK(O(transId, amt, oldTrace, oldRefNo, cardNo, expr, oldDate, oldAuthNo));
}POSTRANS_REQUEST;

typedef struct
{
	string respCode, respMsg, amt, cardNo, expr, trace, batch, refNo, authNo, date, time, merchantNo, merchantName, terminalNo, sn;
	XPACK(O(respCode, respMsg, amt, cardNo, expr, trace, batch, refNo, authNo, date, time, merchantNo, merchantName, terminalNo, sn));
}POSTRANS_RESPONSE;

void C激活工具Dlg::OnBnClickedButton2()
{
	char szData[1024 * 2];
	POSTRANS_REQUEST request;
	POSTRANS_RESPONSE response;

	request.transId = "07";
	string strInput = xpack::json::encode(request);
	memset(szData, 0x00, sizeof(szData));
	PosTrans((char *)strInput.c_str(), szData);
	xpack::json::decode(szData, response);
	if (response.respCode.compare("00") == 0)
	{
		m_sn = response.sn.c_str();
		UpdateData(FALSE);
		m_tips = "请生成激活码";
		UpdateData(FALSE);
	}
	else
	{
		AfxMessageBox(response.respMsg.c_str());
	}
}
