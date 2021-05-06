#include "StdAfx.h"
#include ".\ciocompletionport.h"
#include "IOCompletionPortDlg.h"

//WSARecv�� WSASend�� Overlapped I/O �۾� ó���� ���� ������
unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	cIOCompletionPort* pOverlappedEvent = (cIOCompletionPort*)p;
	pOverlappedEvent->WokerThread();
	return 0;
}

//Client�� ������ �ޱ����� ������
unsigned int WINAPI CallAccepterThread(LPVOID p)
{
	cIOCompletionPort* pOverlappedEvent = (cIOCompletionPort*)p;
	pOverlappedEvent->AccepterThread();
	return 0;
}

cIOCompletionPort::cIOCompletionPort(void)
{
	////////////////////////////////////////////////////
	//��� ��� ������ �ʱ�ȭ
	m_pMainDlg = NULL;
	m_bWorkerRun = true;
	m_bAccepterRun = true;
	m_nClientCnt = 0;
	m_hAccepterThread = NULL;
	m_hIOCP = NULL;
	m_socketListen = INVALID_SOCKET;
	ZeroMemory( m_szSocketBuf , 1024 );
	for( int i = 0; i < MAX_WORKERTHREAD; i++ )
		m_hWorkerThread[ i ] = NULL;

	//client������ ������ ����ü�� ����
	m_pClientInfo = new stClientInfo[ MAX_CLIENT ];
 }

cIOCompletionPort::~cIOCompletionPort(void)
{
	//������ ����� ������.
	WSACleanup();
	//�� ����� ��ü�� ����
	if( m_pClientInfo )
	{
		delete [] m_pClientInfo;
		m_pClientInfo = NULL;
	}
}

//������ �ʱ�ȭ�ϴ� �Լ�
bool cIOCompletionPort::InitSocket()
{
	WSADATA wsaData;
	//������ ���� 2,2�� �ʱ�ȭ �Ѵ�.
	int nRet = WSAStartup( MAKEWORD(2,2) , &wsaData );
	if( 0 != nRet )
	{
		m_pMainDlg->OutputMsg("[����] WSAStartup()�Լ� ���� : %d", WSAGetLastError() );
		return false;
	}

	//���������� TCP , Overlapped I/O ������ ����
	m_socketListen = WSASocket( AF_INET , SOCK_STREAM 
		, IPPROTO_TCP , NULL , NULL , WSA_FLAG_OVERLAPPED );

	if( INVALID_SOCKET == m_socketListen )
	{
		m_pMainDlg->OutputMsg("[����] socket()�Լ� ���� : %d", WSAGetLastError() );
		return false;
	}
		
	m_pMainDlg->OutputMsg("���� �ʱ�ȭ ����");
	return true;
}

//�ش� ������ ���� ��Ų��.
void cIOCompletionPort::CloseSocket( stClientInfo* pClientInfo 
									, bool bIsForce )
{
	struct linger stLinger = {0, 0};	// SO_DONTLINGER�� ����

	// bIsForce�� true�̸� SO_LINGER, timeout = 0���� �����Ͽ�
	// ���� ���� ��Ų��. ���� : ������ �ս��� ������ ���� 
	if( true == bIsForce )
		stLinger.l_onoff = 1; 

	//socketClose������ ������ �ۼ����� ��� �ߴ� ��Ų��.
	shutdown( pClientInfo->m_socketClient, SD_BOTH );
	//���� �ɼ��� �����Ѵ�.
	setsockopt( pClientInfo->m_socketClient, SOL_SOCKET,
		SO_LINGER, (char *)&stLinger, sizeof(stLinger) );
	//���� ������ ���� ��Ų��. 
	closesocket( pClientInfo->m_socketClient );
	
	pClientInfo->m_socketClient = INVALID_SOCKET;
}

//������ �ּ������� ���ϰ� �����Ű�� ���� ��û�� �ޱ� ���� ������ ����ϴ� �Լ�
bool cIOCompletionPort::BindandListen( int nBindPort )
{
	SOCKADDR_IN		stServerAddr;
	stServerAddr.sin_family = AF_INET;
    //���� ��Ʈ�� �����Ѵ�.
	stServerAddr.sin_port = htons( nBindPort );
	//� �ּҿ��� ������ �����̶� �޾Ƶ��̰ڴ�.
	//���� ������� �̷��� �����Ѵ�. ���� �� �����ǿ����� ������ �ް� �ʹٸ�
	//�� �ּҸ� inet_addr�Լ��� �̿��� ������ �ȴ�.
	stServerAddr.sin_addr.s_addr = htonl( INADDR_ANY );

	//������ ������ ���� �ּ� ������ cIOCompletionPort ������ �����Ѵ�.
	int nRet = bind( m_socketListen, (SOCKADDR*) &stServerAddr 
		, sizeof( SOCKADDR_IN ) );
	if( 0 != nRet )
	{
		m_pMainDlg->OutputMsg("[����] bind()�Լ� ���� : %d", WSAGetLastError() );
		return false;
	}
	
	//���� ��û�� �޾Ƶ��̱� ���� cIOCompletionPort������ ����ϰ� 
	//���Ӵ��ť�� 5���� ���� �Ѵ�.
	nRet = listen( m_socketListen , 5 );
	if( 0 != nRet )
	{
		m_pMainDlg->OutputMsg("[����] listen()�Լ� ���� : %d", WSAGetLastError() );
		return false;
	}
 
	m_pMainDlg->OutputMsg("���� ��� ����..");

	return true;
}

bool cIOCompletionPort::CreateWokerThread()
{
	unsigned int uiThreadId = 0;
	//WaingThread Queue�� ��� ���·� ���� ������� ����
	//����Ǵ� ���� : (cpu���� * 2) + 1 
	for( int i = 0; i < MAX_WORKERTHREAD; i++ )
	{
		m_hWorkerThread[ i ] = (HANDLE)_beginthreadex(NULL, 0, 
			&CallWorkerThread, this
			,CREATE_SUSPENDED, &uiThreadId);
		if(m_hWorkerThread == NULL)
		{
			m_pMainDlg->OutputMsg("WorkerThread Creation Failed: %u", GetLastError());	
			return false;
		}
		ResumeThread( m_hWorkerThread[ i ] );
	}
	m_pMainDlg->OutputMsg("WokerThread ����..");
    return true;
}

bool cIOCompletionPort::CreateAccepterThread()
{
	unsigned int uiThreadId = 0;
	//Client�� ���ӿ�û�� ���� ������ ����
	m_hAccepterThread = (HANDLE)_beginthreadex(NULL, 0,
		                      &CallAccepterThread, this,
							  CREATE_SUSPENDED, &uiThreadId);
	if(m_hAccepterThread == NULL)
	{
		m_pMainDlg->OutputMsg("AccepterThread Creation Failed: %u", GetLastError());	
		return false;
	}

	ResumeThread( m_hAccepterThread );
	m_pMainDlg->OutputMsg("AccepterThread ����..");
	return true;
}

bool cIOCompletionPort::BindIOCompletionPort( stClientInfo* pClientInfo )
{
	HANDLE hIOCP;
	//socket�� pClientInfo�� CompletionPort��ü�� �����Ų��.
	hIOCP = CreateIoCompletionPort((HANDLE)pClientInfo->m_socketClient
		, m_hIOCP
		, reinterpret_cast<ULONG_PTR>( pClientInfo ) , 0);
	if( NULL == hIOCP  || m_hIOCP != hIOCP )
	{
		m_pMainDlg->OutputMsg("[����] CreateIoCompletionPort()�Լ� ����",GetLastError() );
		return false;
	}
	return true;

}

bool cIOCompletionPort::StartServer()
{
	//���ӵ� Ŭ���̾�Ʈ �ּ� ������ ������ ����ü
	bool bRet = CreateWokerThread();
	if( false == bRet )
		return false;
	bRet = CreateAccepterThread();
	if( false == bRet )
		return false;

	//CompletionPort��ü ���� ��û�� �Ѵ�.
	m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE 
		, NULL , NULL , 0 );
	if( NULL == m_hIOCP )
	{
		m_pMainDlg->OutputMsg("[����] CreateIoCompletionPort()�Լ� ����",GetLastError() );
		return false;
	}
	
	m_pMainDlg->OutputMsg("���� ����");

	return true;
}

bool cIOCompletionPort::BindRecv( stClientInfo* pClientInfo )
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	
	//Overlapped I/O�� ���� �� ������ ������ �ش�.
	pClientInfo->m_stRecvOverlappedEx.m_wsaBuf.len = MAX_SOCKBUF;
	pClientInfo->m_stRecvOverlappedEx.m_wsaBuf.buf =
		pClientInfo->m_stRecvOverlappedEx.m_szBuf;
	pClientInfo->m_stRecvOverlappedEx.m_eOperation = OP_RECV;

	int nRet = WSARecv(	pClientInfo->m_socketClient, 	
					&(pClientInfo->m_stRecvOverlappedEx.m_wsaBuf),
					1, 
					&dwRecvNumBytes, 
					&dwFlag, 
					(LPWSAOVERLAPPED)&(pClientInfo->m_stRecvOverlappedEx), 
					NULL);

	//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
	if(nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		m_pMainDlg->OutputMsg("[����] WSARecv()�Լ� ���� : %d", WSAGetLastError() );
		return false;
	}
	return true;
}

bool cIOCompletionPort::SendMsg( stClientInfo* pClientInfo 
								, char* pMsg 
								, int nLen )
{
	DWORD dwRecvNumBytes = 0;

	//���۵� �޼����� ����
	CopyMemory( pClientInfo->m_stSendOverlappedEx.m_szBuf, pMsg , nLen );

	
	//Overlapped I/O�� ���� �� ������ ������ �ش�.
	pClientInfo->m_stSendOverlappedEx.m_wsaBuf.len = nLen;
	pClientInfo->m_stSendOverlappedEx.m_wsaBuf.buf =
		pClientInfo->m_stSendOverlappedEx.m_szBuf;
	pClientInfo->m_stSendOverlappedEx.m_eOperation = OP_SEND;
	
	int nRet = WSASend(	pClientInfo->m_socketClient , 	
					&(pClientInfo->m_stSendOverlappedEx.m_wsaBuf),
					1, 
					&dwRecvNumBytes, 
					0, 
					(LPWSAOVERLAPPED)&(pClientInfo->m_stSendOverlappedEx), 
					NULL);

	//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
	if(nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		m_pMainDlg->OutputMsg("[����] WSASend()�Լ� ���� : %d", WSAGetLastError() );
		return false;
	}
	return true;

}

stClientInfo* cIOCompletionPort::GetEmptyClientInfo()
{
	for( int i = 0 ; i < MAX_CLIENT ; i++ )
	{
		if ( INVALID_SOCKET == m_pClientInfo[ i ].m_socketClient )
			return &m_pClientInfo[ i ];
	}
	return NULL;
}

//������� ������ �޴� ������
void cIOCompletionPort::AccepterThread()
{
	SOCKADDR_IN		stClientAddr;
	int nAddrLen = sizeof( SOCKADDR_IN );
	while( m_bAccepterRun )
	{
		//������ ���� ����ü�� �ε����� ���´�.
		stClientInfo* pClientInfo = GetEmptyClientInfo();
		if( NULL == pClientInfo )
		{
			m_pMainDlg->OutputMsg("[����] Client Full");
			return;
		}

		//Ŭ���̾�Ʈ ���� ��û�� ���� ������ ��ٸ���.
		pClientInfo->m_socketClient = accept( m_socketListen, 
			(SOCKADDR*) &stClientAddr , &nAddrLen );

		if( INVALID_SOCKET == pClientInfo->m_socketClient )
			continue;

		//I/O Completion Port��ü�� ������ �����Ų��.
		bool bRet = BindIOCompletionPort( pClientInfo );
		if( false == bRet )
			return;
		//Recv Overlapped I/O�۾��� ��û�� ���´�.
		bRet = BindRecv( pClientInfo );
		if( false == bRet )
			return;

		m_pMainDlg->OutputMsg( "Ŭ���̾�Ʈ ���� : IP(%s) SOCKET(%d)", 
			inet_ntoa( stClientAddr.sin_addr ),
            pClientInfo->m_socketClient );
		//Ŭ���̾�Ʈ ���� ����
		m_nClientCnt++;
	}
}
void cIOCompletionPort::WokerThread()
{
	//CompletionKey�� ���� ������ ����
	stClientInfo* pClientInfo = NULL;
	//�Լ� ȣ�� ���� ����
	BOOL bSuccess = TRUE;
	//Overlapped I/O�۾����� ���۵� ������ ũ��
	DWORD dwIoSize = 0;
	//I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������
	LPOVERLAPPED lpOverlapped = NULL;

	while( m_bWorkerRun )
	{
		//////////////////////////////////////////////////////
		//�� �Լ��� ���� ��������� WaitingThread Queue��
		//��� ���·� ���� �ȴ�.
		//�Ϸ�� Overlapped I/O�۾��� �߻��ϸ� IOCP Queue����
		//�Ϸ�� �۾��� ������ �� ó���� �Ѵ�.
		//�׸��� PostQueuedCompletionStatus()�Լ������� �����
		//�޼����� �����Ǹ� �����带 �����Ѵ�.
		//////////////////////////////////////////////////////
		bSuccess = GetQueuedCompletionStatus( m_hIOCP,
			&dwIoSize,					// ������ ���۵� ����Ʈ
			(PULONG_PTR)&pClientInfo,		// CompletionKey
			&lpOverlapped,				// Overlapped IO ��ü
			INFINITE );					// ����� �ð�
		
		//client�� ������ ��������..			
		if( FALSE == bSuccess && 0 == dwIoSize )
		{
			m_pMainDlg->OutputMsg("socket(%d) ���� ����",
				pClientInfo->m_socketClient );
			CloseSocket( pClientInfo );
			continue;
		}

		//����� ������ ���� �޼��� ó��..
		if( TRUE == bSuccess && 0 == dwIoSize &&
			NULL == lpOverlapped)
		{
			m_bWorkerRun = false;
			continue;
		}
		if( NULL == lpOverlapped )
			continue;

		stOverlappedEx* pOverlappedEx = 
			(stOverlappedEx*)lpOverlapped;
		
		//Overlapped I/O Recv�۾� ��� �� ó��
		if( OP_RECV == pOverlappedEx->m_eOperation )
		{
			pOverlappedEx->m_szBuf[ dwIoSize ] = NULL;
			m_pMainDlg->OutputMsg("[����] bytes : %d , msg : %s"
				, dwIoSize , pOverlappedEx->m_szBuf );
			
			//Ŭ���̾�Ʈ�� �޼����� �����Ѵ�.
			BindRecv( pClientInfo );
		}
		//Overlapped I/O Send�۾� ��� �� ó��
		else if( OP_SEND == pOverlappedEx->m_eOperation )
		{
			m_pMainDlg->OutputMsg("[�۽�] bytes : %d , msg : %s"
				, dwIoSize , pOverlappedEx->m_szBuf );
		}
		//���� ��Ȳ
		else
		{
			m_pMainDlg->OutputMsg("socket(%d)���� ���ܻ�Ȳ",
				pClientInfo->m_socketClient );
		}
   	}
}

void cIOCompletionPort::DestroyThread()
{
	for( int i = 0; i < MAX_WORKERTHREAD; i++ )
	{
		//WaitingThread Queue���� ��� ���� �����忡 
		//����� ���� �޼����� ������.
		PostQueuedCompletionStatus( m_hIOCP, 0, 0, NULL );
	}

	for( int i = 0; i < MAX_WORKERTHREAD; i++ )
	{
		//������ �ڵ��� �ݰ� �����尡 ����� ������ ��ٸ���.
		CloseHandle( m_hWorkerThread[ i ] );
		WaitForSingleObject( m_hWorkerThread[ i ], INFINITE );
	}

	m_bAccepterRun = false;
	//Accepter �����带 �����Ѵ�.
	closesocket( m_socketListen );
	//������ ���Ḧ ��ٸ���
	WaitForSingleObject( m_hAccepterThread, INFINITE );
}