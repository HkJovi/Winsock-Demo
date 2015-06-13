
// ServerDlg.h : header file
//

#pragma once
#define WM_SERVER WM_USER + 101


// CServerDlg dialog
class CServerDlg : public CDialogEx
{
// Construction
public:
	CServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	SOCKET m_ListenSock;
	SOCKET m_ClientSock;
	CString m_StrMsg;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnSock(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnClose();
};
