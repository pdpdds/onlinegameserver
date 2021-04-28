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
	srand( GetTickCount() );
	cPlayer* pMyPlayer = PlayerManager()->GetMyPlayer();
	if( NULL == pMyPlayer )
		return;
	int nPosX = pMyPlayer->GetPos() % COL_LINE;
	int nPosY = pMyPlayer->GetPos() / COL_LINE;
	int nTemp = 0;
	if( rand()%2 )
		nTemp = ( rand() %5 ) * -1;
	else
		nTemp = ( rand() %5 ) * 1;
			
	//랜덤으로 방향을 설정함
	int nDir = rand() % 3;
	switch( nDir )
	{
		//가로
	case 0:
		{
			nPosX += nTemp;
		}
		break;
		//세로
	case 1:
		{
			nPosY += nTemp;
		}
		break;
		//대각선
	case 2:
		{
			nPosX += nTemp;
			nPosY += nTemp;
		}
		break;
	}
	
	pMyPlayer->SetTPos( (nPosY * COL_LINE) +nPosX );
	///////////////////////////////////////////
	//움직임  패킷을 서버로 보낸다.
	MovePlayerCn Move;
	Move.s_nLength = sizeof( MovePlayerCn );
	Move.s_sType = MovePlayer_Cn;
	Move.s_dwCPos = pMyPlayer->GetPos();
	Move.s_dwTPos = pMyPlayer->GetTPos();
	AsyncSocket()->SendMsg( (char*)&Move , sizeof( MovePlayerCn ));
	
}