// CriticalSection.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
/////////////////////////////////////////////////////////////////////////////////////
//
//  이 예제는 크리티컬 섹션 객체를 사용하여 동기화한 예제이다.
//
#include "stdafx.h"
#include <windows.h>
#include <process.h>
using namespace std;

int g_nInteger = 0;
CRITICAL_SECTION csInteger;
	
unsigned int __stdcall ThreadFunc( void* lpVoid )
{

	while( true )
	{
		//csInteger객체 소유권 획득
		EnterCriticalSection( &csInteger );
		g_nInteger++;
		cout<<"ThreadID : "<<GetCurrentThreadId()<<" : "<<g_nInteger<<endl;
		if( g_nInteger == 9 || g_nInteger == 10 )
			break;
		//csInteger객체 소유권 해제
		LeaveCriticalSection( &csInteger );
	}
	cout<<"Thread end.. ThreadID : "<<GetCurrentThreadId()<<endl;
	LeaveCriticalSection( &csInteger );
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{

	unsigned int uiThreadID = 0;
    //크리티컬 섹션 객체 초기화
	InitializeCriticalSection( &csInteger );

	//쓰레드 생성
	HANDLE hThread1 = (HANDLE)_beginthreadex( NULL , 0 , ThreadFunc , NULL , CREATE_SUSPENDED  , &uiThreadID );
	cout<<"Create Thread ID : "<<uiThreadID<<endl;
	HANDLE hThread2 = (HANDLE)_beginthreadex( NULL , 0 , ThreadFunc , NULL , CREATE_SUSPENDED  , &uiThreadID );
	cout<<"Create Thread ID : "<<uiThreadID<<endl;
	if( NULL == hThread1 || NULL == hThread2 )
	{
		cout<<"[ERROR] Thread1 or Thread2 is NULL.."<<endl;
		return 0;
	}
	ResumeThread( hThread1 );
	ResumeThread( hThread2 );
	//쓰레드가 종료될 때까지 기다린다.
	WaitForSingleObject( hThread1 , INFINITE );
	WaitForSingleObject( hThread2 , INFINITE );

	//쓰레드 핸들을 닫는다.
	CloseHandle( hThread1 );
	CloseHandle( hThread2 );
	return 0;
}


