// Semaphore.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
//////////////////////////////////////////////////////////////////////////////////////
//
//  이 예제는 뮤텍스를 이용하여 g_nInteger의 증가값을 출력하는 예제이다
//

#include "stdafx.h"
#include <windows.h>
#include <process.h>

using namespace std;
int g_nInteger = 0;
HANDLE g_hSemaphore;
unsigned int __stdcall ThreadFunc(void* lpVoid)
{
	LONG lPre = 0;
	while (true)
	{
		//g_hEvent객체가 신호 받음(Signaled)상태가 
		//될 때까지 대기 상태로 들어간다.
		WaitForSingleObject(g_hSemaphore, INFINITE);
		g_nInteger++;
		cout << "ThreadID : " << GetCurrentThreadId() << " : "
			<< g_nInteger << endl;
		if (g_nInteger >= 8)
			break;
		//g_hSemaphore객체의 소유권을 해제 함으로써 
		//객체의 상태를 신호 받음(Signaled)상태로 바꾸어 준다.

		ReleaseSemaphore(g_hSemaphore, 1, &lPre);
	}
	cout << "Thread end.. ThreadID : "
		<< GetCurrentThreadId() << endl;
	ReleaseSemaphore(g_hSemaphore, 1, &lPre);
	return 0;
}
int _tmain(int argc, _TCHAR* argv[])
{
	unsigned int uiThreadID = 0;
	g_hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
	if (NULL == g_hSemaphore)
	{
		cout << "CreateMutex() 함수 호출 실패 : " <<
			GetLastError() << endl;
	}

	//쓰레드 생성
	HANDLE hThread1 = (HANDLE)_beginthreadex(NULL, 0
		, ThreadFunc, NULL, CREATE_SUSPENDED, &uiThreadID);
	cout << "Create Thread ID : " << uiThreadID << endl;

	HANDLE hThread2 = (HANDLE)_beginthreadex(NULL, 0
		, ThreadFunc, NULL, CREATE_SUSPENDED, &uiThreadID);
	cout << "Create Thread ID : " << uiThreadID << endl;

	HANDLE hThread3 = (HANDLE)_beginthreadex(NULL, 0
		, ThreadFunc, NULL, CREATE_SUSPENDED, &uiThreadID);
	cout << "Create Thread ID : " << uiThreadID << endl;

	if (NULL == hThread1 || NULL == hThread2 || NULL == hThread3)
	{
		cout << "[ERROR] Thread1 or hThread2 is NULL.." << endl;
		return 0;
	}

	//쓰레드를 실행
	ResumeThread(hThread1);
	ResumeThread(hThread2);
	ResumeThread(hThread3);
	//g_hMutex객체의 소유권을 해제 함으로써 
	//객체의 상태를 신호 받음(Signaled)상태로 바꾸어 준다.
	LONG lPre = 0;
	ReleaseSemaphore(g_hSemaphore, 1, &lPre);

	//쓰레드가 종료될 때까지 기다린다.
	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);

	//g_hMutex객체의 핸들을 닫는다.
	CloseHandle(g_hSemaphore);
	//쓰레드 핸들을 닫는다.
	CloseHandle(hThread1);
	CloseHandle(hThread2);
	return 0;
}

/*
SOCKET socket = WSASocket(
	{
		///////////////////////////////////////////////////////////////////////////////
		//CompletionPort객체를 생성하는 예
		HANDLE hIOCPHandle;
//Completion Port객체를 생성한다.
hIOCPHandle = CreateIoCompletionPort(NULL, hIOCP, NULL , 0);
//87번 에러는 잘못된 인수 에러인데. 이것은 소켓 재사용을 하면 나온다.
if (NULL == hIOCPHandle)
{
	cout << "CreateIoCompletionPort()함수 실패" << endl;
	return false;
}

///////////////////////////////////////////////////////////////////////////////
//CompletionPort객체와 소켓을 연결하는 예
socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP
	, NULL, 0, WSA_FLAG_OVERLAPPED);
HANDLE hIOCP = CreateIoCompletionPort((HANDLE)socket, hIOCPHandle, 1 , 0);
//87번 에러는 잘못된 인수 에러인데. 이것은 소켓 재사용을 하면 나온다.
if (NULL == hIOCP)
{
	cout << "CreateIoCompletionPort()함수 실패 : "
		<< GetLastError() << endl;
	return false;
}


m_hIOCP = hIOCP;*/

