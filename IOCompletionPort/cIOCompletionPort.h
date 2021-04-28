#pragma once
class CIOCompletionPortDlg;
#define MAX_SOCKBUF 1024	//패킷 크기
#define MAX_CLIENT 100		//총 접속할수 있는 클라이언트 수
#define MAX_WORKERTHREAD 4  //쓰레드 풀에 넣을 쓰레드 수

enum enumOperation{
	OP_RECV,
	OP_SEND
};
//WSAOVERLAPPED구조체를 확장 시켜서 필요한 정보를 더 넣었다.
struct stOverlappedEx{

	WSAOVERLAPPED m_wsaOverlapped;		//Overlapped I/O구조체
	SOCKET		m_socketClient;			//클라이언트 소켓
	WSABUF		m_wsaBuf;				//Overlapped I/O작업 버퍼
	char		m_szBuf[ MAX_SOCKBUF ]; //데이터 버퍼
	enumOperation m_eOperation;			//작업 동작 종류
};	
	
//클라이언트 정보를 담기위한 구조체
struct stClientInfo{
	SOCKET			m_socketClient;			//Cliet와 연결되는 소켓
	stOverlappedEx	m_stRecvOverlappedEx;	//RECV Overlapped I/O작업을 위한 변수
	stOverlappedEx	m_stSendOverlappedEx;	//SEND Overlapped I/O작업을 위한 변수
	//생성자에서 멤버 변수들을 초기화
	stClientInfo()
	{
		ZeroMemory( &m_stRecvOverlappedEx , sizeof( stOverlappedEx ) );
		ZeroMemory( &m_stSendOverlappedEx , sizeof( stOverlappedEx ) );
		m_socketClient = INVALID_SOCKET;
	}
};

class cIOCompletionPort
{
public:
	cIOCompletionPort(void);
	~cIOCompletionPort(void);

	//------서버 클라이언트 공통함수-------//
	//소켓을 초기화하는 함수
	bool InitSocket();
	//소켓의 연결을 종료 시킨다.
	void CloseSocket( stClientInfo* pClientInfo , bool bIsForce = false );
	
	//------서버용 함수-------//
	//서버의 주소정보를 소켓과 연결시키고 접속 요청을 받기 위해 
	//소켓을 등록하는 함수
	bool BindandListen( int nBindPort );
	//접속 요청을 수락하고 메세지를 받아서 처리하는 함수
	bool StartServer();
	//WaitingThread Queue에서 대기할 쓰레드들을 생성
	bool CreateWokerThread();
	//accept요청을 처리하는 쓰레드 생성
	bool CreateAccepterThread();
	//사용하지 않는 클라이언트 정보 구조체를 반환한다.
	stClientInfo* GetEmptyClientInfo();

	//CompletionPort객체와 소켓과 CompletionKey를
	//연결시키는 역할을 한다.
	bool BindIOCompletionPort( stClientInfo* pClientInfo );
  	//WSARecv Overlapped I/O 작업을 시킨다.
	bool BindRecv( stClientInfo* pClientInfo );
	//WSASend Overlapped I/O작업을 시킨다.
	bool SendMsg( stClientInfo* pClientInfo , char* pMsg , int nLen );
	//Overlapped I/O작업에 대한 완료 통보를 받아 
	//그에 해당하는 처리를 하는 함수
	void WokerThread();
	//사용자의 접속을 받는 쓰레드
	void AccepterThread();
	
	//메인 윈도우의 포인터를 저장
	void SetMainDlg( CIOCompletionPortDlg* pMainDlg ) 
	{ m_pMainDlg = pMainDlg; }
	CIOCompletionPortDlg* GetMainDlg() { return m_pMainDlg; }
	
	//생성되어있는 쓰레드를 파괴한다.
	void DestroyThread();
	
private:
	//클라이언트 정보 저장 구조체
	stClientInfo* m_pClientInfo;
	//클라이언트의 접속을 받기위한 리슨 소켓
	SOCKET		m_socketListen;
	//접속 되어있는 클라이언트 수
	int			m_nClientCnt;
	//메인 윈도우 포인터
	CIOCompletionPortDlg* m_pMainDlg;

	//작업 쓰레드 핸들--WaitingThread Queue에 들어갈 쓰레드
	HANDLE		m_hWorkerThread[ MAX_WORKERTHREAD ];
	//접속 쓰레드 핸들
	HANDLE		m_hAccepterThread;
	//CompletionPort객체 핸들
	HANDLE		m_hIOCP;
	//작업 쓰레드 동작 플래그
	bool		m_bWorkerRun;
	//접속 쓰레드 동작 플래그
	bool		m_bAccepterRun;
	//소켓 버퍼
	char		m_szSocketBuf[ 1024 ];
};