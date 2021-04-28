#pragma once
#include "cTickThread.h"

enum eNpcServerSystemMsg{
	SYSTEM_UPDATE_NPCPOS,
};

class cIocpNpcServer : public cIocpServer , cSingleton
{
	DECLEAR_SINGLETON( cIocpNpcServer );

public:
	cIocpNpcServer(void);
	~cIocpNpcServer(void);

	struct FuncProcess
	{
		void (*funcProcessPacket)( cConnection* pConnection,  DWORD dwSize , char* pRecvedMsg );

		FuncProcess()
		{
			funcProcessPacket = NULL;
		}
	};

	//client가 접속 수락이 되었을 때 호출되는 함수
	virtual	bool	OnAccept( cConnection *lpConnection );
	//client에서 packet이 도착했을 때 순서 성 있게 처리되어지는 패킷처리
	virtual	bool	OnRecv(cConnection* lpConnection,  DWORD dwSize , char* pRecvedMsg);
	//client에서 packet이 도착했을 때 순서 성 없이 곧바로 처리 되는 패킷처리
	virtual	bool	OnRecvImmediately(cConnection* lpConnection,  DWORD dwSize , char* pRecvedMsg);
	//client와 연결이 종료되었을 때 호출되는 함수
	virtual	void	OnClose(cConnection* lpConnection);
	
	virtual bool	OnSystemMsg( cConnection* lpConnection , DWORD dwMsgType , LPARAM lParam );

	//서버 시작 함수
	virtual bool	ServerStart();

	inline cConnection* GetGameServerConn() { return m_pGameServerConn; }
	//.ini화일로 부터 문자열을 읽어온다.
	int				GetINIString( char* szOutStr , char* szAppName , char* szKey , int nSize , char* szFileName );
	//.ini화일로 부터 숫자를 읽어온다.
	int				GetINIInt( char* szAppName , char* szKey , char* szFileName );

	//틱쓰레드 포인터 반환
	inline cTickThread*     GetTickThread() { return m_pTickThread; }
	inline DWORD	GetServerTick() { return m_pTickThread->GetTickCount(); }

	//패킷처리 함수 설정
	void			InitProcessFunc();

	void			ProcessSystemMsg( cConnection* pConnection, DWORD dwMsgType , LPARAM lParam );
private:
	char m_szLogFileName[ MAX_LOGFILENAME_LENGTH ];
	//틱 쓰레드
	cTickThread*		m_pTickThread;
	//패킷 처리함수 포인터
	FuncProcess			m_FuncProcess[ MAX_PROCESSFUNC ];
	cConnection*		m_pGameServerConn;
	
};

CREATE_FUNCTION( cIocpNpcServer , IocpNpcServer );