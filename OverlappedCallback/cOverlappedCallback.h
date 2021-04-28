#pragma once
class COverlappedCallbackDlg;
#define MAX_SOCKBUF 1024

//Overlapped I/O작업 동작 종류
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
	void*		m_pOverlappedCallback;	//콜백 함수 포인터
};
	
class cOverlappedCallback
{
public:
	cOverlappedCallback(void);
	~cOverlappedCallback(void);
	//------서버 클라이언트 공통함수-------//
	//소켓을 초기화하는 함수
	bool InitSocket();
	//소켓의 연결을 종료 시킨다.
	void CloseSocket( SOCKET socketClose , bool bIsForce = false );
	
	//------서버용 함수-------//
	//서버의 주소정보를 소켓과 연결시키고 접속 요청을 받기 위해 그 소켓을 등록하는 함수
	bool BindandListen( int nBindPort );
	//접속 요청을 수락하고 메세지를 받아서 처리하는 함수
	bool StartServer();
	//accept요청을 처리하는 쓰레드 생성
	bool CreateAccepterThread();
	//WSARecv Overlapped I/O 작업을 시킨다.
	bool BindRecv( SOCKET socket );
	//WSASend Overlapped I/O작업을 시킨다.
	bool SendMsg( SOCKET socket , char* pMsg , int nLen );
	//사용자의 접속을 받는 쓰레드
	void AccepterThread();
	
	void SetMainDlg( COverlappedCallbackDlg* pMainDlg ) { m_pMainDlg = pMainDlg; }
	COverlappedCallbackDlg* GetMainDlg() { return m_pMainDlg; }
	
	//생성되어있는 쓰레드를 파괴한다.
	void DestroyThread();
	
private:

	//접속 되어있는 클라이언트 수
	int			m_nClientCnt;
	//메인 윈도우 포인터
	COverlappedCallbackDlg* m_pMainDlg;
	//접속 쓰레드 핸들
	HANDLE		m_hAccepterThread;
	//접속 쓰레드 동작 플래그
	bool		m_bAccepterRun;
	SOCKET		m_sockListen;
	//소켓 버퍼
	char		m_szSocketBuf[ 1024 ];
};
