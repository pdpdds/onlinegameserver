// Event.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
//////////////////////////////////////////////////////////////////////////////////////
//
//  이 예제는 자동 리셋 이벤트를 이용하여 g_nInteger의 증가값을 출력하는 예제이다
//

#include "stdafx.h"
#include <windows.h>
#include <process.h>
using namespace std;
int g_nInteger = 0;
HANDLE g_hEvent;
unsigned int __stdcall ThreadFunc( void* lpVoid )
{

	while( true )
	{
		//g_hEvent객체가 신호 받음(Signaled)상태가 될 때까지 대기 상태로 들어간다.
		WaitForSingleObject( g_hEvent , INFINITE );
		g_nInteger++;
		cout<<"ThreadID : "<<GetCurrentThreadId()<<" : "<<g_nInteger<<endl;
		if( g_nInteger == 9 || g_nInteger == 10 )
			break;
		//g_hEvent객체의 상태를 신호 받음(Signaled)상태로 바꾸어 준다.
		SetEvent( g_hEvent );
	}
	cout<<"Thread end.. ThreadID : "<<GetCurrentThreadId()<<endl;
	SetEvent( g_hEvent );
	return 0;
}
int _tmain(int argc, _TCHAR* argv[])
{
	unsigned int uiThreadID = 0;
    g_hEvent = CreateEvent( NULL , FALSE , FALSE , NULL );
	if( NULL == g_hEvent )
	{
		cout<<"CreateEvent() 함수 호출 실패 : "<<
			GetLastError()<<endl;
	}

	//쓰레드 생성
	HANDLE hThread1 = (HANDLE)_beginthreadex( NULL , 0 , ThreadFunc , NULL , CREATE_SUSPENDED  , &uiThreadID );
	cout<<"Create Thread ID : "<<uiThreadID<<endl;
	HANDLE hThread2 = (HANDLE)_beginthreadex( NULL , 0 , ThreadFunc , NULL , CREATE_SUSPENDED  , &uiThreadID );
	cout<<"Create Thread ID : "<<uiThreadID<<endl;
	if( NULL == hThread1 || NULL == hThread2 )
	{
		cout<<"[ERROR] Thread1 or hThread2 is NULL.."<<endl;
		return 0;
	}

	//쓰레드를 실행
	ResumeThread( hThread1 );
	ResumeThread( hThread2 );
	//g_hEvent객체의 상태를 신호 받음(Signaled)상태로 바꾸어 준다.
	SetEvent( g_hEvent );
	
	//쓰레드가 종료될 때까지 기다린다.
	WaitForSingleObject( hThread1 , INFINITE );
	WaitForSingleObject( hThread2 , INFINITE );

	//g_hEvent객체의 사용을 종료한다.
	CloseHandle( g_hEvent );

	//쓰레드 핸들을 닫는다.
	CloseHandle( hThread1 );
	CloseHandle( hThread2 );
	return 0;
}

