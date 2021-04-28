#pragma once

class cProcessPacket
{
public:
	cProcessPacket(void);
	~cProcessPacket(void);

	static void fnNPCLoginPlayerCn( cConnection* pConnection,  DWORD dwSize , char* pRecvedMsg );
	static void fnNPCMovePlayerCn( cConnection* pConnection,  DWORD dwSize , char* pRecvedMsg );
	static void fnNPCLogoutPlayerCn( cConnection* pConnection,  DWORD dwSize , char* pRecvedMsg );
	static void fnNPCDeadPlayerCn( cConnection* pConnection,  DWORD dwSize , char* pRecvedMsg );
	
};
