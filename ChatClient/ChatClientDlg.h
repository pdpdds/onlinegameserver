// ChatClientDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "resource.h"
#include "cAsyncSocket.h"
#define MAX_SOCKBUF	1024
// CChatClientDlg ��ȭ ����
class CChatClientDlg : public CDialog
{
// ����
public:
	CChatClientDlg(CWnd* pParent = NULL);	// ǥ�� ������

// ��ȭ ���� ������
	enum { IDD = IDD_CHATCLIENT_DIALOG };

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
// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_ctChatList;
	CString m_szChatMsg;
	CString m_szIP;
	int m_nPort;
	int m_nRestRecvLen;
	cAsyncSocket m_AsyncSocket;
	char	m_szSocketBuf[ MAX_SOCKBUF * 4 ];
	afx_msg void OnBnClickedConnect();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnOK();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
