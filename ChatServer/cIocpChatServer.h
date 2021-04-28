#pragma once

class cIocpChatServer : public cIocpServer , cSingleton
{
	DECLEAR_SINGLETON( cIocpChatServer );
public:
	cIocpChatServer(void);
	~cIocpChatServer(void);

	//client가 접속 수락이 되었을 때 호출되는 함수
	virtual	bool	OnAccept( cConnection *lpConnection );
	//client에서 packet이 도착했을 때 순서 성 있게 처리되어지는 패킷처리
	virtual	bool	OnRecv(cConnection* lpConnection,  DWORD dwSize , char* pRecvedMsg);
	//client에서 packet이 도착했을 때 순서 성 없이 곧바로 처리 되는 패킷처리
	virtual	bool	OnRecvImmediately(cConnection* lpConnection,  DWORD dwSize , char* pRecvedMsg);
	//client와 연결이 종료되었을 때 호출되는 함수
	virtual	void	OnClose(cConnection* lpConnection);
	
	virtual bool	OnSystemMsg( cConnection* lpConnection , DWORD dwMsgType , LPARAM lParam );


};
CREATE_FUNCTION( cIocpChatServer , IocpChatServer )