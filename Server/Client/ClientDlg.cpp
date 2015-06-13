
// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientDlg dialog



CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CClientDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CClientDlg::OnBnClickedButtonSend)
	ON_MESSAGE(WM_CLIENT, &CClientDlg::OnSock)
END_MESSAGE_MAP()


// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	SetDlgItemText(IDC_EDIT_IP, "10.128.254.14");
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientDlg::OnPaint()
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
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CClientDlg::OnBnClickedButtonConnect()
{
	// TODO: Add your control notification handler code here
	CString strBtnName;
	GetDlgItemText(IDC_BUTTON_CONNECT, strBtnName);
	if (strBtnName == "连接")
	{
		SetDlgItemText(IDC_BUTTON_CONNECT, "断开");
		char szIpAddr[MAXBYTE] = { 0 };
		GetDlgItemText(IDC_EDIT_IP, szIpAddr, MAXBYTE);

		m_Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_Socket == INVALID_SOCKET)
		{
			AfxMessageBox(_T("新建Socket失败！"));
			closesocket(m_Socket);
			return;
		}
		WSAAsyncSelect(m_Socket, GetSafeHwnd(), WM_CLIENT, FD_READ | FD_CONNECT | FD_CLOSE);
		sockaddr_in ServerAddr;
		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_addr.S_un.S_addr = inet_addr(szIpAddr);
		ServerAddr.sin_port = htons(1234);
		
		connect(m_Socket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr));
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_CONNECT, "连接");
		closesocket(m_Socket);
	}
}


void CClientDlg::OnBnClickedButtonSend()
{
	// TODO: Add your control notification handler code here
	char szBuf[MAXBYTE] = { 0 };
	GetDlgItemText(IDC_EDIT_CMD, szBuf, MAXBYTE);

	send(m_Socket, szBuf, MAXBYTE, 0);
}


afx_msg LRESULT CClientDlg::OnSock(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam))
	{
		return 1;
	}
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CONNECT:
	{
		AfxMessageBox("连接成功");
		break;
	}
	case FD_READ:
	{
		char szBuf[MAXBYTE] = { 0 };
		recv(m_Socket, szBuf, MAXBYTE, 0);
		AfxMessageBox(szBuf);
		break;
	}
	}
	return 0;
}
