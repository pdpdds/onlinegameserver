#include "StdAfx.h"
#include ".\cnpcmanager.h"

IMPLEMENT_SINGLETON( cNpcManager );

cNpcManager::cNpcManager(void)
{
	
}

cNpcManager::~cNpcManager(void)
{
	NPC_IT npc_it;
	for( npc_it = m_mapNpc.begin() ; npc_it != m_mapNpc.end() ; npc_it++ )
	{
		cNpc* pNpc = (cNpc*)npc_it->second;
		delete pNpc;
	}
	m_mapNpc.clear();
}

bool cNpcManager::AddNpc( DWORD dwNpcKey , DWORD dwNpcPos , DWORD dwNpcType , char* szNpcName )
{
	NPC_IT npc_it;
	npc_it = m_mapNpc.find( dwNpcKey );
	//이미 같은키의 NPC가 추가되어 있다.
	if( npc_it != m_mapNpc.end() )
	{
		LOG( LOG_ERROR_LOW ,
			"SYSTEM | cNpcManager::AddNpc() | NpcKey(%d)는 이미 m_mapNpc에 존재하고 있습니다.",
			dwNpcKey );
		return false;
	}
	cNpc* pNpc = new cNpc;
	pNpc->SetPos( dwNpcPos );
	pNpc->SetKey( dwNpcKey );
	pNpc->SetName( szNpcName );
	pNpc->SetType( (eNpcType)dwNpcType );

	m_mapNpc.insert( NPC_PAIR( pNpc->GetKey() , pNpc ) );
	return true;
}

bool cNpcManager::RemoveNpc( DWORD dwNpcKey )
{
	NPC_IT npc_it;
	npc_it = m_mapNpc.find( dwNpcKey );
	//같은키의 NPC가 존재하지 않는다.
	if( npc_it == m_mapNpc.end() )
	{
		LOG( LOG_ERROR_LOW ,
			"SYSTEM | cNpcManager::RemoveNpc() | NpcKey(%d)는 m_mapNpc에 존재하지 않습니다.",
			dwNpcKey );
		return false;
	}

	m_mapNpc.erase( npc_it );
	return true;
}

cNpc* cNpcManager::FindNpc( DWORD dwNpcKey )
{
    NPC_IT npc_it;
	npc_it = m_mapNpc.find( dwNpcKey );
	//같은키의 NPC가 존재하지 않는다.
	if( npc_it == m_mapNpc.end() )
	{
		LOG( LOG_ERROR_LOW ,
			"SYSTEM | cNpcManager::FindNpc() | NpcKey(%d)는 m_mapNpc에 존재하지 않습니다.",
			dwNpcKey );
		return NULL;
	}
	return (cNpc*)npc_it->second;
}

void cNpcManager::DrawNpc( CDC* pDC )
{
	cPlayer* pMyPlayer = PlayerManager()->GetMyPlayer();
	if( NULL == pMyPlayer )
		return;
    int nPosX = pMyPlayer->GetPos() % COL_LINE;
	int nPosY = pMyPlayer->GetPos() / COL_LINE;
		
	BYTE byMyArea = ( ( nPosY / CELLROW_LINE ) * ALL_CELLCOL_LINE ) + nPosX / CELLROW_LINE; 
	//다른 플레이어를 파란 네모로 그린다.
	NPC_IT npc_it;
	for( npc_it = m_mapNpc.begin() ; npc_it != m_mapNpc.end() ; npc_it++ )
	{
		
		cNpc* pNpc = (cNpc*)npc_it->second;
		nPosX = pNpc->GetPos() % COL_LINE;
		nPosY = pNpc->GetPos() / COL_LINE;
		
		BYTE byNpcArea = ( ( nPosY / CELLROW_LINE ) * ALL_CELLCOL_LINE ) + nPosX / CELLROW_LINE; 
		//내 플레이어 근처에 없다면 그리지 않는다.
		int nDiff1 = ( byNpcArea % ALL_CELLCOL_LINE ) - ( byMyArea % ALL_CELLCOL_LINE );
		int nDiff2 = ( byNpcArea / ALL_CELLCOL_LINE ) - ( byMyArea / ALL_CELLCOL_LINE );
		if( abs( nDiff1 ) > 1 || abs( nDiff2 ) > 1 )
			continue;


		CBrush brushNpc;
		LOGBRUSH logbrushNpc;
		logbrushNpc.lbStyle = BS_SOLID;
		logbrushNpc.lbHatch = 0;
		if( pNpc->GetType() == DETECT_NPC &&  pNpc->GetState() == NPC_NORMAL )
			logbrushNpc.lbColor = NPCDETECT_COLOR;
		else if( pNpc->GetType() == DETECT_NPC &&  pNpc->GetState() == NPC_DISCOMPORT )
			logbrushNpc.lbColor = NPCDISCOMPORT_COLOR;
		else if( pNpc->GetType() == DETECT_NPC &&  pNpc->GetState() == NPC_ANGRY )
			logbrushNpc.lbColor = NPCANGLY_COLOR;
		else
			logbrushNpc.lbColor = NPCNORMAL_COLOR;
		
		brushNpc.CreateBrushIndirect( &logbrushNpc );
		CBrush* oldBrushNpc = pDC->SelectObject( &brushNpc );
		nPosX *= TILE_SIZE;
		nPosY *= TILE_SIZE;
		pDC->Rectangle( CRect( nPosX + START_XPOS , nPosY + START_YPOS ,
			nPosX + TILE_SIZE + START_XPOS, nPosY + TILE_SIZE + START_YPOS) );
		pDC->SelectObject( oldBrushNpc );
		DeleteObject( (HGDIOBJ)&brushNpc );

	}
}

cNpc* cNpcManager::GetNpcByPos( DWORD dwTagetPos )
{
	NPC_IT npc_it;
	for( npc_it = m_mapNpc.begin() ; npc_it != m_mapNpc.end() ; npc_it++ )
	{
		cNpc* pNpc = (cNpc*)npc_it->second;
		
		if( pNpc->GetPos() == dwTagetPos )
			return pNpc;
	}
	return NULL;
}

