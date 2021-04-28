#pragma once
#include "cFsm.h"
class cNpc
{
public:
	cNpc(void);
	virtual ~cNpc(void);

	virtual void	OnProcess();  //NPC 상태 처리
	virtual void	Init();       //초기화
	virtual void	DoWander();   //랜덤으로 움직임
	virtual void	DoAttack();   //플레이어 공격

	void			SetTempNpcInfo();
	inline DWORD	GetKey() { return m_dwNpcKey; }
	inline DWORD	GetPos() { return m_dwPos; }
	inline void		SetPos( DWORD dwCPos ) { m_dwPos = dwCPos; }
	inline char*	GetName() { return m_szName; }
	inline eNpcType GetType() { return m_eNpcType; }
	inline void		SetType( eNpcType NpcType ) { m_eNpcType = NpcType; }
	inline eState	GetState() { return m_pFsmClass->GetCurState(); }
	inline void		SetEvent( eEvent Event ) { m_pFsmClass->TranslateState( Event ); }
	inline void		SetTagetPlayerPKey( DWORD dwTagetPlayerPKey ) 
	{ m_dwTagetPlayerPKey = dwTagetPlayerPKey; }

protected:
	eNpcType	m_eNpcType;		//NPC종류
	DWORD		m_dwPos;		//NPC위치
	
	char		m_szName[ MAX_NPCNAME ];	//NPC이름
	DWORD		m_dwNpcKey;					//NPC키
	DWORD		m_dwTagetPlayerPKey;		//탐지된 플레이어 개인키

	cFsm::cFsmClass*  m_pFsmClass;			//유한 상태 기계 클래스
};
