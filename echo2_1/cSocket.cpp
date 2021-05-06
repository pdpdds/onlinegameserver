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
	//������ ����� ������.
	WSACleanup();
	
}

//������ �ʱ�ȭ�ϴ� �Լ�
bool cSocket::InitSocket()
{
	WSADATA wsaData;
	//������ ���� 2,2�� �ʱ�ȭ �Ѵ�.
	int nRet = WSAStartup( MAKEWORD(2,2) , &wsaData );
	if( 0 != nRet )
	{
		cout<<"[����] ��ġ : cSocket::InitSocket() , ���� : WSAStartup()�Լ� ���� , ErrorCode : "
			<<WSAGetLastError()<<endl;
		return false;
	}

	//���������� TCP ������ ����
	m_socket = socket( AF_INET , SOCK_STREAM , IPPROTO_TCP );
	if( INVALID_SOCKET == m_socket )
	{
		cout<<"[����] ��ġ : cSocket::InitSocket() , ���� : socket()�Լ� ���� , ErrorCode : "
			<<WSAGetLastError()<<endl;
		return false;
	}

	cout<<"���� �ʱ�ȭ ����.."<<endl;

	return true;
}

//�ش� ������ ���� ��Ų��.
void cSocket::CloseSocket( SOCKET socketClose , bool bIsForce )
{
	struct linger stLinger = {0, 0};	// SO_DONTLINGER�� ����

	// bIsForce�� true�̸� SO_LINGER, timeout = 0���� �����Ͽ�
	// ���� ���� ��Ų��. ���� : ������ �ս��� ������ ���� 
	if( true == bIsForce )
		stLinger.l_onoff = 1; 

	//socketClose������ ������ �ۼ����� ��� �ߴ� ��Ų��.
	shutdown( socketClose, SD_BOTH );
	//���� �ɼ��� �����Ѵ�.
	setsockopt( socketClose, SOL_SOCKET, SO_LINGER, (char *)&stLinger, sizeof(stLinger) );
	//���� ������ ���� ��Ų��. 
	closesocket( socketClose );
	
	socketClose = INVALID_SOCKET;
}

//������ �ּ������� ���ϰ� �����Ű�� ���� ��û�� �ޱ� ���� �� ������ ����ϴ� �Լ�
bool cSocket::BindandListen( int nBindPort )
{
	SOCKADDR_IN		stServerAddr;

	//-- htons,htonl�� ����Ʈ ������ ȣ��Ʈ ����Ʈ �������� ��Ʈ��ũ ����Ʈ ������
	//-- �ٲٴ� ��Ȱ�� �Ѵ�. htons�� 2bytes�����͸� htonl�� 4bytes�����͸� �ٲ۴�.
	//-- ������ �ý��۸��� ����Ʈ ������ �ٸ��� ������� ���ڰ迭 x86 CPU������
	//-- ���� 3�� 2bytes�� ǥ���Ѵٸ� "03 00"�̴� ������ ����Ѷ� 6800�迭 CPU
	//-- ������ "00 03"�� �ȴ�. ���ڰ� ��Ʋ ��Ʈ�� ���ڰ� �򿣵�� �̴�.
	//-- �츮�� ���� ���� �迭�� CPU�� ���⶧���� ��Ʋ����� �̴�.
	//-- �׷��� �� �Լ��� ������ ��d�� ���� ��ȯ�Ͽ� �����Ѵ�.

	stServerAddr.sin_family = AF_INET;
    //���� ��Ʈ�� �����Ѵ�.
	stServerAddr.sin_port = htons( nBindPort );
	//� �ּҿ��� ������ �����̶� �޾Ƶ��̰ڴ�.
	//���� ������� �̷��� �����Ѵ�. ���� �� �����ǿ����� ������ �ް� �ʹٸ�
	//�� �ּҸ� inet_addr�Լ��� �̿��� ������ �ȴ�.
	stServerAddr.sin_addr.s_addr = htonl( INADDR_ANY );

	//������ ������ ���� �ּ� ������ m_socket ������ �����Ѵ�.
	int nRet = bind( m_socket , (SOCKADDR*) &stServerAddr , sizeof( SOCKADDR_IN ) );
	if( 0 != nRet )
	{
		cout<<"[����] ��ġ : cSocket::BindandListen() , ���� : bind()�Լ� ���� , ErrorCode : "
			<<WSAGetLastError()<<endl;
		return false;
	}
	//���� ��û�� �޾Ƶ��̱� ���� m_socket������ ����ϰ� ���Ӵ��ť�� 5���� ���� �Ѵ�.
	nRet = listen( m_socket , 5 );
	if( 0 != nRet )
	{
		cout<<"[����] ��ġ : cSocket::BindandListen() , ���� : listen()�Լ� ���� , ErrorCode : "
			<<WSAGetLastError()<<endl;
		return false;
	}
	cout<<"���� ��� ����.."<<endl;
	return true;
}

bool cSocket::StartServer()
{
	char szOutStr[ 1024 ];
	//���ӵ� Ŭ���̾�Ʈ �ּ� ������ ������ ����ü
	SOCKADDR_IN		stClientAddr;
	int nAddrLen = sizeof( SOCKADDR_IN );


	cout<< "���� ����.." << endl;
	//Ŭ���̾�Ʈ ���� ��û�� ���� ������ ��ٸ���.
	m_socketConnect = accept( m_socket , (SOCKADDR*) &stClientAddr , &nAddrLen );
	if( INVALID_SOCKET == m_socketConnect )
	{
		cout<<"[����] ��ġ : cSocket::StartServer() , ���� : accept()�Լ� ���� , ErrorCode : "
			<<WSAGetLastError()<<endl;
		return false;
	}
	sprintf( szOutStr , "Ŭ���̾�Ʈ ���� : IP(%s) SOCKET(%d)" 
		, inet_ntoa( stClientAddr.sin_addr ) , (DWORD)m_socketConnect );
    
	cout<<szOutStr<<endl;

	//Ŭ���̾�Ʈ���� �޼����� ���� �ٽ� Ŭ���̾�Ʈ���� ������.
	while( true )
	{
		int nRecvLen = recv( m_socketConnect , m_szSocketBuf , 1024 , 0 );
		if( 0 == nRecvLen )
		{
			cout<<"Ŭ���̾�Ʈ�� ������ ����Ǿ����ϴ�."<<endl;
			CloseSocket( m_socketConnect );
			
			//�ٽ� ������ ������ Ŭ���̾�Ʈ ���� ��û�� �޴´�.
			StartServer();
			return false;
		}
		else if( -1 == nRecvLen )
		{
			cout<<"[����] ��ġ : cSocket::StartServer() , ���� : recv()�Լ� ���� , ErrorCode : "
				<<WSAGetLastError()<<endl;
			CloseSocket( m_socketConnect );

			//�ٽ� ������ ������ Ŭ���̾�Ʈ ���� ��û�� �޴´�.
			StartServer();
			return false;
		}
		m_szSocketBuf[ nRecvLen ] = NULL;
		cout<<"�޼��� ���� : ���� bytes["<< nRecvLen << "] , ���� : ["<< m_szSocketBuf << "]" << endl;

		int nSendLen = send( m_socketConnect , m_szSocketBuf , nRecvLen , 0 );
		if( -1 == nSendLen )
		{
			cout<<"[����] ��ġ : cSocket::StartServer() , ���� : send()�Լ� ���� , ErrorCode : "
				<<WSAGetLastError()<<endl;
			CloseSocket( m_socketConnect );

			//�ٽ� ������ ������ Ŭ���̾�Ʈ ���� ��û�� �޴´�.
			StartServer();
			return false;
		}
		cout<<"�޼��� �۽� : �۽� bytes["<< nSendLen << "] , ���� : ["<< m_szSocketBuf << "]" << endl;

	}

	//Ŭ���̾�Ʈ ���� ����
	CloseSocket( m_socketConnect );
	//���� ������ ���� ����
	CloseSocket( m_socket );

	cout<<"���� ���� ����.."<<endl;
	return true;
	

}
bool cSocket::Connect( char* szIP , int nPort )
{
	//������ ���� ������ ������ ����ü
	SOCKADDR_IN		stServerAddr;

	char			szOutMsg[ 1024 ];
	stServerAddr.sin_family = AF_INET;
    //������ ���� ��Ʈ�� �ִ´�.
	stServerAddr.sin_port = htons( nPort );
	//������ ���� ip�� �ִ´�.
	stServerAddr.sin_addr.s_addr = inet_addr( szIP );

	int nRet = connect( m_socket , (sockaddr*) &stServerAddr , sizeof( sockaddr ) );
	if( SOCKET_ERROR == nRet )
	{
		cout<<"[����] ��ġ : cSocket::Connect() , ���� : connect()�Լ� ���� , ErrorCode : "
			<<WSAGetLastError()<<endl;
		return false;
	}
	cout<< "���� ����.." << endl;
	while( true )
	{
		cout<<">>";
		cin>>szOutMsg;
		if( 0 == _strcmpi( szOutMsg , "quit" ) )
			break;
		
		int nSendLen = send( m_socket , szOutMsg , (int)strlen( szOutMsg ) , 0 );
		if( -1 == nSendLen )
		{
			cout<<"[����] ��ġ : cSocket::Connect() , ���� : send()�Լ� ���� , ErrorCode : "
				<<WSAGetLastError()<<endl;

			return false;
		}
		cout<<"�޼��� �۽� : �۽� bytes["<< nSendLen << "] , ���� : ["<< szOutMsg << "]" << endl;

		int nRecvLen = recv( m_socket , m_szSocketBuf , 1024 , 0 );
		if( 0 == nRecvLen )
		{
			cout<<"Ŭ���̾�Ʈ�� ������ ����Ǿ����ϴ�."<<endl;
			CloseSocket( m_socket );
			return false;
		}
		else if( -1 == nRecvLen )
		{
			cout<<"[����] ��ġ : cSocket::Connect() , ���� : recv()�Լ� ���� , ErrorCode : "
				<<WSAGetLastError()<<endl;
			CloseSocket( m_socket );
			return false;
		}
		m_szSocketBuf[ nRecvLen ] = NULL;
		cout<<"�޼��� ���� : ���� bytes["<< nRecvLen << "] , ���� : ["<< m_szSocketBuf << "]" << endl;

	}

	CloseSocket( m_socket );
	cout<<" Ŭ���̾�Ʈ ��������.."<<endl;
	
	return true;
}