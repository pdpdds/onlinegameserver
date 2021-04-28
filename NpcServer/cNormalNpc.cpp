#include "StdAfx.h"
#include ".\cnormalnpc.h"

cNormalNpc::cNormalNpc(void)
{
	Init();
}

cNormalNpc::~cNormalNpc(void)
{
}

void cNormalNpc::Init()
{
	m_pFsmClass->InsertFsmState( NPC_NORMAL , EVENT_PLAYER_ATTACK , NPC_DISCOMPORT );
	m_pFsmClass->InsertFsmState( NPC_ANGRY , EVENT_PLAYER_RUNAWAY , NPC_ANGRY );
	m_pFsmClass->SetCurFsmState( NPC_NORMAL );
	cNpc::Init();
}

void cNormalNpc::OnProcess()
{
	//switch( m_pFsmClass->GetCurState() )
	//{
	//default:
		//{
			cNpc::OnProcess();
		//}
	//	break;
	//}
}
