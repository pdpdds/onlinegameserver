#pragma once
#include "GameClientDlg.h"
class cProcessPacket 
{
public:
	cProcessPacket(void);
	~cProcessPacket(void);
	static void fnMyPlayerInfoAq( CGameClientDlg* pClientDlg , char* pRecvBuf , int nRecvLen );
	static void fnWorldPlayerInfoVAq( CGameClientDlg* pClientDlg ,char* pRecvBuf , int nRecvLen );
	static void fnLoginPlayerSn( CGameClientDlg* pClientDlg ,char* pRecvBuf , int nRecvLen );
	static void fnLogoutPlayerSn( CGameClientDlg* pClientDlg ,char* pRecvBuf , int nRecvLen );
	static void fnMovePlayerSn( CGameClientDlg* pClientDlg ,char* pRecvBuf , int nRecvLen );
	//NPC관련 패킷
	static void fnNPCNpcInfoVSn( CGameClientDlg* pClientDlg ,char* pRecvBuf , int nRecvLen );
	static void fnNPCUpdateNpcVSn( CGameClientDlg* pClientDlg ,char* pRecvBuf , int nRecvLen );
	static void fnAttackNpcToPlayerSn( CGameClientDlg* pClientDlg ,char* pRecvBuf , int nRecvLen );
	
};
