
// PortTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PortTest.h"
#include "PortTestDlg.h"
#include <WS2tcpip.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
HANDLE hMutex;


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CPortTestDlg dialog



CPortTestDlg::CPortTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPortTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPortTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPortTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CPortTestDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CPortTestDlg message handlers

BOOL CPortTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	GetDlgItem(IDC_IPADDRESS)->SetWindowText(L"10.128.254.19");
	GetDlgItem(IDC_EDIT)->SetWindowText(L"3389");
	WSADATA wsaData;
	if (WSAStartup(0x0002, &wsaData))
	{
		MessageBox(L"Init TCP/IP stack error!");
		return 1;
	}
	if (wsaData.wVersion != 0x0002)
	{
		MessageBox(L"Winsock version is incorrect!");
		WSACleanup();
		return 1;
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPortTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPortTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPortTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPortTestDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_IPADDRESS)->GetWindowText(m_threaddata.ipAddress);
	GetDlgItem(IDC_EDIT)->GetWindowText(m_threaddata.port);	
	if ((hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"sam")) == NULL)
	{
		//如果没有其他进程创建这个互斥量，则重新创建
		hMutex = CreateMutex(NULL, FALSE, L"sam");
	}
	if (WaitForSingleObject(hMutex, 100) == WAIT_TIMEOUT)
	{
		AfxMessageBox(L"线程正在运行！");
		return;
	}
	else
	{
		ReleaseMutex(hMutex);
	}
	AfxBeginThread((AFX_THREADPROC)threadM, &m_threaddata);
}


UINT threadM(LPVOID pParam)
{
	WaitForSingleObject(hMutex, INFINITE);
	PTHREADDATA data = (PTHREADDATA)pParam;
	CString ip = data->ipAddress;
	CString ipport = data ->port;

	struct sockaddr_in sin;
	SOCKET sd;
	// Create the local socket
	if ((sd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) == INVALID_SOCKET) {
		printf("Create socket error!");
		ReleaseMutex(hMutex);
		return 1;
	}

	int IpPort = _ttoi(ipport);

	int nLength = ip.GetLength();
	int nBytes = WideCharToMultiByte(CP_ACP, 0, ip, nLength, NULL, 0, NULL, NULL);
	char* IpAddr = new char[nBytes + 1];
	memset(IpAddr, 0, nLength + 1);
	WideCharToMultiByte(CP_OEMCP, 0, ip, nLength, IpAddr, nBytes, NULL, NULL);
	IpAddr[nBytes] = 0;

	// Connect to the victim IP Address
	sin.sin_family = AF_INET;
	inet_pton(AF_INET, IpAddr, (PVOID)&sin.sin_addr.s_addr);
	sin.sin_port = htons((short)IpPort);
	if (connect(sd, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR) 
	{
		printf("Connect the remote IP error!");
		closesocket(sd);
		AfxMessageBox(L"远程IP未开启端口！");
		delete[] IpAddr;
		ReleaseMutex(hMutex);
		return 1;
	}
	else
	{
		AfxMessageBox(L"端口已打开！");
	}
	closesocket(sd);
	delete[] IpAddr;
	ReleaseMutex(hMutex);
	return 1;
}