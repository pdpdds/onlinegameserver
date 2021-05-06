#include "Precompile.h"

cConnection::cConnection(void)
{
	m_sockListener		= INVALID_SOCKET;
	m_socket			= INVALID_SOCKET;
	m_lpRecvOverlappedEx = NULL;
	m_lpSendOverlappedEx = NULL;
	
	m_nSendBufSize = 0;
	m_nRecvBufSize = 0;
	
	InitializeConnection();
}

void cConnection::InitializeConnection() 
{
	ZeroMemory( m_szIp , MAX_IP_LENGTH );
	m_socket			= INVALID_SOCKET;
	m_bIsConnect		= FALSE;
	m_bIsClosed			= FALSE;
	m_bIsSend			= TRUE;
	m_dwSendIoRefCount	= 0;
	m_dwRecvIoRefCount	= 0;
	m_dwAcceptIoRefCount = 0;

	m_ringRecvBuffer.Initialize();
	m_ringSendBuffer.Initialize();
}

cConnection::~cConnection(void)
{
	m_sockListener  = INVALID_SOCKET;
	m_socket    = INVALID_SOCKET;
}

bool cConnection::CreateConnection( INITCONFIG &initConfig )
{
	m_nIndex	= initConfig.nIndex;
	m_sockListener = initConfig.sockListener;
	
	m_lpRecvOverlappedEx = new OVERLAPPED_EX( this );
	m_lpSendOverlappedEx = new OVERLAPPED_EX( this );
	m_ringRecvBuffer.Create( initConfig.nRecvBufSize * initConfig.nRecvBufCnt );
	m_ringSendBuffer.Create( initConfig.nSendBufSize * initConfig.nSendBufCnt );
	
	m_nRecvBufSize = initConfig.nRecvBufSize;
	m_nSendBufSize = initConfig.nSendBufSize;

	return BindAcceptExSock();
}

bool cConnection::ConnectTo( char* szIp , unsigned short usPort )
{
	SOCKADDR_IN	si_addr;
	int			nRet;
	int			nZero = 0;

	// create listen socket.
	m_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if( INVALID_SOCKET == m_socket )
	{
		LOG( LOG_ERROR_LOW , "SYSTEM | cConnection::ConnectTo() | WSASocket() , Socket Creation Failed : LastError(%u)", GetLastError() );	
		return false;
	}
	
	// bind listen socket with si_addr struct.
	si_addr.sin_family		= AF_INET;
	si_addr.sin_port		= htons(usPort);
	si_addr.sin_addr.s_addr = inet_addr(szIp);

	nRet = WSAConnect( m_socket, (sockaddr*)&si_addr , sizeof( sockaddr ) , NULL , NULL , NULL , NULL );

	if( SOCKET_ERROR == nRet )
	{
		LOG( LOG_ERROR_LOW , 
			"SYSTEM | cConnection::ConnectTo() | WSAConnect() , WSAConnect Failed : LastError(%u)", 
			GetLastError() );	
		return false;
	}

	HANDLE	hIOCP = IocpServer()->GetWorkerIOCP();
	if( BindIOCP( hIOCP ) == false )
	{
		LOG( LOG_ERROR_LOW ,
			"SYSTEM | cConnection::ConnectTo() | BindIOCP() , BindIOCP Failed : LastError(%u)", 
			GetLastError() );	
		return false;
	}

	m_bIsConnect = TRUE;

	if( RecvPost( m_ringRecvBuffer.GetBeginMark() , 0 ) == false )
	{
		LOG( LOG_ERROR_LOW , 
			"SYSTEM | cConnection::ConnectTo() | RecvPost() , BindRecv Failed : LastError(%u)", 
			GetLastError() );	
		return false;
	}
	return true;
}

bool cConnection::BindAcceptExSock()
{
	//리슨 소켓이 없다면 acceptex에 bind하지 않는다.
	if( 0 == m_sockListener )
		return true;
    DWORD dwBytes = 0;
	memset( &m_lpRecvOverlappedEx->s_Overlapped, 0, sizeof(OVERLAPPED) );
	m_lpRecvOverlappedEx->s_WsaBuf.buf = m_szAddressBuf;
	m_lpRecvOverlappedEx->s_lpSocketMsg = &m_lpRecvOverlappedEx->s_WsaBuf.buf[0];
	m_lpRecvOverlappedEx->s_WsaBuf.len  = m_nRecvBufSize;
	m_lpRecvOverlappedEx->s_eOperation = OP_ACCEPT;
	m_lpRecvOverlappedEx->s_lpConnection = this;
	m_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP,
		NULL, 0, WSA_FLAG_OVERLAPPED);
	if( INVALID_SOCKET == m_socket )
	{
		LOG( LOG_ERROR_NORMAL ,
			"SYSTEM | cConnection::BindAcceptExSock() | WSASocket() Failed: error[%u]"
			, GetLastError() );
		return false;
	}

	IncrementAcceptIoRefCount();

	BOOL bRet = AcceptEx( m_sockListener , m_socket , 
		m_lpRecvOverlappedEx->s_WsaBuf.buf,
		0 ,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&dwBytes,
		(LPOVERLAPPED)m_lpRecvOverlappedEx
		);
	if( !bRet && WSAGetLastError() != WSA_IO_PENDING )
	{  
		DecrementAcceptIoRefCount();
		LOG( LOG_ERROR_NORMAL ,
			"SYSTEM | cConnection::BindAcceptExSock() | AcceptEx() Failed: error[%u]" 
			, GetLastError() );
		return false;
	}
	return true;
}

bool cConnection::CloseConnection(bool bForce )
{
	cMonitor::Owner lock( m_csConnection );
	{
		struct linger li = {0, 0};	// Default: SO_DONTLINGER
		if( bForce )
			li.l_onoff = 1; // SO_LINGER, timeout = 0
		if( NULL != IocpServer() && TRUE == m_bIsConnect )
			IocpServer()->OnClose( this );		
		shutdown( m_socket, SD_BOTH );
		setsockopt( m_socket, SOL_SOCKET, SO_LINGER, (char *)&li, sizeof(li) );
		closesocket( m_socket );

		m_socket = INVALID_SOCKET;
		if( m_lpRecvOverlappedEx != NULL )
		{
			m_lpRecvOverlappedEx->s_dwRemain = 0;
			m_lpRecvOverlappedEx->s_nTotalBytes = 0;
		}

		if( m_lpSendOverlappedEx != NULL )
		{
			m_lpSendOverlappedEx->s_dwRemain = 0;
			m_lpSendOverlappedEx->s_nTotalBytes = 0;
		}
		//connection을 다시 초기화 시켜준다.
		InitializeConnection();
		BindAcceptExSock();
	}
	return true;
}

char* cConnection::PrepareSendPacket(int slen)
{
	if( false == m_bIsConnect )
		return NULL;
	char* pBuf = m_ringSendBuffer.ForwardMark( slen );

	if( NULL == pBuf )
	{
		IocpServer()->CloseConnection( this );
		LOG( LOG_ERROR_NORMAL ,
			"SYSTEM | cConnection::PrepareSendPacket() | Socket[%d] SendRingBuffer overflow",
			m_socket);
		return NULL;
	}
	ZeroMemory( pBuf, slen );
	CopyMemory( pBuf , &slen , PACKET_SIZE_LENGTH );
	return pBuf;
}

bool cConnection::ReleaseSendPacket( LPOVERLAPPED_EX lpSendOverlappedEx )
{
	if( NULL == lpSendOverlappedEx )
		return false;
	
	m_ringSendBuffer.ReleaseBuffer( m_lpSendOverlappedEx->s_WsaBuf.len );
	lpSendOverlappedEx = NULL;
	return true;
}

bool cConnection::BindIOCP(HANDLE& hIOCP)
{
	HANDLE hIOCPHandle;
	cMonitor::Owner lock( m_csConnection );

	hIOCPHandle = CreateIoCompletionPort((HANDLE)m_socket,
		hIOCP, reinterpret_cast<ULONG_PTR>( this ) , 0);
	
	if( NULL == hIOCPHandle  || hIOCP != hIOCPHandle )
	{
		LOG( LOG_ERROR_NORMAL ,
			"SYSTEM | cConnection::BindIOCP() | CreateIoCompletionPort() Failed : %d",
			GetLastError() );
		return false;
	}
	m_hIOCP = hIOCP;
    return true;
}

bool cConnection::RecvPost( char* pNext , DWORD dwRemain )
{
	int					nRet = 0;
	DWORD				dwFlag = 0;
	DWORD				dwRecvNumBytes = 0;

	if( false == m_bIsConnect || NULL == m_lpRecvOverlappedEx)
		return false;
	m_lpRecvOverlappedEx->s_eOperation = OP_RECV;
	m_lpRecvOverlappedEx->s_dwRemain = dwRemain;
	int nMoveMark = dwRemain - ( m_ringRecvBuffer.GetCurrentMark() - pNext );
	m_lpRecvOverlappedEx->s_WsaBuf.len  = m_nRecvBufSize;
	m_lpRecvOverlappedEx->s_WsaBuf.buf = 
				m_ringRecvBuffer.ForwardMark( nMoveMark , m_nRecvBufSize , dwRemain );
	if( NULL == m_lpRecvOverlappedEx->s_WsaBuf.buf )
	{
		IocpServer()->CloseConnection( this );
		LOG( LOG_ERROR_NORMAL ,
			"SYSTEM | cConnection::RecvPost() | Socket[%d] RecvRingBuffer overflow..", 
			m_socket );
		return false;
	}
	m_lpRecvOverlappedEx->s_lpSocketMsg = 
		m_lpRecvOverlappedEx->s_WsaBuf.buf - dwRemain;
	
	memset( &m_lpRecvOverlappedEx->s_Overlapped, 0, sizeof(OVERLAPPED) );
	IncrementRecvIoRefCount();

	int ret = WSARecv(
		m_socket,
		&m_lpRecvOverlappedEx->s_WsaBuf,
		1,
		&dwRecvNumBytes, 
		&dwFlag,
		&m_lpRecvOverlappedEx->s_Overlapped,
		NULL );
	
	if( ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING )
	{
		DecrementRecvIoRefCount();
        IocpServer()->CloseConnection( this );
		LOG( LOG_ERROR_NORMAL ,
			"SYSTEM | cConnection::RecvPost() | WSARecv() Failed : %u",
			GetLastError() );
		
		return false;
	}
	return true;
}

bool cConnection::SendPost( int nSendSize )
{
	DWORD dwBytes;
	//보내는 양이 있다면, 즉 IocpServer class의 DoSend()에서 불린게 아니라 PrepareSendPacket()함수를 부르고 
	//SendPost가 불렸다면 보내는 양이 있고 DoSend에서 불렸다면 0이 온다.
	if( nSendSize > 0 )
		m_ringSendBuffer.SetUsedBufferSize( nSendSize );

	if( InterlockedCompareExchange( (LPLONG)&m_bIsSend , FALSE, TRUE) == TRUE )
	{
		int nReadSize;
		char* pBuf = m_ringSendBuffer.GetBuffer( m_nSendBufSize , &nReadSize );
		if( NULL == pBuf )
		{
			InterlockedExchange( (LPLONG)&m_bIsSend , TRUE);
			return false;
		}
		m_lpSendOverlappedEx->s_dwRemain = 0;
		m_lpSendOverlappedEx->s_eOperation = OP_SEND;
		m_lpSendOverlappedEx->s_nTotalBytes = nReadSize;
		ZeroMemory( &m_lpSendOverlappedEx->s_Overlapped , sizeof( OVERLAPPED ));
		m_lpSendOverlappedEx->s_WsaBuf.len = nReadSize;
		m_lpSendOverlappedEx->s_WsaBuf.buf = pBuf;
		m_lpSendOverlappedEx->s_lpConnection = this;
		
		IncrementSendIoRefCount();

		int ret = WSASend(
			m_socket,
			&m_lpSendOverlappedEx->s_WsaBuf,
			1,
			&dwBytes,
			0,
			&m_lpSendOverlappedEx->s_Overlapped,
			NULL );
		if( ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING )
		{
			DecrementSendIoRefCount();
            IocpServer()->CloseConnection( this );
			LOG( LOG_ERROR_NORMAL ,
				"[ERROR] socket[%u] WSASend(): SOCKET_ERROR, %u\n",
			m_socket , WSAGetLastError() );
			InterlockedExchange( (LPLONG)&m_bIsSend , FALSE);
			return false;
		}
	}
	return true;
}
