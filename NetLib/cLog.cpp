#include "Precompile.h"

IMPLEMENT_SINGLETON( cLog )

cLog::cLog(void)
{
	ZeroMemory( m_nLogInfoTypes , MAX_STORAGE_TYPE * sizeof( int ) );      
	ZeroMemory( m_szLogFileName ,  MAX_FILENAME_LENGTH );
	ZeroMemory( m_szIP ,  MAX_IP_LENGTH );
	ZeroMemory( m_szDSNNAME ,  MAX_DSN_NAME );
	ZeroMemory( m_szDSNID ,  MAX_DSN_ID );
	ZeroMemory( m_szDSNPW ,  MAX_DSN_PW );
    m_eLogFileType = FILETYPE_NONE;
	m_hWnd = NULL;
	m_hLogFile = NULL;
	m_sockUdp = INVALID_SOCKET;
	m_nMsgBufferIdx = 0;
	m_nUDPPort = DEFAULT_UDPPORT;
	m_nTCPPort = DEFAULT_TCPPORT;
	m_nServerType = 0;
	m_dwFileMaxSize = 0;
	
}

cLog::~cLog(void)
{
	
}

void cLog::OnProcess()
{
	size_t nLogCount = m_queueLogMsg.GetQueueSize();
	for( unsigned int i = 0; i < nLogCount ; i++ )
	{
		sLogMsg* pLogMsg = m_queueLogMsg.GetFrontQueue();
		//로그를 찍는다.
		LogOutput( pLogMsg->s_eLogInfoType , pLogMsg->s_szOutputString );
		m_queueLogMsg.PopQueue();
	}
}

bool cLog::Init( sLogConfig &LogConfig )
{
	//만약 초기화전에 연결이 되어있었다면 모든 연결을 CLOSE 시킨다.
	CloseAllLog();
	char        strtime[ 100 ];
	//각설정값 셋팅
	CopyMemory( m_nLogInfoTypes , 
		LogConfig.s_nLogInfoTypes ,
		MAX_STORAGE_TYPE * sizeof( int ) );      
	
    time_t      curtime;
    struct tm   *loctime;

    curtime = time( NULL);
    loctime = localtime( &curtime );
	strftime( strtime, 100, "%m월%d일%H시%M분", loctime );
	//LOG디렉토리생성
	CreateDirectory( "./LOG" , NULL );
	sprintf( m_szLogFileName ,
		"./Log/%s_%s.log" , LogConfig.s_szLogFileName , strtime );
	strncpy( m_szIP , LogConfig.s_szIP , MAX_IP_LENGTH );
	strncpy( m_szDSNNAME , LogConfig.s_szDSNNAME , MAX_DSN_NAME );
	strncpy( m_szDSNID , LogConfig.s_szDSNID , MAX_DSN_ID );
	strncpy( m_szDSNPW , LogConfig.s_szDSNPW , MAX_DSN_PW );
	m_eLogFileType = LogConfig.s_eLogFileType;
	m_nTCPPort = LogConfig.s_nTCPPort;
	m_nUDPPort = LogConfig.s_nUDPPort;
	m_nServerType = LogConfig.s_nServerType;
	m_dwFileMaxSize = LogConfig.s_dwFileMaxSize;

	m_hWnd = LogConfig.s_hWnd;
	bool bRet = false;
	
	//파일로그를 설정했다면
	if( LOG_NONE != m_nLogInfoTypes[ STORAGE_FILE ] )
		bRet = InitFile();
    if( false == bRet )
		goto error;
	//db로그를 설정했다면
	if( LOG_NONE != m_nLogInfoTypes[ STORAGE_DB ] )
		bRet = InitDB();
	if( false == bRet )
		goto error;
	//udp로그를 설정했다면 
	if( LOG_NONE != m_nLogInfoTypes[ STORAGE_UDP ] )
		bRet = InitUDP();
	if( false == bRet )
		goto error;

	m_nLogInfoTypes[ STORAGE_TCP ] = LOG_NONE;
	//tcp로그를 설정했다면
	if( LOG_NONE != m_nLogInfoTypes[ STORAGE_TCP ] )
		bRet = InitTCP();
	if( false == bRet )
		goto error;

	CreateThread( LogConfig.s_dwProcessTick );
	Run();
	return true;

//에러가 생겼다면..
error:
	CloseAllLog();
	return false;
}

void cLog::CloseAllLog()
{
	ZeroMemory( m_nLogInfoTypes , MAX_STORAGE_TYPE * sizeof( int ) );      
	ZeroMemory( m_szLogFileName ,  MAX_FILENAME_LENGTH );
	ZeroMemory( m_szIP ,  MAX_IP_LENGTH );
	ZeroMemory( m_szDSNNAME ,  MAX_DSN_NAME );
	ZeroMemory( m_szDSNID ,  MAX_DSN_ID );
	ZeroMemory( m_szDSNPW ,  MAX_DSN_PW );
	m_nUDPPort = DEFAULT_UDPPORT;
	m_nTCPPort = DEFAULT_TCPPORT;
	m_eLogFileType = FILETYPE_NONE;
	m_hWnd = NULL;
	m_nMsgBufferIdx = 0;
	
	//화일 로그를 끝낸다.
	if( m_hLogFile )
	{
		CloseHandle( m_hLogFile );
		m_hLogFile = NULL;
	}
	//udp소켓을 초기화 시킨다. 
	if( INVALID_SOCKET != m_sockUdp )
	{
		closesocket( m_sockUdp );
		m_sockUdp = INVALID_SOCKET;
	}
	//TCP소켓을 초기화 시킨다.
	if( INVALID_SOCKET != m_sockTcp )
	{
		shutdown( m_sockTcp, SD_BOTH );
		closesocket( m_sockTcp );
		m_sockTcp = INVALID_SOCKET;
	}
	//쓰레드 종료
	Stop();	
}

bool cLog::InitDB()
{
	/*-- 
	디비 연결
	--*/
	return true;

}

bool cLog::InitFile()
{
	m_hLogFile = CreateFile(	m_szLogFileName ,
								GENERIC_WRITE,                
								FILE_SHARE_READ ,            
								NULL,                         
								OPEN_ALWAYS,               
								FILE_ATTRIBUTE_NORMAL,     
								NULL);     

	if( NULL == m_hLogFile )
		return false;
	return true;
}

bool cLog::InitUDP()
{
	WSADATA		WsaData;
	int nRet = WSAStartup(MAKEWORD(2,2), &WsaData);
	if(nRet)
		return false;
	return true;

}

bool cLog::InitTCP()
{
	WSADATA		WsaData;
	int nRet = WSAStartup(MAKEWORD(2,2), &WsaData);
	if(nRet)
		return false;

	if( INVALID_SOCKET != m_sockTcp )
		return false;

	sockaddr_in Addr;
	memset( (char *)&Addr, 0x00, sizeof(Addr) );
	Addr.sin_family = AF_INET;
	Addr.sin_addr.s_addr = inet_addr( m_szIP ); 
	Addr.sin_port = htons( m_nTCPPort );
	//보낼 패킷길이.
	m_sockTcp = socket( AF_INET , SOCK_STREAM , IPPROTO_IP );
	

	nRet = connect( m_sockTcp , (sockaddr*)&Addr , sizeof( sockaddr ) );
	if( SOCKET_ERROR == nRet )
		return false;
	return true;

}

void cLog::LogOutput( enumLogInfoType eLogInfo ,
					 char *szOutputString )
{
	///////////////////////////////////////////////////////////////////////
	//통신 프로토콜로 다른 프로그램으로 로그를 보낼때는
	//받은 타입과 메세지를 보내야한다.
	if( m_nLogInfoTypes[ STORAGE_UDP ] & eLogInfo )
	{
		OutputUDP( eLogInfo , szOutputString );
	}
	if( m_nLogInfoTypes[ STORAGE_TCP ] & eLogInfo )
	{
		OutputTCP( eLogInfo , szOutputString );
	}

	//로그, 시간 : 정보형태 : 정보레벨 : 클래스 : 함수 : 에러원인
	//현재시간을 얻어온다.
	char        szTime[ 25 ];
	time_t      curTime;
    struct tm   *locTime;
	//LOG ENUM과 StringTable간의 정보를 매치 시킨다.
	int nIdx = ( int )eLogInfo;
	if( 0 != ( eLogInfo >> 8 ) )
		nIdx = ( eLogInfo >> 8 ) + 0x20 - 3;
	else if( 0 != ( eLogInfo >> 4 ) )
		nIdx = ( eLogInfo >> 4 ) + 0x10 - 1;
	if( nIdx < 0 || nIdx > 31 )
		return;
	curTime = time( NULL );
    locTime = localtime( &curTime );
    strftime( szTime, 25 , "%Y/%m/%d(%H:%M:%S)", locTime );
	sprintf( m_szOutStr, "%s | %s | %s | %s%c%c"
		, szTime , ( eLogInfo >> 4 ) ? "에러" : "정보" 
		, szLogInfoType_StringTable[ nIdx ] 
		, szOutputString  , 0x0d , 0x0a );

    	
    if( m_nLogInfoTypes[ STORAGE_FILE ] & eLogInfo )
	{
		OutputFile( m_szOutStr );
	}
	if( m_nLogInfoTypes[ STORAGE_DB ] & eLogInfo )
	{
		OutputDB( m_szOutStr );
	}
	if( m_nLogInfoTypes[ STORAGE_WINDOW ] & eLogInfo )
	{
		OutputWindow( eLogInfo , m_szOutStr );
	}
	if( m_nLogInfoTypes[ STORAGE_OUTPUTWND ] & eLogInfo )
	{
		OutputDebugWnd( m_szOutStr );
	}
}

void cLog::OutputFile( char* szOutputString )
{
	if( NULL == m_hLogFile )
		return;

	DWORD dwWrittenBytes = 0;
	DWORD dwSize = 0;
	dwSize = GetFileSize( m_hLogFile , NULL );
	//화일 용량이 제한에 걸렸다면.
	if( dwSize > m_dwFileMaxSize || dwSize > MAX_LOGFILE_SIZE )
	{
		char        strtime[ 100 ];
		time_t      curtime;
		struct tm   *loctime;

		curtime = time( NULL);
		loctime = localtime( &curtime );
		strftime( strtime, 100, "%m월%d일%H시%M분", loctime );
		m_szLogFileName[ strlen( m_szLogFileName ) - 21 ] = NULL;
		sprintf( m_szLogFileName , "%s_%s.log" , m_szLogFileName , strtime );
		CloseHandle( m_hLogFile );
		m_hLogFile = NULL;
		InitFile();
	}
	//화일에 끝으로 파일 포인터를 옮긴다. 
	SetFilePointer( m_hLogFile , 0 , 0 , FILE_END );
	BOOL bRet = WriteFile( m_hLogFile , szOutputString ,
		(DWORD)strlen( szOutputString ) , &dwWrittenBytes , NULL );
}

void cLog::OutputDB( char* szOutputString )
{
	//-- DB에 로그를 남기는 코드
}

void cLog::OutputWindow( enumLogInfoType eLogInfo 
						, char* szOutputString )
{
	if( NULL == m_hWnd )
		return;
	SendMessage( m_hWnd , WM_DEBUGMSG , ( WPARAM )szOutputString 
		, ( LPARAM )eLogInfo );
}

void cLog::OutputDebugWnd( char* szOutputString )
{
	//디버거창에 출력해준다.
	OutputDebugString( szOutputString );
}

void cLog::OutputUDP( enumLogInfoType eLogInfo , char* szOutputString )
{
	//udp packet을 보낼곳 정보
	sockaddr_in Addr;
	memset( (char *)&Addr, 0x00, sizeof(Addr) );
	Addr.sin_family = AF_INET;
	Addr.sin_addr.s_addr = inet_addr( m_szIP ); 
	Addr.sin_port = htons( m_nUDPPort );
	//보낼 패킷길이.
	int nBufLen = (int)strlen( szOutputString );

	if( INVALID_SOCKET == m_sockUdp )
	{
		m_sockUdp = socket( AF_INET , SOCK_DGRAM , IPPROTO_IP );
	}
	
	int iResult = sendto( m_sockUdp , (char*)szOutputString, nBufLen,
		0 , (const struct sockaddr *)&Addr, sizeof(Addr));
	
}

void cLog::OutputTCP( enumLogInfoType eLogInfo , char* szOutputString )
{
	int nLen = (int)strlen( szOutputString );
	int iResult = send( m_sockTcp , (char*)szOutputString, nLen , 0 );

}

void cLog::LogOutputLastErrorToMsgBox( char *szOutputString )
{
	int nLastError = GetLastError();
	if( nLastError == 0 ) 
		return;
	LPVOID pDump;
	DWORD  result;
	result = FormatMessage(	
							FORMAT_MESSAGE_ALLOCATE_BUFFER |
							FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							nLastError,
							MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
							(LPTSTR)&pDump,
							0,
							NULL
							);

	sprintf( g_szOutStr , "에러위치 : %s \n에러번호 : %d\n설명 : %s",
		szOutputString , nLastError , (char*)pDump );
	MessageBox( NULL , g_szOutStr , "GetLastError" , MB_OK );
	
	if(result) 
		LocalFree(pDump);

}

//초기화 함수
bool INIT_LOG( sLogConfig &LogConfig )
{
	return Log()->Init( LogConfig );
}
//로그를 남기는 함수
void LOG( enumLogInfoType eLogInfoType , char *szOutputString , ... )
{
	cMonitor::Owner lock( g_csLog );

	size_t nQueueCnt = Log()->GetQueueSize();
	//현재 큐 Size를 초과하였다면 
	if( MAX_QUEUE_CNT == nQueueCnt )
		return;
	
	va_list	argptr; 
	va_start( argptr, szOutputString );
	vsprintf( g_stLogMsg[ nQueueCnt ].s_szOutputString ,
		szOutputString, argptr );
	va_end( argptr );

	g_stLogMsg[ nQueueCnt ].s_eLogInfoType = eLogInfoType;
	
	Log()->InsertMsgToQueue( &g_stLogMsg[ nQueueCnt ] );
}
//라스트에러를 메세지 박스로 찍는 함수
void LOG_LASTERROR( char *szOutputString , ... )
{
	va_list	argptr; 
    va_start( argptr, szOutputString );
	vsprintf( g_szOutStr , szOutputString, argptr );
	va_end( argptr );

	Log()->LogOutputLastErrorToMsgBox( g_szOutStr );
}
//로그를 끝낸다.
void CLOSE_LOG()
{
	Log()->CloseAllLog();
}
