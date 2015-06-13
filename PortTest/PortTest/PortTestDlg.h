
// PortTestDlg.h : header file
//

#pragma once

typedef struct _THREADDATA_
{
	CString ipAddress;
	CString port;
}THREADDATA,* PTHREADDATA;


// CPortTestDlg dialog
class CPortTestDlg : public CDialogEx
{
// Construction
public:
	CPortTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PORTTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	THREADDATA m_threaddata;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};

UINT threadM(LPVOID pParam);