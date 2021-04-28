
//////////////////////////////////////////////////////////////////////////////////////////
/*					파일		  : cAsyncSocket.h											*/
/*					프로젝트 종류 : Win32 Console										*/
/*					설명		  : 소켓 클래스     									*/
/*					작성일자	  : 2004.09.02											*/
/*					작성자		  : 강군												*/
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <winsock2.h>
#define WM_SOCKETMSG WM_USER+1
class CAsyncSelectDlg;
class cAsyncSocket
{
public:
	cAsyncSocket(void);
	~cAsyncSocket(void);
	

	//------서버 클라이언트 공통함수-------//
	//소켓을 초기화하는 함수
	bool InitSocket( HWND hWnd );
	//소켓의 연결을 종료 시킨다.
	void CloseSocket( SOCKET socketClose , bool bIsForce = false );
	
	//------서버용 함수-------//
	//서버의 주소정보를 소켓과 연결시키고 접속 요청을 받기 위해 그 소켓을 등록하는 함수
	bool BindandListen( int nBindPort );
	//접속 요청을 수락하고 메세지를 받아서 처리하는 함수
	bool StartServer();

	
	void SetMainDlg( CAsyncSelectDlg* pMainDlg ) { m_pMainDlg = pMainDlg; }
	
private:
	
	//클라이언트의 접속요청을 기다리는 소켓
	SOCKET		m_socketListen;

	//네트워크 이벤트가 발생하였을때 메세지를 보낼 윈도우 핸들
	HWND		m_hWnd;
	//메인 윈도우 포인터
	CAsyncSelectDlg* m_pMainDlg;

	//소켓 버퍼
	char		m_szSocketBuf[ 1024 ];

	
};
