#pragma once
#include "cPlayer.h"

class cPlayerManager : public cSingleton , cMonitor
{
	DECLEAR_SINGLETON( cPlayerManager );
public:
	cPlayerManager(void);
	~cPlayerManager(void);

	typedef pair< DWORD , cPlayer* >  PLAYER_PAIR;
	typedef map< DWORD , cPlayer* >   PLAYER_MAP;
	typedef PLAYER_MAP::iterator	  PLAYER_IT;	

	bool CreatePlayer( INITCONFIG &initConfig , DWORD dwMaxPlayer );
	bool AddPlayer( cPlayer* pPlayer );
	bool RemovePlayer( cPlayer* pPlayer );
	cPlayer* FindPlayer( DWORD dwPkey );
	inline int GetPlayerCnt() { return (int)m_mapPlayer.size(); }

	void CreateTempPlayer( int nTempPlayerCnt );
	void DestroyTempPlayer();
	void UpdateTempPlayerPos();

	void CheckKeepAliveTick( DWORD dwServerTick );
	
	////////////////////////////////////////////////////////////////
	//전송 관련 함수들..

	//게임 월드에 접속되어있는 플레이어들 정보를 접속한 클라이언트에 전송
	//몇명의 사용자를 보내야할 지 알 수 없으므로 가변길이 패킷으로 보냄
	void Send_WorldPlayerInfosToConnectPlayer( cPlayer* pPlayer );
	//다른 플레이어들에게 플레이어가 접속 하였다는 것을 알림
	void Send_LoginPlayer( cPlayer* pPlayer );
	//다른 플레이어들에게 플레이어가 접속을 종료하였다는 것을 알림
	void Send_LogoutPlayer( cPlayer* pPlayer );
	//플레이어가 이동하였다는 것을 월드에 있는 모든 다른 플레이어들에게 알림
	void Send_MovePlayer( cPlayer* pPlayer );
	//플레이어에게 NPC서버에서 받은 버퍼를 그대로 보낸다.
	void Send_RecvBufferFromNpcServer( char* pNpcInfo , DWORD dwSize );
protected:
	PLAYER_MAP		m_mapPlayer;
	PLAYER_MAP		m_mapTempPlayer;

	cPlayer*		m_pPlayer;
	cPlayer*		m_pTempPlayer;
	cMonitor		m_csPlayer;

};

CREATE_FUNCTION( cPlayerManager , PlayerManager );

