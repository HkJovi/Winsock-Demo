
// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerDlg dialog



CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SERVER, &CServerDlg::OnSock)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CServerDlg message handlers

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
	m_ListenSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	WSAAsyncSelect(m_ListenSock, GetSafeHwnd(), WM_SERVER, FD_ACCEPT);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = ADDR_ANY;
	addr.sin_port = htons(1234);

	bind(m_ListenSock, (SOCKADDR *)&addr, sizeof(addr));
	listen(m_ListenSock, SOMAXCONN);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CServerDlg::OnPaint()
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
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



afx_msg LRESULT CServerDlg::OnSock(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam))
	{
		return 1;
	}
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
	{
		sockaddr_in ClientAddr;
		int nSize = sizeof(ClientAddr);

		m_ClientSock = accept(m_ListenSock, (SOCKADDR *)&ClientAddr, &nSize);
		WSAAsyncSelect(m_ClientSock, GetSafeHwnd(), WM_SERVER, FD_READ | FD_CLOSE);
		char *p = inet_ntoa(ClientAddr.sin_addr);
		m_StrMsg.Format("请求IP：%s:%d", p, ntohs(ClientAddr.sin_port));

		char szBuf[MAXBYTE] = { 0 };
		lstrcpy(szBuf, m_StrMsg.GetBuffer());
		send(m_ClientSock, (const char*)&szBuf, sizeof(szBuf), 0);

		break;
	}
	case FD_READ:
	{
		char szBuf[MAXBYTE] = { 0 };
		recv(m_ClientSock, szBuf, MAXBYTE, 0);
		m_StrMsg = "对方发来命令";
		m_StrMsg += szBuf;

		break;
	}
	case FD_CLOSE:
	{
		closesocket(m_ClientSock);
		m_StrMsg = "对方关闭连接";
		break;
	}
	}
	CString temp;
	GetDlgItemText(IDC_EDIT, temp);

	m_StrMsg += "\r\n----------------------\r\n";
	m_StrMsg += temp;

	SetDlgItemText(IDC_EDIT, m_StrMsg);
	m_StrMsg = "";
	return 0;
}


void CServerDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	closesocket(m_ListenSock);
	closesocket(m_ClientSock);
	WSACleanup();
	CDialogEx::OnClose();
}
