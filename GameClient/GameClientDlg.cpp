// GameClientDlg.cpp : ���� ����
//


#include "stdafx.h"
#include "HelpDlg.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include ".\gameclientdlg.h"

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


// CGameClientDlg ��ȭ ����



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


// CGameClientDlg �޽��� ó����

BOOL CGameClientDlg::OnInitDialog()
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

	AsyncSocket()->SetMainDlg( this );
	//100���� �÷��̾ �����Ѵ�.
	PlayerManager()->CreatePlayer( 100 );
	
	m_pDC = GetDC();
	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	
	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CGameClientDlg::OnPaint() 
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
		
		DrawField( m_pDC );
		PlayerManager()->DrawPlayer( m_pDC );
		NpcManager()->DrawNpc( m_pDC );
		
		
	}
}
//��� �޼���
void CGameClientDlg::OutputMsg( char *szOutputString , ... )
{
	char szOutStr[ 1024 ];
	va_list	argptr; 
    va_start( argptr, szOutputString );
	vsprintf( szOutStr , szOutputString, argptr );
	va_end( argptr );
	m_ctOutput.SetCurSel( m_ctOutput.AddString( szOutStr ) );
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
HCURSOR CGameClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGameClientDlg::DrawField( CDC* pDC )
{
	/////////////////////////////////////////////////////
	//������ ���� ���� ���� ����
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
	//�ʷϻ� ���� �߰� ����
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
	//Ŭ���̾�Ʈ�� �����ִ� ���� �� �׸� �ڽ��� �׸���
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
			//������ ���� ������ �Ʒ� �밢������ �׸� �׸���
			pDC->FrameRect( &CRect( ( nStartX - nStartY )+ ( i * TILE_SIZE )  + START_XPOS , START_YPOS + ( i * TILE_SIZE ) , 
				( nStartX - nStartY )+ ( i * TILE_SIZE )  + START_XPOS + TILE_SIZE , START_YPOS + ( i * TILE_SIZE ) + TILE_SIZE )  , NULL ) ;
		}
	}
	else if( nPosX < nPosY )
	{
		nCnt = ROW_LINE - ( nPosY - nPosX );
		for(int  i = 0 ; i < nCnt ; i++ )
		{
			//������ ���� ������ �Ʒ� �밢������ �׸� �׸���
			pDC->FrameRect( &CRect( ( i * TILE_SIZE )  + START_XPOS , ( nStartY - nStartX )+ START_YPOS + ( i * TILE_SIZE ) , 
				( i * TILE_SIZE )  + START_XPOS + TILE_SIZE ,( nStartY - nStartX )+ START_YPOS + ( i * TILE_SIZE ) + TILE_SIZE )  , NULL ) ;
		}
	}

	if( ( nPosX + nPosY ) <= COL_LINE )
	{
		nCnt = nPosX+nPosY + 1;
		for(int  i = 0 ; i < nCnt ; i++ )
		{
			//������ ������ ���� �Ʒ� �밢������ �׸� �׸���
			pDC->FrameRect( &CRect( ( nStartY + nStartX ) - ( i * TILE_SIZE )  + START_XPOS ,  START_YPOS + ( i * TILE_SIZE ) , 
				( nStartY + nStartX ) - ( i * TILE_SIZE )+ START_XPOS + TILE_SIZE , START_YPOS + ( i * TILE_SIZE ) + TILE_SIZE )  , NULL ) ;
		}
	}
	else if( ( nPosX + nPosY ) > COL_LINE )
	{
		nCnt = ( COL_LINE * 2 ) - ( nPosX + nPosY ) - 1;
		for(int  i = 0 ; i < nCnt ; i++ )
		{
			//������ ������ ���� �Ʒ� �밢������ �׸� �׸���
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
	//�α� ��Ŷ�� ������ ������.
	LoginPlayerRq Login;
	Login.s_nLength = sizeof( LoginPlayerRq );
	Login.s_sType = LoginPlayer_Rq;
	AsyncSocket()->SendMsg( (char*)&Login , sizeof( LoginPlayerRq ));
	//���� ���� ��Ŷ�� ������ ���� Ÿ�̸� ����
	SetTimer( 2 , 1000 , NULL );

}
LRESULT CGameClientDlg::OnSocketMsg( WPARAM wParam , LPARAM lParam )
{

	SOCKET sock = (SOCKET)wParam;

	//����üũ
	int nError = WSAGETSELECTERROR( lParam );
	if( 0 != nError )
	{
		OutputMsg( "[����] WSAGETSELECTERROR : %d ", nError );
		AsyncSocket()->CloseSocket();
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
				AsyncSocket()->CloseSocket();
				return false;
			}
			else if( -1 == nRecvLen )
			{
				OutputMsg("[ERROR] recv ���� : %d " , WSAGetLastError() );
				AsyncSocket()->CloseSocket();
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
			OutputMsg("[CLOSE] Ŭ���̾�Ʈ ���� ���� : SOCKET(%d)", sock );
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
		//���忡 �ִ� �÷��̾� �������� �޴´�.
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

		OutputMsg("[ERROR] ���ǵ��� ���� �޽��� ����");
		break;
	}

}


void CGameClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	//ȭ�� ���� �̶��
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
	//KeepAlive��Ŷ�� ������ ���ؼ�
	else if( nIDEvent == 2 )
	{
		if( AsyncSocket()->GetSocket() == INVALID_SOCKET )
		{
			KillTimer( 2 );
			return;
		}
		///////////////////////////////////////////
		//���� ������ ���� KeepAlive��Ŷ�� ���� ������ ������.
		KeepAliveCn KeepCn;
		KeepCn.s_nLength = sizeof( KeepAliveCn );
		KeepCn.s_sType = KeepAlive_Cn;
		AsyncSocket()->SendMsg( (char*)&KeepCn , sizeof( KeepAliveCn ));
	}
	CDialog::OnTimer(nIDEvent);
}

void CGameClientDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	//���콺�� Ŭ���� ��ǥ�� �ʵ带 �Ѿ ��� ����	
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
	//�밢��,����, ���ΰ� �������� �ߴٸ�
	if( ( nDestPosX == nPosX ) || 
		( nDestPosY == nPosY ) || 
		( abs(( nPosX - nDestPosX )) == abs(( nPosY - nDestPosY )) ) )
	{
		pMyPlayer->SetTPos( (nDestPosY * COL_LINE) +nDestPosX );
		SetTimer( 1, UPDATETIME , NULL );
		///////////////////////////////////////////
		//������  ��Ŷ�� ������ ������.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CGameClientDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	//���콺�� Ŭ���� ��ǥ�� �ʵ带 �Ѿ ��� ����	
	if( ( INVALIDATERECT + START_XPOS ) < point.x || 
		( INVALIDATERECT + START_YPOS ) < point.y )
		return;

	int nDestPosX =  ( point.x - START_XPOS ) / TILE_SIZE;
	int nDestPosY =  ( point.y - START_YPOS ) / TILE_SIZE;
	DWORD dwDestPos = ( nDestPosY * COL_LINE ) + nDestPosX;
	//Ŭ���� ���� �÷��̾��� ������ ���� ����Ѵ�
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
	//Ŭ���� NPC�� ������ ���� ����Ѵ�.
	cNpc* pNpc = NpcManager()->GetNpcByPos( dwDestPos );
	if( NULL != pNpc )
	{
		SetDlgItemText( IDC_NNAME , pNpc->GetName() );
		SetDlgItemInt( IDC_NPOS , pNpc->GetPos() );
		SetDlgItemInt( IDC_NKEY , pNpc->GetKey() );
		if( pNpc->GetType() == DETECT_NPC )
			SetDlgItemText( IDC_NTYPE , "������" );
		else
			SetDlgItemText( IDC_NTYPE , "�İ���" );
		if( pNpc->GetState() == NPC_NORMAL )
			SetDlgItemText( IDC_NSTATE , "�¼�" );
		else if( pNpc->GetState() == NPC_DISCOMPORT )
			SetDlgItemText( IDC_NSTATE , "���" );
		else if( pNpc->GetState() == NPC_ANGRY )
			SetDlgItemText( IDC_NSTATE , "�г�" );
		m_dwDetailNpcKey = pNpc->GetKey();
	}


	CDialog::OnRButtonDown(nFlags, point);
}

void CGameClientDlg::OnBnClickedHelp()
{
	CHelpDlg HelpDlg;
	HelpDlg.DoModal();
	
}
