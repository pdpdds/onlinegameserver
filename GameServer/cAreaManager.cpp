#include "StdAfx.h"
#include ".\careamanager.h"

IMPLEMENT_SINGLETON( cAreaManager );

cAreaManager::cAreaManager(void)
{
}

cAreaManager::~cAreaManager(void)
{
}

bool cAreaManager::AddPlayerToArea( cPlayer* pPlayer , BYTE byArea )
{
	cMonitor::Owner lock( m_csArea );
	if( byArea < 0 || byArea >= MAX_AREA )
	{
		LOG( LOG_ERROR_LOW , 
			"SYSTEM | cAreaManager::AddPlayerToArea() | Wrong Area : %d" ,
			byArea );
		return false;
	}

	AREA_IT area_it;
	area_it = m_mapArea[ byArea ].find( pPlayer );
	//이미 접속되어 있는 플레이어라면
	if( area_it != m_mapArea[ byArea ].end() )
	{
		LOG( LOG_ERROR_NORMAL , 
			"SYSTEM | cAreaManager::AddPlayerToArea() | PKey[%d]는 이미 m_mapArea[%d]에 있습니다.",
			pPlayer->GetPKey() , byArea );
		return false;
	}
	pPlayer->SetArea( byArea );
	m_mapArea[ byArea ].insert( AREA_PAIR( pPlayer, byArea ) );
	return true;
}

bool cAreaManager::RemovePlayerFromArea( cPlayer* pPlayer , BYTE byArea )
{
	cMonitor::Owner lock( m_csArea );
	if( byArea < 0 || byArea >= MAX_AREA )
	{
		LOG( LOG_ERROR_LOW , 
			"SYSTEM | cAreaManager::RemovePlayerFromArea() | Wrong Area : %d" ,
			byArea );
		return false;
	}
	
	AREA_IT area_it;
	area_it = m_mapArea[ byArea ].find( pPlayer );
	//이미 접속되어 있는 플레이어라면
	if( area_it == m_mapArea[ byArea ].end() )
	{
		LOG( LOG_ERROR_NORMAL , 
			"SYSTEM | cAreaManager::RemovePlayerFromArea() | PKey[%d]는 m_mapArea[%d]에 존재하지 않습니다.",
			pPlayer->GetPKey() , byArea );
		return false;
	}
	pPlayer->SetArea( 0xFF );
	m_mapArea[ byArea ].erase( pPlayer );
	return true;
}

bool cAreaManager::TransAreaPlayer( cPlayer* pPlayer )
{
	cMonitor::Owner lock( m_csArea );
	BYTE byOldArea = pPlayer->GetArea();
	BYTE byNewArea = GetPosToArea( pPlayer->GetPos() );
	if( 0xFF == byNewArea )
		return false;
	//지역이 바뀌지 않았다면 
	if( byOldArea == byNewArea )
		return false;
	
    bool bRet = RemovePlayerFromArea( pPlayer , byOldArea );
	if( false == bRet )
		return false;
	bRet = AddPlayerToArea( pPlayer , byNewArea );
	if( false == bRet )
	{
		//실패하였다면 아까 빼낸 영역에 다시 넣어 준다.
		AddPlayerToArea( pPlayer , byOldArea );
		return false;
	}
	LOG( LOG_INFO_LOW , 
		"SYSTEM | cAreaManager::UpdateActiveAreas() | 이전 영역[%d] -> 이동 영역[%d]"
		, byOldArea, byNewArea );	
	//활동영역과 비활동 영역을 새롭게 갱신한다.
	UpdateActiveAreas( pPlayer );
	
	return true;
}

void cAreaManager::UpdateActiveAreas( cPlayer* pPlayer )
{
	cMonitor::Owner lock( m_csArea );
	//현재 플레이어가 속한 영역과 주위의 8개의 영역이 플레이어가 영향을
	//줄 수 있는 영역이다. 최대 9개의 영역에 영향을 줄 수 있는데 그 영역을 구한다.
	BYTE byArea = GetPosToArea( pPlayer->GetPos() );
	if( 0xFF == byArea )
		return;

	BYTE byNewActiveAreas[ MAX_ACTIVE_AREAS ];

	memset( byNewActiveAreas , 0xFF , MAX_ACTIVE_AREAS );
	
	//왼쪽 위
	int nArea = byArea - ALL_CELLCOL_LINE - 1;
	if( nArea >= 0 )
		byNewActiveAreas[ DIR_LEFTUP ] = nArea;
    //위
	nArea = byArea - ALL_CELLCOL_LINE ;
	if( nArea >= 0 )
		byNewActiveAreas[ DIR_UP ] = nArea;
	//오른쪽 위
	nArea = byArea - ALL_CELLCOL_LINE + 1;
	if( nArea >= 0 )
		byNewActiveAreas[ DIR_RIGHTUP ] = nArea;
	//왼쪽
	nArea = byArea - 1;
	if( nArea >= 0 )
		byNewActiveAreas[ DIR_LEFT ] = nArea;
	//중간
	byNewActiveAreas[ DIR_CENTER ] = byArea;
	//오른쪽
	nArea = byArea + 1;
	if( nArea < MAX_AREA )
		byNewActiveAreas[ DIR_RIGHT ] = nArea;
	//왼쪽 아래
	nArea = byArea + ALL_CELLCOL_LINE - 1;
	if( nArea < MAX_AREA )
		byNewActiveAreas[ DIR_LEFTDOWN ] = nArea;
	//아래
	nArea = byArea + ALL_CELLCOL_LINE;
	if( nArea > MAX_AREA )
		byNewActiveAreas[ DIR_DOWN ] = nArea;
	//오른쪽 아래
	nArea = byArea + ALL_CELLCOL_LINE + 1;
	if( nArea < MAX_AREA )
		byNewActiveAreas[ DIR_RIGHTDOWN ] = nArea;
	
	///////////////////////////////////////////////////////
	//현재 활동영역과 이전 활동영역을 비교해서 이전 활동 영역중
	//현재 활동하지 않는 영역 구함
	BYTE* pPlayerActiveAreas = pPlayer->GetActiveAreas();
	BYTE* pPlayerInActiveAreas = pPlayer->GetInActiveAreas();
	memset( pPlayerInActiveAreas , 0xFF , MAX_INACTIVE_AREAS );
	BYTE byInActiveAreaCnt = 0;
	for( int i = 0 ; i < MAX_ACTIVE_AREAS ; i++ )
	{
		//설정된 영역이 없다면
		if( 0xFF == pPlayerActiveAreas[ i ] )
			continue;
		int j = 0;
		for(j = 0 ; j < MAX_ACTIVE_AREAS ; j++ )
		{
			if( pPlayerActiveAreas[ i ] == byNewActiveAreas[ j ] )
				break;
		}
		//겹치는 영역이 아니라면 즉 InActiveArea라면
		if( MAX_ACTIVE_AREAS == j )
			pPlayerInActiveAreas[ byInActiveAreaCnt++ ] = pPlayerActiveAreas[ i ];
	}
	//플레이어 활동 영역 새로운 영역으로 갱신한다.
	CopyMemory( pPlayerActiveAreas , byNewActiveAreas , MAX_ACTIVE_AREAS );

	LOG( LOG_INFO_LOW , 
		"SYSTEM | cAreaManager::UpdateActiveAreas() | ActiveArea[%d][%d][%d][%d][%d][%d][%d][%d][%d]"
		, pPlayerActiveAreas[0], pPlayerActiveAreas[1], pPlayerActiveAreas[2], pPlayerActiveAreas[3],
		 pPlayerActiveAreas[4], pPlayerActiveAreas[5], pPlayerActiveAreas[6], pPlayerActiveAreas[7],
		  pPlayerActiveAreas[8] );

	LOG( LOG_INFO_LOW , 
		"SYSTEM | cAreaManager::UpdateActiveAreas() | InActiveArea[%d][%d][%d][%d][%d][%d][%d][%d][%d]"
		, pPlayerInActiveAreas[0], pPlayerInActiveAreas[1], pPlayerInActiveAreas[2], pPlayerInActiveAreas[3],
		 pPlayerInActiveAreas[4], pPlayerInActiveAreas[5], pPlayerInActiveAreas[6], pPlayerInActiveAreas[7],
		  pPlayerInActiveAreas[8] );
	return;
}

BYTE cAreaManager::GetPosToArea( DWORD dwPos )
{
	cMonitor::Owner lock( m_csArea );
	int nPosX = dwPos % COL_LINE;
	int nPosY = dwPos / COL_LINE;

	BYTE byArea = ( ( nPosY / CELLROW_LINE ) * ALL_CELLCOL_LINE ) + nPosX / CELLROW_LINE; 
	if( byArea < 0 || byArea >= MAX_AREA )
	{
		LOG( LOG_ERROR_LOW , 
			"SYSTEM | cAreaManager::GetPosToArea() | Wrong Pos : %d" ,
			dwPos );
		return 0xFF;
	}
	return byArea;
}

void cAreaManager::Send_MovePlayerToActiveAreas( cPlayer* pPlayer )
{
	cMonitor::Owner lock( m_csArea );
	BYTE* pActiveAreas = pPlayer->GetActiveAreas();
	AREA_IT area_it;
	for( int i = 0 ; i < MAX_ACTIVE_AREAS ; i++ )
	{
		BYTE byActiveArea = pActiveAreas[ i ];
		if( 0xFF == byActiveArea || byActiveArea >= MAX_AREA )
			continue;

		for( area_it = m_mapArea[ byActiveArea ].begin(); area_it != m_mapArea[ byActiveArea ].end() ; area_it++ )
		{
			cPlayer* pAreaPlayer = (cPlayer*)area_it->first;
			if( pAreaPlayer == pPlayer || pAreaPlayer->GetIsConfirm() == false )
				continue;
			MovePlayerSn* pMove = (MovePlayerSn*)pAreaPlayer->PrepareSendPacket( sizeof(MovePlayerSn) );
			if( NULL == pMove )
				continue;
			pMove->s_dwPKey = pPlayer->GetPKey();
			pMove->s_sType = MovePlayer_Sn;
			pMove->s_dwCPos = pPlayer->GetBPos();
			pMove->s_dwTPos = pPlayer->GetPos();
			pAreaPlayer->SendPost( sizeof(MovePlayerSn) );
		}
	}
	
}
	
void cAreaManager::Send_MovePlayerToInActiveAreas( cPlayer* pPlayer )
{
	cMonitor::Owner lock( m_csArea );
	BYTE* pInActiveAreas = pPlayer->GetInActiveAreas();
	AREA_IT area_it;
	for( int i = 0 ; i < MAX_INACTIVE_AREAS ; i++ )
	{
		BYTE byInActiveArea = pInActiveAreas[ i ];
		if( 0xFF == byInActiveArea || byInActiveArea >= MAX_AREA )
			continue;

		for( area_it = m_mapArea[ byInActiveArea ].begin(); area_it != m_mapArea[ byInActiveArea ].end() ; area_it++ )
		{
			cPlayer* pAreaPlayer = (cPlayer*)area_it->first;
			if( pAreaPlayer == pPlayer || pAreaPlayer->GetIsConfirm() == false )
				continue;
			MovePlayerSn* pMove = (MovePlayerSn*)pAreaPlayer->PrepareSendPacket( sizeof(MovePlayerSn) );
			if( NULL == pMove )
				continue;
			pMove->s_dwPKey = pPlayer->GetPKey();
			pMove->s_sType = MovePlayer_Sn;
			pMove->s_dwCPos = pPlayer->GetBPos();
			pMove->s_dwTPos = pPlayer->GetPos();
			pAreaPlayer->SendPost( sizeof(MovePlayerSn) );
		}
	}
}