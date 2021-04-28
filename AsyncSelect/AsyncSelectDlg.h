// AsyncSelectDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "cAsyncSocket.h"

#include "resource.h"
#define MAX_SOCKBUF 1024
// CAsyncSelectDlg 대화 상자
class CAsyncSelectDlg : public CDialog
{
// 생성
public:
	CAsyncSelectDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_ASYNCSELECT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

public:
	//네트워크 이벤트가 발생하여 WM_SOCKETMSG를 받으면 
	//이 함수가 실행이 된다.wParam이 이벤트가 발생한 소켓이고
	//lParam은 이벤트 종류와 에러코드를 가지고 있다.
	LRESULT OnSocketMsg( WPARAM wParam , LPARAM lParam );

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
	//메세지를 출력하기 위해 만든 리스트 박스 컨트롤
	CListBox m_ctOutput;
	//소켓 버퍼
	char	 m_szSocketBuf[ MAX_SOCKBUF ];
	//WSAAsyncSelect모델을 적용 시킨 클래스 변수
	cAsyncSocket m_AsyncSocket;
	

	afx_msg void OnBnClickedStartserver();
};
