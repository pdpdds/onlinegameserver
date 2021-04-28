#include "StdAfx.h"
#include ".\cdetectnpc.h"

cDetectNpc::cDetectNpc(void)
{
	Init();
}

cDetectNpc::~cDetectNpc(void)
{
}

void cDetectNpc::Init()
{
	m_pFsmClass->InsertFsmState( NPC_NORMAL , EVENT_PLAYER_APPEAR , NPC_DISCOMPORT );
	m_pFsmClass->InsertFsmState( NPC_DISCOMPORT , EVENT_PLAYER_ATTACK , NPC_ANGRY );
	m_pFsmClass->InsertFsmState( NPC_DISCOMPORT , EVENT_DISCOMPORT_INDEX , NPC_ANGRY );
	m_pFsmClass->InsertFsmState( NPC_DISCOMPORT , EVENT_PLAYER_RUNAWAY , NPC_NORMAL );
	m_pFsmClass->InsertFsmState( NPC_ANGRY , EVENT_PLAYER_RUNAWAY , NPC_NORMAL );
	
	m_pFsmClass->SetCurFsmState( NPC_NORMAL );
	//코드 삽입
    cNpc::Init();
	InitDetect();
}
void cDetectNpc::InitDetect()
{
	m_dwTagetPlayerPKey = 0;
	m_byDisComportIndex = 0;
}
void cDetectNpc::OnProcess()
{
	switch( m_pFsmClass->GetCurState() )
	{
	case NPC_NORMAL:
		{
			DoDetect();
			DoWander();
		}
		break;
	case NPC_DISCOMPORT:
		{
			DoDisComport();
		}
		break;
	default:
		{
			cNpc::OnProcess();
		}
		break;
	}
}

void cDetectNpc::DoDetect()
{
	PlayerManager()->DetectPlayerFromNpc( this );
}

void cDetectNpc::DoDisComport()
{
	cPlayer* pPlayer = PlayerManager()->FindPlayer( m_dwTagetPlayerPKey );
	//탐지된 대상이 나가거나 없어짐
	if( NULL == pPlayer || pPlayer->GetIsDead() )
	{
		InitDetect();
		m_pFsmClass->TranslateState( EVENT_PLAYER_RUNAWAY );
		return;
	}
	//NPC 탐지 범위 밖으로 플레이어가 도망갔는지 체크
	DWORD dwPlayerPos = pPlayer->GetPos();
	int nDiff1 = ( dwPlayerPos % COL_LINE ) - ( m_dwPos % COL_LINE );
	int nDiff2 = ( dwPlayerPos / COL_LINE ) - ( m_dwPos / COL_LINE );
	if( abs( nDiff1 ) > 1 || abs( nDiff2 ) > 1 )
	{
		InitDetect();
		m_pFsmClass->TranslateState( EVENT_PLAYER_RUNAWAY );
		return;
	}
	//플레이어가 도망가지 않고 계속 있으면 NPC가 화냄 상태로
	++m_byDisComportIndex;
	if( MAX_DISCOMPORT_INDEX == m_byDisComportIndex )
	{
		m_byDisComportIndex = 0;
		m_pFsmClass->TranslateState( EVENT_DISCOMPORT_INDEX );
	}
}