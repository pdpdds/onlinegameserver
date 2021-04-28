#include "stdafx.h"
#include <iostream>

using namespace std;
#include "cAsyncSocket.h"
#include "AsyncSelectDlg.h"

cAsyncSocket::cAsyncSocket(void)
{
	m_socketListen = INVALID_SOCKET;
	m_hWnd = NULL;
	m_pMainDlg = NULL;
	ZeroMemory( m_szSocketBuf , 1024 );
}

cAsyncSocket::~cAsyncSocket(void)
{
	//윈속의 사용을 끝낸다.
	WSACleanup();
}

//소켓을 초기화하는 함수
bool cAsyncSocket::InitSocket( HWND hWnd )
{
	WSADATA wsaData;
	//윈속을 버젼 2,2로 초기화 한다.
	int nRet = WSAStartup( MAKEWORD(2,2) , &wsaData );
	if( 0 != nRet )
	{
		m_pMainDlg->OutputMsg("[에러] WSAStartup()함수 실패 : %d"
			, WSAGetLastError() );
		return false;
	}

	//연결지향형 TCP 소켓을 생성
	m_socketListen = socket( AF_INET , SOCK_STREAM , IPPROTO_TCP );
	if( INVALID_SOCKET == m_socketListen )
	{
		m_pMainDlg->OutputMsg("[에러] socket()함수 실패 : %d"
			, WSAGetLastError() );
		return false;
	}
	
	//윈도우 핸들 저장
	m_hWnd = hWnd;
	m_pMainDlg->OutputMsg("소켓 초기화 성공");
	return true;
}

//해당 소켓을 종료 시킨다.
void cAsyncSocket::CloseSocket( SOCKET socketClose , bool bIsForce )
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
bool cAsyncSocket::BindandListen( int nBindPort )
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
	int nRet = bind( m_socketListen , (SOCKADDR*) &stServerAddr , sizeof( SOCKADDR_IN ) );
	if( 0 != nRet )
	{
		m_pMainDlg->OutputMsg("[에러] bind()함수 실패 : %d"
			, WSAGetLastError() );
		return false;
	}
	
	//접속 요청을 받아들이기 위해 m_socketListen소켓을 등록하고 접속대기큐를 5개로 설정 한다.
	nRet = listen( m_socketListen , 5 );
	if( 0 != nRet )
	{
		m_pMainDlg->OutputMsg("[에러] listen()함수 실패 : %d"
			, WSAGetLastError() );
		return false;
	}
	m_pMainDlg->OutputMsg("서버 등록 성공..");
	return true;
}

bool cAsyncSocket::StartServer()
{
	
	//소켓과 네트워크 이벤트를 등록한다.
	int nRet = WSAAsyncSelect( m_socketListen , m_hWnd , WM_SOCKETMSG , FD_ACCEPT | FD_CLOSE );
	if( SOCKET_ERROR == nRet )
	{
		m_pMainDlg->OutputMsg("[에러] WSAAsyncSelect()함수 실패 : %d"
			, WSAGetLastError() );
		return false;
	}
	return true;
}
