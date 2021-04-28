#include "StdAfx.h"
#include ".\cplayermanager.h"

IMPLEMENT_SINGLETON( cPlayerManager );

cPlayerManager::cPlayerManager(void)
{
	m_pPlayer = NULL;
	m_pTempPlayer = NULL;
}

cPlayerManager::~cPlayerManager(void)
{
	if( NULL == m_pPlayer )
		delete [] m_pPlayer;
	if( NULL == m_pTempPlayer )
		delete [] m_pTempPlayer;
}

bool cPlayerManager::CreatePlayer( INITCONFIG &initConfig , DWORD dwMaxPlayer )
{
	m_pPlayer = new cPlayer[ dwMaxPlayer ];
	for( int i=0 ; i< (int)dwMaxPlayer ; i++ )
	{
		initConfig.nIndex = i ;
		if( m_pPlayer[i].CreateConnection( initConfig ) == false )
			return false;
	}
	return true;
}

bool cPlayerManager::AddPlayer( cPlayer* pPlayer )
{
	cMonitor::Owner lock( m_csPlayer );
	PLAYER_IT player_it;
	player_it = m_mapPlayer.find( pPlayer->GetPKey() );
	//이미 접속되어 있는 플레이어라면
	if( player_it != m_mapPlayer.end() )
	{
		LOG( LOG_ERROR_NORMAL , 
			"SYSTEM | cPlayerManager::AddPlayer() | PKey(%d)는 이미 m_mapPlayer에 있습니다.",
			pPlayer->GetPKey() );
		return false;
	}
	//플레이어 고유키할당
	pPlayer->SetPKey( IocpGameServer()->GeneratePrivateKey() );
	
	m_mapPlayer.insert( PLAYER_PAIR( pPlayer->GetPKey() , pPlayer ) );
	return true;
}

bool cPlayerManager::RemovePlayer( cPlayer* pPlayer )
{
	cMonitor::Owner lock( m_csPlayer );
	PLAYER_IT player_it;
	player_it = m_mapPlayer.find( pPlayer->GetPKey() );
	//해당 플레이어가 없는 경우
	if( player_it == m_mapPlayer.end() )
	{
		LOG( LOG_ERROR_NORMAL , 
			"SYSTEM | cPlayerManager::RemovePlayer() | PKey(%d)는 m_mapPlayer에 없습니다.",
			pPlayer->GetPKey() );
		return false;
	}
	m_mapPlayer.erase( pPlayer->GetPKey() );
	
	AreaManager()->RemovePlayerFromArea( pPlayer, pPlayer->GetArea() );
	return true;
}
cPlayer* cPlayerManager::FindPlayer( DWORD dwPkey )
{
	cMonitor::Owner lock( m_csPlayer );
	PLAYER_IT player_it;
	player_it = m_mapPlayer.find( dwPkey );
	//해당 플레이어가 없는 경우
	if( player_it == m_mapPlayer.end() )
	{
		LOG( LOG_ERROR_NORMAL , 
			"SYSTEM | cPlayerManager::FindPlayer() | PKey(%d)는 m_mapPlayer에 없습니다.",
			dwPkey );
		return NULL;
	}
	return (cPlayer*)player_it->second;
}

void cPlayerManager::CreateTempPlayer( int nTempPlayerCnt )
{
	m_pTempPlayer = new cPlayer[ nTempPlayerCnt ];
	for( int i = 0 ; i < nTempPlayerCnt ; i++ )
	{
		m_pTempPlayer[ i ].SetPKey( IocpGameServer()->GeneratePrivateKey() );
		m_pTempPlayer[ i ].SetTempPlayInfo();
		m_mapTempPlayer.insert( 
			PLAYER_PAIR( m_pTempPlayer[ i ].GetPKey() , &m_pTempPlayer[ i ] ) );
	}
	LOG( LOG_INFO_LOW , 
		"SYSTEM | cPlayerManager::DestroyTempPlayer() | 가상 플레이어 생성 성공!!");
}

void cPlayerManager::DestroyTempPlayer()
{
	cMonitor::Owner lock( m_csPlayer );
	m_mapTempPlayer.clear();
	delete [] m_pTempPlayer;
	LOG( LOG_INFO_LOW , 
		"SYSTEM | cPlayerManager::DestroyTempPlayer() | 가상 플레이어 파괴 성공!!");
}

void cPlayerManager::UpdateTempPlayerPos()
{
	cMonitor::Owner lock( m_csPlayer );
	PLAYER_IT player_it;
	for( player_it = m_mapTempPlayer.begin(); player_it != m_mapTempPlayer.end() ; player_it++ )
	{
		cPlayer* pTempPlayer = player_it->second;
		int nPosX = pTempPlayer->GetPos() % COL_LINE;
		int nPosY = pTempPlayer->GetPos() / COL_LINE;
		int nTemp = 0;
		if( rand()%2 )
			nTemp = ( rand() %2 + 1) * -1;
		else
			nTemp = ( rand() %2 + 1) * 1;

		//랜덤으로 방향을 설정함
		int nDir = rand() % 3;
		switch( nDir )
		{
			//가로
		case 0:
			{
				nPosX += nTemp;
				//필드를 벗어날경우
				if( nPosX <= 0 )
					nPosX += (nTemp * nTemp);
				else if( nPosX >= ROW_LINE )
					nPosX -= (nTemp * nTemp);
			}
			break;
			//세로
		case 1:
			{
				nPosY += nTemp;
				if( nPosY <= 0 )
					nPosY += (nTemp * nTemp);
				else if( nPosY >= ROW_LINE )
					nPosY -= (nTemp * nTemp);
			}
			break;
			//대각선
		case 2:
			{
				nPosX += nTemp;
				nPosY += nTemp;
				if( nPosX <= 0  )
					nPosX += (nTemp * nTemp);
				else if( nPosX >= ROW_LINE )
					nPosX -= (nTemp * nTemp);
				if( nPosY <= 0 )
					nPosY += (nTemp * nTemp);
				else if( nPosY >= ROW_LINE )
					nPosY -= (nTemp * nTemp);
			}
			break;
		}

		pTempPlayer->SetBPos( pTempPlayer->GetPos() );
		pTempPlayer->SetPos( (nPosY * COL_LINE) + nPosX );
		bool bRet = AreaManager()->TransAreaPlayer( pTempPlayer );
		AreaManager()->Send_MovePlayerToActiveAreas( pTempPlayer );
		//현재 플레이어가 다른 지역으로 이동하였다면
		if( true == bRet )
			AreaManager()->Send_MovePlayerToInActiveAreas( pTempPlayer );
		
	}
}

void cPlayerManager::CheckKeepAliveTick( DWORD dwServerTick )
{
	cMonitor::Owner lock( m_csPlayer );
	PLAYER_IT player_it;
	for( player_it = m_mapPlayer.begin(); player_it != m_mapPlayer.end() ; player_it++ )
	{
		cPlayer* pPlayer = (cPlayer*)player_it->second;
		if( pPlayer->GetIsConfirm() == false )
			continue;
		//현재 서버 틱과 플레이어가 KeepAlive_Cn패킷을 보낸 서버 틱과의 차이가 KEEPALIVE_TICK보다
		//크다면 잘못된 연결의 플레이어라고 판단하고 연결을 종료한다.
		if( ( IocpGameServer()->GetServerTick() - pPlayer->GetKeepAliveTick() ) > KEEPALIVE_TICK )
		{
			IocpGameServer()->CloseConnection( pPlayer );
			LOG( LOG_INFO_NORMAL ,
				"SYSTEM | cPlayerManager::CheckKeepAliveTick() |  PKey(%d) 플레이어에게 일정 시간동안 패킷이 오지 않음",
				pPlayer->GetPKey() );
		}
	}
}

void cPlayerManager::Send_WorldPlayerInfosToConnectPlayer( cPlayer* pPlayer )
{
	cMonitor::Owner lock( m_csPlayer );
	PLAYER_IT player_it;
	VBuffer()->Init();
	VBuffer()->SetShort( WorldPlayerInfo_VAq );
	short sPlayerCnt = (short)m_mapPlayer.size() + (short)m_mapTempPlayer.size() - 1;
	VBuffer()->SetShort( sPlayerCnt );
	for( player_it = m_mapPlayer.begin(); player_it != m_mapPlayer.end() ; player_it++ )
	{
		cPlayer* pWorldPlayer = (cPlayer*)player_it->second;
		if( pWorldPlayer == pPlayer )
			continue;
		VBuffer()->SetInteger( pWorldPlayer->GetPKey() );
		VBuffer()->SetString( pWorldPlayer->GetId() );
		VBuffer()->SetString( pWorldPlayer->GetNickName() );
		VBuffer()->SetString( pWorldPlayer->GetName() );
		VBuffer()->SetInteger( pWorldPlayer->GetPos() );
		VBuffer()->SetChar( pWorldPlayer->GetLevel() );
		VBuffer()->SetChar( pWorldPlayer->GetStr() );
		VBuffer()->SetChar( pWorldPlayer->GetDur() );
		VBuffer()->SetInteger( pWorldPlayer->GetHp() );
		VBuffer()->SetInteger( pWorldPlayer->GetExp() );
	}
	for( player_it = m_mapTempPlayer.begin(); player_it != m_mapTempPlayer.end() ; player_it++ )
	{
		cPlayer* pTempPlayer = (cPlayer*)player_it->second;
		VBuffer()->SetInteger( pTempPlayer->GetPKey() );
		VBuffer()->SetString( pTempPlayer->GetId() );
		VBuffer()->SetString( pTempPlayer->GetNickName() );
		VBuffer()->SetString( pTempPlayer->GetName() );
		VBuffer()->SetInteger( pTempPlayer->GetPos() );
		VBuffer()->SetChar( pTempPlayer->GetLevel() );
		VBuffer()->SetChar( pTempPlayer->GetStr() );
		VBuffer()->SetChar( pTempPlayer->GetDur() );
		VBuffer()->SetInteger( pTempPlayer->GetHp() );
		VBuffer()->SetInteger( pTempPlayer->GetExp() );
	}
	char* pWorldPlayerInfos = pPlayer->PrepareSendPacket( VBuffer()->GetCurBufSize() );
	if( NULL == pWorldPlayerInfos )
		return;
	VBuffer()->CopyBuffer( pWorldPlayerInfos );
	pPlayer->SendPost( VBuffer()->GetCurBufSize() );
}

void cPlayerManager::Send_LoginPlayer( cPlayer* pPlayer )
{
	cMonitor::Owner lock( m_csPlayer );
	PLAYER_IT player_it;
	for( player_it = m_mapPlayer.begin(); player_it != m_mapPlayer.end() ; player_it++ )
	{
		cPlayer* pWorldPlayer = (cPlayer*)player_it->second;
		if( pWorldPlayer == pPlayer )
			continue;
		LoginPlayerSn* pLogin = 
			(LoginPlayerSn*)pWorldPlayer->PrepareSendPacket( sizeof(LoginPlayerSn) );
		if( NULL == pLogin )
			continue;
		pLogin->s_sType = LoginPlayer_Sn;
		pLogin->s_byDur = pPlayer->GetDur();
		pLogin->s_byLevel = pPlayer->GetLevel();
		pLogin->s_byStr = pPlayer->GetStr();
		pLogin->s_dwExp = pPlayer->GetExp();
		pLogin->s_dwHp = pPlayer->GetHp();
		pLogin->s_dwPKey = pPlayer->GetPKey();
		pLogin->s_dwPos = pPlayer->GetPos();
		strncpy( pLogin->s_szId , pPlayer->GetId() , MAX_ID_LENGTH );
		strncpy( pLogin->s_szName , pPlayer->GetName() , MAX_NAME_LENGTH );
		strncpy( pLogin->s_szNickName , pPlayer->GetNickName() , MAX_NICKNAME_LENGTH );
		pWorldPlayer->SendPost( sizeof(LoginPlayerSn) );
	}
}

void cPlayerManager::Send_LogoutPlayer( cPlayer* pPlayer )
{
	cMonitor::Owner lock( m_csPlayer );
	PLAYER_IT player_it;
	for( player_it = m_mapPlayer.begin(); player_it != m_mapPlayer.end() ; player_it++ )
	{
		cPlayer* pWorldPlayer = (cPlayer*)player_it->second;
		if( pWorldPlayer == pPlayer )
			continue;
		LogoutPlayerSn* pLogout = 
			(LogoutPlayerSn*)pWorldPlayer->PrepareSendPacket( sizeof(LogoutPlayerSn) );
		if( NULL == pLogout )
			continue;
		pLogout->s_dwPKey = pPlayer->GetPKey();
		pLogout->s_sType = LogoutPlayer_Sn;
		pWorldPlayer->SendPost( sizeof(LogoutPlayerSn) );
	}
}

void cPlayerManager::Send_MovePlayer( cPlayer* pPlayer )
{
	cMonitor::Owner lock( m_csPlayer );
	PLAYER_IT player_it;
	for( player_it = m_mapPlayer.begin(); player_it != m_mapPlayer.end() ; player_it++ )
	{
		cPlayer* pWorldPlayer = (cPlayer*)player_it->second;
		if( pWorldPlayer == pPlayer )
			continue;
		MovePlayerSn* pMove = 
			(MovePlayerSn*)pWorldPlayer->PrepareSendPacket( sizeof(MovePlayerSn) );
		if( NULL == pMove )
			continue;
		pMove->s_dwPKey = pPlayer->GetPKey();
		pMove->s_sType = MovePlayer_Sn;
		pMove->s_dwCPos = pPlayer->GetBPos();
		pMove->s_dwTPos = pPlayer->GetPos();
		pWorldPlayer->SendPost( sizeof(MovePlayerSn) );
	}
}
void cPlayerManager::Send_RecvBufferFromNpcServer( char* pRecvBuffer , DWORD dwSize )
{
	cMonitor::Owner lock( m_csPlayer );
	PLAYER_IT player_it;
	for( player_it = m_mapPlayer.begin(); player_it != m_mapPlayer.end() ; player_it++ )
	{
		cPlayer* pWorldPlayer = (cPlayer*)player_it->second;
		char* pSendBuffer = pWorldPlayer->PrepareSendPacket( dwSize );
		if( NULL == pSendBuffer )
			continue;
		CopyMemory( pSendBuffer , pRecvBuffer , dwSize );
		pWorldPlayer->SendPost( dwSize );
	}
}

