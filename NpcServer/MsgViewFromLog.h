#if !defined(AFX_MsgViewFromLog_H__2C8777BF_BA93_4858_B921_D518A11AEDE4__INCLUDED_)
#define AFX_MsgViewFromLog_H__2C8777BF_BA93_4858_B921_D518A11AEDE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgViewFromLog.h : header file
//
#include <afxcview.h>
#include "NpcServerDoc.h"

#define MAX_LINES 1000
/////////////////////////////////////////////////////////////////////////////
// CMsgViewFromLog view
class CMsgViewFromLog : public CListView , cSingleton
{
	DECLEAR_SINGLETON( CMsgViewFromLog );
public:
	CMsgViewFromLog();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMsgViewFromLog)


// Operations
public:
	void	__fastcall OutputDebugMsg( enumLogInfoType eLogInfoType , char* szDebugMsg );
	static CListCtrl *m_pListCtrl;
// Operations
public:
	CImageList*				m_pImageList;					//정보,에러에 대한 이미지를 가지고 있다.
	CNpcServerDoc* __fastcall GetDocument()
	{
		//return dynamic_cast<CMapServerDoc*>( m_pDocument );
		return (CNpcServerDoc*)( m_pDocument );
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgViewFromLog)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMsgViewFromLog();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMsgViewFromLog)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CREATE_FUNCTION( CMsgViewFromLog, LogView );
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MsgViewFromLog_H__2C8777BF_BA93_4858_B921_D518A11AEDE4__INCLUDED_)
