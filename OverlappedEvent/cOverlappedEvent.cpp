#include "StdAfx.h"
#include ".\coverlappedevent.h"
#include "overlappedEventDlg.h"


//WSARecv와 WSASend의 Overlapped I/O 작업 처리
unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	cOverlappedEvent* pOverlappedEvent = (cOverlappedEvent*)p;
	pOverlappedEvent->WokerThread();
	return 0;
}
unsigned int WINAPI CallAccepterThread(LPVOID p)
{
	cOverlappedEvent* pOverlappedEvent = (cOverlappedEvent*)p;
	pOverlappedEvent->AccepterThread();
	return 0;
}

cOverlappedEvent::cOverlappedEvent(void)
{
	//변수들을 초기화한다.
	m_pMainDlg = NULL;
	m_bWorkerRun = true;
	m_bAccepterRun = true;
	m_nClientCnt = 0;
	m_hWorkerThread = NULL;
	m_hAccepterThread = NULL;
	ZeroMemory( m_szSocketBuf , 1024 );
    
	//클라이언트 구조체의 초기화
	for( int i = 0; i < WSA_MAXIMUM_WAIT_EVENTS; i++ )
	{
		m_stClientInfo.m_socketClient[ i ] = INVALID_SOCKET;
		m_stClientInfo.m_eventHandle[ i ] = WSACreateEvent();
		ZeroMemory( &m_stClientInfo.m_stOverlappedEx[ i ] , sizeof( WSAOVERLAPPED ) );
	}
}

cOverlappedEvent::~cOverlappedEvent(void)
{
	//윈속의 사용을 끝낸다.
	WSACleanup();

	//listen소켓을 닫는다.
	closesocket( m_stClientInfo.m_socketClient[ 0 ] );
	SetEvent( m_stClientInfo.m_eventHandle[ 0 ] );
	m_bWorkerRun = false;
	m_bAccepterRun = false;
	//쓰레드 종료를 기다린다
	WaitForSingleObject( m_hWorkerThread, INFINITE );
	WaitForSingleObject( m_hAccepterThread, INFINITE );
	
	
}

//소켓을 초기화하는 함수
bool cOverlappedEvent::InitSocket()
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
void cOverlappedEvent::CloseSocket( SOCKET socketClose , bool bIsForce )
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
bool cOverlappedEvent::BindandListen( int nBindPort )
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
		m_pMainDlg->OutputMsg("[에러] bind()함수 실패 : %d"
			, WSAGetLastError() );
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

bool cOverlappedEvent::CreateWokerThread()
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

bool cOverlappedEvent::CreateAccepterThread()
{
	unsigned int uiThreadId = 0;

	m_hAccepterThread = (HANDLE)_beginthreadex(NULL, 0, &CallAccepterThread, this, CREATE_SUSPENDED , &uiThreadId);
	if(m_hAccepterThread == NULL)
	{
		m_pMainDlg->OutputMsg("AccepterThread Creation Failed: %u", GetLastError());	
		return false;
	}

	ResumeThread( m_hAccepterThread );
	m_pMainDlg->OutputMsg("AccepterThread 시작..");
	return true;
}

//사용되지 않은 Index반환
int cOverlappedEvent::GetEmptyIndex()
{
	//0번째 배열은 정보갱신용 이벤트이다.
	for( int i = 1; i < WSA_MAXIMUM_WAIT_EVENTS ; i++ )
	{
		if( INVALID_SOCKET == m_stClientInfo.m_socketClient[ i ] )
			return  i;
	}
	return -1;
}

bool cOverlappedEvent::StartServer()
{
	//접속된 클라이언트 주소 정보를 저장할 구조체
	bool bRet = CreateWokerThread();
	if( false == bRet )
		return false;
	bRet = CreateAccepterThread();
	if( false == bRet )
		return false;
	
	//정보 갱신용 이벤트 생성
	m_stClientInfo.m_eventHandle[ 0 ] = WSACreateEvent();
	m_pMainDlg->OutputMsg("서버 시작");
	return true;
}

bool cOverlappedEvent::BindRecv( int nIdx )
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	m_stClientInfo.m_eventHandle[ nIdx ] = WSACreateEvent();

	//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
	m_stClientInfo.m_stOverlappedEx[ nIdx ].m_wsaOverlapped.hEvent = 
		m_stClientInfo.m_eventHandle[ nIdx ];
	m_stClientInfo.m_stOverlappedEx[ nIdx ].m_wsaBuf.len = MAX_SOCKBUF;
	m_stClientInfo.m_stOverlappedEx[ nIdx ].m_wsaBuf.buf =
		m_stClientInfo.m_stOverlappedEx[ nIdx ].m_szBuf;
	m_stClientInfo.m_stOverlappedEx[ nIdx ].m_nIdx = nIdx;
	m_stClientInfo.m_stOverlappedEx[ nIdx ].m_eOperation = OP_RECV;

	int nRet = WSARecv(	m_stClientInfo.m_socketClient[ nIdx ] , 	
					&(m_stClientInfo.m_stOverlappedEx[ nIdx ].m_wsaBuf),
					1, 
					&dwRecvNumBytes, 
					&dwFlag, 
					(LPWSAOVERLAPPED)&(m_stClientInfo.m_stOverlappedEx[ nIdx ]), 
					NULL);

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if(nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		m_pMainDlg->OutputMsg("[에러] WSARecv()함수 실패 : %d", WSAGetLastError() );
		return false;
	}
	return true;
}
bool cOverlappedEvent::SendMsg( int nIdx , char* pMsg , int nLen )
{
	DWORD dwRecvNumBytes = 0;

	//전송될 메세지를 복사
	CopyMemory( m_stClientInfo.m_stOverlappedEx[ nIdx ].m_szBuf ,
		pMsg , nLen );

	
	//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
	m_stClientInfo.m_stOverlappedEx[ nIdx ].m_wsaOverlapped.hEvent = 
		m_stClientInfo.m_eventHandle[ nIdx ];
	m_stClientInfo.m_stOverlappedEx[ nIdx ].m_wsaBuf.len = nLen;
	m_stClientInfo.m_stOverlappedEx[ nIdx ].m_wsaBuf.buf =
		m_stClientInfo.m_stOverlappedEx[ nIdx ].m_szBuf;
	m_stClientInfo.m_stOverlappedEx[ nIdx ].m_nIdx = nIdx;
	m_stClientInfo.m_stOverlappedEx[ nIdx ].m_eOperation = OP_SEND;

	int nRet = WSASend(	m_stClientInfo.m_socketClient[ nIdx ] , 	
					&(m_stClientInfo.m_stOverlappedEx[ nIdx ].m_wsaBuf),
					1, 
					&dwRecvNumBytes, 
					0, 
					(LPWSAOVERLAPPED)&(m_stClientInfo.m_stOverlappedEx[ nIdx ]), 
					NULL);

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if(nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		m_pMainDlg->OutputMsg("[에러] WSASend()함수 실패 : %d", WSAGetLastError() );
		return false;
	}
	return true;
}

//사용자의 접속을 받는 쓰레드
void cOverlappedEvent::AccepterThread()
{
	SOCKADDR_IN		stClientAddr;
	int nAddrLen = sizeof( SOCKADDR_IN );
	while( m_bAccepterRun )
	{
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
		
		bool bRet = BindRecv( nIdx );
		if( false == bRet )
			return;

		m_pMainDlg->OutputMsg( "클라이언트 접속 : IP(%s) SOCKET(%d)" , 
			inet_ntoa( stClientAddr.sin_addr ) , 
			m_stClientInfo.m_socketClient[ nIdx ] );

		//클라이언트 갯수 증가
		m_nClientCnt++;
		//클라이언트가 접속되었으므로 WokerThread로 정보갱신 알림
		WSASetEvent( m_stClientInfo.m_eventHandle[ 0 ] );

	}
}
void cOverlappedEvent::WokerThread()
{
	while( m_bWorkerRun )
	{
		//////////////////////////////////////////////////////////////////
		//요청한 Overlapped I/O작업이 완료되었는지 이벤트를 기다린다.
		//
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
		//이벤트를 리셋
		WSAResetEvent( m_stClientInfo.m_eventHandle[ dwObjIdx ] );
		//접속이 들어왔다.
		if( WSA_WAIT_EVENT_0 == dwObjIdx )
			continue;

		//Overlapped I/O에 대한 결과 처리
		OverlappedResult( dwObjIdx );
	}
}

void cOverlappedEvent::OverlappedResult( int nIdx )
{
	DWORD dwTransfer = 0;
	DWORD dwFlags = 0;

	BOOL bRet = WSAGetOverlappedResult( m_stClientInfo.m_socketClient[ nIdx ] ,
		(LPWSAOVERLAPPED)&m_stClientInfo.m_stOverlappedEx[ nIdx ] ,
		&dwTransfer ,
		FALSE,
		&dwFlags );
	if( TRUE == bRet && 0 == dwTransfer )
	{
		m_pMainDlg->OutputMsg("[에러] WSAGetOverlappedResult 실패 : %d" ,
				WSAGetLastError() );
        return;
	}

	//접속이 끊김
	if( 0 == dwTransfer  )
	{
		m_pMainDlg->OutputMsg("[접속 끊김] socket : %d" ,m_stClientInfo.m_socketClient[ nIdx ] );
		CloseSocket( m_stClientInfo.m_socketClient[ nIdx ] );
		m_nClientCnt--;
        return;
	}
	
	stOverlappedEx* pOverlappedEx = &m_stClientInfo.m_stOverlappedEx[ nIdx ];
	switch ( pOverlappedEx->m_eOperation )
	{
		//WSARecv로 Overlapped I/O가 완료되었다.
	case OP_RECV:
		{
			pOverlappedEx->m_szBuf[ dwTransfer ] = NULL;
			m_pMainDlg->OutputMsg("[수신] bytes : %d , msg : %s", dwTransfer , 
				pOverlappedEx->m_szBuf );
			
			//클라이언트에 메세지를 에코한다.
			SendMsg( nIdx , pOverlappedEx->m_szBuf , dwTransfer );
		}
		break;
		//WSASend로 Overlapped I/O가 완료되었다.
	case OP_SEND:
		{
			pOverlappedEx->m_szBuf[ dwTransfer ] = NULL;
			m_pMainDlg->OutputMsg("[송신] bytes : %d , msg : %s", dwTransfer , 
				pOverlappedEx->m_szBuf );
			//다시 Recv Overlapped I/O를 걸어준다.
			BindRecv( nIdx );
		}
		break;
	default:
		{
			m_pMainDlg->OutputMsg("정의되지 않은 Operation");
		}
		break;
	}
}
void cOverlappedEvent::DestroyThread()
{
	closesocket( m_stClientInfo.m_socketClient[ 0 ] );
	SetEvent( m_stClientInfo.m_eventHandle[ 0 ] );
	m_bWorkerRun = false;
	m_bAccepterRun = false;
	//쓰레드 종료를 기다린다
	WaitForSingleObject( m_hWorkerThread, INFINITE );
	WaitForSingleObject( m_hAccepterThread, INFINITE );
}