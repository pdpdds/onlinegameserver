#include "StdAfx.h"
#include ".\cprocesspacket.h"

cProcessPacket::cProcessPacket(void)
{
}

cProcessPacket::~cProcessPacket(void)
{
}
void cProcessPacket::fnNPCLoginPlayerCn( cConnection* pConnection,  DWORD dwSize , char* pRecvedMsg )
{
	NPCLoginPlayerCn* pLoginPlayer = (NPCLoginPlayerCn*)pRecvedMsg;
    //플레이어 추가 
	cPlayer* pPlayer = PlayerManager()->GetEmptyPlayer();
	if( NULL == pPlayer )
	{
		LOG( LOG_INFO_LOW,
		"SYSTEM | cProcessPacket::fnNPCLoginPlayerCn() | 더 이상 플레이어가 NPC서버로 로긴할 수 없습니다.");
		return;	
	}
	pPlayer->SetPKey( pLoginPlayer->s_dwPKey );
	pPlayer->SetPos( pLoginPlayer->s_dwPos );
	PlayerManager()->AddPlayer( pPlayer );

	NpcManager()->GatherVBuffer_NpcInfo();
	ConnectionManager()->Send_GatherVBuffer();

	LOG( LOG_INFO_LOW,
		"SYSTEM | cProcessPacket::fnNPCLoginPlayerCn() | PKey(%d)플레이어 로그인, 현재 플레이어 수(%d)",
		pLoginPlayer->s_dwPKey , PlayerManager()->GetPlayerCnt() );
}

void cProcessPacket::fnNPCMovePlayerCn( cConnection* pConnection,  DWORD dwSize , char* pRecvedMsg )
{
	NPCMovePlayerCn* pMovePlayer = (NPCMovePlayerCn*)pRecvedMsg;
	LOG( LOG_INFO_LOW,
		"SYSTEM | cProcessPacket::fnNPCMovePlayerCn() | PKey(%d)플레이어 이동",
		pMovePlayer->s_dwPKey );

	cPlayer* pPlayer = PlayerManager()->FindPlayer( pMovePlayer->s_dwPKey );
	if( NULL == pPlayer )
		return;
	pPlayer->SetPos( pMovePlayer->s_dwPos );
}

void cProcessPacket::fnNPCLogoutPlayerCn( cConnection* pConnection,  DWORD dwSize , char* pRecvedMsg )
{
	NPCLogoutPlayerCn* pLogoutPlayer = (NPCLogoutPlayerCn*)pRecvedMsg;
	//플레이어 제거
	PlayerManager()->RemovePlayer( pLogoutPlayer->s_dwPKey );
	LOG( LOG_INFO_LOW,
		"SYSTEM | cProcessPacket::fnNPCLogoutPlayerCn() | PKey(%d)플레이어 로그아웃, 현재 플레이어 수(%d)",
		pLogoutPlayer->s_dwPKey , PlayerManager()->GetPlayerCnt() );
}

void cProcessPacket::fnNPCDeadPlayerCn( cConnection* pConnection,  DWORD dwSize , char* pRecvedMsg )
{
	NPCDeadPlayerCn* pDeadPlayer = (NPCDeadPlayerCn*)pRecvedMsg;
	LOG( LOG_INFO_LOW,
		"SYSTEM | cProcessPacket::fnNPCLogoutPlayerCn() | PKey(%d)플레이어가 죽음!!",
		pDeadPlayer->s_dwPKey  );
	cPlayer* pPlayer = PlayerManager()->FindPlayer( pDeadPlayer->s_dwPKey );
	if( NULL == pPlayer )
		return;
	pPlayer->SetIsDead( true );

}