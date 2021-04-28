// EventSelectDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "cEventSelect.h"
#include "resource.h"
// CEventSelectDlg 대화 상자
class CEventSelectDlg : public CDialog
{
// 생성
public:
	CEventSelectDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_EVENTSELECT_DIALOG };

	void OutputMsg( char *szOutputString , ... );

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원
	

// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_ctOutput;
	cEventSelect m_EventSelect;
	afx_msg void OnBnClickedStartserver();
	afx_msg void OnBnClickedCancel();
};
