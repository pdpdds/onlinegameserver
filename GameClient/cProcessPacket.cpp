#include "StdAfx.h"
#include ".\cprocesspacket.h"

cProcessPacket::cProcessPacket(void)
{
}

cProcessPacket::~cProcessPacket(void)
{
}

void cProcessPacket::fnMyPlayerInfoAq( CGameClientDlg* pClientDlg ,char* pRecvBuf , int nRecvLen )
{
	MyPlayerInfoAq* pMyInfoAq = (MyPlayerInfoAq*)pRecvBuf;
	
	cPlayer* pMyPlayer = PlayerManager()->GetEmptyPlayer();
	if( NULL == pMyPlayer )
		return;
	PlayerManager()->SetMyPlayer( pMyPlayer );
	pMyPlayer->SetDur( pMyInfoAq->s_byDur );
	pMyPlayer->SetStr( pMyInfoAq->s_byStr );
	pMyPlayer->SetHp( pMyInfoAq->s_dwHp );
	pMyPlayer->SetId( pMyInfoAq->s_szId );
	pMyPlayer->SetName( pMyInfoAq->s_szName );
	pMyPlayer->SetNickName( pMyInfoAq->s_szNickName );
	pMyPlayer->SetLevel( pMyInfoAq->s_byLevel );
	pMyPlayer->SetPKey( pMyInfoAq->s_dwPKey );
	pMyPlayer->SetPos( pMyInfoAq->s_dwPos );
	pMyPlayer->SetExp( pMyInfoAq->s_dwExp );
	PlayerManager()->AddPlayer( pMyPlayer );

	pClientDlg->SetDlgItemText( IDC_ID , pMyPlayer->GetId() );
	pClientDlg->SetDlgItemText( IDC_NAME , pMyPlayer->GetName() );
	pClientDlg->SetDlgItemText( IDC_NICKNAME , pMyPlayer->GetNickName() );
	pClientDlg->SetDlgItemInt( IDC_LEVEL , pMyPlayer->GetLevel() );
	pClientDlg->SetDlgItemInt( IDC_DUR , pMyPlayer->GetDur() );
	pClientDlg->SetDlgItemInt( IDC_STR , pMyPlayer->GetStr() );
	pClientDlg->SetDlgItemInt( IDC_HP , pMyPlayer->GetHp() );
	pClientDlg->SetDlgItemInt( IDC_POS , pMyPlayer->GetPos() );
	pClientDlg->SetDlgItemInt( IDC_EXP , pMyPlayer->GetExp() );
	pClientDlg->SetDlgItemInt( IDC_PKEY , pMyPlayer->GetPKey() );
}

void cProcessPacket::fnWorldPlayerInfoVAq( CGameClientDlg* pClientDlg ,char* pRecvBuf , int nRecvLen )
{
	cPlayer* pMyPlayer = PlayerManager()->GetMyPlayer();
	if( NULL == pMyPlayer )
		return;
	int nPosX = pMyPlayer->GetPos() % COL_LINE;
	int nPosY = pMyPlayer->GetPos() / COL_LINE;

	BYTE byMyArea = ( ( nPosY / CELLROW_LINE ) * ALL_CELLCOL_LINE ) + nPosX / CELLROW_LINE;

	short sPlayerCnt = 0;
	VBuffer()->SetBuffer( pRecvBuf + PACKET_SIZE_LENGTH + PACKET_TYPE_LENGTH );
	VBuffer()->GetShort( sPlayerCnt );			//월드에 있는 플레이어 수 

	for( int i = 0; i < sPlayerCnt ; i++ )
	{
		cPlayer* pPlayer = PlayerManager()->GetEmptyPlayer();
		if( NULL == pPlayer )
			return;
		pPlayer->SetPlayerInfoFromPacket( VBuffer()->GetCurMark() );
		PlayerManager()->AddPlayer( pPlayer );
		nPosX = pPlayer->GetPos() % COL_LINE;
		nPosY = pPlayer->GetPos() / COL_LINE;

		BYTE byPlayerArea = ( ( nPosY / CELLROW_LINE ) * ALL_CELLCOL_LINE ) + nPosX / CELLROW_LINE; 
		//내 플레이어 근처에 없다면 그리지 않는다.
		int nDiff1 = ( byPlayerArea % ALL_CELLCOL_LINE ) - ( byMyArea % ALL_CELLCOL_LINE );
		int nDiff2 = ( byPlayerArea / ALL_CELLCOL_LINE ) - ( byMyArea / ALL_CELLCOL_LINE );
		if( abs( nDiff1 ) > 1 || abs( nDiff2 ) > 1 )
			pPlayer->SetPos( -1 );
	}
}

void cProcessPacket::fnLoginPlayerSn( CGameClientDlg* pClientDlg ,char* pRecvBuf , int nRecvLen )
{
	LoginPlayerSn* pLogin = (LoginPlayerSn*)pRecvBuf;
	cPlayer* pPlayer = PlayerManager()->GetEmptyPlayer();
	if( NULL == pPlayer )
		return;
	pPlayer->SetDur( pLogin->s_byDur );
	pPlayer->SetStr( pLogin->s_byStr );
	pPlayer->SetHp( pLogin->s_dwHp );
	pPlayer->SetId( pLogin->s_szId );
	pPlayer->SetName( pLogin->s_szName );
	pPlayer->SetNickName( pLogin->s_szNickName );
	pPlayer->SetLevel( pLogin->s_byLevel );
	pPlayer->SetPKey( pLogin->s_dwPKey );
	pPlayer->SetPos( pLogin->s_dwPos );
	pPlayer->SetExp( pLogin->s_dwExp );
	PlayerManager()->AddPlayer( pPlayer );
}

void cProcessPacket::fnLogoutPlayerSn( CGameClientDlg* pClientDlg ,char* pRecvBuf , int nRecvLen )
{
	LogoutPlayerSn* pLogout = (LogoutPlayerSn*)pRecvBuf;

	PlayerManager()->RemovePlayer( pLogout->s_dwPKey );

}

void cProcessPacket::fnMovePlayerSn( CGameClientDlg* pClientDlg ,char* pRecvBuf , int nRecvLen )
{
	MovePlayerSn* pMove = (MovePlayerSn*)pRecvBuf;
	cPlayer* pPlayer = PlayerManager()->FindPlayer( pMove->s_dwPKey );
	if( NULL == pPlayer )
		return;
	pPlayer->SetVisible( true );
	pPlayer->SetPos( pMove->s_dwCPos );
	pPlayer->SetTPos( pMove->s_dwTPos );

	if( pClientDlg->m_dwDetailPlayerKey == pMove->s_dwPKey )
	{
		pClientDlg->SetDlgItemInt( IDC_TPOS , pMove->s_dwCPos );
	}
}

void cProcessPacket::fnNPCNpcInfoVSn( CGameClientDlg* pClientDlg ,char* pRecvBuf , int nRecvLen )
{
	short sNpcCnt = 0;
	VBuffer()->SetBuffer( pRecvBuf + PACKET_SIZE_LENGTH + PACKET_TYPE_LENGTH );
	VBuffer()->GetShort( sNpcCnt );			
	DWORD dwNpcKey = 0 , dwNpcPos = 0 , dwNpcType = 0;
	char szNpcName[ MAX_NPCNAME ];
	for( int i = 0 ; i < sNpcCnt ; i++)
	{	
		VBuffer()->GetInteger( (int&)dwNpcKey  );
		VBuffer()->GetInteger( (int&)dwNpcPos );
		VBuffer()->GetInteger( (int&)dwNpcType );
		VBuffer()->GetString( szNpcName );
		NpcManager()->AddNpc( dwNpcKey , dwNpcPos, dwNpcType, szNpcName );
	}

}

void cProcessPacket::fnNPCUpdateNpcVSn( CGameClientDlg* pClientDlg ,char* pRecvBuf , int nRecvLen )
{
	pClientDlg->OutputMsg("[정보] NPC 갱신 정보 받음");
	short sNpcCnt = 0;
	VBuffer()->SetBuffer( pRecvBuf + PACKET_SIZE_LENGTH + PACKET_TYPE_LENGTH );
	VBuffer()->GetShort( sNpcCnt );			
	DWORD dwNpcKey = 0 , dwNpcPos = 0 , dwNpcState = 0;
	for( int i = 0 ; i < sNpcCnt ; i++)
	{	
		VBuffer()->GetInteger( (int&)dwNpcKey  );
		VBuffer()->GetInteger( (int&)dwNpcPos );
		VBuffer()->GetInteger( (int&)dwNpcState );
		
		cNpc* pNpc = NpcManager()->FindNpc( dwNpcKey );
		if( NULL == pNpc )
			continue;
		pNpc->SetPos( dwNpcPos );
		pNpc->SetState( (eState)dwNpcState );
		//상태를 보여주는 NPC 갱신
		if( pClientDlg->m_dwDetailNpcKey == dwNpcKey )
		{
			pClientDlg->SetDlgItemInt( IDC_NPOS , pNpc->GetPos() );
			if( pNpc->GetState() == NPC_NORMAL )
				pClientDlg->SetDlgItemText( IDC_NSTATE , "온순" );
			else if( pNpc->GetState() == NPC_DISCOMPORT )
				pClientDlg->SetDlgItemText( IDC_NSTATE , "흥분" );
			else if( pNpc->GetState() == NPC_ANGRY )
				pClientDlg->SetDlgItemText( IDC_NSTATE , "분노" );
		}
	}
}

void cProcessPacket::fnAttackNpcToPlayerSn( CGameClientDlg* pClientDlg ,char* pRecvBuf , int nRecvLen )
{
	NPCAttackNpcToPlayerSn* pAttack = (NPCAttackNpcToPlayerSn*)pRecvBuf;
	pClientDlg->OutputMsg("[정보] Key(%d) NPC가 PKey(%d) 플레이어를 공격한다." ,
		pAttack->s_dwNpcKey,
		pAttack->s_dwPKey);

	cPlayer* pMyPlayer = PlayerManager()->GetMyPlayer();
	if( NULL == pMyPlayer )
		return;
	if( pMyPlayer->GetPKey() != pAttack->s_dwPKey )
		return;
	if( pMyPlayer->GetHp() > 0 )
	{
		pMyPlayer->SetHp( pMyPlayer->GetHp() - 1 );
		pClientDlg->SetDlgItemInt( IDC_HP , pMyPlayer->GetHp() );
		if( pMyPlayer->GetHp() == 0 )
		{
			pClientDlg->OutputMsg("캐릭터가 죽었습니다.!!!!");
			pClientDlg->SetDlgItemText( IDC_HP , "죽음" );
		}
		
	}
}
