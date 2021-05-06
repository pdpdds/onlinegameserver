lpConnection// ChatClientDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include ".\chatclientdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CChatClientDlg ��ȭ ����



CChatClientDlg::CChatClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChatClientDlg::IDD, pParent)
	, m_szChatMsg(_T(""))
	, m_szIP(_T("127.0.0.1"))
	, m_nPort(8000)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nRestRecvLen = 0;
}

void CChatClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTPUT, m_ctChatList);
	DDX_Text(pDX, IDC_CHATMSG, m_szChatMsg);
	DDX_Text(pDX, IDC_IP, m_szIP);
	DDX_Text(pDX, IDC_PORT, m_nPort);
}

BEGIN_MESSAGE_MAP(CChatClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE( WM_SOCKETMSG , OnSocketMsg )
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CONNECT, OnBnClickedConnect)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CChatClientDlg �޽��� ó����

BOOL CChatClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	//���� �ʱ�ȭ
	
	m_AsyncSocket.SetMainDlg( this );
	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

void CChatClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CChatClientDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
HCURSOR CChatClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//��� �޼���
void CChatClientDlg::OutputMsg( char *szOutputString , ... )
{
	char szOutStr[ 1024 ];
	va_list	argptr; 
    va_start( argptr, szOutputString );
	vsprintf( szOutStr , szOutputString, argptr );
	va_end( argptr );
	m_ctChatList.SetCurSel( m_ctChatList.AddString( szOutStr ) );
}


void CChatClientDlg::OnBnClickedConnect()
{
	UpdateData( FALSE );
	m_AsyncSocket.InitSocket( m_hWnd );
	m_AsyncSocket.ConnectTo( m_szIP.GetBuffer( m_szIP.GetLength() ), m_nPort );
	//test
//	SetTimer( 1 , 1 , NULL );
}

LRESULT CChatClientDlg::OnSocketMsg( WPARAM wParam , LPARAM lParam )
{

	SOCKET sock = (SOCKET)wParam;

	//����üũ
	int nError = WSAGETSELECTERROR( lParam );
	if( 0 != nError )
	{
		OutputMsg( "[����] WSAGETSELECTERROR : %d ", nError );
		m_AsyncSocket.CloseSocket( sock );
		return false;
	}
	
	//�̺�Ʈ üũ
	int nEvent = WSAGETSELECTEVENT( lParam );
	switch( nEvent )
	{
	case FD_READ:
		{
			int nRecvLen = recv( sock , m_szSocketBuf + m_nRestRecvLen , MAX_SOCKBUF , 0 );
			if( 0 == nRecvLen )
			{
				OutputMsg("[CLOSE] Ŭ���̾�Ʈ�� ������ ���� �Ǿ����ϴ�.");
				m_AsyncSocket.CloseSocket( sock );
				return false;
			}
			else if( -1 == nRecvLen )
			{
				OutputMsg("[ERROR] recv ���� : %d " , WSAGetLastError() );
				m_AsyncSocket.CloseSocket( sock );
			}

			m_nRestRecvLen += nRecvLen;

			//��Ŷ������ ũ�⺸�� ���� ���Դٸ�
			if( m_nRestRecvLen < 4 )
				return true;
			//��Ŷ�� ���̸� ���´�.
			int nMsgLen = 0;
			char* pCurrent = m_szSocketBuf;
			CopyMemory( &nMsgLen , pCurrent , 4 );
			//�ϳ��� ������ ��Ŷ�� ��� �޾Ҵٸ�.. 
            while( nMsgLen <= m_nRestRecvLen )
			{
				ProcessPacket( m_szSocketBuf , nMsgLen );
				m_nRestRecvLen -= nMsgLen;
				if( m_nRestRecvLen <= 0 )
					break;
				pCurrent += nMsgLen;
				CopyMemory( &nMsgLen , pCurrent , 4 );
			}
			MoveMemory( m_szSocketBuf , pCurrent , m_nRestRecvLen );

			break;
		}
	case FD_CLOSE:
		{
			OutputMsg("[CLOSE] Ŭ���̾�Ʈ ���� ���� : SOCKET(%d)", sock );
			m_AsyncSocket.CloseSocket( sock );
		}
		break;
	}
	return true;

}
void CChatClientDlg::ProcessPacket( char* pRecvBuf , int nRecvLen )
{
	unsigned short usType;
	CopyMemory( &usType , pRecvBuf + 4 , 2 );
	switch( usType )
	{
	case PACKET_CHAT:
		{
			Packet_Chat* pChat = (Packet_Chat*)pRecvBuf;
			OutputMsg("[%s] : %s", pChat->s_szIP , pChat->s_szChatMsg );
		}
		break;
	default:
		OutputMsg("[ERROR] ���ǵ��� ���� �޽��� ����");
		break;
	}

}

BOOL CChatClientDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		HWND hWnd;
		GetDlgItem( IDC_CHATMSG , &hWnd );
		if( pMsg->hwnd == hWnd )
		{
			UpdateData( TRUE );

			Packet_Chat Chat;
			Chat.s_nLength = sizeof( Packet_Chat );
			Chat.s_sType = PACKET_CHAT;
			strcpy( Chat.s_szIP , "127.0.0.1" );
			strncpy( Chat.s_szChatMsg , m_szChatMsg.GetBuffer( m_szChatMsg.GetLength() ) ,m_szChatMsg.GetLength() );
			Chat.s_szChatMsg[ m_szChatMsg.GetLength() ] = NULL;
			m_AsyncSocket.SendMsg( (char*)&Chat , sizeof( Packet_Chat ));

			m_szChatMsg="";
			UpdateData( FALSE );
			return TRUE;
		}

	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CChatClientDlg::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	
}

void CChatClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	Packet_Chat Chat;
	Chat.s_nLength = sizeof( Packet_Chat );
	Chat.s_sType = PACKET_CHAT;
	strcpy( Chat.s_szIP , "127.0.0.1" );
	strcpy( Chat.s_szChatMsg , "asfasdfsd" );
	Chat.s_szChatMsg[ m_szChatMsg.GetLength() ] = NULL;
	m_AsyncSocket.SendMsg( (char*)&Chat , sizeof( Packet_Chat ));
	CDialog::OnTimer(nIDEvent);
}
