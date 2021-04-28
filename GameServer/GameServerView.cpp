// GameServerView.cpp : CGameServerView 클래스의 구현
//

#include "stdafx.h"
#include "GameServer.h"

#include "GameServerDoc.h"
#include "GameServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGameServerView

IMPLEMENT_DYNCREATE(CGameServerView, CView)

BEGIN_MESSAGE_MAP(CGameServerView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CGameServerView 생성/소멸

CGameServerView::CGameServerView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CGameServerView::~CGameServerView()
{
}

BOOL CGameServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CGameServerView 그리기

void CGameServerView::OnDraw(CDC* /*pDC*/)
{
	CGameServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CGameServerView 인쇄

BOOL CGameServerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CGameServerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CGameServerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CGameServerView 진단

#ifdef _DEBUG
void CGameServerView::AssertValid() const
{
	CView::AssertValid();
}

void CGameServerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGameServerDoc* CGameServerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGameServerDoc)));
	return (CGameServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CGameServerView 메시지 처리기
