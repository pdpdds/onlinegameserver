#pragma once
#include "cPlayer.h"
#include "cDetectNpc.h"
class cPlayerManager : public cSingleton , cMonitor
{
	DECLEAR_SINGLETON( cPlayerManager );
public:
	cPlayerManager(void);
	~cPlayerManager(void);

	typedef pair< DWORD , cPlayer* >  PLAYER_PAIR;
	typedef map< DWORD , cPlayer* >   PLAYER_MAP;
	typedef PLAYER_MAP::iterator	  PLAYER_IT;	

	bool CreatePlayer( DWORD dwMaxPlayer );
	bool AddPlayer( cPlayer* pPlayer );
	bool RemovePlayer( DWORD dwPKey );
	cPlayer* FindPlayer( DWORD dwPKey );
	cPlayer* GetEmptyPlayer();
	inline int GetPlayerCnt() { return (int)m_mapPlayer.size(); }

	void DetectPlayerFromNpc( cDetectNpc* pDetectNpc );

protected:
	PLAYER_MAP		m_mapPlayer;
	cPlayer*		m_pPlayer;
	cMonitor		m_csPlayer;
	DWORD			m_dwMaxPlayer;

};

CREATE_FUNCTION( cPlayerManager , PlayerManager );

