// MsgViewFromLog.cpp : implementation file
//
#include "stdafx.h"
#include "NpcServer.h"
#include "MsgViewFromLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgViewFromLog
IMPLEMENT_DYNCREATE(CMsgViewFromLog, CListView)

IMPLEMENT_SINGLETON( CMsgViewFromLog );

CListCtrl* CMsgViewFromLog::m_pListCtrl = NULL;
CMsgViewFromLog::CMsgViewFromLog()
{
	m_pImageList = new CImageList;
}

CMsgViewFromLog::~CMsgViewFromLog()
{
	if( m_pImageList )
		delete m_pImageList;
}


BEGIN_MESSAGE_MAP(CMsgViewFromLog, CListView)
	//{{AFX_MSG_MAP(CMsgViewFromLog)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsgViewFromLog drawing

void CMsgViewFromLog::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CMsgViewFromLog diagnostics

#ifdef _DEBUG
void CMsgViewFromLog::AssertValid() const
{
	CListView::AssertValid();
}

void CMsgViewFromLog::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMsgViewFromLog message handlers

void CMsgViewFromLog::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	
}

BOOL CMsgViewFromLog::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |=(LVS_SHOWSELALWAYS | LVS_REPORT );
	
	return CListView::PreCreateWindow(cs);
}

int CMsgViewFromLog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//GetDocument()->m_pMsgViewFromLog = this;

	CListCtrl& listCtl = GetListCtrl();
	if ( !IsWindow( listCtl.GetSafeHwnd() ) )	return -1;
	m_pListCtrl = &listCtl;
	listCtl.InsertColumn( 0,"시간",LVCFMT_LEFT,150 );
	listCtl.InsertColumn( 1,"종류",LVCFMT_LEFT,50 );
	listCtl.InsertColumn( 2,"등급",LVCFMT_LEFT,130 );
	listCtl.InsertColumn( 3,"받은곳",LVCFMT_LEFT,100 );
	listCtl.InsertColumn( 4,"위치",LVCFMT_LEFT,200 );
	listCtl.InsertColumn( 5,"메세지",LVCFMT_LEFT,700 );
	
	listCtl.SendMessage( LVM_SETEXTENDEDLISTVIEWSTYLE,
						LVS_EX_FULLROWSELECT |
						LVS_EX_GRIDLINES |
						LVS_EX_TWOCLICKACTIVATE |
						LVS_EX_SUBITEMIMAGES,
						LVS_EX_FULLROWSELECT |
						LVS_EX_GRIDLINES |
						LVS_EX_TWOCLICKACTIVATE |
						LVS_EX_SUBITEMIMAGES 
						);
	
	HIMAGELIST hImgList;
	
	hImgList = ImageList_LoadImage( ::AfxGetInstanceHandle() , MAKEINTRESOURCE( IDB_BITMAP1 ) , 17 
		, 1 , RGB( 255,255,255 ) , IMAGE_BITMAP , 0 );
	
	m_pImageList->Attach( hImgList );

	listCtl.SetImageList( m_pImageList , LVSIL_SMALL );
	
	return 0;
}

void	__fastcall CMsgViewFromLog::OutputDebugMsg( enumLogInfoType eLogInfoType , char *szDebugMsg )
{
	
	int nCount = m_pListCtrl->GetItemCount();
	//최대 라인수가 넘었다면 초기화
	if( nCount >= MAX_LINES )
	{
		m_pListCtrl->DeleteAllItems();
	}
	//미리 빈공간을 입력해둔다
	LVITEM lvItem;
	lvItem.mask = LVIF_IMAGE | LVIF_TEXT;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.cchTextMax = 1024;
	lvItem.pszText = "";
	lvItem.iItem = nCount;
	lvItem.iSubItem = 0;
	lvItem.iImage = 0;

	//만약 에러가 아니라면 아이콘을 정보로 바꾼다.
	if( eLogInfoType < LOG_ERROR_LOW )
		lvItem.iImage = 1;

	m_pListCtrl->InsertItem( &lvItem );

	//m_pListCtrl->SetItemData( 
	int i = 0;
	for(i = 0 ; i < 5 ; i++ )
	{
		char* pFind = strchr( szDebugMsg , '|' ); 
		if( pFind == NULL )
			break;
		szDebugMsg[ pFind - szDebugMsg ] = NULL;
		
		m_pListCtrl->SetItemText( nCount , i , szDebugMsg );
		szDebugMsg = pFind + 1;
	}
	m_pListCtrl->SetItemText( nCount , i ,  szDebugMsg );
	m_pListCtrl->EnsureVisible( nCount , true );

}
//end of file