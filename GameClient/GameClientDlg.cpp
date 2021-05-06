// GameClientDlg.cpp : 구현 파일
//


#include "stdafx.h"
#include "HelpDlg.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include ".\gameclientdlg.h"

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


// CGameClientDlg 대화 상자



CGameClientDlg::CGameClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGameClientDlg::IDD, pParent)
	, m_szIp(_T("127.0.0.1"))
	, m_nPort(8080)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nRestRecvLen = 0;
	m_dwDetailPlayerKey = 0;
	m_dwDetailNpcKey = 0;
	m_TickThread.CreateThread( 1500 );
}

void CGameClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_szIp);
	DDX_Text(pDX, IDC_EDIT2, m_nPort);
	DDX_Control(pDX, IDC_OUTPUT, m_ctOutput);
}

BEGIN_MESSAGE_MAP(CGameClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_MESSAGE( WM_SOCKETMSG , OnSocketMsg )

	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	
	ON_BN_CLICKED(IDC_HELPDLG, OnBnClickedHelp)
END_MESSAGE_MAP()


// CGameClientDlg 메시지 처리기

BOOL CGameClientDlg::OnInitDialog()
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

	AsyncSocket()->SetMainDlg( this );
	//100명의 플레이어를 생성한다.
	PlayerManager()->CreatePlayer( 100 );
	
	m_pDC = GetDC();
	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CGameClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGameClientDlg::OnPaint() 
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
		
		DrawField( m_pDC );
		PlayerManager()->DrawPlayer( m_pDC );
		NpcManager()->DrawNpc( m_pDC );
		
		
	}
}
//출력 메세지
void CGameClientDlg::OutputMsg( char *szOutputString , ... )
{
	char szOutStr[ 1024 ];
	va_list	argptr; 
    va_start( argptr, szOutputString );
	vsprintf( szOutStr , szOutputString, argptr );
	va_end( argptr );
	m_ctOutput.SetCurSel( m_ctOutput.AddString( szOutStr ) );
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CGameClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGameClientDlg::DrawField( CDC* pDC )
{
	/////////////////////////////////////////////////////
	//검은선 격자 제일 작은 영역
	for( int i = 0 ; i < COL_LINE + 1; i++ )
	{
		pDC->MoveTo( CPoint( START_XPOS , START_YPOS + ( i * TILE_SIZE ) ) );
		pDC->LineTo( CPoint( ( ROW_LINE * TILE_SIZE ) + START_XPOS  , START_YPOS + ( i * TILE_SIZE ) ) );
	}

	for(int  i = 0 ; i < ROW_LINE + 1 ; i++ )
	{
		pDC->MoveTo( CPoint( START_XPOS + ( i * TILE_SIZE ) , START_YPOS ) );
		pDC->LineTo( CPoint( START_XPOS + ( i * TILE_SIZE ), ( (COL_LINE + 1)* TILE_SIZE ) + START_XPOS ) );
	}

	//////////////////////////////////////////////////////
	//초록색 격자 중간 영역
	CPen pen;
	LOGPEN logpen;
	logpen.lopnStyle = PS_SOLID;
	logpen.lopnColor = RGB( 0,180,120);
	logpen.lopnWidth = CPoint( 1,0 );
	pen.CreatePenIndirect( &logpen );
	
	CPen* oldPen = pDC->SelectObject( &pen );
	for(int  i = 0 ; i < (COL_LINE / (CELLCOL_LINE )) + 1; i++ )
	{
		pDC->MoveTo( CPoint( START_XPOS , START_YPOS + ( i * TILE_SIZE )  + ( (i*( CELLCOL_LINE - 1 )) * TILE_SIZE ) ) );
		pDC->LineTo( CPoint( ( ROW_LINE * TILE_SIZE ) + START_XPOS  , START_YPOS + ( i * TILE_SIZE ) + ( (i*( CELLCOL_LINE - 1 )) * TILE_SIZE ) )) ;
	}

	for(int  i = 0 ; i < (ROW_LINE / (CELLROW_LINE )) + 1 ; i++ )
	{
		pDC->MoveTo( CPoint( START_XPOS + ( i * TILE_SIZE ) + ( (i*( CELLROW_LINE - 1 )) * TILE_SIZE ), START_YPOS ) );
		pDC->LineTo( CPoint( START_XPOS + ( i * TILE_SIZE )+ ( (i*( CELLROW_LINE - 1 )) * TILE_SIZE ), ( (COL_LINE + 1) * TILE_SIZE ) + START_XPOS ) );
	}
	DeleteObject( &pen );
	pDC->SelectObject( oldPen );
	
	
	///////////////////////////////////////////////////////
	//클라이언트가 갈수있는 영역 흰 네모 박스로 그리기
	cPlayer* pMyPlayer = PlayerManager()->GetMyPlayer();
	if( NULL == pMyPlayer )
		return;
	int nPosX = pMyPlayer->GetPos() % COL_LINE;
	int nPosY = pMyPlayer->GetPos() / COL_LINE;
	int nStartX = nPosX * TILE_SIZE;
	int nStartY = nPosY * TILE_SIZE;
		
	for(int  i = 0 ; i < COL_LINE ; i++ )
	{
		pDC->FrameRect( &CRect( nStartX + START_XPOS , START_YPOS + ( i * TILE_SIZE ) , 
			nStartX + TILE_SIZE + START_XPOS, START_YPOS + ( i * TILE_SIZE ) + TILE_SIZE )  , NULL ) ;
	}
	for(int  i = 0 ; i < ROW_LINE ; i++ )
	{
		pDC->FrameRect( &CRect( ( i * TILE_SIZE )  + START_XPOS , START_YPOS + nStartY , 
			 ( i * TILE_SIZE )  + TILE_SIZE + START_XPOS, START_YPOS + nStartY + TILE_SIZE )  , NULL ) ;
	}
	int nCnt = 0;
	if( nPosX >= nPosY )
	{
		nCnt = COL_LINE - ( nPosX - nPosY );
		for(int  i = 0 ; i < nCnt ; i++ )
		{
			//왼쪽위 부터 오른쪽 아래 대각선까지 네모를 그리기
			pDC->FrameRect( &CRect( ( nStartX - nStartY )+ ( i * TILE_SIZE )  + START_XPOS , START_YPOS + ( i * TILE_SIZE ) , 
				( nStartX - nStartY )+ ( i * TILE_SIZE )  + START_XPOS + TILE_SIZE , START_YPOS + ( i * TILE_SIZE ) + TILE_SIZE )  , NULL ) ;
		}
	}
	else if( nPosX < nPosY )
	{
		nCnt = ROW_LINE - ( nPosY - nPosX );
		for(int  i = 0 ; i < nCnt ; i++ )
		{
			//왼쪽위 부터 오른쪽 아래 대각선까지 네모를 그리기
			pDC->FrameRect( &CRect( ( i * TILE_SIZE )  + START_XPOS , ( nStartY - nStartX )+ START_YPOS + ( i * TILE_SIZE ) , 
				( i * TILE_SIZE )  + START_XPOS + TILE_SIZE ,( nStartY - nStartX )+ START_YPOS + ( i * TILE_SIZE ) + TILE_SIZE )  , NULL ) ;
		}
	}

	if( ( nPosX + nPosY ) <= COL_LINE )
	{
		nCnt = nPosX+nPosY + 1;
		for(int  i = 0 ; i < nCnt ; i++ )
		{
			//오른쪽 위부터 왼쪽 아래 대각선까지 네모를 그리기
			pDC->FrameRect( &CRect( ( nStartY + nStartX ) - ( i * TILE_SIZE )  + START_XPOS ,  START_YPOS + ( i * TILE_SIZE ) , 
				( nStartY + nStartX ) - ( i * TILE_SIZE )+ START_XPOS + TILE_SIZE , START_YPOS + ( i * TILE_SIZE ) + TILE_SIZE )  , NULL ) ;
		}
	}
	else if( ( nPosX + nPosY ) > COL_LINE )
	{
		nCnt = ( COL_LINE * 2 ) - ( nPosX + nPosY ) - 1;
		for(int  i = 0 ; i < nCnt ; i++ )
		{
			//오른쪽 위부터 왼쪽 아래 대각선까지 네모를 그리기
			pDC->FrameRect( 
				&CRect( ( ( COL_LINE - 1 )* TILE_SIZE ) - ( i * TILE_SIZE ) + START_XPOS , 
				( nStartY + nStartX )  -( ( COL_LINE - 1 ) * TILE_SIZE ) + START_YPOS + ( i * TILE_SIZE ) , 
				( ( COL_LINE - 1 ) * TILE_SIZE ) - ( i * TILE_SIZE ) + START_XPOS + TILE_SIZE ,
				( nStartY + nStartX )  -( ( COL_LINE - 1 ) * TILE_SIZE ) + START_YPOS + ( i * TILE_SIZE ) + TILE_SIZE )  , NULL ) ;
		}
	}
	
}

void CGameClientDlg::OnBnClickedOk()
{
	UpdateData( FALSE );
	AsyncSocket()->InitSocket( m_hWnd );
	AsyncSocket()->ConnectTo( m_szIp.GetBuffer( m_szIp.GetLength() ), m_nPort );

	///////////////////////////////////////////
	//로긴 패킷을 서버로 보낸다.
	LoginPlayerRq Login;
	Login.s_nLength = sizeof( LoginPlayerRq );
	Login.s_sType = LoginPlayer_Rq;
	AsyncSocket()->SendMsg( (char*)&Login , sizeof( LoginPlayerRq ));
	//연결 유지 패킷을 보내기 위해 타이머 설정
	SetTimer( 2 , 1000 , NULL );

}
LRESULT CGameClientDlg::OnSocketMsg( WPARAM wParam , LPARAM lParam )
{

	SOCKET sock = (SOCKET)wParam;

	//에러체크
	int nError = WSAGETSELECTERROR( lParam );
	if( 0 != nError )
	{
		OutputMsg( "[에러] WSAGETSELECTERROR : %d ", nError );
		AsyncSocket()->CloseSocket();
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
				AsyncSocket()->CloseSocket();
				return false;
			}
			else if( -1 == nRecvLen )
			{
				OutputMsg("[ERROR] recv 실패 : %d " , WSAGetLastError() );
				AsyncSocket()->CloseSocket();
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
				ProcessPacket( pCurrent , nMsgLen );
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
			AsyncSocket()->CloseSocket();
		}
		break;
	}
	return true;

}
void CGameClientDlg::ProcessPacket( char* pRecvBuf , int nRecvLen )
{
	unsigned short usType;
	CopyMemory( &usType , pRecvBuf + 4 , 2 );
	switch( usType )
	{
	case MyPlayerInfo_Aq:
		{
			cProcessPacket::fnMyPlayerInfoAq( this ,pRecvBuf , nRecvLen );
			Invalidate();
		}
		break;
		//월드에 있는 플레이어 정보들을 받는다.
	case WorldPlayerInfo_VAq:
		{
			cProcessPacket::fnWorldPlayerInfoVAq( this , pRecvBuf , nRecvLen );
			Invalidate();
		}
		break;
	case LoginPlayer_Sn:
		{
			cProcessPacket::fnLoginPlayerSn( this ,pRecvBuf , nRecvLen );
			Invalidate();
		}
		break;
	case LogoutPlayer_Sn:
		{
			cProcessPacket::fnLogoutPlayerSn( this ,pRecvBuf , nRecvLen );
			Invalidate();
		}
		break;
	case MovePlayer_Sn:
		{
			cProcessPacket::fnMovePlayerSn( this ,pRecvBuf , nRecvLen );
			SetTimer( 1 , UPDATETIME , NULL );
		}
		break;
	case NPC_NpcInfo_VSn:
		{
			cProcessPacket::fnNPCNpcInfoVSn( this ,pRecvBuf , nRecvLen );
		}
		break;
	case NPC_UpdateNpc_VSn:
		{
			cProcessPacket::fnNPCUpdateNpcVSn( this ,pRecvBuf , nRecvLen );
			Invalidate();
		}
        break;
	case NPC_AttackNpcToPlayer_Sn:
		{
			cProcessPacket::fnAttackNpcToPlayerSn( this ,pRecvBuf , nRecvLen );
		}
		break;
	default:

		OutputMsg("[ERROR] 정의되지 않은 메시지 도착");
		break;
	}

}


void CGameClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	//화면 갱신 이라면
	if( nIDEvent == 1 )
	{
		cPlayer* pMyPlayer = PlayerManager()->GetMyPlayer();
		if( NULL == pMyPlayer )
			return;
		bool bRet = PlayerManager()->UpdatePlayersPos();
		if( false == bRet )
		{
			KillTimer( 1 );
		}

		SetDlgItemInt( IDC_POS , pMyPlayer->GetPos() );
		InvalidateRect( CRect( START_XPOS , START_YPOS , START_XPOS + INVALIDATERECT ,
			START_YPOS + INVALIDATERECT)  );
	}
	//KeepAlive패킷을 보내기 위해서
	else if( nIDEvent == 2 )
	{
		if( AsyncSocket()->GetSocket() == INVALID_SOCKET )
		{
			KillTimer( 2 );
			return;
		}
		///////////////////////////////////////////
		//연결 유지를 위해 KeepAlive패킷을 게임 서버에 보낸다.
		KeepAliveCn KeepCn;
		KeepCn.s_nLength = sizeof( KeepAliveCn );
		KeepCn.s_sType = KeepAlive_Cn;
		AsyncSocket()->SendMsg( (char*)&KeepCn , sizeof( KeepAliveCn ));
	}
	CDialog::OnTimer(nIDEvent);
}

void CGameClientDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	//마우스를 클릭한 좌표가 필드를 넘어갈 경우 무시	
	if( ( INVALIDATERECT + START_XPOS ) < point.x || 
		( INVALIDATERECT + START_YPOS ) < point.y )
		return;
	cPlayer* pMyPlayer = PlayerManager()->GetMyPlayer();
	if( NULL == pMyPlayer )
		return;
	int nPosX = pMyPlayer->GetPos() % COL_LINE;
	int nPosY = pMyPlayer->GetPos() / COL_LINE;
	int nDestPosX =  ( point.x - START_XPOS ) / TILE_SIZE;
	int nDestPosY =  ( point.y - START_YPOS ) / TILE_SIZE;
	//대각선,가로, 세로가 방향으로 했다면
	if( ( nDestPosX == nPosX ) || 
		( nDestPosY == nPosY ) || 
		( abs(( nPosX - nDestPosX )) == abs(( nPosY - nDestPosY )) ) )
	{
		pMyPlayer->SetTPos( (nDestPosY * COL_LINE) +nDestPosX );
		SetTimer( 1, UPDATETIME , NULL );
		///////////////////////////////////////////
		//움직임  패킷을 서버로 보낸다.
		MovePlayerCn Move;
		Move.s_nLength = sizeof( MovePlayerCn );
		Move.s_sType = MovePlayer_Cn;
		Move.s_dwCPos = pMyPlayer->GetPos();
		Move.s_dwTPos = pMyPlayer->GetTPos();
		AsyncSocket()->SendMsg( (char*)&Move , sizeof( MovePlayerCn ));
		
	}
	CDialog::OnLButtonDown(nFlags, point);
}



void CGameClientDlg::OnMouseMove(UINT nFlags, CPoint point)
{

	CDialog::OnMouseMove(nFlags, point);
}

void CGameClientDlg::OnDestroy()
{
	CDialog::OnDestroy();
	cSingleton::releaseAll();
	m_TickThread.DestroyThread();
	m_pDC->DeleteDC();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CGameClientDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	//마우스를 클릭한 좌표가 필드를 넘어갈 경우 무시	
	if( ( INVALIDATERECT + START_XPOS ) < point.x || 
		( INVALIDATERECT + START_YPOS ) < point.y )
		return;

	int nDestPosX =  ( point.x - START_XPOS ) / TILE_SIZE;
	int nDestPosY =  ( point.y - START_YPOS ) / TILE_SIZE;
	DWORD dwDestPos = ( nDestPosY * COL_LINE ) + nDestPosX;
	//클릭된 상대방 플레이어의 정보를 얻어와 출력한다
	cPlayer* pTagetPlayer = PlayerManager()->GetPlayerByPos( dwDestPos );
	if( NULL != pTagetPlayer )
	{
		SetDlgItemText( IDC_TID , pTagetPlayer->GetId() );
		SetDlgItemText( IDC_TNAME , pTagetPlayer->GetName() );
		SetDlgItemText( IDC_TNICKNAME , pTagetPlayer->GetNickName() );
		SetDlgItemInt( IDC_TLEVEL , pTagetPlayer->GetLevel() );
		SetDlgItemInt( IDC_TDUR , pTagetPlayer->GetDur() );
		SetDlgItemInt( IDC_TSTR , pTagetPlayer->GetStr() );
		SetDlgItemInt( IDC_THP , pTagetPlayer->GetHp() );
		SetDlgItemInt( IDC_TPOS , pTagetPlayer->GetPos() );
		SetDlgItemInt( IDC_TEXP , pTagetPlayer->GetExp() );
		SetDlgItemInt( IDC_TPKEY , pTagetPlayer->GetPKey() );
		m_dwDetailPlayerKey = pTagetPlayer->GetPKey();
	}
	//클릭된 NPC의 정보를 얻어와 출력한다.
	cNpc* pNpc = NpcManager()->GetNpcByPos( dwDestPos );
	if( NULL != pNpc )
	{
		SetDlgItemText( IDC_NNAME , pNpc->GetName() );
		SetDlgItemInt( IDC_NPOS , pNpc->GetPos() );
		SetDlgItemInt( IDC_NKEY , pNpc->GetKey() );
		if( pNpc->GetType() == DETECT_NPC )
			SetDlgItemText( IDC_NTYPE , "선공격" );
		else
			SetDlgItemText( IDC_NTYPE , "후공격" );
		if( pNpc->GetState() == NPC_NORMAL )
			SetDlgItemText( IDC_NSTATE , "온순" );
		else if( pNpc->GetState() == NPC_DISCOMPORT )
			SetDlgItemText( IDC_NSTATE , "흥분" );
		else if( pNpc->GetState() == NPC_ANGRY )
			SetDlgItemText( IDC_NSTATE , "분노" );
		m_dwDetailNpcKey = pNpc->GetKey();
	}


	CDialog::OnRButtonDown(nFlags, point);
}

void CGameClientDlg::OnBnClickedHelp()
{
	CHelpDlg HelpDlg;
	HelpDlg.DoModal();
	
}
