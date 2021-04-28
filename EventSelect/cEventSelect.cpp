#include "StdAfx.h"
#include ".\ceventselect.h"
#include "EventSelectDlg.h"

//WSARecv와 WSASend의 Overlapped I/O 작업 처리
unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	cEventSelect* pEventSelect = (cEventSelect*)p;
	pEventSelect->WokerThread();
	return 0;
}

cEventSelect::cEventSelect(void)
{
	//변수들을 초기화한다.
	m_pMainDlg = NULL;
	m_bWorkerRun = true;
	m_nClientCnt = 0;
	m_hWorkerThread = NULL;
	ZeroMemory( m_szSocketBuf , 1024 );
    
	//클라이언트 구조체의 초기화
	for( int i = 0; i < WSA_MAXIMUM_WAIT_EVENTS; i++ )
	{
		m_stClientInfo.m_socketClient[ i ] = INVALID_SOCKET;
		m_stClientInfo.m_eventHandle[ i ] = WSACreateEvent();
	}
}

cEventSelect::~cEventSelect(void)
{
	//윈속의 사용을 끝낸다.
	WSACleanup();
}

//소켓을 초기화하는 함수
bool cEventSelect::InitSocket()
{
	WSADATA wsaData;
	//윈속을 버젼 2,2로 초기화 한다.
	int nRet = WSAStartup( MAKEWORD(2,2) , &wsaData );
	if( 0 != nRet )
	{
		m_pMainDlg->OutputMsg("[에러] WSAStartup()함수 실패 : %d", WSAGetLastError() );
		return false;
	}

	//연결지향형 TCP , Overlapped I/O 소켓을 생성
	m_stClientInfo.m_socketClient[ 0 ] = WSASocket( AF_INET , SOCK_STREAM 
		, IPPROTO_TCP , NULL , NULL , WSA_FLAG_OVERLAPPED );

	if( INVALID_SOCKET == m_stClientInfo.m_socketClient[ 0 ] )
	{
		m_pMainDlg->OutputMsg("[에러] socket()함수 실패 : %d", WSAGetLastError() );
		return false;
	}
	
	//윈도우 핸들 저장
	m_pMainDlg->OutputMsg("소켓 초기화 성공");
	return true;
}

//해당 소켓을 종료 시킨다.
void cEventSelect::CloseSocket( SOCKET socketClose , bool bIsForce )
{
	struct linger stLinger = {0, 0};	// SO_DONTLINGER로 설정

	// bIsForce가 true이면 SO_LINGER, timeout = 0으로 설정하여
	// 강제 종료 시킨다. 주의 : 데이터 손실이 있을수 있음 
	if( true == bIsForce )
		stLinger.l_onoff = 1; 

	//socketClose소켓의 데이터 송수신을 모두 중단 시킨다.
	shutdown( socketClose, SD_BOTH );
	//소켓 옵션을 설정한다.
	setsockopt( socketClose, SOL_SOCKET, SO_LINGER, (char *)&stLinger, sizeof(stLinger) );
	//소켓 연결을 종료 시킨다. 
	closesocket( socketClose );
	
	socketClose = INVALID_SOCKET;
}

//서버의 주소정보를 소켓과 연결시키고 접속 요청을 받기 위해 그 소켓을 등록하는 함수
bool cEventSelect::BindandListen( int nBindPort )
{
	SOCKADDR_IN		stServerAddr;
	stServerAddr.sin_family = AF_INET;
    //서버 포트를 설정한다.
	stServerAddr.sin_port = htons( nBindPort );
	//어떤 주소에서 들어오는 접속이라도 받아들이겠다.
	//보통 서버라면 이렇게 설정한다. 만약 한 아이피에서만 접속을 받고 싶다면
	//그 주소를 inet_addr함수를 이용해 넣으면 된다.
	stServerAddr.sin_addr.s_addr = htonl( INADDR_ANY );

	//위에서 지정한 서버 주소 정보와 m_socketListen 소켓을 연결한다.
	int nRet = bind( m_stClientInfo.m_socketClient[ 0 ] 
	, (SOCKADDR*) &stServerAddr , sizeof( SOCKADDR_IN ) );
	if( 0 != nRet )
	{
		m_pMainDlg->OutputMsg("[에러] bind()함수 실패 : %d", WSAGetLastError() );
		return false;
	}

	nRet = WSAEventSelect( m_stClientInfo.m_socketClient[ 0 ] ,
		m_stClientInfo.m_eventHandle[ 0 ] ,	FD_ACCEPT | FD_CLOSE );

	if( SOCKET_ERROR == nRet )
	{
		m_pMainDlg->OutputMsg("[에러] WSAEventSelect()함수 실패 : %d", WSAGetLastError() );
		return false;
	}

	//접속 요청을 받아들이기 위해 m_socketListen소켓을 등록하고 접속대기큐를 5개로 설정 한다.
	nRet = listen( m_stClientInfo.m_socketClient[ 0 ] , 5 );
	if( 0 != nRet )
	{
		m_pMainDlg->OutputMsg("[에러] listen()함수 실패 : %d", WSAGetLastError() );
		return false;
	}
	
	m_pMainDlg->OutputMsg("서버 등록 성공..");

	return true;
}

bool cEventSelect::CreateWokerThread()
{
	unsigned int uiThreadId = 0;

	m_hWorkerThread = (HANDLE)_beginthreadex(NULL, 0, &CallWorkerThread, this, CREATE_SUSPENDED , &uiThreadId);
	if(m_hWorkerThread == NULL)
	{
		m_pMainDlg->OutputMsg("WorkerThread Creation Failed: %u", GetLastError());	
		return false;
	}

	ResumeThread( m_hWorkerThread );
	m_pMainDlg->OutputMsg("WokerThread 시작..");
	return true;
}

//사용되지 않은 Index반환
int cEventSelect::GetEmptyIndex()
{
	//0번째 배열은 정보갱신용 이벤트이다.
	for( int i = 1; i < WSA_MAXIMUM_WAIT_EVENTS ; i++ )
	{
		if( INVALID_SOCKET == m_stClientInfo.m_socketClient[ i ] )
			return  i;
	}
	return -1;
}

bool cEventSelect::StartServer()
{
	//접속된 클라이언트 주소 정보를 저장할 구조체
	bool bRet = CreateWokerThread();
	if( false == bRet )
		return false;
	
	m_pMainDlg->OutputMsg("서버 시작");
	return true;
}

void cEventSelect::WokerThread()
{
	WSANETWORKEVENTS wsaNetworkEvents;
	
	while( m_bWorkerRun )
	{
		//////////////////////////////////////////////////////////////////
		//요청한 Overlapped I/O작업이 완료되었는지 이벤트를 기다린다.
		DWORD dwObjIdx = WSAWaitForMultipleEvents( WSA_MAXIMUM_WAIT_EVENTS , 
			m_stClientInfo.m_eventHandle ,
			FALSE ,
			INFINITE,
			FALSE );
		//에러 발생
		if( WSA_WAIT_FAILED == dwObjIdx )
		{
			m_pMainDlg->OutputMsg("[에러] WSAWaitForMultipleEvents 실패 : %d" ,	WSAGetLastError() );
			break;
		}
		WSAEnumNetworkEvents( m_stClientInfo.m_socketClient[ dwObjIdx ] ,
			m_stClientInfo.m_eventHandle[ dwObjIdx ] ,
			&wsaNetworkEvents );

		//쓰레드가 종료 이벤트를 받았다면
		if( false == m_bWorkerRun && 0 == dwObjIdx )
			break;
		//접속 수락 이벤트가 발생
		if( wsaNetworkEvents.lNetworkEvents & FD_ACCEPT )
		{
			if( 0 != wsaNetworkEvents.iErrorCode[ FD_ACCEPT_BIT ] )
			{
				m_pMainDlg->OutputMsg("[에러] WSAEnumNetworkEvents 실패 : %d", WSAGetLastError() );
			}
			DoAccept( dwObjIdx );
			
		}
		//데이터가 도착했다는 이벤트 발생
		else if( wsaNetworkEvents.lNetworkEvents & FD_READ )
		{
			if( 0 != wsaNetworkEvents.iErrorCode[ FD_READ_BIT ] )
			{
				m_pMainDlg->OutputMsg("[에러] WSAEnumNetworkEvents 실패 : %d", WSAGetLastError() );
				break;
			}
			DoRecv( dwObjIdx );
		}
		//접속이 끊겼다는 이벤트 발생
		else if( wsaNetworkEvents.lNetworkEvents & FD_CLOSE )
		{
			CloseSocket( m_stClientInfo.m_socketClient[ dwObjIdx ] );
			m_pMainDlg->OutputMsg("[종료] socket [%d]클라이언트 종료", 
				m_stClientInfo.m_socketClient[ dwObjIdx ] );
			m_stClientInfo.m_socketClient[ dwObjIdx ] = INVALID_SOCKET;
		}
	}
}
void cEventSelect::DoAccept( DWORD dwObjIdx )
{
	SOCKADDR_IN		stClientAddr;
	int nAddrLen = sizeof( SOCKADDR_IN );
	//접속을 받을 구조체의 인덱스를 얻어온다.
	int nIdx = GetEmptyIndex();
	if( -1 == nIdx )
	{
		m_pMainDlg->OutputMsg("[에러] Client Full");
		return;
	}
	//클라이언트 접속 요청이 들어올 때까지 기다린다.
	m_stClientInfo.m_socketClient[ nIdx ] = accept( m_stClientInfo.m_socketClient[ 0 ], 
		(SOCKADDR*) &stClientAddr , &nAddrLen );

	if( INVALID_SOCKET == m_stClientInfo.m_socketClient[ nIdx ] )
		return;
	int nRet = WSAEventSelect( m_stClientInfo.m_socketClient[ nIdx ] ,
		m_stClientInfo.m_eventHandle[ nIdx ] ,
		FD_READ | FD_CLOSE );
	if( SOCKET_ERROR == nRet )
	{
		m_pMainDlg->OutputMsg("[에러] WSAEventSelect()함수 실패 : %d", WSAGetLastError() );
		return;
	}
	//클라이언트 갯수 증가
	m_nClientCnt++;
	m_pMainDlg->OutputMsg( "클라이언트 접속 : IP(%s) SOCKET(%d)" , 
		inet_ntoa( stClientAddr.sin_addr ) , 
		m_stClientInfo.m_socketClient[ nIdx ] );
}

void cEventSelect::DoRecv( DWORD dwObjIdx )
{
	int nRecvLen = recv( m_stClientInfo.m_socketClient[ dwObjIdx ] , m_szSocketBuf , MAX_SOCKBUF , 0 );
	if( 0 == nRecvLen )
	{
		m_pMainDlg->OutputMsg("클라이언트와 연결이 종료 되었습니다.");
		CloseSocket( m_stClientInfo.m_socketClient[ dwObjIdx ] );
		return;
	}
	else if( -1 == nRecvLen )
	{
		m_pMainDlg->OutputMsg("[에러] recv 실패 : %d " , WSAGetLastError() );
		CloseSocket( m_stClientInfo.m_socketClient[ dwObjIdx ] );
	}
	m_szSocketBuf[ nRecvLen ] = NULL;
	m_pMainDlg->OutputMsg( "socket[%d] , 메세지 수신 : %d bytes , 내용 : %s" 
		, m_stClientInfo.m_socketClient[ dwObjIdx ] , nRecvLen , m_szSocketBuf );

	int nSendLen = send( m_stClientInfo.m_socketClient[ dwObjIdx ] , m_szSocketBuf , nRecvLen , 0 );
	if( -1 == nSendLen )
	{
		m_pMainDlg->OutputMsg("[에러] send 실패 : %d " , WSAGetLastError() );
		CloseSocket( m_stClientInfo.m_socketClient[ dwObjIdx ] );
		return;
	}
	m_pMainDlg->OutputMsg( "socket[%d] , 메세지 송신 : %d bytes , 내용 : %s" 
		, m_stClientInfo.m_socketClient[ dwObjIdx ] , nSendLen , m_szSocketBuf );
}

void cEventSelect::DestroyThread()
{
	m_bWorkerRun = false;
	closesocket( m_stClientInfo.m_socketClient[ 0 ] );
	SetEvent( m_stClientInfo.m_eventHandle[ 0 ] );
	//쓰레드 종료를 기다린다
	WaitForSingleObject( m_hWorkerThread, INFINITE );
}