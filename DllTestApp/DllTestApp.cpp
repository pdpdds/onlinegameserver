// DllTestApp.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "../DllTest/DllTest.h"
//#pragma comment( lib, "DllTest.lib" )

int _tmain(int argc, _TCHAR* argv[])
{
	cMyClass MyClass;
	MyClass.Sum( 100 , 200 );
	MyClass.OutputSum();
	return 0;
}

#pragma