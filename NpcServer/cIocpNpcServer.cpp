#include "StdAfx.h"
#include ".\ciocpnpcserver.h"
#include "cProcessPacket.h"

IMPLEMENT_SINGLETON( cIocpNpcServer );

cIocpNpcServer::cIocpNpcServer(void)
{
	if( NULL == m_pIocpServer )
		m_pIocpServer = this;
	m_pTickThread = new cTickThread;
	InitProcessFunc();
}

cIocpNpcServer::~cIocpNpcServer(void)
{
	if( m_pTickThread )
	{
		m_pTickThread->DestroyThread();
		delete m_pTickThread;
	}
}

void cIocpNpcServer::InitProcessFunc()
{
	m_FuncProcess[ NPC_LoginPlayer_Cn  % MAX_PROCESSFUNC ].funcProcessPacket = cProcessPacket::fnNPCLoginPlayerCn;
	m_FuncProcess[ NPC_LogoutPlayer_Cn  % MAX_PROCESSFUNC ].funcProcessPacket = cProcessPacket::fnNPCLogoutPlayerCn;
	m_FuncProcess[ NPC_MovePlayer_Cn  % MAX_PROCESSFUNC ].funcProcessPacket = cProcessPacket::fnNPCMovePlayerCn;
	m_FuncProcess[ NPC_DeadPlayer_Cn  % MAX_PROCESSFUNC ].funcProcessPacket = cProcessPacket::fnNPCDeadPlayerCn;
}

//client가 접속 수락이 되었을 때 호출되는 함수
bool cIocpNpcServer::OnAccept( cConnection *lpConnection )
{
	ConnectionManager()->AddConnection( lpConnection );
	LOG( LOG_INFO_LOW , 
		"SYSTEM | cIocpNpcServer::OnAccept() | IP[%s] Socket[%d] 접속 Connection Cnt[%d]",
		lpConnection->GetConnectionIp(), lpConnection->GetSocket() ,
		ConnectionManager()->GetConnectionCnt() );
	return true;
}

//client에서 packet이 도착했을 때 순서 성 있게 처리되어지는 패킷처리
bool cIocpNpcServer::OnRecv(cConnection* lpConnection,  DWORD dwSize , char* pRecvedMsg)
{
	unsigned short usType;
	CopyMemory( &usType , pRecvedMsg + 4 , PACKET_TYPE_LENGTH );
	unsigned short usTempType = usType % MAX_PROCESSFUNC;
	if( usTempType < 0 || usTempType > MAX_PROCESSFUNC || NULL == m_FuncProcess[ usTempType ].funcProcessPacket )
	{
		LOG( LOG_ERROR_NORMAL , "SYSTEM | cIocpNpcServer::OnRecv() | 정의되지 않은 패킷(%d)",
			usType );
		return true;
	}
	m_FuncProcess[ usTempType ].funcProcessPacket( lpConnection , dwSize , pRecvedMsg );
	return true;
}

//client에서 packet이 도착했을 때 순서 성 없이 곧바로 처리 되는 패킷처리
bool cIocpNpcServer::OnRecvImmediately(cConnection* lpConnection,  DWORD dwSize , char* pRecvedMsg)
{
	/////////////////////////////////////////////////////////////////
	//패킷이 처리되면 return true; 처리 되지 않았다면 return false;
	return false;
}

//client와 연결이 종료되었을 때 호출되는 함수
void cIocpNpcServer::OnClose(cConnection* lpConnection)
{
	//플레이어를 관리 목록에서 삭제한다.
	ConnectionManager()->RemoveConnection( lpConnection );
	LOG( LOG_INFO_LOW , 
		"SYSTEM | cIocpNpcServer::OnClose() | IP[%s] Socket[%d] 종료 Connection Cnt[%d]",
		lpConnection->GetConnectionIp(), lpConnection->GetSocket() ,
		ConnectionManager()->GetConnectionCnt());
}

bool cIocpNpcServer::OnSystemMsg( cConnection* lpConnection , DWORD dwMsgType , LPARAM lParam )
{
	eNpcServerSystemMsg typeSystemMsg = (eNpcServerSystemMsg)dwMsgType;
	switch( typeSystemMsg )
	{
	case SYSTEM_UPDATE_NPCPOS:
		{
			NpcManager()->GatherVBuffer_UpdateNpc();
			ConnectionManager()->Send_GatherVBuffer();
			//LOG( LOG_INFO_LOW , "SYSTEM | cIocpNpcServer::OnSystemMsg() | SYSTEM_UPDATE_NPCPOS..");
		}
		break;
	}

	return true;
}

//ini화일에서 스트링을 읽어오는 함수
int cIocpNpcServer::GetINIString( char* szOutStr , char* szAppName , char* szKey ,int nSize , char* szFileName )
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
int	cIocpNpcServer::GetINIInt( char* szAppName , char* szKey , char* szFileName )
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

bool cIocpNpcServer::ServerStart()
{
	CTime time = CTime::GetCurrentTime();
	char szOutStr[ 1024];

	if( -1 == GetINIString( m_szLogFileName , BASE_SERVER  , "LOGFILE"  ,  100 , INIFILE_NAME ) )
		return false;

	//서버 정보 초기화
	INITCONFIG initConfig;
	CString		szLogName;	
	int nMaxConnectionCnt = 0;

	//////////////////////////////////////////////////////////////////
	//로그 초기화
	sLogConfig LogConfig;
	strncpy( LogConfig.s_szLogFileName , m_szLogFileName , MAX_FILENAME_LENGTH );
	LogConfig.s_nLogInfoTypes[ STORAGE_OUTPUTWND ] = LOG_ALL;
	LogConfig.s_nLogInfoTypes[ STORAGE_WINDOW ] = LOG_ALL;
    LogConfig.s_nLogInfoTypes[ STORAGE_FILE ] = LOG_ERROR_ALL;
	LogConfig.s_hWnd = AfxGetMainWnd()->m_hWnd;
	INIT_LOG( LogConfig );

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
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpNpcServer::ServerStart() | [Base_LogFileName] %s", m_szLogFileName );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpNpcServer::ServerStart() | [Base_Queue] Process Pakcet 개수 : %d", initConfig.nProcessPacketCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpNpcServer::ServerStart() | [Base_Buffer] Send Buffer 개수 : %d ", initConfig.nSendBufCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpNpcServer::ServerStart() | [Base_Buffer] Recv Buffer 개수 : %d ", initConfig.nRecvBufCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpNpcServer::ServerStart() | [Base_Buffer] Send Buffer 크기 : %d ", initConfig.nSendBufSize );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpNpcServer::ServerStart() | [Base_Buffer] Recv Buffer 크기 : %d ", initConfig.nRecvBufSize );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpNpcServer::ServerStart() | [Base_Buffer] Send 총 할당 버퍼: %d ", initConfig.nSendBufSize * initConfig.nSendBufCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpNpcServer::ServerStart() | [Base_Buffer] Recv 총 할당 버퍼: %d ", initConfig.nRecvBufSize * initConfig.nRecvBufCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpNpcServer::ServerStart() | [Base_Connection] 접속할수 있는 최대 수 : %d", nMaxConnectionCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpNpcServer::ServerStart() | [Base_Connection] Server Binding Port : %d", initConfig.nServerPort );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpNpcServer::ServerStart() | [Base_Thread] WorkerThread Cnt : %d", initConfig.nWorkerThreadCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpNpcServer::ServerStart() | [Base_Thread] Process WorkerThread Cnt : %d", initConfig.nProcessThreadCnt );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpNpcServer::ServerStart() | NpcServer Start.." );
	LOG( LOG_INFO_NORMAL , "SYSTEM | cIocpNpcServer::ServerStart() | 서버 시작 시간 :  %d년%d월%d일%d시%d분%d초",
		time.GetYear(),time.GetMonth(),time.GetDay(), time.GetHour(),time.GetMinute(),time.GetSecond() );

	sprintf(szOutStr 
		,"[%d년%d월%d일%d시%d분%d초] NpcServer Start..."
		,time.GetYear(),time.GetMonth(),time.GetDay(),
		time.GetHour(),time.GetMinute(),time.GetSecond() );

	AfxGetMainWnd()->SetWindowText( szOutStr );
	if( false == cIocpServer::ServerStart( initConfig ) )
		return false;
	if( false == ConnectionManager()->CreateConnection( initConfig , nMaxConnectionCnt ) )
		return false;

	PlayerManager()->CreatePlayer( 100 );
	//0.5 tick으로 thread 시작
	m_pTickThread->CreateThread( SERVER_TICK );
	m_pTickThread->Run();
	return true;
}

void cIocpNpcServer::ProcessSystemMsg( cConnection* pConnection , DWORD dwMsgType , LPARAM lParam )
{
	LPPROCESSPACKET lpProcessPacket = 
		GetProcessPacket( OP_SYSTEM , (LPARAM)dwMsgType , lParam );
	if( NULL == lpProcessPacket )
		return;

	if( 0 == PostQueuedCompletionStatus( m_hProcessIOCP , 
		0 , (ULONG_PTR)pConnection, (LPOVERLAPPED)lpProcessPacket ) )
	{
		ClearProcessPacket( lpProcessPacket );
		LOG( LOG_ERROR_NORMAL , 
			"SYSTEM | cIocpNpcServer::ProcessSystemMsg() | PostQueuedCompletionStatus Failed : [%d], socket[%d]" 
			, GetLastError() , pConnection->GetSocket() );
	}
}