// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "GameServer.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_SERVER_START, OnServerStart)
	ON_COMMAND(ID_SERVER_STOP, OnServerStop)
	ON_COMMAND(ID_CONNECT_NPCSERVER, OnConnectNpcServer)
	ON_COMMAND(ID_SERVER_CREATETEMPPLAYER, OnCreateTempPlayer)
	ON_COMMAND(ID_SERVER_DESTROYTEMPPLAYER, OnDestroyTempPlayer)
	
	ON_MESSAGE( WM_DEBUGMSG , OnDebugMsg )
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	IocpGameServer();
	LogView();
}

CMainFrame::~CMainFrame()
{
	cSingleton::releaseAll();
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.
	return TRUE;
	
}


// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 메시지 처리기


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CVisualObject	*m_pTab1 = new CVisualObject( 1,"",pContext,RUNTIME_CLASS(CTabWnd),CVisualObject::TOS_TABBOTTOM );
	// each views
	CVisualObject	*m_pUserLogView = new CVisualObject(2,"LOG",pContext,RUNTIME_CLASS(CMsgViewFromLog));
	// add to frame
	m_frameWork.Add( m_pTab1 );
	m_frameWork.Add( m_pTab1,m_pUserLogView );
	
	delete m_pTab1->GetWnd();
	delete m_pUserLogView->GetWnd();
	return m_frameWork.Create( this );
}

void CMainFrame::OnServerStart()
{
	IocpGameServer()->ServerStart();
}

void CMainFrame::OnServerStop()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	
}

void CMainFrame::OnConnectNpcServer()
{
	IocpGameServer()->ConnectToNpcServer();
}

void CMainFrame::OnCreateTempPlayer()
{
	PlayerManager()->CreateTempPlayer( MAX_TEMPPLAYER );
}

void CMainFrame::OnDestroyTempPlayer()
{
	PlayerManager()->DestroyTempPlayer();
}

LRESULT CMainFrame::OnDebugMsg( WPARAM wParam , LPARAM lParam )
{
	char* pMsg = (char*)wParam;
	pMsg[ strlen( pMsg ) - 2 ] = NULL;		//맨뒤에 \n 문자 제거를 위해
	LogView()->OutputDebugMsg( (enumLogInfoType)lParam , (char*)wParam );
	return true;
}
