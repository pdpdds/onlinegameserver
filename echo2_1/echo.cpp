#include <iostream>
using namespace std;
#include "cSocket.h"

#define SERVER_IP		"127.0.0.1"
#define SERVER_PORT		8000
#include <windows.h>
#include <process.h>

int main( int argc, char* argv[] ) 
{
	if( argc < 2 )
	{
		cout<<"usage : client.exe [ option : [ /server ][ /client ] ]"<<endl;
		return 0;
	}

	//소켓 객체 생성
	cSocket Socket;

	//echo 서버로 시작
	if( _strcmpi( argv[ 1 ] , "/server" ) == 0 )
	{
		Socket.InitSocket();
		Socket.BindandListen( SERVER_PORT );
		Socket.StartServer();
	}
	//echo 클라이언트로 시작
	else if( _strcmpi( argv[ 1 ] , "/client" ) == 0 )
	{
		Socket.InitSocket();
		Socket.Connect( SERVER_IP , SERVER_PORT );
	}
	else
	{
		cout<<"usage : client.exe option : [ /server ][ /client ]"<<endl;
		return 0;
	}
	return 0;
}


