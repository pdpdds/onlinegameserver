#include "StdAfx.h"
#include ".\cnpc.h"

cNpc::cNpc(void)
{
	Init();	
}

cNpc::~cNpc(void)
{
	
}

void cNpc::Init()
{
	m_eNpcType = NONE_NPC;
	m_dwCPos = 0;
	m_dwTPos = 0;
	m_dwNpcKey = 0;

	ZeroMemory( m_szName , MAX_NPCNAME );
}
