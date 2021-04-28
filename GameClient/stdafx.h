// stdafx.h : 잘 변경되지 않고 자주 사용하는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Windows 헤더에서 거의 사용되지 않는 내용을 제외시킵니다.
#endif

#define _WIN32_WINNT _WIN32_WINNT_MAXVER

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소
#include <afxext.h>         // MFC 익스텐션
#include <afxdisp.h>        // MFC 자동화 클래스

#include <afxdtctl.h>		// Internet Explorer 4 공용 컨트롤에 대한 MFC 지원
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows 공용 컨트롤에 대한 MFC 지원
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <map>
using namespace std;
#define ROW_LINE 60
#define COL_LINE 60
#define CELLROW_LINE 6
#define CELLCOL_LINE 6
#define TILE_SIZE 12
#define START_XPOS 10
#define START_YPOS 20
#define UPDATETIME 300
#define ALL_CELLCOL_LINE (ROW_LINE / CELLROW_LINE)
#define INVALIDATERECT COL_LINE * TILE_SIZE


#define MY_PLAYER_COLOR RGB( 255,255,255 )
#define DEAD_PLAYER_COLOR RGB( 0,0,0 )
#define OTHER_PLAYER_COLOR RGB( 0 , 0 , 255 )
#define MOUSECURSOR_COLOR RGB( 255,0,0 )
#define NPCNORMAL_COLOR RGB( 128,128,0 )
#define NPCDETECT_COLOR RGB( 120, 0,0 )
#define NPCDISCOMPORT_COLOR RGB( 0,255,0 )
#define NPCANGLY_COLOR RGB( 255,0,0 )

#include "resource.h"
#include "../GameServer/GameDefine.h"
#include "../NetLib/Precompile.h"
#include "../GameServer/Packet.h"
#include "cPlayerManager.h"
#include "cAsyncSocket.h"
#include "cNpcManager.h"

