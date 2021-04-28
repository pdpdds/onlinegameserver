// OverlappedCallbackDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "resource.h"
#include "cOverlappedCallback.h"

// COverlappedCallbackDlg 대화 상자
class COverlappedCallbackDlg : public CDialog
{
// 생성
public:
	COverlappedCallbackDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_OVERLAPPEDCALLBACK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원


public:
		//출력 메세지
	void OutputMsg( char *szOutputString , ... );

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
	//overlapped i/o class
	cOverlappedCallback	m_OverlappedCallback;
	//메세지를 출력하기 위해 만든 리스트 박스 컨트롤
	CListBox m_ctOutput;
	//소켓 버퍼
	char	 m_szSocketBuf[ MAX_SOCKBUF ];
	
	
	afx_msg void OnBnClickedStartserver();
	afx_msg void OnBnClickedCancel();
};
