// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#include "Tab.h"
#pragma once
class CMainFrame : public CFrameWnd
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성
public:

// 작업
public:
	CVisualFramework	m_frameWork;
	CVisualObject	*m_pTab1;
	CVisualObject	*m_pUserLogView; 

	LRESULT OnDebugMsg( WPARAM wParam , LPARAM lParam );
// 재정의
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// 메시지 맵 함수를 생성했습니다.
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
	afx_msg void OnServerStart();
	afx_msg void OnServerStop();
	afx_msg void OnCreateTempPlayer();
	afx_msg void OnDestroyTempPlayer();
	afx_msg void OnConnectNpcServer();

};

