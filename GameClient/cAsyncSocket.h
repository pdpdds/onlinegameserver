
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
class CGameClientDlg;
class cAsyncSocket : public cSingleton
{
	DECLEAR_SINGLETON( cAsyncSocket );
public:
	cAsyncSocket(void);
	~cAsyncSocket(void);
	

	//------서버 클라이언트 공통함수-------//
	//소켓을 초기화하는 함수
	bool InitSocket( HWND hWnd );
	//소켓의 연결을 종료 시킨다.
	void CloseSocket( bool bIsForce = false );
	SOCKET GetSocket() { return m_socket; };
	
	//------클라이언트용 함수------//
	bool ConnectTo( char* szIP , int nPort );

	//------공통 함수------//
	int SendMsg( char* pMsg , int nLen );

	
	void SetMainDlg( CGameClientDlg* pMainDlg ) { m_pMainDlg = pMainDlg; }

	
private:
	
	//클라이언트의 접속요청을 기다리는 소켓
	SOCKET		m_socket;
	//네트워크 이벤트가 발생하였을때 메세지를 보낼 윈도우 핸들
	HWND		m_hWnd;
	//메인 윈도우 포인터
	CGameClientDlg* m_pMainDlg;
	//소켓 버퍼
	char		m_szSocketBuf[ 1024 ];
};
CREATE_FUNCTION( cAsyncSocket , AsyncSocket );
