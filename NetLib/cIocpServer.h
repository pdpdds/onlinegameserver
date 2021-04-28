#pragma once

class NETLIB_API cIocpServer : public cMonitor 
{
public:
	cIocpServer(void);
	~cIocpServer(void);

	///////////////////////////////   초기화 함수  //////////////////////////
	bool				InitializeSocket();

	//////////////////////////////  동작 함수 /////////////////////////////////
	// Network I/O를 처리하는 Thread
	void	WorkerThread();
	// 순서성 있는 패킷을 처리하는 Thread
	void	ProcessThread();
	// client의 접속 끊기 처리
	bool	CloseConnection( cConnection* lpConnection );
	
	bool	ProcessPacket( cConnection* lpConnection , char* pCurrent , DWORD dwCurrentSize );
	
	virtual bool	ServerStart( INITCONFIG &initConfig );
	virtual bool	ServerOff();
	SOCKET			GetListenSocket() { return m_ListenSock; }
	unsigned short	GetServerPort() { return m_usPort; }
	char*			GetServerIp() { return m_szIp; }
	inline HANDLE	GetWorkerIOCP() { return m_hWorkerIOCP; }
	void	DoAccept( LPOVERLAPPED_EX lpOverlappedEx );
	void	DoRecv( LPOVERLAPPED_EX lpOverlappedEx , DWORD dwIoSize );
	void	DoSend( LPOVERLAPPED_EX lpOverlappedEx , DWORD dwIoSize );
	LPPROCESSPACKET	GetProcessPacket( eOperationType operationType, LPARAM lParam, WPARAM wParam );
	void	ClearProcessPacket( LPPROCESSPACKET lpProcessPacket );
	///////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////  순수 가상 함수   ///////////////////////////////
	//client가 접속될때 호출되는 함수
	virtual	bool	OnAccept( cConnection *lpConnection ) = 0;
	//client에서 packet이 도착하여 순서 성 있는 패킷을 처리할 때 호출되는 함수
	virtual	bool	OnRecv(cConnection* lpConnection, DWORD dwSize , char* pRecvedMsg) = 0;
	//client에서 packet이 도착하여 순서 성 없는 패킷을 처리할 때 호출되는 함수
	virtual	bool	OnRecvImmediately(cConnection* lpConnection, DWORD dwSize , char* pRecvedMsg) = 0;
	//client 접속 종료시 호출되는 함수
	virtual	void	OnClose(cConnection* lpConnection) = 0;
	//서버에서 ProcessThread가 아닌 다른 쓰레드에서 발생시킨 
	//메시지가 순서 성있게 처리되야 한다면 이 함수를 사용.
	virtual bool	OnSystemMsg( cConnection* lpConnection , DWORD dwMsgType , LPARAM lParam ) = 0;
	/////////////////////////////////////////////////////////////////////////////////////
	
	static cIocpServer* GetIocpServer() { return cIocpServer::m_pIocpServer; }
	static cIocpServer* m_pIocpServer;

private:

	bool				CreateProcessThreads();
	bool				CreateWorkerThreads();
	void				GetProperThreadsCount();
	bool				CreateWorkerIOCP();
	bool				CreateProcessIOCP();
	bool				CreateListenSock();
	// No copies do not implement
    cIocpServer(const cIocpServer &rhs);
    cIocpServer &operator=(const cIocpServer &rhs);

protected:
	
	SOCKET				m_ListenSock;

	HANDLE				m_hWorkerIOCP;
	HANDLE				m_hProcessIOCP;
	
	HANDLE				m_hWorkerThread[ MAX_WORKER_THREAD ];
	HANDLE				m_hProcessThread[ MAX_PROCESS_THREAD ];

	unsigned short		m_usPort;
	char				m_szIp[MAX_IP_LENGTH];

	DWORD				m_dwTimeTick;
	DWORD				m_dwWorkerThreadCount;
	DWORD				m_dwProcessThreadCount;

	bool				m_bWorkThreadFlag;
	bool				m_bProcessThreadFlag;

	LPPROCESSPACKET		m_lpProcessPacket;
	DWORD				m_dwProcessPacketCnt;
};

inline cIocpServer* IocpServer()
{
	return cIocpServer::GetIocpServer();
}

