// NpcServerDoc.cpp : CNpcServerDoc 클래스의 구현
//

#include "stdafx.h"
#include "NpcServer.h"

#include "NpcServerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNpcServerDoc

IMPLEMENT_DYNCREATE(CNpcServerDoc, CDocument)

BEGIN_MESSAGE_MAP(CNpcServerDoc, CDocument)
END_MESSAGE_MAP()


// CNpcServerDoc 생성/소멸

CNpcServerDoc::CNpcServerDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CNpcServerDoc::~CNpcServerDoc()
{
}

BOOL CNpcServerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 다시 초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CNpcServerDoc serialization

void CNpcServerDoc::Serialize(CArchive& ar)
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


// CNpcServerDoc 진단

#ifdef _DEBUG
void CNpcServerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNpcServerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CNpcServerDoc 명령
