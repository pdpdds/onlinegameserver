#include "StdAfx.h"
#include ".\ciocompletionport.h"
#include "IOCompletionPortDlg.h"

//WSARecv와 WSASend의 Overlapped I/O 작업 처리를 위한 쓰레드
unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	cIOCompletionPort* pOverlappedEvent = (cIOCompletionPort*)p;
	pOverlappedEvent->WokerThread();
	return 0;
}

//Client의 접속을 받기위한 쓰레드
unsigned int WINAPI CallAccepterThread(LPVOID p)
{
	cIOCompletionPort* pOverlappedEvent = (cIOCompletionPort*)p;
	pOverlappedEvent->AccepterThread();
	return 0;
}

cIOCompletionPort::cIOCompletionPort(void)
{
	////////////////////////////////////////////////////
	//모든 멤버 변수들 초기화
	m_pMainDlg = NULL;
	m_bWorkerRun = true;
	m_bAccepterRun = true;
	m_nClientCnt = 0;
	m_hAccepterThread = NULL;
	m_hIOCP = NULL;
	m_socketListen = INVALID_SOCKET;
	ZeroMemory( m_szSocketBuf , 1024 );
	for( int i = 0; i < MAX_WORKERTHREAD; i++ )
		m_hWorkerThread[ i ] = NULL;

	//client정보를 저장한 구조체를 생성
	m_pClientInfo = new stClientInfo[ MAX_CLIENT ];
 }

cIOCompletionPort::~cIOCompletionPort(void)
{
	//윈속의 사용을 끝낸다.
	WSACleanup();
	//다 사용한 객체를 삭제
	if( m_pClientInfo )
	{
		delete [] m_pClientInfo;
		m_pClientInfo = NULL;
	}
}

//소켓을 초기화하는 함수
bool cIOCompletionPort::InitSocket()
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
	m_socketListen = WSASocket( AF_INET , SOCK_STREAM 
		, IPPROTO_TCP , NULL , NULL , WSA_FLAG_OVERLAPPED );

	if( INVALID_SOCKET == m_socketListen )
	{
		m_pMainDlg->OutputMsg("[에러] socket()함수 실패 : %d", WSAGetLastError() );
		return false;
	}
		
	m_pMainDlg->OutputMsg("소켓 초기화 성공");
	return true;
}

//해당 소켓을 종료 시킨다.
void cIOCompletionPort::CloseSocket( stClientInfo* pClientInfo 
									, bool bIsForce )
{
	struct linger stLinger = {0, 0};	// SO_DONTLINGER로 설정

	// bIsForce가 true이면 SO_LINGER, timeout = 0으로 설정하여
	// 강제 종료 시킨다. 주의 : 데이터 손실이 있을수 있음 
	if( true == bIsForce )
		stLinger.l_onoff = 1; 

	//socketClose소켓의 데이터 송수신을 모두 중단 시킨다.
	shutdown( pClientInfo->m_socketClient, SD_BOTH );
	//소켓 옵션을 설정한다.
	setsockopt( pClientInfo->m_socketClient, SOL_SOCKET,
		SO_LINGER, (char *)&stLinger, sizeof(stLinger) );
	//소켓 연결을 종료 시킨다. 
	closesocket( pClientInfo->m_socketClient );
	
	pClientInfo->m_socketClient = INVALID_SOCKET;
}

//서버의 주소정보를 소켓과 연결시키고 접속 요청을 받기 위해 소켓을 등록하는 함수
bool cIOCompletionPort::BindandListen( int nBindPort )
{
	SOCKADDR_IN		stServerAddr;
	stServerAddr.sin_family = AF_INET;
    //서버 포트를 설정한다.
	stServerAddr.sin_port = htons( nBindPort );
	//어떤 주소에서 들어오는 접속이라도 받아들이겠다.
	//보통 서버라면 이렇게 설정한다. 만약 한 아이피에서만 접속을 받고 싶다면
	//그 주소를 inet_addr함수를 이용해 넣으면 된다.
	stServerAddr.sin_addr.s_addr = htonl( INADDR_ANY );

	//위에서 지정한 서버 주소 정보와 cIOCompletionPort 소켓을 연결한다.
	int nRet = bind( m_socketListen, (SOCKADDR*) &stServerAddr 
		, sizeof( SOCKADDR_IN ) );
	if( 0 != nRet )
	{
		m_pMainDlg->OutputMsg("[에러] bind()함수 실패 : %d", WSAGetLastError() );
		return false;
	}
	
	//접속 요청을 받아들이기 위해 cIOCompletionPort소켓을 등록하고 
	//접속대기큐를 5개로 설정 한다.
	nRet = listen( m_socketListen , 5 );
	if( 0 != nRet )
	{
		m_pMainDlg->OutputMsg("[에러] listen()함수 실패 : %d", WSAGetLastError() );
		return false;
	}
 
	m_pMainDlg->OutputMsg("서버 등록 성공..");

	return true;
}

bool cIOCompletionPort::CreateWokerThread()
{
	unsigned int uiThreadId = 0;
	//WaingThread Queue에 대기 상태로 넣을 쓰레드들 생성
	//권장되는 개수 : (cpu개수 * 2) + 1 
	for( int i = 0; i < MAX_WORKERTHREAD; i++ )
	{
		m_hWorkerThread[ i ] = (HANDLE)_beginthreadex(NULL, 0, 
			&CallWorkerThread, this
			,CREATE_SUSPENDED, &uiThreadId);
		if(m_hWorkerThread == NULL)
		{
			m_pMainDlg->OutputMsg("WorkerThread Creation Failed: %u", GetLastError());	
			return false;
		}
		ResumeThread( m_hWorkerThread[ i ] );
	}
	m_pMainDlg->OutputMsg("WokerThread 시작..");
    return true;
}

bool cIOCompletionPort::CreateAccepterThread()
{
	unsigned int uiThreadId = 0;
	//Client의 접속요청을 받을 쓰레드 생성
	m_hAccepterThread = (HANDLE)_beginthreadex(NULL, 0,
		                      &CallAccepterThread, this,
							  CREATE_SUSPENDED, &uiThreadId);
	if(m_hAccepterThread == NULL)
	{
		m_pMainDlg->OutputMsg("AccepterThread Creation Failed: %u", GetLastError());	
		return false;
	}

	ResumeThread( m_hAccepterThread );
	m_pMainDlg->OutputMsg("AccepterThread 시작..");
	return true;
}

bool cIOCompletionPort::BindIOCompletionPort( stClientInfo* pClientInfo )
{
	HANDLE hIOCP;
	//socket과 pClientInfo를 CompletionPort객체와 연결시킨다.
	hIOCP = CreateIoCompletionPort((HANDLE)pClientInfo->m_socketClient
		, m_hIOCP
		, reinterpret_cast<ULONG_PTR>( pClientInfo ) , 0);
	if( NULL == hIOCP  || m_hIOCP != hIOCP )
	{
		m_pMainDlg->OutputMsg("[에러] CreateIoCompletionPort()함수 실패",GetLastError() );
		return false;
	}
	return true;

}

bool cIOCompletionPort::StartServer()
{
	//접속된 클라이언트 주소 정보를 저장할 구조체
	bool bRet = CreateWokerThread();
	if( false == bRet )
		return false;
	bRet = CreateAccepterThread();
	if( false == bRet )
		return false;

	//CompletionPort객체 생성 요청을 한다.
	m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE 
		, NULL , NULL , 0 );
	if( NULL == m_hIOCP )
	{
		m_pMainDlg->OutputMsg("[에러] CreateIoCompletionPort()함수 실패",GetLastError() );
		return false;
	}
	
	m_pMainDlg->OutputMsg("서버 시작");

	return true;
}

bool cIOCompletionPort::BindRecv( stClientInfo* pClientInfo )
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	
	//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
	pClientInfo->m_stRecvOverlappedEx.m_wsaBuf.len = MAX_SOCKBUF;
	pClientInfo->m_stRecvOverlappedEx.m_wsaBuf.buf =
		pClientInfo->m_stRecvOverlappedEx.m_szBuf;
	pClientInfo->m_stRecvOverlappedEx.m_eOperation = OP_RECV;

	int nRet = WSARecv(	pClientInfo->m_socketClient, 	
					&(pClientInfo->m_stRecvOverlappedEx.m_wsaBuf),
					1, 
					&dwRecvNumBytes, 
					&dwFlag, 
					(LPWSAOVERLAPPED)&(pClientInfo->m_stRecvOverlappedEx), 
					NULL);

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if(nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		m_pMainDlg->OutputMsg("[에러] WSARecv()함수 실패 : %d", WSAGetLastError() );
		return false;
	}
	return true;
}

bool cIOCompletionPort::SendMsg( stClientInfo* pClientInfo 
								, char* pMsg 
								, int nLen )
{
	DWORD dwRecvNumBytes = 0;

	//전송될 메세지를 복사
	CopyMemory( pClientInfo->m_stSendOverlappedEx.m_szBuf, pMsg , nLen );

	
	//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
	pClientInfo->m_stSendOverlappedEx.m_wsaBuf.len = nLen;
	pClientInfo->m_stSendOverlappedEx.m_wsaBuf.buf =
		pClientInfo->m_stSendOverlappedEx.m_szBuf;
	pClientInfo->m_stSendOverlappedEx.m_eOperation = OP_SEND;
	
	int nRet = WSASend(	pClientInfo->m_socketClient , 	
					&(pClientInfo->m_stSendOverlappedEx.m_wsaBuf),
					1, 
					&dwRecvNumBytes, 
					0, 
					(LPWSAOVERLAPPED)&(pClientInfo->m_stSendOverlappedEx), 
					NULL);

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if(nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		m_pMainDlg->OutputMsg("[에러] WSASend()함수 실패 : %d", WSAGetLastError() );
		return false;
	}
	return true;

}

stClientInfo* cIOCompletionPort::GetEmptyClientInfo()
{
	for( int i = 0 ; i < MAX_CLIENT ; i++ )
	{
		if ( INVALID_SOCKET == m_pClientInfo[ i ].m_socketClient )
			return &m_pClientInfo[ i ];
	}
	return NULL;
}

//사용자의 접속을 받는 쓰레드
void cIOCompletionPort::AccepterThread()
{
	SOCKADDR_IN		stClientAddr;
	int nAddrLen = sizeof( SOCKADDR_IN );
	while( m_bAccepterRun )
	{
		//접속을 받을 구조체의 인덱스를 얻어온다.
		stClientInfo* pClientInfo = GetEmptyClientInfo();
		if( NULL == pClientInfo )
		{
			m_pMainDlg->OutputMsg("[에러] Client Full");
			return;
		}

		//클라이언트 접속 요청이 들어올 때까지 기다린다.
		pClientInfo->m_socketClient = accept( m_socketListen, 
			(SOCKADDR*) &stClientAddr , &nAddrLen );

		if( INVALID_SOCKET == pClientInfo->m_socketClient )
			continue;

		//I/O Completion Port객체와 소켓을 연결시킨다.
		bool bRet = BindIOCompletionPort( pClientInfo );
		if( false == bRet )
			return;
		//Recv Overlapped I/O작업을 요청해 놓는다.
		bRet = BindRecv( pClientInfo );
		if( false == bRet )
			return;

		m_pMainDlg->OutputMsg( "클라이언트 접속 : IP(%s) SOCKET(%d)", 
			inet_ntoa( stClientAddr.sin_addr ),
            pClientInfo->m_socketClient );
		//클라이언트 갯수 증가
		m_nClientCnt++;
	}
}
void cIOCompletionPort::WokerThread()
{
	//CompletionKey를 받을 포인터 변수
	stClientInfo* pClientInfo = NULL;
	//함수 호출 성공 여부
	BOOL bSuccess = TRUE;
	//Overlapped I/O작업에서 전송된 데이터 크기
	DWORD dwIoSize = 0;
	//I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터
	LPOVERLAPPED lpOverlapped = NULL;

	while( m_bWorkerRun )
	{
		//////////////////////////////////////////////////////
		//이 함수로 인해 쓰레드들은 WaitingThread Queue에
		//대기 상태로 들어가게 된다.
		//완료된 Overlapped I/O작업이 발생하면 IOCP Queue에서
		//완료된 작업을 가져와 뒤 처리를 한다.
		//그리고 PostQueuedCompletionStatus()함수에의해 사용자
		//메세지가 도착되면 쓰레드를 종료한다.
		//////////////////////////////////////////////////////
		bSuccess = GetQueuedCompletionStatus( m_hIOCP,
			&dwIoSize,					// 실제로 전송된 바이트
			(PULONG_PTR)&pClientInfo,		// CompletionKey
			&lpOverlapped,				// Overlapped IO 객체
			INFINITE );					// 대기할 시간
		
		//client가 접속을 끊었을때..			
		if( FALSE == bSuccess && 0 == dwIoSize )
		{
			m_pMainDlg->OutputMsg("socket(%d) 접속 끊김",
				pClientInfo->m_socketClient );
			CloseSocket( pClientInfo );
			continue;
		}

		//사용자 쓰레드 종료 메세지 처리..
		if( TRUE == bSuccess && 0 == dwIoSize &&
			NULL == lpOverlapped)
		{
			m_bWorkerRun = false;
			continue;
		}
		if( NULL == lpOverlapped )
			continue;

		stOverlappedEx* pOverlappedEx = 
			(stOverlappedEx*)lpOverlapped;
		
		//Overlapped I/O Recv작업 결과 뒤 처리
		if( OP_RECV == pOverlappedEx->m_eOperation )
		{
			pOverlappedEx->m_szBuf[ dwIoSize ] = NULL;
			m_pMainDlg->OutputMsg("[수신] bytes : %d , msg : %s"
				, dwIoSize , pOverlappedEx->m_szBuf );
			
			//클라이언트에 메세지를 에코한다.
			BindRecv( pClientInfo );
		}
		//Overlapped I/O Send작업 결과 뒤 처리
		else if( OP_SEND == pOverlappedEx->m_eOperation )
		{
			m_pMainDlg->OutputMsg("[송신] bytes : %d , msg : %s"
				, dwIoSize , pOverlappedEx->m_szBuf );
		}
		//예외 상황
		else
		{
			m_pMainDlg->OutputMsg("socket(%d)에서 예외상황",
				pClientInfo->m_socketClient );
		}
   	}
}

void cIOCompletionPort::DestroyThread()
{
	for( int i = 0; i < MAX_WORKERTHREAD; i++ )
	{
		//WaitingThread Queue에서 대기 중인 쓰레드에 
		//사용자 종료 메세지를 보낸다.
		PostQueuedCompletionStatus( m_hIOCP, 0, 0, NULL );
	}

	for( int i = 0; i < MAX_WORKERTHREAD; i++ )
	{
		//쓰레드 핸들을 닫고 쓰레드가 종료될 때까지 기다린다.
		CloseHandle( m_hWorkerThread[ i ] );
		WaitForSingleObject( m_hWorkerThread[ i ], INFINITE );
	}

	m_bAccepterRun = false;
	//Accepter 쓰레드를 종요한다.
	closesocket( m_socketListen );
	//쓰레드 종료를 기다린다
	WaitForSingleObject( m_hAccepterThread, INFINITE );
}