#pragma once
#pragma warning( disable:4786 )
#pragma warning( disable:4251 )
#pragma warning( disable:4311 )
#pragma warning( disable:4244 )
#pragma warning( disable:4996 )

#include <iostream>
#include <winsock2.h>
#include <Mswsock.h>
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <ws2spi.h>
#include <winbase.h>
#include <process.h>
#include <vector>
#include <map>
#include <queue>
#include <list>
#include <fstream>
#include <math.h>

using namespace std;

#include "CommonDef.h"
#include "cMonitor.h"
#include "cSingleton.h"
#include "cThread.h"
#include "cQueue.h"
#include "cLog.h"
#include "cConnection.h"
#include "cIocpServer.h"
#include "cVBuffer.h"
