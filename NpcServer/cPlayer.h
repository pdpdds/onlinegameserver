#pragma once

class cPlayer
{
public:
	cPlayer(void);
	~cPlayer(void);
	
	inline DWORD GetPKey() { return m_dwPKey; }
	inline void SetPKey( DWORD dwPKey ) { m_dwPKey = dwPKey; }
	inline DWORD GetPos() { return m_dwPos; }
	inline void	SetPos( DWORD dwPos ) { m_dwPos = dwPos; }
	inline BYTE	GetArea() { return m_byArea; }
	inline void SetArea( BYTE byArea ) { m_byArea = byArea; }
	inline bool GetIsDead() { return m_bIsDead; }
	inline void SetIsDead( bool bIsDead ) { m_bIsDead = bIsDead; }
	//변수를 초기화 시킨다.
	void Init();
	


private:
	//캐릭터의 기본 속성
	DWORD	m_dwPKey;	//플레이어 고유 번호
	DWORD	m_dwPos;	//플레이어 현재 위치          
	bool	m_bIsDead;	//죽었는지 판단
	BYTE	m_byArea;	//영역

};
