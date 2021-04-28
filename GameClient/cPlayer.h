#pragma once

class cPlayer
{
public:
	cPlayer(void);
	~cPlayer(void);

	inline DWORD GetPKey() { return m_dwPKey; }
	inline void SetPKey( DWORD dwPKey ) { m_dwPKey = dwPKey; }
	inline char* GetId() { return m_szId; }
	inline void	 SetId( char* szId ) { strncpy( m_szId , szId, MAX_ID_LENGTH ); }
	inline char* GetNickName() { return m_szNickName; }
	inline void	 SetNickName( char* szNickName ) { strncpy( m_szNickName, szNickName, MAX_NICKNAME_LENGTH ); }
	inline char* GetName() { return m_szName; }
	inline void	 SetName( char* szName ) { strncpy( m_szName , szName, MAX_NAME_LENGTH ); }
	inline DWORD GetPos() { return m_dwPos; }
	inline void	SetPos( DWORD dwPos ) { m_dwPos = dwPos; }
	inline DWORD GetTPos() { return m_dwTPos; }
	inline void	SetTPos( DWORD dwTPos ) { m_dwTPos = dwTPos; }
	inline BYTE GetLevel() { return m_byLevel; }
	inline void SetLevel( BYTE byLevel ) { m_byLevel = byLevel; }
	inline BYTE GetStr() { return m_byStr; }
	inline void SetStr( BYTE byStr ) { m_byStr = byStr; }
	inline BYTE GetDur() { return m_byDur; }
	inline void SetDur( BYTE byDur ) { m_byDur = byDur; }
	inline DWORD GetHp() { return m_dwHp; }
	inline void SetHp( DWORD dwHp ) { m_dwHp = dwHp; }
	inline DWORD GetExp() { return m_dwExp; }
	inline void SetExp( DWORD dwExp ) { m_dwExp = dwExp; }
	inline bool GetVisible() { return m_bVisible; }
	inline void SetVisible( bool bVisible ) { m_bVisible = bVisible; }
	

	//변수를 초기화 시킨다.
	void Init();
	//플레이어 정보 패킷을 받아 정보를 설정한다.
	void SetPlayerInfoFromPacket( char* pPlayerInfo );
	
private:
	//캐릭터의 기본 속성
	DWORD	m_dwPKey;	//플레이어 고유 번호
	char	m_szId[ MAX_ID_LENGTH ];  //아이디
	char	m_szNickName[ MAX_NICKNAME_LENGTH ];  //별명
	char	m_szName[ MAX_NAME_LENGTH ];  //이름

	DWORD	m_dwPos;	//플레이어 위치          
	BYTE	m_byLevel;	//레벨
	BYTE	m_byStr;	//공격력
	BYTE	m_byDur;	//방어력
	DWORD	m_dwHp;		//생명력
	DWORD	m_dwExp;	//경험치
	bool	m_bVisible;	//출력

	DWORD	m_dwTPos;	//움직일 위치

};
