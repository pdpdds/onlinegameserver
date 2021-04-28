#pragma once
#include "cPlayer.h"

class cPlayerManager : public cSingleton
{
	DECLEAR_SINGLETON( cPlayerManager );
public:
	cPlayerManager(void);
	~cPlayerManager(void);

	typedef pair< DWORD , cPlayer* >  PLAYER_PAIR;
	typedef map< DWORD , cPlayer* >   PLAYER_MAP;
	typedef PLAYER_MAP::iterator	  PLAYER_IT;	

	//플레이어를 생성
	bool CreatePlayer( DWORD dwMaxPlayer );
	//플레이어를 추가
	bool AddPlayer( cPlayer* pPlayer );
	//플레이어를 삭제
	bool RemovePlayer( DWORD dwPKey );
	//고유 키에 해당하는 플레이어를 반환
	cPlayer* FindPlayer( DWORD dwPKey );
	//빈 플레이어 포인터를 반환한다.
	cPlayer* GetEmptyPlayer();
	//내 플레이어 정보를 반환한다.
	cPlayer* GetMyPlayer() { return m_pMyPlayer; }
	//좌표에 있는 플레이어 포인터 반환
	cPlayer* GetPlayerByPos( DWORD dwTagetPos );
	void SetMyPlayer( cPlayer* pPlayer ) { m_pMyPlayer = pPlayer; }
	//플레이어를 찍는다.
	void DrawPlayer( CDC* pDC );
	inline int __fastcall	GetPlayerCnt() { return (int)m_mapPlayer.size(); }

	bool UpdatePlayersPos();
	

protected:
	PLAYER_MAP		m_mapPlayer;
	cPlayer*		m_pPlayer;

	cPlayer*		m_pMyPlayer;

	DWORD			m_dwMaxPlayerCnt;
};

CREATE_FUNCTION( cPlayerManager , PlayerManager );
