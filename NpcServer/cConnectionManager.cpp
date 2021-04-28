#include "StdAfx.h"
#include ".\cconnectionmanager.h"

IMPLEMENT_SINGLETON( cConnectionManager )

cConnectionManager::cConnectionManager(void)
{
	m_pConnection = NULL;
}

cConnectionManager::~cConnectionManager(void)
{
	if( NULL == m_pConnection )
		delete [] m_pConnection;
}

bool cConnectionManager::CreateConnection( INITCONFIG &initConfig , DWORD dwMaxConnection )
{
	cMonitor::Owner lock( m_csConnection );

	m_pConnection = new cConnection[ dwMaxConnection ];
	for( int i=0 ; i < (int)dwMaxConnection ; i++ )
	{
		initConfig.nIndex = i ;
		if( m_pConnection[i].CreateConnection( initConfig ) ==	false )
			return false;
	}
	return true;
}

bool cConnectionManager::AddConnection( cConnection* pConnection )
{
	cMonitor::Owner lock( m_csConnection );
	CONN_IT conn_it;
	conn_it = m_mapConnection.find( pConnection );
	//이미 접속되어 있는 연결이라면
	if( conn_it != m_mapConnection.end() )
	{
		LOG( LOG_INFO_NORMAL , 
			"SYSTEM | cConnectionManager::AddConnection() | index[%d]는 이미 m_mapConnection에 있습니다.",
			pConnection->GetIndex() );
		return false;
	}
	m_mapConnection.insert( CONN_PAIR( pConnection , GetTickCount() ) );
	return true;
}

bool cConnectionManager::RemoveConnection( cConnection* pConnection )
{
	cMonitor::Owner lock( m_csConnection );
	CONN_IT conn_it;
	conn_it = m_mapConnection.find( pConnection );
	//접속되어 있는 연결이 없는경우
	if( conn_it == m_mapConnection.end() )
	{
		LOG( LOG_INFO_NORMAL , 
			"SYSTEM | cConnectionManager::RemoveConnection() | index[%d]는 m_mapConnection에 없습니다.",
			pConnection->GetIndex() );
		return false;
	}
	m_mapConnection.erase( pConnection );
	return true;
}

void cConnectionManager::Send_GatherVBuffer()
{
	cMonitor::Owner lock( m_csConnection );
	CONN_IT conn_it;
	for( conn_it = m_mapConnection.begin() ; conn_it != m_mapConnection.end() ; conn_it++ )
	{
		cConnection* pConnection = (cConnection*)conn_it->first;
		char* pSendBuffer = pConnection->PrepareSendPacket( VBuffer()->GetCurBufSize() );
		if( NULL == pSendBuffer )
			continue;
		VBuffer()->CopyBuffer( pSendBuffer );
		pConnection->SendPost( VBuffer()->GetCurBufSize() );
	}
}

void cConnectionManager::Send_AttackNpcToPlayerSn( DWORD dwNpcKey , DWORD dwPKey )
{
	cMonitor::Owner lock( m_csConnection );
	CONN_IT conn_it;
	for( conn_it = m_mapConnection.begin() ; conn_it != m_mapConnection.end() ; conn_it++ )
	{
		cConnection* pConnection = (cConnection*)conn_it->first;
		NPCAttackNpcToPlayerSn* pAttackNpc = 
			(NPCAttackNpcToPlayerSn*)pConnection->PrepareSendPacket( sizeof( NPCAttackNpcToPlayerSn ) );
		if( NULL == pAttackNpc )
			continue;
		pAttackNpc->s_sType = NPC_AttackNpcToPlayer_Sn;
		pAttackNpc->s_dwNpcKey = dwNpcKey;
		pAttackNpc->s_dwPKey = dwPKey;
		pConnection->SendPost( sizeof( NPCAttackNpcToPlayerSn ) );
	}
}