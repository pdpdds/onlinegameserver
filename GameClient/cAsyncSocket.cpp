
//////////////////////////////////////////////////////////////////////////////////////////
/*					파일		  : client.cpp											*/
/*					프로젝트 종류 : Win32 Console										*/
/*					설명		  : 메인 												*/
/*					작성일자	  : 2004.09.02											*/
/*					작성자		  : 강군												*/
///////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <iostream>

using namespace std;
#include "cAsyncSocket.h"
#include "GameClientDlg.h"

IMPLEMENT_SINGLETON( cAsyncSocket );

cAsyncSocket::cAsyncSocket(void)
{
	m_socket = INVALID_SOCKET;
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
	if( INVALID_SOCKET != m_socket )
		CloseSocket();

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
	m_socket = socket( AF_INET , SOCK_STREAM , IPPROTO_TCP );
	if( INVALID_SOCKET == m_socket )
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
void cAsyncSocket::CloseSocket( bool bIsForce )
{
	struct linger stLinger = {0, 0};	// SO_DONTLINGER로 설정

	// bIsForce가 true이면 SO_LINGER, timeout = 0으로 설정하여
	// 강제 종료 시킨다. 주의 : 데이터 손실이 있을수 있음 
	if( true == bIsForce )
		stLinger.l_onoff = 1; 

	//m_socket소켓의 데이터 송수신을 모두 중단 시킨다.
	shutdown( m_socket, SD_BOTH );
	//소켓 옵션을 설정한다.
	setsockopt( m_socket, SOL_SOCKET, SO_LINGER, (char *)&stLinger, sizeof(stLinger) );
	//소켓 연결을 종료 시킨다. 
	closesocket( m_socket );
	
	m_socket = INVALID_SOCKET;
}


bool cAsyncSocket::ConnectTo( char* szIP , int nPort )
{
	//접속할 서버 정보를 저장할 구조체
	SOCKADDR_IN		stServerAddr;

	stServerAddr.sin_family = AF_INET;
    //접속할 서버 포트를 넣는다.
	stServerAddr.sin_port = htons( nPort );
	//접속할 서버 ip를 넣는다.
	stServerAddr.sin_addr.s_addr = inet_addr( szIP );

	int nRet = connect( m_socket , (sockaddr*) &stServerAddr , sizeof( sockaddr ) );
	if( SOCKET_ERROR == nRet )
	{
		m_pMainDlg->OutputMsg("[ERROR] 접속 실패 : connect() failed");
		return false;
	}
	WSAAsyncSelect( m_socket , m_hWnd , WM_SOCKETMSG , FD_READ | FD_CLOSE );
	m_pMainDlg->OutputMsg("[SUCCESS] 접속 성공");

	return true;
}

int cAsyncSocket::SendMsg( char* pMsg , int nLen )
{
	return send( m_socket , pMsg , nLen , 0 );
}
