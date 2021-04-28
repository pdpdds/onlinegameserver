#include "StdAfx.h"
#include ".\ctickthread.h"

cTickThread::cTickThread(void)
{
}

cTickThread::~cTickThread(void)
{
}

void cTickThread::OnProcess()
{
	if( ( m_dwTickCount % 2 ) == 0 )
	{
		NpcManager()->UpdateNpc();
		IocpNpcServer()->ProcessSystemMsg( (cConnection*)1 , SYSTEM_UPDATE_NPCPOS , 0 );
	}
}