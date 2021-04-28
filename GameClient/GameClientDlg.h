// GameClientDlg.h : 헤더 파일
//

#pragma once
#define MAX_SOCKBUF	1024

#include "afxwin.h"

#include "GameClientDlg.h"
#include "cTickThread.h"
#include "cProcessPacket.h"




// CGameClientDlg 대화 상자
class CGameClientDlg : public CDialog
{
// 생성
public:
	CGameClientDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_GAMECLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

public :
	//네트워크 이벤트가 발생하여 WM_SOCKETMSG를 받으면 
	//이 함수가 실행이 된다.wParam이 이벤트가 발생한 소켓이고
	//lParam은 이벤트 종류와 에러코드를 가지고 있다.
	LRESULT OnSocketMsg( WPARAM wParam , LPARAM lParam );

	//출력 메세지
	void OutputMsg( char *szOutputString , ... );
	void ProcessPacket( char* pRecvBuf , int nRecvLen );
	void DrawField( CDC* pDC );

// 구현
protected:
	HICON m_hIcon;

	int m_nRestRecvLen;
	cTickThread	m_TickThread;
	char	m_szSocketBuf[ MAX_SOCKBUF * 4 ];

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_szIp;
	CPoint	m_Point;
	CDC* m_pDC;
	
	int m_nPort;
	DWORD m_dwDetailNpcKey;
	DWORD m_dwDetailPlayerKey;
	afx_msg void OnBnClickedOk();
	CListBox m_ctOutput;
	afx_msg void OnStnClickedId2();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	
	afx_msg void OnBnClickedHelp();
};
