#include "StdAfx.h"
#include ".\cplayermanager.h"
#include "resource.h"
#include "GameClientDlg.h"

IMPLEMENT_SINGLETON( cPlayerManager );

cPlayerManager::cPlayerManager(void)
{
	m_pMyPlayer = NULL;
	m_pPlayer = NULL;
	m_dwMaxPlayerCnt = 0;
	
}

 
cPlayerManager::~cPlayerManager(void)
{
	if( NULL == m_pPlayer )
		delete [] m_pPlayer;
}

bool cPlayerManager::CreatePlayer( DWORD dwMaxPlayer )
{
	m_dwMaxPlayerCnt = dwMaxPlayer;
	m_pPlayer = new cPlayer[ dwMaxPlayer ];
	return true;
}

bool cPlayerManager::AddPlayer( cPlayer* pPlayer )
{
	PLAYER_IT player_it;
	player_it = m_mapPlayer.find( pPlayer->GetPKey() );
	//이미 접속되어 있는 플레이어라면
	if( player_it != m_mapPlayer.end() )
	{
		LOG( LOG_INFO_NORMAL , 
			"SYSTEM | cPlayerManager::AddPlayer() | Key[%d]는 이미 m_mapPlayer에 있습니다.",
			pPlayer->GetPKey() );
		return false;
	}
	m_mapPlayer.insert( PLAYER_PAIR( pPlayer->GetPKey() , pPlayer ) );
	return true;
}

bool cPlayerManager::RemovePlayer( DWORD dwPKey )
{
	PLAYER_IT player_it;
	player_it = m_mapPlayer.find( dwPKey );
	//해당 플레이어가 없는 경우
	if( player_it == m_mapPlayer.end() )
	{
		LOG( LOG_INFO_NORMAL , 
			"SYSTEM | cPlayerManager::RemovePlayer() | Key[%d]는 m_mapPlayer에 없습니다.",
			dwPKey );
		return false;
	}
	m_mapPlayer.erase( dwPKey );
	return true;
}

cPlayer* cPlayerManager::FindPlayer( DWORD dwPKey )
{
	PLAYER_IT player_it;
	player_it = m_mapPlayer.find( dwPKey );
	//해당 플레이어가 없는 경우
	if( player_it == m_mapPlayer.end() )
	{
		LOG( LOG_INFO_NORMAL , 
			"SYSTEM | cPlayerManager::FindPlayer() | Key[%d]는 m_mapPlayer에 없습니다.",
			dwPKey );
		return NULL;
	}
	return player_it->second;
}

cPlayer* cPlayerManager::GetEmptyPlayer()
{
	for( int i = 0 ; i < (int)m_dwMaxPlayerCnt ; i++ )
	{
		if( 0 == m_pPlayer[ i ].GetPKey() )
		{
			return &m_pPlayer[ i ];
		}
	}
	return NULL;
}
cPlayer* cPlayerManager::GetPlayerByPos( DWORD dwTagetPos )
{
	PLAYER_IT player_it;
	for( player_it = m_mapPlayer.begin() ; player_it != m_mapPlayer.end() ; player_it++ )
	{
		cPlayer* pPlayer = (cPlayer*)player_it->second;
		
		if( pPlayer->GetPos() == dwTagetPos )
			return pPlayer;
	}
	return NULL;
}

void cPlayerManager::DrawPlayer( CDC* pDC )
{
	if( NULL == m_pMyPlayer )
		return;

    //자신 플레이어를 흰 네모로 그린다.
    int nPosX = m_pMyPlayer->GetPos() % COL_LINE;
	int nPosY = m_pMyPlayer->GetPos() / COL_LINE;
		
	BYTE byMyArea = ( ( nPosY / CELLROW_LINE ) * ALL_CELLCOL_LINE ) + nPosX / CELLROW_LINE; 

	nPosX *= TILE_SIZE;
	nPosY *= TILE_SIZE;

	CBrush brushMyPlayer;
	LOGBRUSH logbrushMyPlayer;
	logbrushMyPlayer.lbStyle = BS_SOLID;
	if( m_pMyPlayer->GetHp() == 0 )
		logbrushMyPlayer.lbColor = DEAD_PLAYER_COLOR;
	else
		logbrushMyPlayer.lbColor = MY_PLAYER_COLOR;
	logbrushMyPlayer.lbHatch = 0;
	brushMyPlayer.CreateBrushIndirect( &logbrushMyPlayer );
	
	CBrush* oldBrushMyPlayer = pDC->SelectObject( &brushMyPlayer );

	pDC->Rectangle( CRect( nPosX + START_XPOS , nPosY + START_YPOS ,
		nPosX + TILE_SIZE + START_XPOS, nPosY + TILE_SIZE + START_YPOS) );
	pDC->SelectObject( oldBrushMyPlayer );
	DeleteObject( &brushMyPlayer );

	//다른 플레이어를 파란 네모로 그린다.
	CBrush brushPlayer;
	LOGBRUSH logbrushPlayer;
	logbrushPlayer.lbStyle = BS_SOLID;
	logbrushPlayer.lbColor = OTHER_PLAYER_COLOR;
	logbrushPlayer.lbHatch = 0;
	brushPlayer.CreateBrushIndirect( &logbrushPlayer );
	
	CBrush* oldBrushPlayer = pDC->SelectObject( &brushPlayer );
	PLAYER_IT player_it;
	for( player_it = m_mapPlayer.begin() ; player_it != m_mapPlayer.end() ; player_it++ )
	{
		cPlayer* pPlayer = (cPlayer*)player_it->second;
		if( pPlayer == m_pMyPlayer )
			continue;
		
        nPosX = pPlayer->GetPos() % COL_LINE;
		nPosY = pPlayer->GetPos() / COL_LINE;
		
		BYTE byPlayerArea = ( ( nPosY / CELLROW_LINE ) * ALL_CELLCOL_LINE ) + nPosX / CELLROW_LINE; 
		//내 플레이어 근처에 없다면 그리지 않는다.
		int nDiff1 = ( byPlayerArea % ALL_CELLCOL_LINE ) - ( byMyArea % ALL_CELLCOL_LINE );
		int nDiff2 = ( byPlayerArea / ALL_CELLCOL_LINE ) - ( byMyArea / ALL_CELLCOL_LINE );

		
		if( abs( nDiff1 ) > 1 || abs( nDiff2 ) > 1 )
		{
			pPlayer->SetVisible( false );
			continue;
		}
		else
		{
			pPlayer->SetVisible( true );
		}

		if( pPlayer->GetVisible() == false )
			continue;

		nPosX *= TILE_SIZE;
		nPosY *= TILE_SIZE;
		pDC->Rectangle( CRect( nPosX + START_XPOS , nPosY + START_YPOS ,
			nPosX + TILE_SIZE + START_XPOS, nPosY + TILE_SIZE + START_YPOS) );
	}
	pDC->SelectObject( oldBrushPlayer );
	DeleteObject( &brushPlayer );
}

bool cPlayerManager::UpdatePlayersPos()
{
	bool bRet = false;
	PLAYER_IT player_it;
	for( player_it = m_mapPlayer.begin() ; player_it != m_mapPlayer.end() ; player_it++ )
	{
		cPlayer* pPlayer = (cPlayer*)player_it->second;
		int nPosX = pPlayer->GetPos() % COL_LINE;
		int nPosY = pPlayer->GetPos() / COL_LINE;
		int nDestPosX =  pPlayer->GetTPos() % COL_LINE;
		int nDestPosY =  pPlayer->GetTPos() / COL_LINE;
		int nInvalidatePosX = nPosX * TILE_SIZE;
		int nInvalidatePosY = nPosY * TILE_SIZE;
		if( ( nDestPosX == nPosX ) && ( nDestPosY == nPosY ) ) 
			continue;
		if( pPlayer->GetTPos() == 0 )
			continue;
		bRet = true;
		//세로 방향
		if( ( nDestPosX == nPosX ) )
		{
			//갈곳이 아래쪽이라면
			if( ( nDestPosY - nPosY ) > 0 )
				pPlayer->SetPos( ( nPosX  ) + ( ( nPosY + 1 ) * COL_LINE ) );
			//갈곳이 위쪽이라면
			else if( ( nDestPosY - nPosY ) < 0 )
				pPlayer->SetPos( ( nPosX ) + ( ( nPosY - 1 ) * COL_LINE ) );
		}
		//가로 방향
		else if( ( nDestPosY == nPosY ) )
		{
			//갈곳이 아래쪽이라면
			if( ( nDestPosX - nPosX ) > 0 )
				pPlayer->SetPos( ( nPosX + 1  ) + ( nPosY * COL_LINE ) );
			//갈곳이 위쪽이라면
			else if( ( nDestPosX - nPosX ) < 0 )
				pPlayer->SetPos( ( nPosX - 1 ) + ( nPosY * COL_LINE ) );
		}

		//대각선 방향
		else if( abs(( nPosX - nDestPosX )) == abs(( nPosY - nDestPosY )) ) 
		{
			//왼쪽 위 대각선
			if( ( ( nDestPosX - nPosX ) < 0 ) && ( ( nDestPosY - nPosY ) < 0 ) )
				pPlayer->SetPos( ( nPosX - 1  ) + ( ( nPosY - 1 ) * COL_LINE ) );
			//오른쪽 아래 대각선
			else if( ( ( nDestPosX - nPosX ) > 0 ) && ( ( nDestPosY - nPosY ) > 0 ) )
				pPlayer->SetPos( ( nPosX + 1  ) + ( ( nPosY + 1 ) * COL_LINE ) );
			//오른쪽 위 대각선
			else if( ( ( nDestPosX - nPosX ) > 0 ) && ( ( nDestPosY - nPosY ) < 0 ) )
				pPlayer->SetPos( ( nPosX + 1  ) + ( ( nPosY - 1 ) * COL_LINE ) );
			//왼쪽 아래  대각선
			else if( ( ( nDestPosX - nPosX ) < 0 ) && ( ( nDestPosY - nPosY ) > 0 ) )
				pPlayer->SetPos( ( nPosX - 1  ) + ( ( nPosY + 1 ) * COL_LINE ) );
		}
	}
	return bRet;
}