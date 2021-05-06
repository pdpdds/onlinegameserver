lpConnection// ChatClientDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include ".\chatclientdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
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


// CChatClientDlg 대화 상자



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


// CChatClientDlg 메시지 처리기

BOOL CChatClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	//소켓 초기화
	
	m_AsyncSocket.SetMainDlg( this );
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CChatClientDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CChatClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//출력 메세지
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

	//에러체크
	int nError = WSAGETSELECTERROR( lParam );
	if( 0 != nError )
	{
		OutputMsg( "[에러] WSAGETSELECTERROR : %d ", nError );
		m_AsyncSocket.CloseSocket( sock );
		return false;
	}
	
	//이벤트 체크
	int nEvent = WSAGETSELECTEVENT( lParam );
	switch( nEvent )
	{
	case FD_READ:
		{
			int nRecvLen = recv( sock , m_szSocketBuf + m_nRestRecvLen , MAX_SOCKBUF , 0 );
			if( 0 == nRecvLen )
			{
				OutputMsg("[CLOSE] 클라이언트와 연결이 종료 되었습니다.");
				m_AsyncSocket.CloseSocket( sock );
				return false;
			}
			else if( -1 == nRecvLen )
			{
				OutputMsg("[ERROR] recv 실패 : %d " , WSAGetLastError() );
				m_AsyncSocket.CloseSocket( sock );
			}

			m_nRestRecvLen += nRecvLen;

			//패킷길이의 크기보다 적게 들어왔다면
			if( m_nRestRecvLen < 4 )
				return true;
			//패킷의 길이를 얻어온다.
			int nMsgLen = 0;
			char* pCurrent = m_szSocketBuf;
			CopyMemory( &nMsgLen , pCurrent , 4 );
			//하나의 완전한 패킷을 모두 받았다면.. 
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
			OutputMsg("[CLOSE] 클라이언트 접속 종료 : SOCKET(%d)", sock );
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
		OutputMsg("[ERROR] 정의되지 않은 메시지 도착");
		break;
	}

}

BOOL CChatClientDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

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
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	
}

void CChatClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	Packet_Chat Chat;
	Chat.s_nLength = sizeof( Packet_Chat );
	Chat.s_sType = PACKET_CHAT;
	strcpy( Chat.s_szIP , "127.0.0.1" );
	strcpy( Chat.s_szChatMsg , "asfasdfsd" );
	Chat.s_szChatMsg[ m_szChatMsg.GetLength() ] = NULL;
	m_AsyncSocket.SendMsg( (char*)&Chat , sizeof( Packet_Chat ));
	CDialog::OnTimer(nIDEvent);
}
