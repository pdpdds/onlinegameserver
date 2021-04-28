#pragma once
#define 	MAX_STORAGE_TYPE		6
#define		MAX_FILENAME_LENGTH		100
#define		MAX_DSN_NAME			100
#define		MAX_DSN_ID				20
#define		MAX_DSN_PW				20
#define		MAX_STRING_LENGTH		1024	
#define		MAX_OUTPUT_LENGTH		1024 * 4
#define		MAX_QUEUE_CNT			10000
#define		WM_DEBUGMSG				WM_USER + 1
#define		DEFAULT_UDPPORT			1555
#define		DEFAULT_TCPPORT			1556
#define		DEFAULT_TICK			1000
#define		MAX_LOGFILE_SIZE		1024 * 200000   //200MB	

static char szLogInfoType_StringTable[][ 100 ] = 
{
		"LOG_NONE",
		"LOG_INFO_LOW",													//0x00000001
		"LOG_INFO_NORMAL",												//0x00000002
		"LOG_INFO_LOW , LOG_INFO_NORMAL",								//0x00000003	
		"LOG_INFO_HIGH",												//0x00000004	
		"LOG_INFO_LOW , LOG_INFO_HIGH",									//0x00000005
		"LOG_INFO_NORMAL , LOG_INFO_HIGH",								//0x00000006
		"LOG_INFO_LOW , LOG_INFO_NORMAL , LOG_INFO_HIGH",				//0x00000007
		"LOG_INFO_CRITICAL",											//0x00000008
		"LOG_INFO_LOW , LOG_INFO_CRITICAL",								//0x00000009
		"LOG_INFO_NORMAL , LOG_INFO_CRITICAL",							//0x0000000A
		"LOG_INFO_LOW , LOG_INFO_NORMAL , LOG_INFO_CRITICAL",			//0x0000000B
		"LOG_INFO_HIGH , LOG_INFO_CRITICAL",							//0x0000000C
		"LOG_INFO_LOW , LOG_INFO_HIGH , LOG_INFO_CRITICAL",				//0x0000000D
		"LOG_INFO_NORMAL , LOG_INFO_HIGH , LOG_INFO_CRITICAL",			//0x0000000E
		"LOG_INFO_ALL",													//0x0000000F
	
		"LOG_ERROR_LOW",												//0x00000010
		"LOG_ERROR_NORMAL",												//0x00000020
		"LOG_ERROR_LOW , LOG_ERROR_NORMAL",								//0x00000030	
		"LOG_ERROR_HIGH",												//0x00000040	
		"LOG_ERROR_LOW , LOG_ERROR_HIGH",								//0x00000050
		"LOG_ERROR_NORMAL , LOG_ERROR_HIGH",							//0x00000060
		"LOG_ERROR_LOW , LOG_ERROR_NORMAL , LOG_ERROR_HIGH",			//0x00000070
		"LOG_ERROR_CRITICAL",											//0x00000080
		"LOG_ERROR_LOW , LOG_ERROR_CRITICAL",							//0x00000090
		"LOG_ERROR_NORMAL , LOG_ERROR_CRITICAL",						//0x000000A0
		"LOG_ERROR_LOW , LOG_ERROR_NORMAL , LOG_ERROR_CRITICAL",		//0x000000B0
		"LOG_ERROR_HIGH , LOG_ERROR_CRITICAL",							//0x000000C0
		"LOG_ERROR_LOW , LOG_ERROR_HIGH , LOG_ERROR_CRITICAL",			//0x000000D0
		"LOG_ERROR_NORMAL , LOG_ERROR_HIGH , LOG_ERROR_CRITICAL",		//0x000000F0
		"LOG_ERROR_ALL",												//0x00000100
		"LOG_ALL"														//0x00000200	
};

enum enumLogInfoType
{
	  LOG_NONE				= 0x00000000
	, LOG_INFO_LOW 			= 0x00000001 
	, LOG_INFO_NORMAL 		= 0x00000002
	, LOG_INFO_HIGH 		= 0x00000004
	, LOG_INFO_CRITICAL 	= 0x00000008
	, LOG_INFO_ALL			= 0x0000000F
	, LOG_ERROR_LOW 		= 0x00000010
	, LOG_ERROR_NORMAL		= 0x00000020
	, LOG_ERROR_HIGH 		= 0x00000040
	, LOG_ERROR_CRITICAL	= 0x00000080
	, LOG_ERROR_ALL			= 0x00000100
	, LOG_ALL				= 0x000001FF
};
     
enum enumLogStorageType 
{
	  STORAGE_FILE		= 0x000000000
	, STORAGE_DB 		= 0x000000001
	, STORAGE_WINDOW 	= 0x000000002
 	, STORAGE_OUTPUTWND	= 0x000000003
	, STORAGE_UDP		= 0x000000004
	, STORAGE_TCP		= 0x000000005
	
};
 
enum enumLogFileType 	
{
	  FILETYPE_NONE		= 0x00
	, FILETYPE_XML		= 0x01
	, FILETYPE_TEXT		= 0x02
	, FILETYPE_ALL		= 0x03
};

//메시지 구조체
struct sLogMsg
{
	enumLogInfoType		s_eLogInfoType;
	char				s_szOutputString[ MAX_OUTPUT_LENGTH ];
};

//- 초기화 구조체 정의
struct sLogConfig
{ 
	////////////////////////////////////////////////////////////////////////////
	//배열순서(파일[0],디비[1],윈도우[2],디버그창[3],udp[4])
	//각배열에 출력하고싶은 LogInfo레벨을 or연산하여 넣는다.
	//예)파일에 LOG_INFO_NORMAL, 윈도우에 LOG_ALL
	//s_eLogInfoType[ STORAGE_FILE ] = LOG_INFO_NORMAL
	//s_eLogInfoType[ STORAGE_FILE ] = LOG_ALL
	int 				s_nLogInfoTypes[ MAX_STORAGE_TYPE ];      
	char				s_szLogFileName[ MAX_FILENAME_LENGTH ];
	//로그 파일의 형식을 지정한다. XML or TEXT 둘다도 가능하다
	enumLogFileType		s_eLogFileType;
	//TCP/UDP로 로그를 남길 IP , PORT , UDP로그를 사용하지 않는다면 값이 없
	//어도 된다.
	char				s_szIP[	MAX_IP_LENGTH ];
	int					s_nUDPPort;
	int					s_nTCPPort;
	//서버 타입 , 로그서버에 등록될 서버타입을 결정
	int					s_nServerType;	

	//DB로 로그를 남길 DSN정보
	char				s_szDSNNAME[ MAX_DSN_NAME ];
	char				s_szDSNID[ MAX_DSN_ID ];
	char				s_szDSNPW[ MAX_DSN_PW ];
	//Window로 로그를 남길 윈도우 핸들
	HWND				s_hWnd;
	//Log 처리 시간 기본으로 1초마다 처리
	DWORD				s_dwProcessTick;
	//Log파일 사이즈가 m_dwFileMaxSize보다 크면 새로운 파일을 만든다. 
	DWORD				s_dwFileMaxSize;
	
	sLogConfig() { 
		ZeroMemory( this , sizeof( sLogConfig ) ); 
		s_dwProcessTick = DEFAULT_TICK;
		s_nUDPPort = DEFAULT_UDPPORT;
		s_nTCPPort = DEFAULT_TCPPORT;
		s_dwFileMaxSize = 1024 * 50000;		//50MB 기본으로 설정 최대 100MB가 까지 가능
	}

};

class NETLIB_API cLog : public cThread , public cSingleton
{
	DECLEAR_SINGLETON( cLog );
public:

	cLog(void);
	~cLog(void);

	//////////////////////////////////////////////////////////////////////////////////////
	//인터페이스 함수
	bool Init( sLogConfig &LogConfig );
	void LogOutput( enumLogInfoType eLogInfo , char *OutputString );
	void LogOutputLastErrorToMsgBox( char *szOutputString );
	//모든 로그를 끝낸다.  
	void CloseAllLog();

	//쓰레드 처리 함수
	virtual void	OnProcess(); 
	void SetHWND( HWND hWnd = NULL ) { m_hWnd = hWnd; }

	/////////////////////////////////////////////////////////////////////////
	//큐에 관련된 함수
	//현재 queue 크기
	size_t	GetQueueSize() { return m_queueLogMsg.GetQueueSize(); }
	//큐에 메세지를 추가한다. 
	inline void InsertMsgToQueue( sLogMsg* pLogMsg ) 
	{
		m_queueLogMsg.PushQueue( pLogMsg ); 
	}
		
private:
	////////////////////////////////////////////////////////////////////////////
	//배열순서(파일[0],디비[1],윈도우[2],디버그창[3],udp[4])
	//각배열에 출력하고싶은 LogInfo레벨을 or연산하여 넣는다.
	//예)파일에 LOG_INFO_NORMAL, 윈도우에 LOG_ALL
	//s_eLogInfoType[ STORAGE_FILE ] = LOG_INFO_NORMAL
	//s_eLogInfoType[ STORAGE_FILE ] = LOG_ALL
	int				 	m_nLogInfoTypes[ MAX_STORAGE_TYPE ];      
	char				m_szLogFileName[ MAX_FILENAME_LENGTH ];
	//로그 파일의 형식을 지정한다. XML or TEXT 둘다도 가능하다
	enumLogFileType		m_eLogFileType;
	///TCP/UDP로 로그를 남길 IP , PORT , UDP로그를 사용하지 않는다면 값이 없
	//어도 된다.
	char				m_szIP[	MAX_IP_LENGTH ];
	int					m_nUDPPort;
	int					m_nTCPPort;
	//서버 타입 , 로그서버에 등록될 서버타입을 결정
	int					m_nServerType;	
	
	//DB로 로그를 남길 DSN정보
	char				m_szDSNNAME[ MAX_DSN_NAME ];
	char				m_szDSNID[ MAX_DSN_ID ];
	char				m_szDSNPW[ MAX_DSN_PW ];
	//로그 저장 변수
	char				m_szOutStr[ MAX_OUTPUT_LENGTH ];
	
	////////////////////////////////////////////////////////////////////////////
	//output 객체의 핸들 변수
	//Window로 로그를 남길 윈도우 핸들
	HWND				m_hWnd;
	//File handle변수
	HANDLE				m_hLogFile;
	//UDP 소켓
	SOCKET				m_sockUdp;
	SOCKET				m_sockTcp;

	//메세지 큐
	cQueue< sLogMsg* >		m_queueLogMsg;
	//현재 메세지 버퍼 위치
	int					m_nMsgBufferIdx;
	DWORD				m_dwFileMaxSize;
	
	/////////////////////////////////////////////////////////////////////////////
	//내부 호출 함수
	//출력관련..함수	
	void OutputFile( char* szOutputString );
	void OutputDB( char* szOutputString );
	void OutputWindow(  enumLogInfoType eLogInfo , char* szOutputString );
	void OutputDebugWnd( char* szOutputString );
	void OutputUDP( enumLogInfoType eLogInfo , char* szOutputString );
	void OutputTCP( enumLogInfoType eLogInfo , char* szOutputString );
	
	
	//초기화 함수들
	bool InitDB();
	bool InitFile();
	bool InitUDP();
	bool InitTCP();
};
CREATE_FUNCTION( cLog , Log )

//글로벌 변수
static char g_szOutStr[ MAX_OUTPUT_LENGTH ];
static sLogMsg g_stLogMsg[ MAX_QUEUE_CNT ];
static cMonitor g_csLog;
//////////////////////////////////////////////////////////////////////////////////////////
//실제 사용자가 쓰는 함수 
//초기화 함수
bool NETLIB_API INIT_LOG( sLogConfig &LogConfig );
//로그를 남기는 함수 
//로고 메세지 쓰는 방법 : ( 에러나 정보가 발생한 함수 | 에러나 정보에관한내용 )
//EX) cIocpServer::BindIocp() | bind() , Failed ..
void NETLIB_API LOG( enumLogInfoType eLogInfoType , char *szOutputString , ... );
//라스트에러를 메세지 박스로 찍는 함수
void NETLIB_API LOG_LASTERROR( char *szOutputString , ... );
//로그를 끝낸다.
void NETLIB_API CLOSE_LOG();
