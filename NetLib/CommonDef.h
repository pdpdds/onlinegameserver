#pragma once

#define MAX_RECVBUFCNT				100				//iocp recv packet pool 에서 버퍼갯수,,
#define MAX_SENDBUFCNT				100				//iocp send packet pool 에서 버퍼갯수,,
#define MAX_QUEUESIZE				10000           //default queue size in cQueue class
#define MAX_QUEUESENDSIZE			1000
#define MAX_PBUFSIZE				4096			//PacketPool에서 버퍼 한개당 size
#define MAX_VPBUFSIZE				1024 * 40		//가변 버퍼 size
#define MAX_RINGBUFSIZE				1024 * 100		//Ringbuffer size
#define DEFUALT_BUFCOUNT			10				//가변 버퍼 갯수.
#define PACKET_SIZE_LENGTH			4
#define PACKET_TYPE_LENGTH			2
#define MAX_IP_LENGTH				20
#define MAX_PROCESS_THREAD			1
#define MAX_WORKER_THREAD			10


#ifdef NETLIB_EXPORTS
#define NETLIB_API __declspec(dllexport)
#else
#define NETLIB_API __declspec(dllimport)
#endif


enum eOperationType
{ 
	//Work IOCP operation
	OP_SEND , 
	OP_RECV , 
	OP_ACCEPT ,
	//Process IOCP operation
	OP_CLOSE,		//접속 종료처리
	OP_RECVPACKET,	//순서 성 패킷 처리
	OP_SYSTEM		//시스템 메시지 처리
};

/////////////////////////////////////////////////
//Connection설정을 위한 config구조체
typedef struct _INITCONFIG
{
	int				nIndex;				//Connection index
	SOCKET			sockListener;		//Listen Socket
	//recv Ringbuffer size  = nRecvBufCnt * nRecvBufSize
	int				nRecvBufCnt;		
	//send Ringbuffer size  = nSendBufCnt * nSendBufSize
	int				nSendBufCnt;		
	int				nRecvBufSize;		
	int				nSendBufSize;		
	int				nProcessPacketCnt;	//최대 처리 패킷의 개수
	int				nServerPort;		
	int				nWorkerThreadCnt;	//i/o 처리를 위한 thread 개수
	int				nProcessThreadCnt;	//패킷처리를 위한 thread 개수

	_INITCONFIG()
	{
		ZeroMemory( this , sizeof( INITCONFIG ) );
	}
}INITCONFIG;


typedef struct _OVERLAPPED_EX		
{
	WSAOVERLAPPED			s_Overlapped;
	WSABUF					s_WsaBuf;
	int						s_nTotalBytes;	
	DWORD					s_dwRemain;
	char*					s_lpSocketMsg;	// Send/Recv Buffer.
	eOperationType			s_eOperation;
	void*					s_lpConnection;
	_OVERLAPPED_EX( void* lpConnection )
	{
		ZeroMemory( this , sizeof( OVERLAPPED_EX ) );
		s_lpConnection = lpConnection;
	}
} OVERLAPPED_EX, *LPOVERLAPPED_EX;


typedef struct _PROCESSPACKET
{
	eOperationType	s_eOperationType;
	WPARAM			s_wParam;
	LPARAM			s_lParam;
	_PROCESSPACKET()
	{
		Init();
	}
	void Init()
	{
		ZeroMemory( this, sizeof( PROCESSPACKET ) );
	}

} PROCESSPACKET, *LPPROCESSPACKET;

