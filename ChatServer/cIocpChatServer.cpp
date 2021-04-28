#include "StdAfx.h"
#include ".\ciocpchatserver.h"

IMPLEMENT_SINGLETON( cIocpChatServer )

cIocpChatServer::cIocpChatServer(void)
{
	if( NULL == m_pIocpServer )
		m_pIocpServer = this;
}

cIocpChatServer::~cIocpChatServer(void)
{
}

//client가 접속 수락이 되었을 때 호출되는 함수
bool cIocpChatServer::OnAccept( cConnection *lpConnection )
{
	ConnectionManager()->AddConnection( lpConnection );
	LOG( LOG_INFO_LOW , 
		"SYSTEM | cIocpChatServer::OnAccept() | IP[%s] Socket[%d] 접속 UserCnt[%d]",
		lpConnection->GetConnectionIp(), lpConnection->GetSocket() ,
		ConnectionManager()->GetConnectionCnt() );

	return true;
}

//client에서 packet이 도착했을 때 순서 성 있게 처리되어지는 패킷처리
bool cIocpChatServer::OnRecv(cConnection* lpConnection,  DWORD dwSize , char* pRecvedMsg)
{
	unsigned short usType;
	CopyMemory( &usType , pRecvedMsg + 4 , PACKET_TYPE_LENGTH );
	switch( usType )
	{
	case PACKET_CHAT:
		{
			Packet_Chat* pChat = (Packet_Chat*)pRecvedMsg;
			ConnectionManager()->BroadCast_Chat( pChat->s_szIP , pChat->s_szChatMsg );
			LOG( LOG_INFO_LOW , "[메세지] IP : %s , Msg : %s" , pChat->s_szIP , pChat->s_szChatMsg );
		}
		break;
	default:
		LOG( LOG_ERROR_NORMAL , "SYSTEM | cIocpChatServer::OnRecv() | 정의되지 않은 패킷(%d)",
			usType );
		break;
	}


	return true;
}

//client에서 packet이 도착했을 때 순서 성 없이 곧바로 처리 되는 패킷처리
bool cIocpChatServer::OnRecvImmediately(cConnection* lpConnection,  DWORD dwSize , char* pRecvedMsg)
{
	/////////////////////////////////////////////////////////////////
	//패킷이 처리되면 return true; 처리 되지 않았다면 return false;
	return false;
}

//client와 연결이 종료되었을 때 호출되는 함수
void cIocpChatServer::OnClose(cConnection* lpConnection)
{
	ConnectionManager()->RemoveConnection( lpConnection );
	LOG( LOG_INFO_LOW , 
		"SYSTEM | cIocpChatServer::OnClose() | IP[%s] Socket[%d] 종료 UserCnt[%d]",
		lpConnection->GetConnectionIp(), lpConnection->GetSocket() ,
		ConnectionManager()->GetConnectionCnt() );

}
bool cIocpChatServer::OnSystemMsg( cConnection* lpConnection , DWORD dwMsgType , LPARAM lParam )
{
	return true;
}