// GameClientDlg.h : ��� ����
//

#pragma once
#define MAX_SOCKBUF	1024

#include "afxwin.h"

#include "GameClientDlg.h"
#include "cTickThread.h"
#include "cProcessPacket.h"




// CGameClientDlg ��ȭ ����
class CGameClientDlg : public CDialog
{
// ����
public:
	CGameClientDlg(CWnd* pParent = NULL);	// ǥ�� ������

// ��ȭ ���� ������
	enum { IDD = IDD_GAMECLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����

public :
	//��Ʈ��ũ �̺�Ʈ�� �߻��Ͽ� WM_SOCKETMSG�� ������ 
	//�� �Լ��� ������ �ȴ�.wParam�� �̺�Ʈ�� �߻��� �����̰�
	//lParam�� �̺�Ʈ ������ �����ڵ带 ������ �ִ�.
	LRESULT OnSocketMsg( WPARAM wParam , LPARAM lParam );

	//��� �޼���
	void OutputMsg( char *szOutputString , ... );
	void ProcessPacket( char* pRecvBuf , int nRecvLen );
	void DrawField( CDC* pDC );

// ����
protected:
	HICON m_hIcon;

	int m_nRestRecvLen;
	cTickThread	m_TickThread;
	char	m_szSocketBuf[ MAX_SOCKBUF * 4 ];

	// �޽��� �� �Լ��� �����߽��ϴ�.
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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	
	afx_msg void OnBnClickedHelp();
};
