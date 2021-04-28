#include "StdAfx.h"
#include ".\ciocpgameserver.h"
#include "cProcessPacket.h"
#define BASE_SERVER "BASE_SERVER"
#define CONNECT_NPCSERVER "CONNECT_NPCSERVER"
#define INIFILE_NAME ".\\GameServer.ini"
IMPLEMENT_SINGLETON( cIocpGameServer );

cIocpGameServer::cIocpGameServer(void)
{
	if( NULL == m_pIocpServer )
		m_pIocpServer = this;
	m_pTickThread = new cTickThread;
	m_dwPrivateKey = 0;
	m_pNpcServerConn = NULL;
	InitProcessFunc();
	
}

cIocpGameServer::~cIocpGameServer(void)
{
	if( m_pTickThread )
	{
		m_pTickThread->DestroyThread();
		delete m_pTickThread;
	}
	if( m_pNpcServerConn )
		delete m_pNpcServerConn;

}
void cIocpGameServer::InitProcessFunc()
{
	//클라이언트에서 받은 패킷
	m_FuncProcess[ LoginPlayer_Rq ].funcProcessPacket = cProcessPacket::fnLoginPlayerRq;
	m_FuncProcess[ MovePlayer_Cn ].funcProcessPacket = cProcessPacket::fnMovePlayerCn;
	m_FuncProcess[ KeepAlive_Cn ].funcProcessPacket = cProcessPacket::fnKeepAliveCn;

	//NPC서버에서 받은 패킷
	m_FuncProcess[ NPC_NpcInfo_VSn ].funcProcessPacket = cProcessPacket::fnNPCNpcInfoVSn;
	m_FuncProcess[ NPC_UpdateNpc_VSn ].funcProcessPacket = cProcessPacket::fnNPCUpdateNpcVSn;
	m_FuncProcess[ NPC_AttackNpcToPlayer_Sn ].funcProcessPacket = cProcessPacket::fnNPCAttackNpcToPlayerSn;
			
}
//client가 접속 수락이 되었을 때 호출되는 함수
bool cIocpGameServer::OnAccept( cConnection *lpConnection )
{
	cPlayer* pPlayer = (cPlayer*)lpConnection;
	
	LOG( LOG_INFO_LOW , 
		"SYSTEM | cIocpGameServer::OnAccept() | IP[%s] Socket[%d] 접속 PlayerCnt[%d]",
		lpConnection->GetConnectionIp(), lpConnection->GetSocket() ,
		PlayerManager()->GetPlayerCnt() );

	return true;
}

//client에서 packet이 도착했을 때 순서 성 있게 처리되어지는 패킷처리
bool cIocpGameServer::OnRecv(cConnection* lpConnection,  DWORD dwSize , char* pRecvedMsg)
{
	unsigned short usType;
	CopyMemory( &usType , pRecvedMsg + 4 , PACKET_TYPE_LENGTH );
	cPlayer* pPlayer = (cPlayer*)lpConnection;
	
	if( usType < 0 || usType > MAX_PROCESSFUNC || NULL == m_FuncProcess[ usType ].funcProcessPacket )
	{
		LOG( LOG_ERROR_NORMAL , "SYSTEM | cIocpGameServer::OnRecv() | 정의되지 않은 패킷(%d)",
			usType );
		return true;
	}
	m_FuncProcess[ usType ].funcProcessPacket( pPlayer , dwSize , pRecvedMsg );
	return true;
}

//client에서 packet이 도착했을 때 순서 성 없이 곧바로 처리 되는 패킷처리
bool cIocpGameServer::OnRecvImmediately(cConnection* lpConnection,  DWORD dwSize , char* pRecvedMsg)
{
	/////////////////////////////////////////////////////////////////
	//패킷이 처리되면 return true; 처리 되지 않았다면 return false;
	return false;
}

//client와 연결이 종료되었을 때 호출되는 함수
void cIocpGameServer::OnClose(cConnection* lpConnection)
{
	cPlayer* pPlayer = (cPlayer*)lpConnection;
	//인증된 사용자가 아니라면 
	if( pPlayer->GetIsConfirm() == false )
		return;
	LOG( LOG_INFO_LOW , 
		"SYSTEM | cIocpGameServer::OnClose() | IP[%s] Socket[%d] PKey[%d] Id[%s] 종료  PlayerCnt[%d]",
		pPlayer->GetConnectionIp(), pPlayer->GetSocket() ,
		pPlayer->GetPKey(), pPlayer->GetId(),
		PlayerManager()->GetPlayerCnt() );
	//플레이어를 관리 목록에서 삭제한다.
	PlayerManager()->RemovePlayer( pPlayer );
	//플레이어가 로그아웃하였다는 것을 월드의 다른 플레이어에게 알린다.
	PlayerManager()->Send_LogoutPlayer( pPlayer );
	//NPC서버에 플레이어가 로그아웃했다는 것을 알림
	if( NULL == m_pNpcServerConn )
		return;
	NPCLogoutPlayerCn* pLogoutPlayer = 
		(NPCLogoutPlayerCn*)m_pNpcServerConn->PrepareSendPacket( sizeof( NPCLogoutPlayerCn ) );
	if( NULL == pLogoutPlayer )
		return;
	pLogoutPlayer->s_sType = NPC_LogoutPlayer_Cn;
	pLogoutPlayer->s_dwPKey = pPlayer->GetPKey();
	m_pNpcServerConn->SendPost( sizeof( NPCLogoutPlayerCn ) );
}

bool cIocpGameServer::OnSystemMsg( cConnection* lpConnection , DWORD dwMsgType , LPARAM lParam )
{
	cPlayer* pPlayer = (cPlayer*)lpConnection;
	if( NULL == pPlayer )
		return true;
	eGameServerSystemMsg typeSystemMsg = (eGameServerSystemMsg)dwMsgType;
	switch( typeSystemMsg )
	{
	case SYSTEM_TEMPMSG:
		{
			//시스템 메시지 처리 부분
		}
		break;
	case SYSTEM_UPDATE_TEMPPLAYERPOS:
		{
			PlayerManager()->UpdateTempPlayerPos();
		}
		break;
	}

	return true;
}

//ini화일에서 스트링을 읽어오는 함수
int cIocpGameServer::GetINIString( char* szOutStr , char* szAppName , char* szKey ,int nSize , char* szFileName )
{
	int ret = GetPrivateProfileString( szAppName  , szKey  , "" , szOutStr , nSize , szFileName);
	if( 0 == ret )
	{
		char szTemp[ 300 ];
		sprintf( szTemp , "[%s]Config File에 [%s]-[%s]항목은 존재하지 않습니다.", szFileName , szAppName ,szKey );
		AfxMessageBox( szTemp );
		return -1;
	}
	return 0;
}

//ini화일에서 숫자를 읽어오는 함수
int	cIocpGameServer::GetINIInt( char* szAppName , char* szKey , char* szFileName )
{
	int ret = GetPrivateProfileInt( szAppName , szKey  , -1 ,  szFileName );
	if( ret < 0 )
	{
		char szTemp[ 300 ];
		sprintf( szTemp , "[%s]Config File에 [%s]-[%s]항목은 존재하지 않습니다.",
			szFileName , szAppName ,szKey );
		AfxMessageBox( szTemp );
	}
	return ret;
}

bool cIocpGameServer::ServerStart()
{
	CTime time = CTime::GetCurrentTime();
	char szOutStr[ 1024];

	if( -1 == GetINIString( m_szLogFileName , BASE_SERVER  , "LOGFILE"  ,  100 , INIFILE_NAME ) )
		return false;

	//////////////////////////////////////////////////////////////////
	//로그 초기화
	sLogConfig LogConfig;
	strncpy( LogConfig.s_szLogFileName , m_szLogFileName , MAX_FILENAME_LENGTH );
	LogConfig.s_nLogInfoTypes[ STORAGE_OUTPUTWND ] = LOG_ALL;
	LogConfig.s_nLogInfoTypes[ STORAGE_WINDOW ] = LOG_ALL;
    LogConfig.s_nLogInfoTypes[ STORAGE_FILE ] = LOG_ERROR_ALL;
	LogConfig.s_hWnd = AfxGetMainWnd()->m_hWnd;
	INIT_LOG( LogConfig );

	//서버 정보 초기화
	INITCONFIG initConfig;
	CString		szLogName;	
	int nMaxConnectionCnt = 0;

	strcpy( m_szLogFileName, szLogName.GetString() );

	if( -1 == ( initConfig.nProcessPacketCnt = GetINIInt( BASE_SERVER  , "PROCESS_PACKET_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nSendBufCnt = GetINIInt( BASE_SERVER  , "SEND_BUFFER_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nRecvBufCnt = GetINIInt( BASE_SERVER  , "RECV_BUFFER_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nSendBufSize = GetINIInt( BASE_SERVER  , "SEND_BUFFER_SIZE"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nRecvBufSize = GetINIInt( BASE_SERVER  , "RECV_BUFFER_SIZE"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nServerPort = GetINIInt( BASE_SERVER  , "SERVER_PORT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( nMaxConnectionCnt = GetINIInt( BASE_SERVER  , "MAX_CONNECTION_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nWorkerThreadCnt = GetINIInt( BASE_SERVER  , "WORKER_THREAD"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nProcessThreadCnt = GetINIInt( BASE_SERVER  , "PROCESS_THREAD"  , INIFILE_NAME ) ) )
		return false;
	
	
	LOG( LOG_INFO_NORMAL , "SYSTEM |  로그 시스템 시작 | ====================================================================== ");
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ServerStart() | [Base_LogFileName] %s", m_szLogFileName );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ServerStart() | [Base_Queue] Process Pakcet 개수 : %d", initConfig.nProcessPacketCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ServerStart() | [Base_Buffer] Send Buffer 개수 : %d ", initConfig.nSendBufCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ServerStart() | [Base_Buffer] Recv Buffer 개수 : %d ", initConfig.nRecvBufCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ServerStart() | [Base_Buffer] Send Buffer 크기 : %d ", initConfig.nSendBufSize );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ServerStart() | [Base_Buffer] Recv Buffer 크기 : %d ", initConfig.nRecvBufSize );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ServerStart() | [Base_Buffer] Send 총 할당 버퍼: %d ", initConfig.nSendBufSize * initConfig.nSendBufCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ServerStart() | [Base_Buffer] Recv 총 할당 버퍼: %d ", initConfig.nRecvBufSize * initConfig.nRecvBufCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ServerStart() | [Base_Connection] 접속할수 있는 최대 수 : %d", nMaxConnectionCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ServerStart() | [Base_Connection] Server Binding Port : %d", initConfig.nServerPort );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ServerStart() | [Base_Thread] WorkerThread Cnt : %d", initConfig.nWorkerThreadCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ServerStart() | [Base_Thread] Process WorkerThread Cnt : %d", initConfig.nProcessThreadCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ServerStart() | GameServer Start.." );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ServerStart() | 서버 시작 시간 :  %d년%d월%d일%d시%d분%d초",
		time.GetYear(),time.GetMonth(),time.GetDay(), time.GetHour(),time.GetMinute(),time.GetSecond() );

	sprintf(szOutStr 
		,"[%d년%d월%d일%d시%d분%d초] GameServer Start..."
		,time.GetYear(),time.GetMonth(),time.GetDay(),
		time.GetHour(),time.GetMinute(),time.GetSecond() );

	AfxGetMainWnd()->SetWindowText( szOutStr );
	if( false == cIocpServer::ServerStart( initConfig ) )
		return false;
	if( false == PlayerManager()->CreatePlayer( initConfig , nMaxConnectionCnt ) )
		return false;
	
	//0.5 tick으로 thread 시작
	m_pTickThread->CreateThread( SERVER_TICK );
	m_pTickThread->Run();
	return true;
}

void cIocpGameServer::ProcessSystemMsg( cPlayer* pPlayer , DWORD dwMsgType , LPARAM lParam )
{
	LPPROCESSPACKET lpProcessPacket = 
		GetProcessPacket( OP_SYSTEM , (LPARAM)dwMsgType , lParam );
	if( NULL == lpProcessPacket )
		return;

	if( 0 == PostQueuedCompletionStatus( m_hProcessIOCP , 
		0 , (ULONG_PTR)pPlayer, (LPOVERLAPPED)lpProcessPacket ) )
	{
		ClearProcessPacket( lpProcessPacket );
		LOG( LOG_ERROR_NORMAL , 
			"SYSTEM | cIocpGameServer::ProcessSystemMsg() | PostQueuedCompletionStatus Failed : [%d], socket[%d]" 
			, GetLastError() , pPlayer->GetSocket() );
	}
}

bool cIocpGameServer::ConnectToNpcServer()
{
	//이미 디비 서버와 연결이 되어있다면 전에 연결을 끊고 다시 연결한다.
	if( NULL != m_pNpcServerConn )
	{
		CloseConnection( m_pNpcServerConn );
		m_pNpcServerConn = NULL;
	}
	m_pNpcServerConn = new cConnection;

	INITCONFIG initConfig;
	char		szIp[30];

	//접속할 NpcServer에 대한 정보를 얻어온다.
	if( -1 == ( initConfig.nSendBufCnt = GetINIInt( CONNECT_NPCSERVER  , "SEND_BUFFER_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nRecvBufCnt = GetINIInt( CONNECT_NPCSERVER  , "RECV_BUFFER_CNT"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nSendBufSize = GetINIInt( CONNECT_NPCSERVER  , "SEND_BUFFER_SIZE"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == ( initConfig.nRecvBufSize = GetINIInt( CONNECT_NPCSERVER  , "RECV_BUFFER_SIZE"  , INIFILE_NAME ) ) )
		return false;
	if( -1 == GetINIString( szIp , CONNECT_NPCSERVER  , "CONNECT_IP"  ,  100 , INIFILE_NAME ) )
		return false;
	if( -1 == ( initConfig.nServerPort = GetINIInt( CONNECT_NPCSERVER  , "CONNECT_PORT"  , INIFILE_NAME ) ) )
		return false;
	
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ConnectToNpcServer() | [NpcServerConn_Buffer] Send Buffer 갯수 : %d ", initConfig.nSendBufCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ConnectToNpcServer() | [NpcServerConn_Buffer] Recv Buffer 갯수 : %d ", initConfig.nRecvBufCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ConnectToNpcServer() | [NpcServerConn_Buffer] Send Buffer 크기 : %d ", initConfig.nSendBufSize );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ConnectToNpcServer() | [NpcServerConn_Buffer] Recv Buffer 크기 : %d ", initConfig.nRecvBufSize );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ConnectToNpcServer() | [NpcServerConn_Buffer] Send 총 할당 버퍼: %d ", initConfig.nSendBufSize * initConfig.nSendBufCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ConnectToNpcServer() | [NpcServerConn_Buffer] Recv 총 할당 버퍼: %d ", initConfig.nRecvBufSize * initConfig.nRecvBufCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ConnectToNpcServer() | [NpcServerConn_Connection] Connect Ip : %s", szIp );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ConnectToNpcServer() | [NpcServerConn_Connection] Connect Port : %d", initConfig.nServerPort );

	m_pNpcServerConn->SetConnectionIp( szIp );
	m_pNpcServerConn->CreateConnection( initConfig );
	if( m_pNpcServerConn->ConnectTo( szIp , initConfig.nServerPort ) == false )
	{
		LOG_LASTERROR( "cIocpGameServer::ConnectToNpcServer() | NpcServer Connect Failed");		
		return false;
	}
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpGameServer::ConnectToNpcServer() | NpcServer [%d]socket 연결 성공" 
		, m_pNpcServerConn->GetSocket() );

	return true;

}
