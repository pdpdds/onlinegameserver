#include "StdAfx.h"
#include ".\cnpcmanager.h"
#include "cDetectNpc.h"
#include "cNormalNpc.h"

IMPLEMENT_SINGLETON( cNpcManager );

cNpcManager::cNpcManager(void)
{
	m_dwGenerateNpcKey = 0;
	srand( GetTickCount()  );
}

cNpcManager::~cNpcManager(void)
{
}

bool cNpcManager::CreateNpc( eNpcType NpcType , DWORD dwNpcCnt )
{
    switch( NpcType )
	{
	case DETECT_NPC:
		{
			cDetectNpc* pDetectNpc = new cDetectNpc[ dwNpcCnt ];
			for( int i = 0 ; i < (int)dwNpcCnt ; i++ )
			{
				pDetectNpc[ i ].SetTempNpcInfo();
				pDetectNpc[ i ].SetType( NpcType );
				AddNpc( &pDetectNpc[ i ] );
			}
		}
		break;
	case NORMAL_NPC:
		{
			cNormalNpc* pNormalNpc = new cNormalNpc[ dwNpcCnt ];
			for( int i = 0 ; i < (int)dwNpcCnt ; i++ )
			{
				pNormalNpc[ i ].SetTempNpcInfo();
				pNormalNpc[ i ].SetType( NpcType );
				AddNpc( &pNormalNpc[ i ] );
			}
		}
		break;
	}
	return true;
}

bool cNpcManager::DestroyNpc()
{
	cMonitor::Owner lock( m_csNpc );
	NPC_IT npc_it;
	for( npc_it = m_mapNpc.begin() ; npc_it != m_mapNpc.end() ; npc_it++ )
	{
		cNpc* pNpc = npc_it->second;
		delete pNpc;
	}
	m_mapNpc.clear();
	return true;
}

bool cNpcManager::AddNpc( cNpc* pNpc )
{
	cMonitor::Owner lock( m_csNpc );
	NPC_IT npc_it;
	npc_it = m_mapNpc.find( pNpc->GetKey() );
	//이미 같은키의 NPC가 추가되어 있다.
	if( npc_it != m_mapNpc.end() )
	{
		LOG( LOG_ERROR_LOW ,
			"SYSTEM | cNpcManager::AddNpc() | NpcKey(%d)는 이미 m_mapNpc에 존재하고 있습니다.",
			pNpc->GetKey() );
		return false;
	}

	m_mapNpc.insert( NPC_PAIR( pNpc->GetKey() , pNpc ) );
	return true;
}

bool cNpcManager::RemoveNpc( DWORD dwNpcKey )
{
	cMonitor::Owner lock( m_csNpc );
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
	cMonitor::Owner lock( m_csNpc );
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

void cNpcManager::UpdateNpc()
{
	cMonitor::Owner lock( m_csNpc );
	NPC_IT npc_it;
	for( npc_it = m_mapNpc.begin() ; npc_it != m_mapNpc.end() ; npc_it++ )
	{
		cNpc* pNpc = (cNpc*)npc_it->second;
		pNpc->OnProcess();
	}
}

void cNpcManager::GatherVBuffer_NpcInfo()
{
	cMonitor::Owner lock( m_csNpc );
	NPC_IT npc_it;
	VBuffer()->Init();
	VBuffer()->SetShort( NPC_NpcInfo_VSn );
	VBuffer()->SetShort( (short)m_mapNpc.size() );
	for( npc_it = m_mapNpc.begin() ; npc_it != m_mapNpc.end() ; npc_it++ )
	{
		cNpc* pNpc = (cNpc*)npc_it->second;
		VBuffer()->SetInteger( pNpc->GetKey() );
		VBuffer()->SetInteger( pNpc->GetPos() );
		VBuffer()->SetInteger( (int)pNpc->GetType() );
		VBuffer()->SetString( pNpc->GetName() );
	}
	
}
void cNpcManager::GatherVBuffer_UpdateNpc()
{
	cMonitor::Owner lock( m_csNpc );
	NPC_IT npc_it;
	VBuffer()->Init();
	VBuffer()->SetShort( NPC_UpdateNpc_VSn );
	VBuffer()->SetShort( (short)m_mapNpc.size() );
	for( npc_it = m_mapNpc.begin() ; npc_it != m_mapNpc.end() ; npc_it++ )
	{
		cNpc* pNpc = (cNpc*)npc_it->second;
		VBuffer()->SetInteger( pNpc->GetKey() );
		VBuffer()->SetInteger( pNpc->GetPos() );
		VBuffer()->SetInteger( (int)pNpc->GetState() );
	}
	
}