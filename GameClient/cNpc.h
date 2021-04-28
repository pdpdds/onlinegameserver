#pragma once
#include "../NpcServer/cFsm.h"
class cNpc
{
public:
	cNpc(void);
	~cNpc(void);

	void			Init();
	inline DWORD	GetKey() { return m_dwNpcKey; }
	inline void		SetKey( DWORD dwNpcKey ) { m_dwNpcKey = dwNpcKey; }
    inline DWORD	GetPos() { return m_dwCPos; }
	inline void		SetPos( DWORD dwCPos ) { m_dwCPos = dwCPos; }
	inline DWORD	GetTPos() { return m_dwTPos; }
	inline void		SetTPos( DWORD dwTPos ) { m_dwTPos = dwTPos; }
	inline char*	GetName() { return m_szName; }
	inline void		SetName( char* pNpcName ) { strncpy( m_szName , pNpcName , MAX_NPCNAME ); }
	inline eNpcType GetType() { return m_eNpcType; }
	inline void		SetType( eNpcType NpcType ) { m_eNpcType = NpcType; }

	inline void		SetState( eState State ) { m_eState = State; }
	inline eState	GetState() { return m_eState; }
protected:
	//NPCÁ¾·ù
	eNpcType	m_eNpcType;
	DWORD		m_dwCPos;
	DWORD		m_dwTPos;
	eState		m_eState;
	char		m_szName[ MAX_NPCNAME ];
	DWORD		m_dwNpcKey;
	
};
