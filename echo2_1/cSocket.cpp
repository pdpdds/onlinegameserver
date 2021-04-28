#include <iostream>
using namespace std;
#include "cSocket.h"

cSocket::cSocket(void)
{
	m_socket = INVALID_SOCKET;
	m_socketConnect = INVALID_SOCKET;

	ZeroMemory( m_szSocketBuf , 1024 );
}

cSocket::~cSocket(void)
{
	//윈속의 사용을 끝낸다.
	WSACleanup();
	
}

//소켓을 초기화하는 함수
bool cSocket::InitSocket()
{
	WSADATA wsaData;
	//윈속을 버젼 2,2로 초기화 한다.
	int nRet = WSAStartup( MAKEWORD(2,2) , &wsaData );
	if( 0 != nRet )
	{
		cout<<"[에러] 위치 : cSocket::InitSocket() , 이유 : WSAStartup()함수 실패 , ErrorCode : "
			<<WSAGetLastError()<<endl;
		return false;
	}

	//연결지향형 TCP 소켓을 생성
	m_socket = socket( AF_INET , SOCK_STREAM , IPPROTO_TCP );
	if( INVALID_SOCKET == m_socket )
	{
		cout<<"[에러] 위치 : cSocket::InitSocket() , 이유 : socket()함수 실패 , ErrorCode : "
			<<WSAGetLastError()<<endl;
		return false;
	}

	cout<<"소켓 초기화 성공.."<<endl;

	return true;
}

//해당 소켓을 종료 시킨다.
void cSocket::CloseSocket( SOCKET socketClose , bool bIsForce )
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
bool cSocket::BindandListen( int nBindPort )
{
	SOCKADDR_IN		stServerAddr;

	//-- htons,htonl은 바이트 순서를 호스트 바이트 순서에서 네트워크 바이트 순서로
	//-- 바꾸는 역활을 한다. htons는 2bytes데이터를 htonl은 4bytes데이터를 바꾼다.
	//-- 이유는 시스템마다 바이트 순서가 다른데 예를들어 인텔계열 x86 CPU에서는
	//-- 숫자 3을 2bytes로 표시한다면 "03 00"이다 하지만 모토롤라 6800계열 CPU
	//-- 에서는 "00 03"이 된다. 전자가 리틀 엔트안 후자가 빅엔디안 이다.
	//-- 우리는 보통 인텔 계열의 CPU를 쓰기때문에 리틀엔디안 이다.
	//-- 그래서 이 함수의 도움을 받d아 값을 변환하여 전달한다.

	stServerAddr.sin_family = AF_INET;
    //서버 포트를 설정한다.
	stServerAddr.sin_port = htons( nBindPort );
	//어떤 주소에서 들어오는 접속이라도 받아들이겠다.
	//보통 서버라면 이렇게 설정한다. 만약 한 아이피에서만 접속을 받고 싶다면
	//그 주소를 inet_addr함수를 이용해 넣으면 된다.
	stServerAddr.sin_addr.s_addr = htonl( INADDR_ANY );

	//위에서 지정한 서버 주소 정보와 m_socket 소켓을 연결한다.
	int nRet = bind( m_socket , (SOCKADDR*) &stServerAddr , sizeof( SOCKADDR_IN ) );
	if( 0 != nRet )
	{
		cout<<"[에러] 위치 : cSocket::BindandListen() , 이유 : bind()함수 실패 , ErrorCode : "
			<<WSAGetLastError()<<endl;
		return false;
	}
	//접속 요청을 받아들이기 위해 m_socket소켓을 등록하고 접속대기큐를 5개로 설정 한다.
	nRet = listen( m_socket , 5 );
	if( 0 != nRet )
	{
		cout<<"[에러] 위치 : cSocket::BindandListen() , 이유 : listen()함수 실패 , ErrorCode : "
			<<WSAGetLastError()<<endl;
		return false;
	}
	cout<<"서버 등록 성공.."<<endl;
	return true;
}

bool cSocket::StartServer()
{
	char szOutStr[ 1024 ];
	//접속된 클라이언트 주소 정보를 저장할 구조체
	SOCKADDR_IN		stClientAddr;
	int nAddrLen = sizeof( SOCKADDR_IN );


	cout<< "서버 시작.." << endl;
	//클라이언트 접속 요청이 들어올 때까지 기다린다.
	m_socketConnect = accept( m_socket , (SOCKADDR*) &stClientAddr , &nAddrLen );
	if( INVALID_SOCKET == m_socketConnect )
	{
		cout<<"[에러] 위치 : cSocket::StartServer() , 이유 : accept()함수 실패 , ErrorCode : "
			<<WSAGetLastError()<<endl;
		return false;
	}
	sprintf( szOutStr , "클라이언트 접속 : IP(%s) SOCKET(%d)" 
		, inet_ntoa( stClientAddr.sin_addr ) , m_socketConnect );
    
	cout<<szOutStr<<endl;

	//클라이언트에서 메세지가 오면 다시 클라이언트에게 보낸다.
	while( true )
	{
		int nRecvLen = recv( m_socketConnect , m_szSocketBuf , 1024 , 0 );
		if( 0 == nRecvLen )
		{
			cout<<"클라이언트와 연결이 종료되었습니다."<<endl;
			CloseSocket( m_socketConnect );
			
			//다시 서버를 시작해 클라이언트 접속 요청을 받는다.
			StartServer();
			return false;
		}
		else if( -1 == nRecvLen )
		{
			cout<<"[에러] 위치 : cSocket::StartServer() , 이유 : recv()함수 실패 , ErrorCode : "
				<<WSAGetLastError()<<endl;
			CloseSocket( m_socketConnect );

			//다시 서버를 시작해 클라이언트 접속 요청을 받는다.
			StartServer();
			return false;
		}
		m_szSocketBuf[ nRecvLen ] = NULL;
		cout<<"메세지 수신 : 수신 bytes["<< nRecvLen << "] , 내용 : ["<< m_szSocketBuf << "]" << endl;

		int nSendLen = send( m_socketConnect , m_szSocketBuf , nRecvLen , 0 );
		if( -1 == nSendLen )
		{
			cout<<"[에러] 위치 : cSocket::StartServer() , 이유 : send()함수 실패 , ErrorCode : "
				<<WSAGetLastError()<<endl;
			CloseSocket( m_socketConnect );

			//다시 서버를 시작해 클라이언트 접속 요청을 받는다.
			StartServer();
			return false;
		}
		cout<<"메세지 송신 : 송신 bytes["<< nSendLen << "] , 내용 : ["<< m_szSocketBuf << "]" << endl;

	}

	//클라이언트 연결 종료
	CloseSocket( m_socketConnect );
	//리슨 소켓의 연결 종료
	CloseSocket( m_socket );

	cout<<"서버 정상 종료.."<<endl;
	return true;
	

}
bool cSocket::Connect( char* szIP , int nPort )
{
	//접속할 서버 정보를 저장할 구조체
	SOCKADDR_IN		stServerAddr;

	char			szOutMsg[ 1024 ];
	stServerAddr.sin_family = AF_INET;
    //접속할 서버 포트를 넣는다.
	stServerAddr.sin_port = htons( nPort );
	//접속할 서버 ip를 넣는다.
	stServerAddr.sin_addr.s_addr = inet_addr( szIP );

	int nRet = connect( m_socket , (sockaddr*) &stServerAddr , sizeof( sockaddr ) );
	if( SOCKET_ERROR == nRet )
	{
		cout<<"[에러] 위치 : cSocket::Connect() , 이유 : connect()함수 실패 , ErrorCode : "
			<<WSAGetLastError()<<endl;
		return false;
	}
	cout<< "접속 성공.." << endl;
	while( true )
	{
		cout<<">>";
		cin>>szOutMsg;
		if( 0 == strcmpi( szOutMsg , "quit" ) )
			break;
		
		int nSendLen = send( m_socket , szOutMsg , strlen( szOutMsg ) , 0 );
		if( -1 == nSendLen )
		{
			cout<<"[에러] 위치 : cSocket::Connect() , 이유 : send()함수 실패 , ErrorCode : "
				<<WSAGetLastError()<<endl;

			return false;
		}
		cout<<"메세지 송신 : 송신 bytes["<< nSendLen << "] , 내용 : ["<< szOutMsg << "]" << endl;

		int nRecvLen = recv( m_socket , m_szSocketBuf , 1024 , 0 );
		if( 0 == nRecvLen )
		{
			cout<<"클라이언트와 연결이 종료되었습니다."<<endl;
			CloseSocket( m_socket );
			return false;
		}
		else if( -1 == nRecvLen )
		{
			cout<<"[에러] 위치 : cSocket::Connect() , 이유 : recv()함수 실패 , ErrorCode : "
				<<WSAGetLastError()<<endl;
			CloseSocket( m_socket );
			return false;
		}
		m_szSocketBuf[ nRecvLen ] = NULL;
		cout<<"메세지 수신 : 수신 bytes["<< nRecvLen << "] , 내용 : ["<< m_szSocketBuf << "]" << endl;

	}

	CloseSocket( m_socket );
	cout<<" 클라이언트 정상종료.."<<endl;
	
	return true;
}