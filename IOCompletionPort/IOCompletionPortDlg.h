// IOCompletionPortDlg.h : 헤더 파일
//

#pragma once
#include "cIOCompletionPort.h"
#include "resource.h"

// CIOCompletionPortDlg 대화 상자
class CIOCompletionPortDlg : public CDialog
{
// 생성
public:
	CIOCompletionPortDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_IOCOMPLETIONPORT_DIALOG };

	void OutputMsg( char *szOutputString , ... );


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

private:
	//IOCompletionPort객체를 생성한다.
	cIOCompletionPort m_IOCompletionPort;

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
	afx_msg void OnBnClickedStartserver();

	//메세지를 출력하기 위해 만든 리스트 박스 컨트롤
	CListBox m_ctOutput;

	afx_msg void OnBnClickedCancel();
};
