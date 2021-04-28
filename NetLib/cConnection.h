#pragma once
#include "cQueue.h"
#include "cRingBuffer.h"
class NETLIB_API cConnection : public cMonitor
{
public:
	cConnection(void);
	~cConnection(void);

public:
	void 	InitializeConnection();
	bool 	CreateConnection( INITCONFIG &initConfig );
	bool 	CloseConnection ( bool bForce=FALSE );
	bool	ConnectTo( char* szIp , unsigned short usPort );
	bool 	BindIOCP( HANDLE& hIOCP );
	bool 	RecvPost( char* pNext , DWORD dwRemain );
	bool 	SendPost( int nSendSize );
	void 	SetSocket(SOCKET socket) { m_socket = socket; }
	SOCKET 	GetSocket() { return m_socket; }
	bool 	BindAcceptExSock();
	char* 	PrepareSendPacket(int slen);
	bool 	ReleaseRecvPacket();
	bool 	ReleaseSendPacket( LPOVERLAPPED_EX lpSendOverlappedEx = NULL );
	
	inline void  SetConnectionIp(char* Ip) { memcpy( m_szIp , Ip , MAX_IP_LENGTH );  }
	inline BYTE*  GetConnectionIp() { return m_szIp; }
	
	inline int  GetIndex() { return m_nIndex; }

	inline int  GetRecvBufSize() { return m_nRecvBufSize; }
    inline int  GetSendBufSize() { return m_nSendBufSize; }

	inline int  GetRecvIoRefCount() { return m_dwRecvIoRefCount; }
	inline int  GetSendIoRefCount() { return m_dwSendIoRefCount; }
	inline int  GetAcceptIoRefCount() { return m_dwAcceptIoRefCount; }

	inline int  IncrementRecvIoRefCount() 
	{ return InterlockedIncrement( (LPLONG)&m_dwRecvIoRefCount ); }
	inline int  IncrementSendIoRefCount() 
	{ return InterlockedIncrement( (LPLONG)&m_dwSendIoRefCount ); }
	inline int  IncrementAcceptIoRefCount() 
	{ return InterlockedIncrement( (LPLONG)&m_dwAcceptIoRefCount ); }
	inline int  DecrementRecvIoRefCount() 
	{ return ( m_dwRecvIoRefCount ? InterlockedDecrement( (LPLONG)&m_dwRecvIoRefCount ) : 0); }
	inline int  DecrementSendIoRefCount() 
	{ return ( m_dwSendIoRefCount ? InterlockedDecrement( (LPLONG)&m_dwSendIoRefCount ) : 0); }
	inline int  DecrementAcceptIoRefCount() 
	{ return ( m_dwAcceptIoRefCount ? InterlockedDecrement( (LPLONG)&m_dwAcceptIoRefCount ) : 0); }


public:

	//Overlapped I/O 요청을 위한 변수
	LPOVERLAPPED_EX		m_lpRecvOverlappedEx;
	LPOVERLAPPED_EX		m_lpSendOverlappedEx;
	//클라이언트와 데이터 송수신을 위한 링 버퍼
	cRingBuffer		m_ringRecvBuffer;
	cRingBuffer		m_ringSendBuffer;
	//클라이언트 주소를 받기위한 버퍼
	char			m_szAddressBuf[ 1024 ];
	//클라이언트와 연결 종료가 되었는지 여부
	BOOL			m_bIsClosed;
	//클라이언트와 연결이 되어있는지 여부
	BOOL			m_bIsConnect;
	//현재 Overlapped I/O 전송 작업을 하고 있는지 여부
	BOOL			m_bIsSend;

private:
	SOCKET		m_socket;
	//한번에 수신할 수 있는 데이터의 최대 크기
	int			m_nRecvBufSize;
	//한번에 송신할 수 있는 데이터의 최대 크기
	int			m_nSendBufSize;
	
	BYTE		m_szIp[MAX_IP_LENGTH];	
	int			m_nIndex;
	
	cMonitor	m_csConnection;
	
	SOCKET		m_sockListener;
	HANDLE		m_hIOCP;
	//Overlapped i/o작업을 요청한 개수
	DWORD		m_dwSendIoRefCount;
	DWORD		m_dwRecvIoRefCount;
	DWORD		m_dwAcceptIoRefCount;

private:
	
	cConnection(const cConnection &rhs);
    cConnection &operator=(const cConnection &rhs);
};

