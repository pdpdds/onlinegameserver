// Tab.h: interface for the CTabItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAB_H__3E6D54FD_30AD_42BE_A063_9237EEDFF061__INCLUDED_)
#define AFX_TAB_H__3E6D54FD_30AD_42BE_A063_9237EEDFF061__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


////////////////////////////////////////////////////////////
class CTabItem  
{
public:
	CTabItem( CWnd*	pParent,LPCTSTR lpszLable,HICON hIcon = NULL );
	CTabItem( const CTabItem& obj );
	virtual ~CTabItem();

	void	SetRect( CRect& rect );
	void	SetFont( CFont*	pFont );
	void	SetIcon( HICON	hIcon );

	CString	GetText();
	void	SetText( LPCTSTR lpsz );

	void	Enable( BOOL bEnable );
	void	EnableTab( BOOL bEnable );
	void	ShowTab( BOOL bShow );

	int		GetLength();
	CWnd*	GetSafeWnd();
	CTabItem& operator=( const CTabItem& obj );

	friend	class CTabWnd;
private:
	CWnd	*m_pWnd;
	CStatic	*m_pCaption;
	BOOL	m_bEnabled;
	BOOL	m_bWndEnabled;
	BOOL	m_bVisible;
	int		m_nMinX,m_nMaxX;
	HICON	m_hIcon;
};
typedef list<CTabItem*> TABITEMLIST;

///////////////////////////////////////
class CVisualFramework;

/////////////////////////////////////////////////////////////////////////////
// CTabWnd window
class CTabWnd : public CWnd
{
	DECLARE_DYNCREATE( CTabWnd )
public:
	enum TABPos{ TP_TOP,TP_BOTTOM };
protected:
	CBrush	m_brushBlack,m_brushLGray;
	CPen	m_penWhite,m_penWhite2,m_penBlack,m_penLGray,m_penDGray,m_penDGray2;
	CFont	m_font;

	int		m_nSelectedTab;
	BOOL	m_bLockFlag;
	TABITEMLIST	m_listTab;
	TABPos	m_nTabPos;
	CTabItem*	findTabItem( int nIndex );

	void	createFont();
	int		drawTabTop( CDC* pDC,int x,CRect& client,CTabItem* pItem );
	int		drawTabBottom( CDC* pDC,int x,CRect& client,CTabItem* pItem );
	int		drawSelTabTop( CDC* pDC,int x,CRect& client,CTabItem* pItem );
	int		drawSelTabBottom( CDC* pDC,int x,CRect& client,CTabItem* pItem );
	void	drawClient( CDC* pDC,CRect& rect );
	void	invalidateTabArea();
	CTabItem*	addTab( CWnd* pWnd,LPCTSTR lpsz );
	BOOL	updateFrame( CFrameWnd* pFrame,CWnd* pWnd );

	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, 
						const RECT& rect, CWnd* pParentWnd, 
						UINT nID = AFX_IDW_PANE_FIRST, 
						CCreateContext* pContext = NULL);
	virtual void ResizeTab( int cx = -1,int cy = -1 );
	virtual CTabItem* CreatePane( LPCTSTR lpszLable,CRuntimeClass* pViewClass,CCreateContext* pContext );
	virtual CTabItem* CreatePane( LPCTSTR lpszLable,int nRow,int nCols,CWnd* pWnd,UINT nID = AFX_IDW_PANE_FIRST );

// Construction
public:
	CTabWnd();
	virtual ~CTabWnd();

	int		GetTabLength();
	int		GetTabCount();
	CWnd*	GetTabWnd( int nIndex );
	CString GetTabLabel( int nIndex );
	void	SetTabLabel( int nIndex,LPCTSTR lpszLabel );

	int		GetTabIndex();
	int		GetTabIndex( CWnd* pWnd );

	void	Enable( int index,BOOL bEnable );
	void	EnableTab( int index,BOOL bEnable );
	void	ShowTab( int nIndex,BOOL bShow );

	BOOL	IsTabEnabled( int index );
	BOOL	IsTabVisible( int index );

	void	SetFont( CFont* pFont );
	void	SetTabPos( TABPos nPos );

	virtual int HitTest( int x,int y );
	virtual int HitTest( CPoint& pt );

	virtual BOOL SetActivePane( int nIndex,BOOL bActive=TRUE );

	virtual BOOL CanSetActivePane( CWnd* pOldPane,CWnd* pNewPane );
	virtual void OnSetActivePane(  CWnd* pOldPane,CWnd* pNewPane );
	// Generated message map functions
protected:
	//{{AFX_MSG(CTabWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnSizeParent( WPARAM wparam,LPARAM lparam );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CVisualFramework;
};

class CVisualObject;
class CVisualFramework;

typedef list<CVisualObject*>		VISUALOBJECTLIST;
typedef map<DWORD,CVisualObject*>	VISUALOBJECTMAP;


/////////////////////////////////////////////////////////////////////////////
class CVisualObject  
{
public:
	enum OBJECTSTYLE{ TOS_TABTOP = 0x00000001,
					  TOS_TABBOTTOM=0x00000002,
					  TOS_SELECTED=0x00000004 };
	enum OBJECTTYPE {	OT_UNKNOWN,
						OT_SPLITTER,
						OT_SPLITTERVIEW,
						OT_SPLITTERSPLITTER,
						OT_TAB,
						OT_TABVIEW,
						OT_VIEW };
private:
	OBJECTTYPE	m_nObjectType;
	DWORD		m_dwID;
	CWnd*		m_pWnd;
	CWnd*		m_pParent;
	CString		m_strTitle;
	CString		m_strDescription;

	int			m_nRows,m_nCols,m_nRowIndex,m_nColIndex;

	CCreateContext	*m_pContext;
	CRuntimeClass	*m_pRuntimeClass;
	CSize			m_size;
	BOOL			m_bEnabled;
	DWORD			m_dwStyle;
	CHAR			m_cHotKey;
	HICON			m_hIcon;

	CVisualObject*	m_pOwner;
	CVisualFramework *m_pFramework;
	VISUALOBJECTLIST	m_listObject;

	void	zeroAll();
	void	checkStyle();
public:
	CVisualObject();
	CVisualObject( DWORD dwID,CCreateContext* pContext,CRuntimeClass* pClass );
	CVisualObject( DWORD dwID,LPCTSTR lpszTitle,CCreateContext* pContext,
					CRuntimeClass* pClass,DWORD dwStyle = 0 );
	CVisualObject( DWORD dwID,LPCTSTR lpszTitle,int nRows,int nCols,
					CCreateContext* pContext,DWORD dwStyle=0 );
	CVisualObject( DWORD dwID,int nRows,int nCol,CCreateContext* pContext ,
					CRuntimeClass* pClass,CSize size,DWORD dwStyle=0 );
	CVisualObject( DWORD dwID,int nRow,int nCol,int nRows,int nCols,
					CCreateContext* pContext,DWORD dwStyle = 0 );
	CVisualObject( const CVisualObject& obj );
	CVisualObject& operator=( const CVisualObject& obj );
	virtual ~CVisualObject();

	void	Destroy( BOOL bDestroy = FALSE );

	BOOL	CanFocus();
	void	SetHotKey( CHAR cHotKey );
	void	SetDescription( LPCTSTR lpszDesc );
	BOOL	SetIcon( UINT uID );
	HICON	GetIcon();				

	BOOL	SetActivePane();
	BOOL	SetActiveTab();

	BOOL	Enable( BOOL bEnable );
	BOOL	EnableTab( BOOL bEnable );
	BOOL	ShowTab( BOOL bShow );

	BOOL	IsEnabled( BOOL& bEnabled );
	BOOL	IsTabEnabled( BOOL& bEnabled );
	BOOL	IsTabVisible( BOOL& bEnabled );

	BOOL	IsTabPane();
	BOOL	IsTabWindow();
	BOOL	IsSplitterWindow();
	BOOL	IsSplitterPane();
	BOOL	IsView();

	DWORD	__fastcall GetID() { return m_dwID; }
	CWnd*	__fastcall GetWnd(){ return m_pWnd; }
	CWnd*	__fastcall GetSafeWnd() { return ::IsWindow( m_pWnd->m_hWnd ) ? m_pWnd:NULL; }
	CString	__fastcall GetDescription() { return m_strDescription; }
	CString	__fastcall GetTitle() { return m_strTitle; }
	CWnd*	__fastcall GetParentWnd() { return m_pParent; }
	CVisualFramework*	__fastcall GetFramework() { return m_pFramework; }
	CVisualObject*		__fastcall GetOwner() { return m_pOwner; }

	friend class CVisualFramework;
	friend class CVisualFrameworkIterator;
};

////////////////////////////////////////////////////////////////////////////////////////
class CVisualFrameworkIterator;

/////////////////////////////////////////////////////////////////////////////
// CVisualFramework command target
class CVisualFramework : public CCmdTarget
{
	DECLARE_DYNCREATE(CVisualFramework)
private:
	CWnd* m_pOwner;

	VISUALOBJECTLIST	m_listObject;
	VISUALOBJECTMAP		m_mapObject;

	BOOL	m_bEnableCtrlTab;

	CVisualObject*	findObject( DWORD dwID );
	CVisualObject*	findObject( CWnd* pWnd );

	void	execDestroy( CVisualObject* pObj );
	BOOL	execCreate( CWnd* pWnd,CVisualObject* pObject );
	BOOL	execCreateView( CWnd* pWnd,CVisualObject* pObject );
	BOOL	execCreateTabView( CWnd* pWnd,CVisualObject* pObject );
	BOOL	execCreateSplitter( CWnd* pWnd,CVisualObject* pObject );
	BOOL	execCreateSplitterView( CWnd* pWnd,CVisualObject* pObject );
	BOOL	execCreateSplitterSplitter( CWnd* pWnd,CVisualObject* pObject );
	BOOL	execCreateTabWnd( CWnd* pWnd,CVisualObject* pObject );
	void	setTabWndProperties( CVisualObject* pObject );

// Attributes
public:
	CVisualFramework();           // protected constructor used by dynamic creation
	virtual ~CVisualFramework();

	BOOL	Add( CVisualObject* pObj );
	BOOL	Add( CVisualObject* pOwner,CVisualObject* pObj );

	virtual BOOL Create( CWnd* pWnd = NULL );
	virtual void Destroy();

	CWnd*	GetWnd();
	CWnd*	GetSafeWnd();
	CWnd*	GetObject( DWORD dwID );
	DWORD	GetObject( CWnd* pWnd );
	CVisualObject* Get( DWORD dwID );
	CVisualObject* Get( CWnd* pWnd );

	BOOL	IsTabPane( CVisualObject* pObject );
	BOOL	IsTabWindow(CVisualObject* pObject );
	BOOL	IsSplitterPane( CVisualObject* pObject );
	BOOL	IsSplitterWindow( CVisualObject* pObject );
	BOOL	IsView( CVisualObject* pObject );

	int		GetCount();
	CVisualObject* GetActiveTab( CVisualObject* pObject );
	BOOL	SetActiveTab( CVisualObject* pObject );

	BOOL	SetActivePane( CVisualObject* pObject );
	CVisualObject* GetActivePane();

	BOOL	Enable( CVisualObject* pObject,BOOL bEnable );
	BOOL	EnableTab( CVisualObject* pObject,BOOL bEnable );
	BOOL	ShowTab( CVisualObject *pObject,BOOL bShow );

	BOOL	IsEnabled( CVisualObject* pObject,BOOL& bEnabled );
	BOOL	IsTabEnabled( CVisualObject* pObject,BOOL& bEnabled );
	BOOL	IsTabVisible(CVisualObject* pObject,BOOL& bEnabled );

	void	SetFont( CFont* pFont );

	void	EnableCtrlTab( BOOL bEnabled );

	virtual CSplitterWnd* CreateSplitter( DWORD dwID );
	virtual BOOL ProcessMessage( MSG* pMsg );
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisualFramework)
	virtual BOOL OnCmdMsg( UINT nID,int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo );
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVisualFramework)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	friend class CVisualFrameworkIterator;
};

/////////////////////////////////////////////////////////////////////////////
class CVisualFrameworkIterator  
{
private:
	enum ITERATORTYPE	{ IT_MAP,IT_LIST };

	VISUALOBJECTMAP	*m_pObjectMap;
	VISUALOBJECTMAP::iterator m_mapIt;
	VISUALOBJECTLIST *m_pObjectList;
	VISUALOBJECTLIST::iterator m_listIt;

	ITERATORTYPE	m_nType;
public:
	CVisualFrameworkIterator( CVisualFramework& obj ) : m_pObjectMap( &(obj.m_mapObject ) )
	{
		m_mapIt = m_pObjectMap->begin();
		m_nType = IT_MAP;
	}
	//////////////////////////////////////////////////
	CVisualFrameworkIterator( CVisualObject& obj ) : m_pObjectList( &(obj.m_listObject ) )
	{
		m_listIt = m_pObjectList->begin();
		m_nType = IT_LIST;
	}
	//////////////////////////////////
	CVisualObject* operator->()		
	{
		return Get();
	}
	////////////////////////////////
	CVisualObject* __fastcall Get()
	{
		if ( m_nType==IT_MAP )	return m_mapIt->second;
		else if ( m_nType==IT_LIST ) return *m_listIt;
		return NULL;
	}
	//////////////////////////////////
	int	__fastcall End()
	{
		if ( m_nType==IT_MAP )			return ( m_mapIt !=m_pObjectMap->end() ) ? 0:1;
		else if ( m_nType==IT_LIST )	return ( m_listIt !=m_pObjectList->end() ) ? 0 : 1;
		else 
			return 1;
	}
	///////////////////////////////////
	int operator++(int)
	{
		if ( m_nType==IT_MAP )			m_mapIt++;
		else if ( m_nType==IT_LIST )	m_listIt++;

		return End();
	}
	virtual ~CVisualFrameworkIterator();

};

#endif // !defined(AFX_TAB_H__3E6D54FD_30AD_42BE_A063_9237EEDFF061__INCLUDED_)

