#pragma once
#include <winsock2.h>

class cSocket
{
public:
	cSocket(void);
	~cSocket(void);
	
	//------서버 클라이언트 공통함수-------//
	//소켓을 초기화하는 함수
	bool InitSocket();
	//소켓의 연결을 종료 시킨다.
	void CloseSocket( SOCKET socketClose , bool bIsForce = false );
	
	//------서버용 함수-------//
	//서버의 주소정보를 소켓과 연결시키고 접속 요청을 받기 위해 그 소켓을 등록하는 함수
	bool BindandListen( int nBindPort );
	//접속 요청을 수락하고 메세지를 받아서 처리하는 함수
	bool StartServer();

	//------클라이언트용 함수-------//
	//IP,Port에 해당하는 서버로 접속을 한다.
	bool Connect( char* pszIP , int nPort );

private:
	//클라이언트에서는 서버에 연결하기 위한 소켓
	//서버에서는 클라이언트의 접속요청을 기다리는 소켓
	SOCKET		m_socket;

	//서버에서 클라이언트의 접속 요청을 수락한 후에 클라이언트와 연결되어 지는 소켓
	SOCKET		m_socketConnect;

	//소켓 버퍼
	char		m_szSocketBuf[ 1024 ];
};
