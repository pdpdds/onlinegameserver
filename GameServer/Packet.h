/*
1 ~ 20    : 클라이언트에서 보내는 요청 패킷의 번호
21 ~ 40   : 클라이언트에서 보내는 알림 패킷의 번호
41 ~ 60   : NPC서버에서 보내는 응답 패킷의 번호
61 ~ 80   : NPC서버에서 보내는 알림 패킷의 번호

101 ~ 130 : 서버에서 클라이언트로 보내는 응답 패킷의 번호
131 ~ 160 : 서버에서 클라이언트로 보내는 알림 패킷의 번호
141 ~ 160 : 서버에서 NPC서버로 보내는 요청 패킷의 번호
161 ~ 180 : 서버에서 NPC서버로 보내는 알림 패킷의 번호



__V__    :  서버나 클라이언트에서 보내는 가변 패킷
___Rq  :  클라이언트에서 요청하는 패킷
___Aq  :  서버에서 보내는 응답 패킷
___Cn  :  클라이언트에서 보내는 알림 패킷
___Sn  :  서버에서 보내는 알림 패킷

NPC___Rq :  서버에서 NPC서버에 요청하는 패킷
NPC___Aq :  NPC서버에서 서버에 응답하는 패킷
NPC___Cn :  서버에서 NPC서버에 보내는 알림 패킷
NPC___Sn :  NPC서버에서 서버에 보내는 알림 패킷

가변 길이 패킷의 경우 패킷이름에 ___V__가 붙는다.
*/

enum ePacket{

	LoginPlayer_Rq		= 1,	//로긴 요청
	MyPlayerInfo_Aq		= 101,	//로긴 요청을 한 플레이어 정보
	WorldPlayerInfo_VAq = 102,	//현재 서버에 있는 플레이어들 정보

	LoginPlayer_Sn		= 121,  //로그인한 플레이어 정보를 다른 플레이어들에게 알림
	LogoutPlayer_Sn		= 122,	//로그 아웃을 했다는것을 다른 플레이어에게 알림
	MovePlayer_Cn		= 23,   //플레이어 이동을 서버에 알림
	MovePlayer_Sn		= 123,  //플레이어 이동을 다른 플레이어에게 알림
	
	/////////////////////////////////////////////////////////////////
	//NPC서버 패킷
	NPC_NpcInfo_VSn		= 61,  //NPC서버에서 NPC정보를 게임 서버에 알림
    NPC_UpdateNpc_VSn		= 62,  //NPC서버에서 NPC가 업데이트 되었다는 것을 게임 서버에 알림
	NPC_AttackNpcToPlayer_Sn = 63,  //NPC가 플레이어를 공격한다는 것을 게임 서버에 알림

	NPC_LoginPlayer_Cn  = 164,   //플레이어가 로긴했다는 것을 NPC서버에 알림
	NPC_LogoutPlayer_Cn  = 165,   //플레이어가 로그아웃했다는 것을 NPC서버에 알림
	NPC_MovePlayer_Cn   = 166,   //플레이어가 움직였다는 것을 NPC서버에 알림
	NPC_DeadPlayer_Cn   = 167,	//플레이어가 죽었다는 것을 NPC서버에 알림

	KeepAlive_Cn		= 99,  //플레이어가 보내는 KeepAlive패킷


};

#pragma pack(1)
//LoginPlayer_Rq	= 1,			//로긴 요청
struct LoginPlayerRq
{
	unsigned int	s_nLength;
	unsigned short	s_sType;
};

//MyPlayerInfo_Aq= 101,			//로긴 요청을 한 플레이어 정보
struct MyPlayerInfoAq
{
	unsigned int	s_nLength;
	unsigned short	s_sType;
	DWORD			s_dwPKey;	//플레이어 고유 번호
	char			s_szId[ MAX_ID_LENGTH ];  //아이디
	char			s_szNickName[ MAX_NICKNAME_LENGTH ];  //별명
	char			s_szName[ MAX_NAME_LENGTH ];  //이름
	DWORD			s_dwPos;	//플레이어 위치
	BYTE			s_byLevel;	//레벨
	BYTE			s_byStr;	//공격력
	BYTE			s_byDur;	//방어력
	DWORD			s_dwHp;		//생명력
	DWORD			s_dwExp;	//경험치
};
	
//WorldPlayerInfo_VAq = 102,		//현재 서버에 있는 플레이어들 정보
struct WorldPlayerInfoVAq
{
	unsigned int	s_nLength;
	unsigned short	s_sType;
	unsigned short	s_sPlayerCnt;	//현재 서버에 접속되어있는 플레이어 명수
	//가변 내용
	DWORD			s_dwPKey;	//플레이어 고유 번호
	char			s_szId[ MAX_ID_LENGTH ];  //아이디
	char			s_szNickName[ MAX_NICKNAME_LENGTH ];  //별명
	char			s_szName[ MAX_NAME_LENGTH ];  //이름
	DWORD			s_dwPos;	//플레이어 위치
	BYTE			s_byLevel;	//레벨
	BYTE			s_byStr;	//공격력
	BYTE			s_byDur;	//방어력
	DWORD			s_dwHp;		//생명력
	DWORD			s_dwExp;	//경험치
};

//LoginPlayer_Sn		= 151,  //로그인한 사용자 정보를 다른 사용자들에게 알린다.
struct LoginPlayerSn
{
	unsigned int	s_nLength;
	unsigned short	s_sType;

	DWORD			s_dwPKey;	//플레이어 고유 번호
	char			s_szId[ MAX_ID_LENGTH ];  //아이디
	char			s_szNickName[ MAX_NICKNAME_LENGTH ];  //별명
	char			s_szName[ MAX_NAME_LENGTH ];  //이름
	DWORD			s_dwPos;	//플레이어 위치
	BYTE			s_byLevel;	//레벨
	BYTE			s_byStr;	//공격력
	BYTE			s_byDur;	//방어력
	DWORD			s_dwHp;		//생명력
	DWORD			s_dwExp;	//경험치
};
//LogoutPlayer_Sn		= 152,	//로그 아웃을 했다는것을 다른 사용자에게 알린다.
struct LogoutPlayerSn
{
	unsigned int	s_nLength;
	unsigned short	s_sType;
	DWORD			s_dwPKey;	//플레이어 고유 번호
};
//MovePlayer_Cn		= 53,   //플레이어 이동을 서버에 알림
struct MovePlayerCn
{
	unsigned int	s_nLength;
	unsigned short	s_sType;
	DWORD			s_dwCPos;	//현재 위치
	DWORD			s_dwTPos;	//이동할 위치
};
//MovePlayer_Sn		= 153,  //플레이어 이동을 다른 플레이어에게 알림
struct MovePlayerSn
{
	unsigned int	s_nLength;
	unsigned short	s_sType;
	DWORD			s_dwPKey;	//이동한 플레이어 고유 키
	DWORD			s_dwCPos;	//현재 위치
	DWORD			s_dwTPos;	//이동할 위치
};	
//NPC_NpcInfo_VSn		= 61,  //NPC서버에서 NPC정보를 게임 서버에 알림
struct NPCNpcInfoVSn
{
	unsigned int	s_nLength;
	unsigned short	s_sType;
	unsigned short  s_sNpcCnt;	//NPC개수
	//가변 내용
	DWORD			s_dwNpcKey; //NPC키
	DWORD			s_dwNpcPos; //NPC위치
	DWORD			s_dwNpcType; //NPC종류
	char*			s_szNpcName; //NPC이름
};
//NPC_UpdateNpc_VSn		= 62,  //NPC서버에서 NPC가 업데이트 되었다는 것을 게임 서버에 알림
struct NPCUpdateNpcVSn
{
	unsigned int	s_nLength;
	unsigned short	s_sType;
	unsigned short  s_sNpcCnt;	//NPC개수
	//가변 내용
	DWORD			s_dwNpcKey; //NPC키
	DWORD			s_dwNpcPos; //NPC위치
	DWORD			s_dwNpcStatus; //NPC상태
};
//NPC_AttackNpcToPlayer_Sn = 63,  //NPC상태가 변화했다는것을 게임 서버에 알림
struct NPCAttackNpcToPlayerSn
{
	unsigned int	s_nLength;
	unsigned short	s_sType;
	DWORD			s_dwNpcKey; //공격하는 NPC키
	DWORD			s_dwPKey;   //공격당하는 플레이어 키
};

//NPC_LoginPlayer_Cn  = 164,   //플레이어가 로긴했다는 것을 NPC서버에 알림
struct NPCLoginPlayerCn
{
	unsigned int	s_nLength;
	unsigned short	s_sType;
	DWORD			s_dwPKey;
	DWORD			s_dwPos;
};
//NPC_LogoutPlayer_Cn  = 165,   //플레이어가 로긴했다는 것을 NPC서버에 알림
struct NPCLogoutPlayerCn
{
	unsigned int	s_nLength;
	unsigned short	s_sType;
	DWORD			s_dwPKey;
};
//NPC_MovePlayer_Cn   = 166,   //플레이어가 움직였다는 것을 NPC서버에 알림
struct NPCMovePlayerCn
{
	unsigned int	s_nLength;
	unsigned short	s_sType;
	DWORD			s_dwPKey;
	DWORD			s_dwPos;
};
//NPC_DeadPlayer_Cn   = 167,	//플레이어가 죽었다는 것을 NPC서버에 알림
struct NPCDeadPlayerCn
{
	unsigned int	s_nLength;
	unsigned short	s_sType;
	DWORD			s_dwPKey;
};
//KeepAlive_Cn		= 99,  //플레이어가 보내는 KeepAlive패킷
struct KeepAliveCn
{
	unsigned int	s_nLength;
	unsigned short	s_sType;
};

//NPC_NPCINFO_Sn		= 161,  //NPC서버에서 NPC정보를 게임 서버에 보낸다.

#pragma pack()
