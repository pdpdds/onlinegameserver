#pragma once
class CEventSelectDlg;
#define MAX_SOCKBUF 1024

	
//클라이언트 정보를 담기위한 구조체지만 0번째 배열에는 더미로 접속이 들어오면 새로운 접속에 대한 
//이벤트감지를 위해 WSAWaitForMultipleEvents를 다시 걸어준다.
struct stClientInfo{
	//이벤트 감지를 위한 이벤트 객체
	WSAEVENT    m_eventHandle[ WSA_MAXIMUM_WAIT_EVENTS ];
	//Cliet와 연결되는 소켓
	SOCKET		m_socketClient[ WSA_MAXIMUM_WAIT_EVENTS ];
};

class cEventSelect
{
public:
	cEventSelect(void);
	~cEventSelect(void);
	
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
	//사용되지 않은 index반환
	int GetEmptyIndex();
	
	//작업 쓰레드, 네트워크 이벤트를 감지하여 해당 작업처리
    void WokerThread();
	void DoAccept( DWORD dwObjIdx );
	void DoRecv( DWORD dwObjIdx );
	
	void SetMainDlg( CEventSelectDlg* pMainDlg ) { m_pMainDlg = pMainDlg; }

	//생성되어있는 쓰레드를 파괴한다.
	void DestroyThread();

private:
	//클라이언트 정보 저장 구조체
	stClientInfo m_stClientInfo;
	//접속 되어있는 클라이언트 수
	int			m_nClientCnt;
	//메인 윈도우 포인터
	CEventSelectDlg* m_pMainDlg;
	//작업 쓰레드 핸들
	HANDLE		m_hWorkerThread;
	//작업 쓰레드 동작 플래그
	bool		m_bWorkerRun;
	//소켓 버퍼
	char		m_szSocketBuf[ 1024 ];
};
