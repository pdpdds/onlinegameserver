#pragma once

class cProcessPacket
{
public:
	cProcessPacket(void);
	~cProcessPacket(void);
	//클라이언트에서 받은 패킷
	static void fnLoginPlayerRq( cPlayer* pPlayer,  DWORD dwSize , char* pRecvedMsg );
	static void fnMovePlayerCn( cPlayer* pPlayer,  DWORD dwSize , char* pRecvedMsg );
	static void fnKeepAliveCn( cPlayer* pPlayer,  DWORD dwSize , char* pRecvedMsg );

	//NPC서버에서 받은 패킷
	static void fnNPCNpcInfoVSn( cPlayer* pPlayer,  DWORD dwSize , char* pRecvedMsg );
	static void fnNPCUpdateNpcVSn( cPlayer* pPlayer,  DWORD dwSize , char* pRecvedMsg );
	static void fnNPCAttackNpcToPlayerSn( cPlayer* pPlayer,  DWORD dwSize , char* pRecvedMsg );

};
