#pragma once

class cConnectionManager :  public cMonitor , cSingleton
{
	DECLEAR_SINGLETON( cConnectionManager )
public:
	cConnectionManager(void);
	~cConnectionManager(void);

	typedef pair< cConnection* , DWORD >  CONN_PAIR;
	typedef map< cConnection* , DWORD >   CONN_MAP;
	typedef CONN_MAP::iterator	 CONN_IT;	

	bool CreateConnection( INITCONFIG &initConfig , DWORD dwMaxConnection );
	bool AddConnection( cConnection* pConnection );
	bool RemoveConnection( cConnection* pConnection );
	inline int __fastcall	GetConnectionCnt() { return (int)m_mapConnection.size(); }
	
	void BroadCast_Chat( char* szIP , char* szChatMsg );

protected:
	CONN_MAP		m_mapConnection;
	cConnection*	m_pConnection;
	cMonitor		m_csConnection;

};
CREATE_FUNCTION( cConnectionManager , ConnectionManager)