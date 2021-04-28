#pragma once
class COverlappedEventDlg;
#define MAX_SOCKBUF 1024
//Overlapped I/O작업의 종류
enum enumOperation{
	OP_RECV,
	OP_SEND
};
//WSAOVERLAPPED구조체를 확장 시켜서 필요한 정보를 더 넣었다.
struct stOverlappedEx{
	WSAOVERLAPPED m_wsaOverlapped;      
	int m_nIdx;							//stClientInfo 구조체 배열 인덱스
	WSABUF m_wsaBuf;
	char m_szBuf[ MAX_SOCKBUF ];
	enumOperation m_eOperation;
};
	
//클라이언트 정보를 담기위한 구조체지만 0번째 배열에는 더미로 접속이 들어오면 새로운 접속에 대한 
//이벤트감지를 위해 WSAWaitForMultipleEvents를 다시 걸어준다.
struct stClientInfo{
	//Cliet와 연결되는 소켓
	SOCKET		m_socketClient[ WSA_MAXIMUM_WAIT_EVENTS ];
	//이벤트 감지를 위한 이벤트 객체
	WSAEVENT    m_eventHandle[ WSA_MAXIMUM_WAIT_EVENTS ];
	stOverlappedEx	m_stOverlappedEx[ WSA_MAXIMUM_WAIT_EVENTS ];
};

class cOverlappedEvent
{
public:
	cOverlappedEvent(void);
	~cOverlappedEvent(void);
	
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

	//Overlapped I/O작업을 위한 쓰레드를 생성
	bool CreateWokerThread();
	//accept요청을 처리하는 쓰레드 생성
	bool CreateAccepterThread();

	//사용되지 않은 index반환
	int GetEmptyIndex();

	//WSARecv Overlapped I/O 작업을 시킨다.
	bool BindRecv( int nIdx );

	//WSASend Overlapped I/O작업을 시킨다.
	bool SendMsg( int nIdx , char* pMsg , int nLen );

	//Overlapped I/O작업에 대한 완료 통보를 받아 
	//그에 해당하는 처리를 하는 함수
	void WokerThread();
	//사용자의 접속을 받는 쓰레드
	void AccepterThread();

	//Overlapped I/O 완료에 대한 결과 처리
	void OverlappedResult( int nIdx );

	void SetMainDlg( COverlappedEventDlg* pMainDlg ) { m_pMainDlg = pMainDlg; }

	//생성되어있는 쓰레드를 파괴한다.
	void DestroyThread();

private:

	//클라이언트 정보 저장 구조체
	stClientInfo m_stClientInfo;

	//접속 되어있는 클라이언트 수
	int			m_nClientCnt;
	//메인 윈도우 포인터
	COverlappedEventDlg* m_pMainDlg;

	//작업 쓰레드 핸들
	HANDLE		m_hWorkerThread;
	//접속 쓰레드 핸들
	HANDLE		m_hAccepterThread;
	//작업 쓰레드 동작 플래그
	bool		m_bWorkerRun;
	//접속 쓰레드 동작 플래그
	bool		m_bAccepterRun;
	
	//소켓 버퍼
	char		m_szSocketBuf[ 1024 ];
};
