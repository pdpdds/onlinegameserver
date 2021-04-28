// GameServerDoc.cpp : CGameServerDoc 클래스의 구현
//

#include "stdafx.h"
#include "GameServer.h"

#include "GameServerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGameServerDoc

IMPLEMENT_DYNCREATE(CGameServerDoc, CDocument)

BEGIN_MESSAGE_MAP(CGameServerDoc, CDocument)
END_MESSAGE_MAP()


// CGameServerDoc 생성/소멸

CGameServerDoc::CGameServerDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CGameServerDoc::~CGameServerDoc()
{
}

BOOL CGameServerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 다시 초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CGameServerDoc serialization

void CGameServerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CGameServerDoc 진단

#ifdef _DEBUG
void CGameServerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGameServerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGameServerDoc 명령
