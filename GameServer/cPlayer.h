#pragma once

class cPlayer : public cConnection
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
	inline DWORD GetBPos() { return m_dwBPos; }
	inline void	SetBPos( DWORD dwBPos ) { m_dwBPos = dwBPos; }
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
	inline DWORD GetKeepAliveTick() { return m_dwKeepAliveTick; }
	inline void SetKeepAliveTick( DWORD dwKeepAliveTick ) { m_dwKeepAliveTick = dwKeepAliveTick; }
	
	inline bool GetIsConfirm() { return m_bIsConfirm; }
	inline void SetIsConfirm( bool bIsConfirm ) { m_bIsConfirm = bIsConfirm; }


	inline BYTE GetArea() { return m_byArea; }
	inline void SetArea( BYTE byArea ) { m_byArea = byArea; }
	inline BYTE* GetInActiveAreas() { return m_byInActiveAreas; }
	inline BYTE* GetActiveAreas() { return m_byActiveAreas; }

	//변수를 초기화 시킨다.
	void Init();
	//테스트를 위해 정보를 임시로 채운다.
    void SetTempPlayInfo();

	////////////////////////////////////////////////////////////////
	//전송 관련 함수들..

	//정보를 접속된 클라이언트에게 보낸다.
	void Send_PlayerInfo();

	

private:
	//캐릭터의 기본 속성
	DWORD	m_dwPKey;	//플레이어 고유 번호
	char	m_szId[ MAX_ID_LENGTH ];  //아이디
	char	m_szNickName[ MAX_NICKNAME_LENGTH ];  //별명
	char	m_szName[ MAX_NAME_LENGTH ];  //이름

	DWORD	m_dwBPos;	//플레이어 이전 위치          
	DWORD	m_dwPos;	//플레이어 현재 위치          
	BYTE	m_byLevel;	//레벨
	BYTE	m_byStr;	//공격력
	BYTE	m_byDur;	//방어력
	DWORD	m_dwHp;		//생명력
	DWORD	m_dwExp;	//경험치

	DWORD   m_dwKeepAliveTick; //KeepAlive_Cn패킷을 받은 시점의 서버 틱

	BYTE	m_byArea;	//영역

	BYTE	m_byInActiveAreas[ MAX_INACTIVE_AREAS ];
	BYTE	m_byActiveAreas[ MAX_ACTIVE_AREAS ];

	bool    m_bIsConfirm; //인증된 사용자라면
};
