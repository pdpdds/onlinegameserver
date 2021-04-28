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
	m_bIsDead = false;
	m_dwPos = 0;	//플레이어 위치          
	m_byArea = 0xFF;
}
