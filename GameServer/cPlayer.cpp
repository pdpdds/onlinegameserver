#include "StdAfx.h"
#include ".\cplayer.h"

cPlayer::cPlayer(void)
{
	srand( GetTickCount() );
	Init();
}

cPlayer::~cPlayer(void)
{
}

//변수를 초기화 시킨다.
void cPlayer::Init()
{
	m_dwPKey = 0;
	ZeroMemory( m_szId , MAX_ID_LENGTH );
	ZeroMemory( m_szNickName , MAX_NICKNAME_LENGTH );
	ZeroMemory( m_szName , MAX_NAME_LENGTH );
	
	memset( m_byInActiveAreas , 0xFF , MAX_INACTIVE_AREAS );
	memset( m_byActiveAreas , 0xFF , MAX_ACTIVE_AREAS );
	
	m_dwBPos = 0;   //플레이어 이전 위치
	m_dwPos = 0;	//플레이어 위치          
	m_byLevel = 0;	//레벨
	m_byStr = 0;	//공격력
	m_byDur = 0;	//방어력
	m_dwHp = 0;		//생명력
	m_dwExp = 0;	//경험치
	m_byArea = 0xFF;
	m_bIsConfirm = false; 
}

//테스트를 위해 정보를 임시로 채운다.
void cPlayer::SetTempPlayInfo()
{
	sprintf( m_szId , "player%d" , m_dwPKey );
	sprintf( m_szNickName , "강군%d" , m_dwPKey );
	sprintf( m_szName , "홍길동%d" , m_dwPKey );
	
	m_dwPos = rand() % 3600 + 1;
	m_byLevel = rand() % 10 + 1;
	m_byStr = rand() % 30 + 1;
	m_byDur = rand() % 30 + 1;
	m_dwHp = 300;
	m_dwExp = rand() % 40;
	m_byArea = AreaManager()->GetPosToArea( m_dwPos );
	AreaManager()->UpdateActiveAreas( this );
	AreaManager()->AddPlayerToArea( this , m_byArea );
}

void cPlayer::Send_PlayerInfo()
{
	MyPlayerInfoAq* pInfo = (MyPlayerInfoAq*)PrepareSendPacket( sizeof( MyPlayerInfoAq ) );
	if( NULL == pInfo )
		return;
	pInfo->s_sType = MyPlayerInfo_Aq;
	pInfo->s_byDur = GetDur();
	pInfo->s_byLevel = GetLevel();
	pInfo->s_byStr = GetStr();
	pInfo->s_dwExp = GetExp();
	pInfo->s_dwHp = GetHp();
	pInfo->s_dwPKey = GetPKey();
	pInfo->s_dwPos = GetPos();
	strncpy( pInfo->s_szId , GetId() , MAX_ID_LENGTH );
	strncpy( pInfo->s_szName , GetName() , MAX_NAME_LENGTH );
	strncpy( pInfo->s_szNickName , GetNickName() , MAX_NICKNAME_LENGTH );
	SendPost( sizeof( MyPlayerInfoAq ) );
}