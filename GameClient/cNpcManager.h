#pragma once
#include "cNpc.h"

class cNpcManager : public cSingleton
{
	DECLEAR_SINGLETON( cNpcManager );
public:
	cNpcManager(void);
	~cNpcManager(void);

	typedef pair< DWORD , cNpc* >  NPC_PAIR;
	typedef map< DWORD , cNpc* >   NPC_MAP;
	typedef NPC_MAP::iterator   NPC_IT;	

	bool AddNpc( DWORD dwNpcKey , DWORD dwNpcPos , DWORD dwNpcType , char* szNpcName );
	bool RemoveNpc( DWORD dwNpcKey );
	cNpc* FindNpc( DWORD dwNpcKey );
	cNpc* GetNpcByPos( DWORD dwTagetPos );
	inline int GetCnt() { return (int)m_mapNpc.size(); }
	
	void DrawNpc( CDC* pDC );
	
private:
	NPC_MAP		m_mapNpc;

};

CREATE_FUNCTION( cNpcManager , NpcManager );