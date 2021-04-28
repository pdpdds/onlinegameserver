// Tab.cpp: implementation of the CTabItem class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Tab.h"

#include <afxpriv.h>        // Needed for WM_SIZEPARENT

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//=============================================================================
// class CTabItem
//
//=============================================================================

// Create a tab item 
CTabItem::CTabItem(CWnd *pParent, LPCTSTR szLabel, HICON hIcon)
{
  m_pWnd = NULL;
  m_nMinX = m_nMaxX = 0;
  m_bVisible = TRUE;
  m_bEnabled = TRUE;
  m_bWndEnabled = TRUE;
  m_hIcon = hIcon;
  RECT rect;
  ::ZeroMemory(&rect,sizeof(RECT));
  m_pCaption = new CStatic;
  ASSERT(m_pCaption);
  m_pCaption->Create(szLabel, WS_CHILD|SS_CENTER|WS_VISIBLE,rect,pParent);
}

CTabItem::CTabItem(const CTabItem& obj)
{
	*this = obj;
}

CTabItem& CTabItem::operator=(const CTabItem& obj)
{
  m_pWnd = obj.m_pWnd;
  m_pCaption = obj.m_pCaption;
  m_bWndEnabled = obj.m_bWndEnabled;
  m_bEnabled = obj.m_bEnabled;
  m_bVisible = obj.m_bVisible;
  m_nMinX = obj.m_nMinX;
  m_nMaxX = obj.m_nMaxX;
  return *this;
}

CTabItem::~CTabItem()
{
  // This is done in CVisualFramework::Destroy()
  //if (m_pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
  //  delete m_pWnd;
  ASSERT(m_pCaption);
  delete m_pCaption;
}

// Set rectangle for tab caption
void CTabItem::SetRect(CRect& rect)
{
  ASSERT(m_pCaption);
  m_pCaption->MoveWindow(&rect);
}

// Set font for tab caption
void CTabItem::SetFont(CFont *pFont)
{
  ASSERT(m_pCaption);
  ASSERT(pFont);
  m_pCaption->SetFont(pFont,FALSE);
}

// Set icon handle for this tab
void CTabItem::SetIcon(HICON hIcon)
{
  m_hIcon = hIcon;
}

// Get tab caption text
CString CTabItem::GetText(void)
{
  ASSERT(m_pCaption);
  CString str;
  m_pCaption->GetWindowText(str);
  return str;
}

int CTabItem::GetLength(void)
{
  return m_nMaxX - m_nMinX;
}

// Set tab caption text
void CTabItem::SetText(LPCTSTR szLabel)
{
  ASSERT(m_pCaption);
  ASSERT(szLabel);
  m_pCaption->SetWindowText(szLabel);
}

// Enable/disable a window
void CTabItem::Enable(BOOL bEnable)
{
  m_bWndEnabled = bEnable;
}

// Enable/disable tab caption
void CTabItem::EnableTab(BOOL bEnable)
{
  ASSERT(m_pCaption);
  m_bEnabled = bEnable;
  m_pCaption->EnableWindow(m_bEnabled);
}

// Show/hide tab caption
void CTabItem::ShowTab(BOOL bShow)
{
  ASSERT(m_pCaption);
  m_bVisible = bShow;
  m_pCaption->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
}

CWnd *CTabItem::GetSafeWnd(void)
{
  return (m_pWnd && ::IsWindow(m_pWnd->m_hWnd)) ? m_pWnd : NULL;
}

//=============================================================================
// class CTabWnd
//
//=============================================================================

#define TABWND_DEFAULT_ID 0x2578
#define TABWND_HEIGHT     30    // Height of the gray border between the toolbar 
                                // and the client area
#define TAB_HEIGHT        20    // Height on the normal tab
#define TABSEL_HEIGHT     20    // Height of the selected tab
#define TAB_SPACE         6     // Add to tab caption text width
#define TAB_DEPL          4     // Distance between the tabs and the client area
#define TAB_MAXLEN        200
#define TAB_ICON          16
#define TAB_ICON_X        2
#define TAB_ICON_Y        2

IMPLEMENT_DYNCREATE(CTabWnd,CWnd)

BEGIN_MESSAGE_MAP(CTabWnd, CWnd)
  //{{AFX_MSG_MAP(CTabWnd)
  ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
  ON_WM_ERASEBKGND()
  ON_WM_PAINT()
  ON_WM_LBUTTONUP()
  ON_WM_DESTROY()
  ON_WM_SIZE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTabWnd::CTabWnd()
{
  m_nSelectedTab = 0;
  m_bLockFlag = FALSE;
  m_nTabPos = TP_BOTTOM;
  // cache most used resources
  m_brushBlack.CreateSolidBrush(RGB(0,0,0));
  m_brushLGray.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
  m_penBlack.CreatePen(PS_SOLID, 1, (COLORREF)0);
  m_penLGray.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNFACE));
  m_penWhite.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNHIGHLIGHT));
  m_penWhite2.CreatePen(PS_SOLID, 2, ::GetSysColor(COLOR_BTNHIGHLIGHT));
  m_penDGray.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW));
  m_penDGray2.CreatePen(PS_SOLID, 2, ::GetSysColor(COLOR_BTNSHADOW));
}

CTabWnd::~CTabWnd()
{
}

// Find a tab within this tab window
CTabItem *CTabWnd::findTabItem(int nIndex)
{
  int nNdx = 0;
  TABITEMLIST::iterator iterator;
  for (iterator = m_listTab.begin(); iterator != m_listTab.end(); iterator++) {
    if (nNdx == nIndex)
      return (*iterator);
    nNdx ++;
  }
  return NULL;
}

// Create a tab window
BOOL CTabWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, 
                     DWORD dwStyle, const RECT& prect, CWnd* pParentWnd, 
                     UINT nID, CCreateContext *pContext)
{
  ASSERT(pParentWnd);

  dwStyle &= ~WS_BORDER;
  CRect rect(prect);
  if (!CWnd::Create(NULL, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
    return FALSE;
  if (pParentWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd))) {
    ((CFrameWnd*)pParentWnd)->ModifyStyleEx(WS_EX_CLIENTEDGE,0,SWP_FRAMECHANGED);
    ((CFrameWnd*)pParentWnd)->RecalcLayout();
  }
  ResizeTab();
  return TRUE;
}

int CTabWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
  createFont();
	return 0;
}

void CTabWnd::OnDestroy() 
{
  CWnd::OnDestroy();
  TABITEMLIST::iterator iterator;
  for (iterator = m_listTab.begin(); iterator != m_listTab.end(); iterator ++) {
    delete (*iterator);
  }
  m_listTab.clear();
  // This is done in CVisualFramework
  //if (GetParent()->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
  //  delete this;
}

// Virtual function to check whether switch to new tab can be done
BOOL CTabWnd::CanSetActivePane(CWnd *pOldPane, CWnd *pNewPane)
{
  return TRUE;
}

// Virtual function (after the switch is done)
void CTabWnd::OnSetActivePane(CWnd *pOldPane, CWnd *pNewPane)
{
}

// Create fonts for tab labels
void CTabWnd::createFont()
{
	NONCLIENTMETRICS metrics;
	metrics.cbSize = sizeof(metrics);
  ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &metrics, 0);

  CWindowDC wdc(NULL);
  int nLPixY = GetDeviceCaps(wdc.m_hDC, LOGPIXELSY);

	m_font.CreateFontIndirect(&metrics.lfStatusFont);
}

// Add a tab to this window
CTabItem *CTabWnd::addTab(CWnd *pWnd, LPCTSTR szLabel)
{
  ASSERT(pWnd);
  ASSERT(szLabel);

  CTabItem *pItem = new CTabItem(this,szLabel);
  pItem->m_pWnd = pWnd;
  m_listTab.insert(m_listTab.end(), pItem);
  return pItem;
}

int CTabWnd::GetTabLength()
{
  int nLength = 0;
  TABITEMLIST::iterator iterator;
  for (iterator = m_listTab.begin(); iterator != m_listTab.end(); iterator++) {
    nLength += (*iterator)->GetLength();
  }
  return nLength;
}

// Get index of currently selected tab
int CTabWnd::GetTabIndex(void)
{
  return m_nSelectedTab;
}

// Get number of tabs
int CTabWnd::GetTabCount(void)
{
  return (int)m_listTab.size();
}

// Get index of the tab associated with specified window
int CTabWnd::GetTabIndex(CWnd *pWnd)
{
  ASSERT(pWnd);

  int nIndex = 0;
  CTabItem *pItem;
  TABITEMLIST::iterator iterator;
  for (iterator = m_listTab.begin(); iterator != m_listTab.end(); iterator++) {
    pItem = *iterator;
    if (pItem->m_pWnd == pWnd)
      return nIndex;
    nIndex ++;
  }
  return -1;
}

// Get pointer to window associated with the specified tab index
CWnd *CTabWnd::GetTabWnd(int index)
{
  CTabItem *pItem = findTabItem(index);
  ASSERT(pItem);
  return ::IsWindow(pItem->m_pWnd->m_hWnd) ? pItem->m_pWnd : NULL;
}

// Get tab caption text of the specified tab
CString CTabWnd::GetTabLabel(int nIndex)
{
  CTabItem *pItem = findTabItem(nIndex);
  ASSERT(pItem);
  return pItem->GetText();
}

// Set text of tab caption
void CTabWnd::SetTabLabel(int nIndex, LPCTSTR szLabel)
{
  ASSERT(szLabel);
  CTabItem *pItem = findTabItem(nIndex);
  ASSERT(pItem);
  pItem->SetText(szLabel);
  invalidateTabArea();
}

// Enable/disable a view
void CTabWnd::Enable(int nIndex, BOOL bEnable)
{
  CTabItem *pItem = findTabItem(nIndex);
  ASSERT(pItem);
  pItem->Enable(bEnable);
  //pItem->m_pWnd->EnableWindow(bEnable);
}

// Cannot disable currently selected tab
void CTabWnd::EnableTab(int nIndex, BOOL bEnable)
{
  ASSERT(nIndex != m_nSelectedTab);
  CTabItem *pItem = findTabItem(nIndex);
  ASSERT(pItem);
  pItem->EnableTab(bEnable);
  invalidateTabArea();
}

// Cannot make invisible currently selected tab
void CTabWnd::ShowTab(int nIndex, BOOL bShow)
{
  ASSERT(nIndex != m_nSelectedTab);
  CTabItem *pItem = findTabItem(nIndex);
  ASSERT(pItem);
  pItem->ShowTab(bShow);
  invalidateTabArea();
}

// Is tab enabled
BOOL CTabWnd::IsTabEnabled(int nIndex)
{
  CTabItem *pItem = findTabItem(nIndex);
  ASSERT(pItem);
  return pItem->m_bEnabled;
}

// Is tab visible
BOOL CTabWnd::IsTabVisible(int nIndex)
{
  CTabItem *pItem = findTabItem(nIndex);
  ASSERT(pItem);
  return pItem->m_bVisible;
}

// Set font
void CTabWnd::SetFont(CFont *pFont)
{
  ASSERT(pFont);
  CWnd::SetFont(pFont);
  m_font.DeleteObject();
  LOGFONT lf;
  pFont->GetLogFont(&lf);
  m_font.CreateFontIndirect(&lf);
  invalidateTabArea();
}

// Set position of tabs (top or bottom)
void CTabWnd::SetTabPos(TABPos nTabPos)
{
  m_nTabPos = nTabPos;
}

// Invalidate rectangle to redraw tabs
void CTabWnd::invalidateTabArea(void)
{
  CRect rect;
  switch (m_nTabPos) {
  case TP_TOP: 
    InvalidateRect(&CRect(0, 0, 32000, TABWND_HEIGHT)); 
    break;
  case TP_BOTTOM:
    GetClientRect(&rect);
    InvalidateRect(&CRect(CPoint(0,rect.Height()-TABWND_HEIGHT), 
                          CSize(32000,TABWND_HEIGHT)));
    break;
  };
}

// Draws a selected tab and returns its height
int CTabWnd::drawSelTabTop(CDC *pDC, int x, CRect& client, CTabItem *pItem)
{
  ASSERT(pItem);
  ASSERT(pDC);

  CString str = pItem->GetText();
  CSize textSize = pDC->GetTextExtent(str);
  textSize.cx += 4;
  if (textSize.cx > TAB_MAXLEN)
    textSize.cx = TAB_MAXLEN;

  int y = TABWND_HEIGHT - TABSEL_HEIGHT - TAB_DEPL;
  int nDeltaWidth = 0;

  if (pItem->m_hIcon) {
    nDeltaWidth = TAB_ICON + TAB_ICON_X;
    textSize.cx += nDeltaWidth;
  }

  // black border, no bottom line
  pDC->SelectObject(&m_penBlack);
  pDC->MoveTo(x,y+TABSEL_HEIGHT-1);
  pDC->LineTo(x,y);
  pDC->LineTo(x+textSize.cx+TAB_SPACE-1, y);
  pDC->LineTo(x+textSize.cx+TAB_SPACE-1, y+TABSEL_HEIGHT);

  // left and upper border in white, double line
  pDC->SelectObject(&m_penWhite2);
  pDC->MoveTo(x+2,y+TABSEL_HEIGHT-1);
  pDC->LineTo(x+2,y+2);
  pDC->LineTo(x+textSize.cx+TAB_SPACE-4, y+2);

  // right border, dark gray, double line
  pDC->SelectObject(&m_penDGray2);
  pDC->MoveTo(x+textSize.cx+TAB_SPACE-2, y+2);
  pDC->LineTo(x+textSize.cx+TAB_SPACE-2, y+TABSEL_HEIGHT-1);

  // clean up
  pDC->SelectObject(&m_penLGray);
  pDC->MoveTo(x-1, y+TABSEL_HEIGHT);
  pDC->LineTo(x+textSize.cx+TAB_SPACE, y+TABSEL_HEIGHT);
  pDC->MoveTo(x-1, y+TABSEL_HEIGHT+1);
  pDC->LineTo(x+textSize.cx+TAB_SPACE, y+TABSEL_HEIGHT+1);

  // a black line to far left and right
  pDC->SelectObject(&m_penBlack);
  pDC->MoveTo(0, y+TABSEL_HEIGHT-1);
  pDC->LineTo(x, y+TABSEL_HEIGHT-1);
  pDC->MoveTo(x+textSize.cx+TAB_SPACE+1, y+TABSEL_HEIGHT-1);
  pDC->LineTo(client.right, y+TABSEL_HEIGHT-1);

  // and a white double line
  pDC->SelectObject(&m_penWhite2);
  if (x!=0) {
    pDC->MoveTo(0, y+TABSEL_HEIGHT+1);
    pDC->LineTo(x, y+TABSEL_HEIGHT+1);
  }
  pDC->MoveTo(x+textSize.cx+TAB_SPACE, y+TABSEL_HEIGHT+1);
  pDC->LineTo(client.right, y+TABSEL_HEIGHT+1);

  // gray inside
  pDC->FillSolidRect(x+3, y+3,textSize.cx+TAB_SPACE-6, TABSEL_HEIGHT, 
                  ::GetSysColor(COLOR_BTNFACE));

  if (pItem->m_hIcon) {
    ::DrawIconEx(pDC->m_hDC,x+4,y+TAB_ICON_Y,pItem->m_hIcon,16,16,0,NULL,DI_NORMAL);
  }

  CRect rect(CPoint(x+nDeltaWidth+TAB_SPACE/2, y+(TAB_HEIGHT-textSize.cy)/2+1), 
             CSize(textSize.cx-nDeltaWidth, textSize.cy));
  pItem->SetFont(&m_font);
  pItem->SetRect(rect);
  
  return textSize.cx+TAB_SPACE;
}

// Draw a selected tab at the bottom
int CTabWnd::drawSelTabBottom(CDC *pDC, int x, CRect& client, CTabItem *pItem)
{
  ASSERT(pItem);
  ASSERT(pDC);

  CString str = pItem->GetText();
  CSize textSize = pDC->GetTextExtent(str);
  textSize.cx += 4;
  if (textSize.cx > TAB_MAXLEN)
    textSize.cx = TAB_MAXLEN;

  int y = client.Height() - TABWND_HEIGHT + TAB_DEPL;
  int nDeltaWidth = 0;

  if (pItem->m_hIcon) {
    nDeltaWidth = TAB_ICON + TAB_ICON_X;
    textSize.cx += nDeltaWidth;
  }

  // black border, no bottom line
  pDC->SelectObject(&m_penBlack);
  pDC->MoveTo(x,y);
  pDC->LineTo(x,y+TABSEL_HEIGHT);
  pDC->LineTo(x+textSize.cx+TAB_SPACE-1, y+TABSEL_HEIGHT);
  pDC->LineTo(x+textSize.cx+TAB_SPACE-1, y);

  // left border in white, double line
  pDC->SelectObject(&m_penWhite2);
  pDC->MoveTo(x+2,y);
  pDC->LineTo(x+2,y+TABSEL_HEIGHT-1);

  // right and bottom border, dark gray, double line
  pDC->SelectObject(&m_penDGray2);
  pDC->LineTo(x+textSize.cx+TAB_SPACE-4, y+TABSEL_HEIGHT-1);
  pDC->MoveTo(x+textSize.cx+TAB_SPACE-2, y);
  pDC->LineTo(x+textSize.cx+TAB_SPACE-2, y+TABSEL_HEIGHT-1);

  // a black line to far left and right
  pDC->SelectObject(&m_penBlack);
  pDC->MoveTo(0, y);
  pDC->LineTo(x, y);
  pDC->MoveTo(x+textSize.cx+TAB_SPACE, y);
  pDC->LineTo(client.right, y);

  // and a gray line to far left and right
  pDC->SelectObject(&m_penDGray);
  if (x != 0) {
    pDC->MoveTo(0, y-1);
    pDC->LineTo(x, y-1);
  }
  pDC->MoveTo(x+textSize.cx+TAB_SPACE-2, y-1);
  pDC->LineTo(client.right, y-1);

  // gray inside
  pDC->FillSolidRect(x+3,y,textSize.cx+TAB_SPACE-6, TABSEL_HEIGHT-2, 
                    ::GetSysColor(COLOR_BTNFACE));

  if (pItem->m_hIcon) {
    ::DrawIconEx(pDC->m_hDC,x+4,y+TAB_ICON_Y,pItem->m_hIcon,16,16,0,NULL,DI_NORMAL);
  }

  CRect rect(CPoint(x+nDeltaWidth+TAB_SPACE/2, y+(TAB_HEIGHT-textSize.cy)/2+1), 
             CSize(textSize.cx-nDeltaWidth, textSize.cy));
  pItem->SetFont(&m_font);
  pItem->SetRect(rect);
  
  return textSize.cx+TAB_SPACE;
}

// Draws an unselected tab and returs its height
int CTabWnd::drawTabTop(CDC *pDC, int x, CRect& client, CTabItem *pItem)
{
  ASSERT(pItem);
  ASSERT(pDC);

  CString str = pItem->GetText();
  CSize textSize = pDC->GetTextExtent(str);
  textSize.cx += 4;
  if (textSize.cx > TAB_MAXLEN)
    textSize.cx = TAB_MAXLEN;

  int y = TABWND_HEIGHT-TAB_HEIGHT-TAB_DEPL;
  int nDeltaWidth = 0;

  if (pItem->m_hIcon) {
    nDeltaWidth = TAB_ICON + TAB_ICON_X;
    textSize.cx += nDeltaWidth;
  }
  
  // black border
  pDC->FrameRect(&CRect(CPoint(x,y), CSize(textSize.cx+TAB_SPACE, TAB_HEIGHT)), 
                  &m_brushBlack);

  pDC->SelectObject(&m_penWhite);
  pDC->MoveTo(x+1, y+1);
  pDC->LineTo(x+1, y+TAB_HEIGHT-1);
  pDC->MoveTo(x+1, y+1);
  pDC->LineTo(x+textSize.cx+TAB_SPACE-2, y+1);

  pDC->SelectObject(&m_penDGray);
  pDC->MoveTo(x+textSize.cx+TAB_SPACE-2, y+1);
  pDC->LineTo(x+textSize.cx+TAB_SPACE-2, y+TAB_HEIGHT-1);

  pDC->FillRect(&CRect(CPoint(x+2,y+2), CSize(textSize.cx+TAB_SPACE-4, TAB_HEIGHT-3)), 
                &m_brushLGray);

  // clean up
  int dy = TABSEL_HEIGHT-TAB_HEIGHT;
  pDC->FillSolidRect(x, y-dy, textSize.cx+TAB_SPACE, dy, GetSysColor(COLOR_BTNFACE));

  if (pItem->m_hIcon) {
    ::DrawIconEx(pDC->m_hDC,x+4,y+TAB_ICON_Y,pItem->m_hIcon,16,16,0,NULL,DI_NORMAL);
  }

  CRect rect(CPoint(x+nDeltaWidth+TAB_SPACE/2, y+(TAB_HEIGHT-textSize.cy)/2+1), 
             CSize(textSize.cx-nDeltaWidth, textSize.cy));
  pItem->SetFont(&m_font);
  pItem->SetRect(rect);
  
  return textSize.cx+TAB_SPACE;
}

// Draw an unselected tab at the bottom
int CTabWnd::drawTabBottom(CDC *pDC, int x, CRect& client, CTabItem *pItem)
{
  ASSERT(pItem);
  ASSERT(pDC);

  CString str = pItem->GetText();
  CSize textSize = pDC->GetTextExtent(str);
  textSize.cx += 4;
  if (textSize.cx > TAB_MAXLEN)
    textSize.cx = TAB_MAXLEN;

  int y = client.Height() - TABWND_HEIGHT + TAB_DEPL;
  int nDeltaWidth = 0;

  if (pItem->m_hIcon) {
    nDeltaWidth = TAB_ICON + TAB_ICON_X;
    textSize.cx += nDeltaWidth;
  }

  // black border
  pDC->FrameRect(&CRect(CPoint(x,y), CSize(textSize.cx+TAB_SPACE, TAB_HEIGHT+1)), 
                  &m_brushBlack);

  // Gray border bottom and right side
  pDC->SelectObject(&m_penDGray);
  pDC->MoveTo(x+1, y+TAB_HEIGHT-1);
  pDC->LineTo(x+textSize.cx+TAB_SPACE-2, y+TAB_HEIGHT-1);
  pDC->MoveTo(x+textSize.cx+TAB_SPACE-2, y);
  pDC->LineTo(x+textSize.cx+TAB_SPACE-2, y+TAB_HEIGHT-1);

  pDC->FillRect(&CRect(CPoint(x+1,y+1), CSize(textSize.cx+TAB_SPACE-4, TAB_HEIGHT-3)), 
                &m_brushLGray);

  if (pItem->m_hIcon) {
    ::DrawIconEx(pDC->m_hDC,x+4,y+TAB_ICON_Y,pItem->m_hIcon,16,16,0,NULL,DI_NORMAL);
  }

  CRect rect(CPoint(x+nDeltaWidth+TAB_SPACE/2, y+(TAB_HEIGHT-textSize.cy)/2+1), 
             CSize(textSize.cx-nDeltaWidth, textSize.cy));
  pItem->SetFont(&m_font);
  pItem->SetRect(rect);
  
  return textSize.cx+TAB_SPACE;
}

// Draw edge arround client area
void CTabWnd::drawClient(CDC *pDc, CRect& rect)
{
  ASSERT(pDc);

  CWnd *pParent = GetParent();
  ASSERT(pParent);
  switch (m_nTabPos) {
  case TP_TOP:
    if (pParent->IsKindOf(RUNTIME_CLASS(CFrameWnd))) {
      pDc->DrawEdge(&rect, EDGE_ETCHED, BF_TOP);
    }
    pDc->Draw3dRect(0,TABWND_HEIGHT, rect.right, rect.bottom-TABWND_HEIGHT,
                   ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
    pDc->Draw3dRect(1,TABWND_HEIGHT+1, rect.right-2, rect.bottom-TABWND_HEIGHT-2,
                    0, ::GetSysColor(COLOR_3DLIGHT));
    break;
  case TP_BOTTOM:
    if (pParent->IsKindOf(RUNTIME_CLASS(CFrameWnd))) {
      pDc->DrawEdge(&rect, EDGE_ETCHED, BF_BOTTOM);
    }
    pDc->Draw3dRect(0,0, rect.right, rect.bottom-TABWND_HEIGHT+1,
                   ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
    pDc->Draw3dRect(1,1, rect.right-2, rect.bottom-TABWND_HEIGHT-1,
                    0, ::GetSysColor(COLOR_3DLIGHT));
    break;
  }
}

void CTabWnd::OnPaint()
{
  CPaintDC dc(this); // device context for painting
  CRect client;

  GetClientRect(&client);
  drawClient(&dc, client);

  int x = 0, nIndex = 0;
  TABITEMLIST::iterator iterator;
  for (iterator = m_listTab.begin(); iterator != m_listTab.end(); iterator++) {
    CTabItem *pItem = *iterator;
    ASSERT(pItem != NULL);
    if (pItem->m_bVisible) {
      pItem->m_nMinX = x;
      if (nIndex != m_nSelectedTab) {
        switch (m_nTabPos) {
        case TP_TOP: x += drawTabTop(&dc, x, client, pItem); break;
        case TP_BOTTOM: x += drawTabBottom(&dc, x, client, pItem); break;
        }
      } else {
        switch (m_nTabPos) {
        case TP_TOP: x += drawSelTabTop(&dc, x, client, pItem); break;
        case TP_BOTTOM: x += drawSelTabBottom(&dc, x, client, pItem); break;
        }
      }
    }
    pItem->m_nMaxX = x;
    nIndex ++;
  }
}

// Returns tab index that holds specified point
int CTabWnd::HitTest(CPoint& point)
{
  return HitTest(point.x,point.y);
}

// Returns tab index that holds specified point
int CTabWnd::HitTest(int x, int y)
{
  int notsel_y_min, sel_y_min, y_max;
  CRect rect;

  GetClientRect(&rect);
  switch (m_nTabPos) {
  case TP_TOP:
    notsel_y_min = TABWND_HEIGHT - TAB_HEIGHT - TAB_DEPL;
    sel_y_min = TABWND_HEIGHT - TABSEL_HEIGHT - TAB_DEPL;
    y_max = TABWND_HEIGHT - TAB_DEPL;
    break;
  case TP_BOTTOM:
    notsel_y_min = rect.Height() - TABWND_HEIGHT + TAB_DEPL;
    sel_y_min = rect.Height() - TABWND_HEIGHT + TAB_DEPL;
    y_max = rect.Height() - TABWND_HEIGHT + TAB_DEPL + TAB_HEIGHT;
    break;
  };

  int nIndex = 0;
  TABITEMLIST::iterator iterator;
  for (iterator = m_listTab.begin(); iterator != m_listTab.end(); iterator++) {
    CTabItem *pItem = (*iterator);
    if (pItem->m_bEnabled && pItem->m_bVisible) {
      if (nIndex != m_nSelectedTab && (y < notsel_y_min || y > y_max)) 
        continue;
      if (nIndex == m_nSelectedTab && (y < sel_y_min || y > y_max)) 
        continue;
      if (x >= pItem->m_nMinX && x <= pItem->m_nMaxX)
        return nIndex;
    }
    nIndex++;
  }
  return -1;
}

// Switch focus to specified tab index
BOOL CTabWnd::SetActivePane(int nIndex, BOOL bActivate)
{
  if (nIndex == -1)
    return FALSE;
  if (nIndex == m_nSelectedTab)
    return TRUE;

  CTabItem *pNewPane = findTabItem(nIndex);
  if (!pNewPane->m_bEnabled || !pNewPane->m_bVisible)
    return FALSE;
  CTabItem *pOldPane = NULL;
  if (m_nSelectedTab != -1)
    pOldPane = findTabItem(m_nSelectedTab);
  if (CanSetActivePane(pOldPane ? pOldPane->m_pWnd : NULL, pNewPane->m_pWnd)) {
    // Deactivate old pane
    if (m_nSelectedTab != -1) {
      pOldPane->m_pWnd->EnableWindow(FALSE);
      pOldPane->m_pWnd->ShowWindow(SW_HIDE);
    }
    // Activate new pane
    pNewPane->m_pWnd->EnableWindow(pNewPane->m_bWndEnabled ? TRUE : FALSE);
    pNewPane->m_pWnd->ShowWindow(SW_SHOW);
    pNewPane->m_pWnd->SetFocus();
    // Save index of new pane
    m_nSelectedTab = nIndex;
    // Invalidate tab
    invalidateTabArea();
    // Inform derived class
    OnSetActivePane(pOldPane ? pOldPane->m_pWnd : NULL, pNewPane->m_pWnd);
    // Update frame window
    if (bActivate) {
      CWnd *pParent = GetParent();
      while (pParent && !pParent->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
        pParent = pParent->GetParent();
      ASSERT(pParent != NULL);
      updateFrame((CFrameWnd*)pParent, pNewPane->m_pWnd);
    }
    return TRUE;
  }
  return FALSE;
}

// Update frame window active view based on newly selected tab item
BOOL CTabWnd::updateFrame(CFrameWnd *pFrame, CWnd *pWnd)
{
  ASSERT(pFrame);
  ASSERT(pWnd);

  if (pWnd->IsKindOf(RUNTIME_CLASS(CView))) {
    // New tab item is a view
    pFrame->SetActiveView((CView*)pWnd);
    return TRUE;
  } else if (pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd))) {
    CSplitterWnd *pSplitter = (CSplitterWnd*)pWnd;
    CWnd *pView = pSplitter->GetActivePane();
    if (pView == NULL) {
      CWnd *pTmpView;
      for (int x = 0; x < pSplitter->GetRowCount(); x ++) {
        for (int y = 0; y < pSplitter->GetColumnCount(); y ++) {
          pTmpView = pSplitter->GetPane(x,y);
          if (pTmpView->IsWindowEnabled()) {
            if (updateFrame(pFrame, pTmpView))
              return TRUE;
          }
        }
      }
    }
    if (pView == NULL)
      pView = pSplitter->GetPane(0,0);
    pFrame->SetActiveView((CView*)pView);
  } else if (pWnd->IsKindOf(RUNTIME_CLASS(CTabWnd))) {
    CTabWnd *pTab = (CTabWnd*)pWnd;
    int nIndex = pTab->GetTabIndex();
    CWnd *pTabWnd = pTab->GetTabWnd(nIndex);
    if (updateFrame(pFrame, pTabWnd))
      return TRUE;
  }
  return FALSE;
}

// Resize tab
void CTabWnd::ResizeTab(int cx, int cy)
{
  CRect rect;
  CWnd *pParent = (CWnd*)GetParent();
  ASSERT(pParent);
  pParent->GetClientRect(&rect); 

  if (pParent->IsKindOf(RUNTIME_CLASS(CSplitterWnd))) {
    CSplitterWnd *splitter = (CSplitterWnd*)pParent;
    ASSERT(pParent);
    int row,col;
    splitter->IsChildPane(this,row,col);
    splitter->RecalcLayout();
  } else if (pParent->IsKindOf(RUNTIME_CLASS(CFrameWnd))) {
    m_bLockFlag = TRUE;
    pParent->RepositionBars(0, 0xFFFF, AFX_IDW_PANE_FIRST, CWnd::reposQuery, &rect);
    MoveWindow(rect.left,rect.top,rect.Width(),rect.Height());
    m_bLockFlag = FALSE;
  }

  m_bLockFlag = TRUE; // reentrancy check (might get called recursivly from OnSize)
  CWnd *pWnd;
  TABITEMLIST::iterator iterator;
  for (iterator = m_listTab.begin(); iterator != m_listTab.end(); iterator++) {
    pWnd = (*iterator)->m_pWnd;
    if (cx == -1 && cy == -1) {
      switch (m_nTabPos) {
      case TP_TOP:
        pWnd->MoveWindow(1, TABWND_HEIGHT+1, 
                         rect.Width()-2, rect.Height()-TABWND_HEIGHT-2);
        break;
      case TP_BOTTOM:
        pWnd->MoveWindow(1, 0, rect.Width()-2, rect.Height()-TABWND_HEIGHT);
        break;
      }
    } else {
      switch (m_nTabPos) {
      case TP_TOP:
        pWnd->MoveWindow(1, TABWND_HEIGHT+1, cx, cy-TABWND_HEIGHT-2);
        break;
      case TP_BOTTOM:
        pWnd->MoveWindow(1, 0, cx, cy-TABWND_HEIGHT);
        break;
      }
    }
  }
  m_bLockFlag=FALSE;
}

// Erase area where the tabs are displayed
BOOL CTabWnd::OnEraseBkgnd(CDC* pDC)
{
  ASSERT(pDC);

  CRect rect;
  GetClientRect(&rect);
  switch (m_nTabPos) {
  case TP_TOP:
    pDC->FillSolidRect(&CRect(0, 0, rect.right, TABWND_HEIGHT), 
                      ::GetSysColor(COLOR_BTNFACE));
    break;
  case TP_BOTTOM:
    pDC->FillSolidRect(&CRect(0, rect.bottom-TABWND_HEIGHT-3, rect.right, rect.bottom), 
                      ::GetSysColor(COLOR_BTNFACE));
    break;
  }
  return TRUE;
}

// Handle couse click on tabs
void CTabWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
  int nNewTab = HitTest(point.x, point.y);
  SetActivePane(nNewTab);
  CWnd::OnLButtonUp(nFlags, point);
}

// Handle resize
LRESULT CTabWnd::OnSizeParent(WPARAM, LPARAM lParam)
{
  if (m_bLockFlag)
    return 0;
  ResizeTab();
  return 0;
}

// Handle resize
void CTabWnd::OnSize(UINT nType, int cx, int cy) 
{
  CWnd::OnSize(nType, cx, cy);
  ResizeTab(cx,cy);
}

// Create a CView derived class as a tab
CTabItem *CTabWnd::CreatePane(LPCTSTR lpszLabel, CRuntimeClass *pViewClass, 
                                 CCreateContext *pContext)
{
  CRect rect, client;
  ASSERT(pViewClass && pContext);

  CWnd *pWnd = (CWnd*)pViewClass->CreateObject();
  if (!pWnd) 
    return NULL;
  
  GetClientRect(&client);
  rect.left = 0;
  rect.top = TABWND_HEIGHT+2;
  rect.right = client.right;
  rect.bottom = client.bottom;

  int dwStyle = AFX_WS_DEFAULT_VIEW;
  if (GetParent()->IsKindOf(RUNTIME_CLASS(CSplitterWnd))) {
    dwStyle &= ~WS_BORDER;
  }

  if (!pWnd->Create(NULL, NULL, dwStyle, rect, this, 13576+( UINT) m_listTab.size(), pContext))
  {
    TRACE0("Warning: couldn't create client area for tab view\n");
    // pWnd will be cleaned up by PostNcDestroy
    return NULL;
  }

  // Insert new tab object into the list
  CTabItem *pTab = addTab(pWnd,lpszLabel);
  ASSERT(pTab);
  if (m_listTab.size() != 1) {
    pWnd->EnableWindow(FALSE);
    pWnd->ShowWindow(SW_HIDE);
  /*
  // Framework is responsible to set the active view
  } else {
    CWnd *pParent = GetParent();
    if (pParent->IsKindOf(RUNTIME_CLASS(CFrameWnd))) {
      ((CFrameWnd*)pParent)->SetActiveView((CView*)pWnd);
    } else if (pParent->IsKindOf(RUNTIME_CLASS(CSplitterWnd))) {
      ((CSplitterWnd*)pParent)->SetActivePane(0,0,pWnd);
    }
  */
  }
  return pTab;
}

// Create a splitter window as a tab
CTabItem *CTabWnd::CreatePane(LPCTSTR lpszLabel, int nRows, int nCols, 
                                 CWnd *pWnd, UINT nID)
{
  ASSERT(pWnd);
  ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)));

  // Moved to CVisualFramework to handle creation of CSplitterWnd derived classes
  //CSplitterWnd *pWnd = new CSplitterWnd;
  //if (!pWnd) 
  //  return NULL;

  int dwStyle = AFX_WS_DEFAULT_VIEW;
  dwStyle &= ~WS_BORDER;

  CSplitterWnd *pSplitter = (CSplitterWnd*)pWnd;
  if (!pSplitter->CreateStatic(this, nRows, nCols, dwStyle, nID)) {
    TRACE0("Warning: couldn't create client area for tab view\n");
    // pWnd will be cleaned up by PostNcDestroy
    return NULL;
  }

  CTabItem *pTab = addTab(pWnd,lpszLabel);
  ASSERT(pTab);
  if (m_listTab.size() != 1) {
    pWnd->EnableWindow(FALSE);
    pWnd->ShowWindow(SW_HIDE);
  } 

  /*
  // Framework will set the active view
  CWnd *paneWnd = pWnd->GetActivePane();
  if (paneWnd) {
    ((CFrameWnd*)GetParent())->SetActiveView((CView*)paneWnd);
  } else {
    paneWnd = pWnd->GetPane(0,0);
    pWnd->SetActivePane(0,0);
    ((CFrameWnd*)GetParent())->SetActiveView((CView*)paneWnd);
  }
  */

  return pTab;
}

//=============================================================================
// class CVisualObject
//
//=============================================================================

// Private constructor
CVisualObject::CVisualObject()
{
}

// Create a plain view
CVisualObject::CVisualObject(DWORD dwId, CCreateContext *pContext, 
                       CRuntimeClass *pClass)
{
  ASSERT(pContext);
  ASSERT(pClass);
  ASSERT(pClass->IsDerivedFrom(RUNTIME_CLASS(CView)));

  zeroAll();
  m_dwID = dwId;
  m_nObjectType = OT_VIEW;
  m_pContext = pContext;
  m_pRuntimeClass = pClass;
  checkStyle();
}

// Create a view within a tab window or a tab window
CVisualObject::CVisualObject(DWORD dwId, LPCTSTR szTitle, CCreateContext *pContext, 
                       CRuntimeClass *pClass, DWORD dwStyle)
{
  ASSERT(szTitle);
  ASSERT(pContext);
  ASSERT(pClass);

  zeroAll();
  m_dwID = dwId;
  if (pClass->IsDerivedFrom(RUNTIME_CLASS(CTabWnd))) {
    m_nObjectType = OT_TAB;
  } else if (pClass->IsDerivedFrom(RUNTIME_CLASS(CView))) {
    m_nObjectType = OT_TABVIEW;
  } else {
    ASSERT(FALSE);
  }
  m_strTitle = szTitle;
  m_pContext = pContext;
  m_pRuntimeClass = pClass;
  m_dwStyle = dwStyle;
  checkStyle();
}

// Create a splitter window
CVisualObject::CVisualObject(DWORD dwId, LPCTSTR szTitle, int nRows, int nCols, 
                       CCreateContext *pContext, DWORD dwStyle)
{
  ASSERT(szTitle);
  ASSERT(pContext);
  ASSERT(nRows);
  ASSERT(nCols);

  zeroAll();
  m_dwID = dwId;
  m_nObjectType = OT_SPLITTER;
  m_strTitle = szTitle;
  m_pContext = pContext;
  m_nRows = nRows;
  m_nCols = nCols;
  m_dwStyle = dwStyle;
  checkStyle();
}

// Create a view within a splitter window
CVisualObject::CVisualObject(DWORD dwId, int nRow, int nCol, CCreateContext *pContext, 
                       CRuntimeClass *pClass, CSize size, DWORD dwStyle)
{
  ASSERT(pContext);
  ASSERT(pClass);
  ASSERT(pClass->IsDerivedFrom(RUNTIME_CLASS(CView)) ||
         pClass->IsDerivedFrom(RUNTIME_CLASS(CTabWnd)));

  zeroAll();
  m_dwID = dwId;
  m_nObjectType = OT_SPLITTERVIEW;
  m_pContext = pContext;
  m_pRuntimeClass = pClass;
  m_nRowIndex = nRow;
  m_nColIndex = nCol;
  m_size = size;
  m_dwStyle = dwStyle;
  checkStyle();
}

// Create a splitter within a splitter window
CVisualObject::CVisualObject(DWORD dwId, int nRow, int nCol, int nRows, int nCols, 
                       CCreateContext *pContext, DWORD dwStyle)
{
  ASSERT(pContext);
  ASSERT(nRows);
  ASSERT(nCols);

  zeroAll();
  m_dwID = dwId;
  m_nObjectType = OT_SPLITTERSPLITTER;
  m_pContext = pContext;
  m_nRowIndex = nRow;
  m_nColIndex = nCol;
  m_nRows = nRows;
  m_nCols = nCols;
  m_dwStyle = dwStyle;
  checkStyle();
}

CVisualObject::CVisualObject(const CVisualObject& obj)
{
  zeroAll();
  *this = obj;
}

CVisualObject::~CVisualObject()
{
  if (m_hIcon) {
    ::DestroyIcon(m_hIcon);
    m_hIcon = NULL;
  }
}

CVisualObject& CVisualObject::operator=(const CVisualObject& obj)
{
  // No need to copy m_listObject since it is populated after
  // this code is executed in STL container
  m_nObjectType = obj.m_nObjectType;
  m_dwID = obj.m_dwID;
  m_pWnd = obj.m_pWnd;
  m_pParent = obj.m_pParent;
  m_strTitle = obj.m_strTitle;
  m_nRows = obj.m_nRows;
  m_nCols = obj.m_nCols;
  m_nRowIndex = obj.m_nRowIndex;
  m_nColIndex = obj.m_nColIndex;
  m_pContext = obj.m_pContext;
  m_pRuntimeClass = obj.m_pRuntimeClass;
  m_size = obj.m_size;
  m_bEnabled = obj.m_bEnabled;
  m_dwStyle = obj.m_dwStyle;
  m_cHotKey = obj.m_cHotKey;
  m_hIcon = obj.m_hIcon;
  m_pOwner = obj.m_pOwner;
  m_pFramework = obj.m_pFramework;
  return *this;
}

void CVisualObject::zeroAll(void)
{
  // No need to zero m_listObject since it is already empty
  m_nObjectType = OT_UNKNOWN;
  m_dwID = 0;
  m_pWnd = NULL;
  m_pParent = NULL;
  m_strTitle = _T("");
  m_nRows = 0;
  m_nCols = 0;
  m_nRowIndex = 0;
  m_nColIndex = 0;
  m_pContext = NULL;
  m_pRuntimeClass = NULL;
  m_size = CSize(0,0);
  m_bEnabled = TRUE;
  m_dwStyle = 0;
  m_cHotKey = 0;
  m_pOwner = NULL;
  m_pFramework = NULL;
  m_hIcon = NULL;
}

// Check if style is valid
void CVisualObject::checkStyle(void)
{
  if ((m_dwStyle & TOS_TABTOP) || (m_dwStyle & TOS_TABBOTTOM)) {
    ASSERT(m_pRuntimeClass);
    // Tab position valid only for tab window derived classes
    ASSERT(m_pRuntimeClass->IsDerivedFrom(RUNTIME_CLASS(CTabWnd)));
  }
  if (m_dwStyle & TOS_SELECTED) {
    // Selected valid only for tab panes that are not splitters and tabs
    // In this case, use CVisualFramework::SetActivePane() to set the active pane
    // once the framework is created
    if (m_pRuntimeClass == NULL) {
      // Splitters canot be dynamically create (m_pRuntimeClass is NULL)
      ASSERT((m_nObjectType != OT_SPLITTER) && (m_nObjectType != OT_SPLITTERVIEW));
    } else {
      ASSERT(!m_pRuntimeClass->IsDerivedFrom(RUNTIME_CLASS(CTabWnd)));
    }
  }
}

// Delete the window pointer and optionally destroy the window
void CVisualObject::Destroy(BOOL bDestroyWindow)
{
  if (m_pWnd) {
    if (bDestroyWindow)
      m_pWnd->DestroyWindow();
    delete m_pWnd;
    m_pWnd = NULL;
  }
}

// If this object is a tab window or splitter window that it 
// cannot be focused
BOOL CVisualObject::CanFocus(void)
{
  ASSERT(m_pWnd);

  if (m_pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)) ||
      m_pWnd->IsKindOf(RUNTIME_CLASS(CTabWnd)))
  {
    return FALSE;
  }
  return TRUE;
}

// Set hot key for this tab object
void CVisualObject::SetHotKey(CHAR cHotKey)
{
  m_cHotKey = cHotKey;
}

// Optional: Set description 
void CVisualObject::SetDescription(LPCTSTR szDesc)
{
  m_strDescription = szDesc;
}

// Optional: Set icon for tab and load it from the resource
BOOL CVisualObject::SetIcon(UINT nIconId)
{
  // Cannot specify icon if already specified
  if (m_hIcon)
    return FALSE;

  // Load icon if possible
  if (nIconId == 0) {
    m_hIcon = NULL;
  } else {
    m_hIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), 
                        MAKEINTRESOURCE(nIconId), 
                        IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    if (m_hIcon == NULL)
      return FALSE;
  }
  return TRUE;
}

// Optional: Return icon handle for tab
HICON CVisualObject::GetIcon(void)
{
  return m_hIcon;
}

// Set this object as active pane
BOOL CVisualObject::SetActivePane(void)
{
  ASSERT(m_pFramework);
  return m_pFramework->SetActivePane(this);
}

// Set this tab to be active tab (not the active pane)
BOOL CVisualObject::SetActiveTab(void)
{
  ASSERT(m_pFramework);
  return m_pFramework->SetActiveTab(this);
}

// Enable/disable this object
BOOL CVisualObject::Enable(BOOL bEnable)
{
  ASSERT(m_pFramework);
  return m_pFramework->Enable(this,bEnable);
}

// Enable/disable tab
BOOL CVisualObject::EnableTab(BOOL bEnable)
{
  ASSERT(m_pFramework);
  return m_pFramework->EnableTab(this,bEnable);
}

// SHow/hide this object
BOOL CVisualObject::ShowTab(BOOL bShow)
{
  ASSERT(m_pFramework);
  return m_pFramework->ShowTab(this,bShow);
}

// Is this object enabled
BOOL CVisualObject::IsEnabled(BOOL& bEnabled)
{
  ASSERT(m_pFramework);
  return m_pFramework->IsEnabled(this,bEnabled);
}

// Is this object enabled
BOOL CVisualObject::IsTabEnabled(BOOL& bEnabled)
{
  ASSERT(m_pFramework);
  return m_pFramework->IsTabEnabled(this,bEnabled);
}

// Is this object visible
BOOL CVisualObject::IsTabVisible(BOOL& bVisible)
{
  ASSERT(m_pFramework);
  return m_pFramework->IsTabVisible(this,bVisible);
}

// Returns TRUE if this object is a tab within a tab window
BOOL CVisualObject::IsTabPane(void)
{
  ASSERT(m_pFramework);
  return m_pFramework->IsTabPane(this);
}

// Returns TRUE if this object is a tab window
BOOL CVisualObject::IsTabWindow(void)
{
  ASSERT(m_pFramework);
  return m_pFramework->IsTabWindow(this);
}

// Returns TRUE if this object is a pane within a splitter window
BOOL CVisualObject::IsSplitterPane(void)
{
  ASSERT(m_pFramework);
  return m_pFramework->IsSplitterPane(this);
}

// Returns TRUE if this object is a splitter window
BOOL CVisualObject::IsSplitterWindow(void)
{
  ASSERT(m_pFramework);
  return m_pFramework->IsSplitterWindow(this);
}

// Returns TRUE if this object is derived from CView 
BOOL CVisualObject::IsView(void)
{
  ASSERT(m_pFramework);
  return m_pFramework->IsView(this);
}

//=============================================================================
// class CVisualFramework
//
//=============================================================================

IMPLEMENT_DYNCREATE(CVisualFramework, CCmdTarget)

BEGIN_MESSAGE_MAP(CVisualFramework, CCmdTarget)
	//{{AFX_MSG_MAP(CVisualFramework)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CVisualFramework::CVisualFramework()
{
  m_pOwner = NULL;
  m_bEnableCtrlTab = TRUE;
}

CVisualFramework::~CVisualFramework()
{
  if (m_mapObject.size() && m_listObject.size()) {
    TRACE0(_T(">>> CVisualFramework::Destroy() called in CVisualFramework destructor\n"));
    TRACE0(_T(">>>   It must be called in CFrameWnd derived class OnDestroy() message handler\n"));
    Destroy();
  }
}

// Find an object in the map with the specified unique id
CVisualObject *CVisualFramework::findObject(DWORD dwId)
{
  VISUALOBJECTMAP::iterator iterator;
  for (iterator = m_mapObject.begin(); iterator != m_mapObject.end(); iterator++) {
    if (dwId == iterator->first)
      return iterator->second;
  }
  return NULL;
}

// Find an object in the map with the specified window
CVisualObject *CVisualFramework::findObject(CWnd *pWnd)
{
  VISUALOBJECTMAP::iterator iterator;
  for (iterator = m_mapObject.begin(); iterator != m_mapObject.end(); iterator++) {
    if (pWnd == iterator->second->m_pWnd)
      return iterator->second;
  }
  return NULL;
}

// Add object to the container (this is a root level object)
// There is only one root level object (either splitter or tab)
BOOL CVisualFramework::Add(CVisualObject *pObject)
{
  ASSERT(pObject);
  ASSERT(m_listObject.size() == 0);   // Only one root level object allowed

  // Root level object is either a view, splitter or a tab
  ASSERT((pObject->m_nObjectType == CVisualObject::OT_TAB) || 
         (pObject->m_nObjectType == CVisualObject::OT_VIEW) || 
         (pObject->m_nObjectType == CVisualObject::OT_SPLITTER));

  if (findObject(pObject->m_dwID) == NULL) {
    m_listObject.insert(m_listObject.end(), pObject);
    pObject->m_pFramework = this;
    m_mapObject[pObject->m_dwID] = pObject;
    return TRUE;
  }
  ASSERT(FALSE);    // Duplicate object Id
  return FALSE;
}

// Add child object to the specified object
BOOL CVisualFramework::Add(CVisualObject *pOwner, CVisualObject *pObject)
{
  ASSERT(pObject);

  #ifdef _DEBUG
  // Validate definition
  if (pOwner->m_nObjectType == CVisualObject::OT_TAB) {
    if ((pObject->m_nObjectType != CVisualObject::OT_TABVIEW) && 
        (pObject->m_nObjectType != CVisualObject::OT_SPLITTER) &&
        (pObject->m_nObjectType != CVisualObject::OT_TAB))
    {
      ASSERT(FALSE);
    }
  } else if (pOwner->m_nObjectType == CVisualObject::OT_SPLITTER) {
    if ((pObject->m_nObjectType != CVisualObject::OT_SPLITTERVIEW) && 
        (pObject->m_nObjectType != CVisualObject::OT_SPLITTERSPLITTER))
    {
      ASSERT(FALSE);
    }
  } else if (pOwner->m_nObjectType == CVisualObject::OT_SPLITTERSPLITTER) {
    if ((pObject->m_nObjectType != CVisualObject::OT_SPLITTERVIEW) && 
        (pObject->m_nObjectType != CVisualObject::OT_SPLITTERSPLITTER))
    {
      ASSERT(FALSE);
    }
  } else if (pOwner->m_nObjectType == CVisualObject::OT_TABVIEW) {
    if ((pObject->m_nObjectType != CVisualObject::OT_SPLITTER))
    {
      ASSERT(FALSE);
    }
  }

  #endif

  if (findObject(pObject->m_dwID) == NULL) {
    pOwner->m_listObject.insert(pOwner->m_listObject.end(), pObject);
    pObject->m_pOwner = pOwner;
    pObject->m_pFramework = this;
    m_mapObject[pObject->m_dwID] = pObject;
    return TRUE;
  }
  ASSERT(FALSE);    // Duplicate object Id
  return FALSE;
}

// Create all objects within the framework
BOOL CVisualFramework::Create(CWnd *pWnd)
{
  ASSERT(pWnd);
  ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)));

  // Save owner for later
  m_pOwner = pWnd;

  // Disable Ctrl+Tab for MDI applications
  if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
    m_bEnableCtrlTab = FALSE;

  // Walk thru visual object hierarchy and create windows
  BOOL rc;
  CVisualObject *pObject;
  VISUALOBJECTLIST::iterator it;
  for (it = m_listObject.begin(); it != m_listObject.end(); it++) {
    pObject = *it;
    rc = execCreate(pWnd, pObject);
    if (rc == FALSE) {
      TRACE0(_T("Create visual object failed!\n"));
      return FALSE;
    }
  }
  
  // Walk thru the map and find first object that can be focused
  // Then set focus
  VISUALOBJECTMAP::iterator mapit;
  for (mapit = m_mapObject.begin(); mapit != m_mapObject.end(); mapit ++) {
    pObject = mapit->second;
    if (pObject->CanFocus()) {
      SetActivePane(pObject);
      break;
    }
  }

  return TRUE;
}

// Destroy all objects in the framework
void CVisualFramework::Destroy(void)
{
  CVisualObject *pObject;

  // Recursive delete of all objects
  VISUALOBJECTLIST::iterator it;
  for (it = m_listObject.begin(); it != m_listObject.end(); it++) {
    pObject = *it;
    execDestroy(pObject);
  }

  // Delete pointers in object map
  VISUALOBJECTMAP::iterator mapit;
  for (mapit = m_mapObject.begin(); mapit != m_mapObject.end(); mapit++) {
    pObject = mapit->second;
    delete pObject;
  }

  // Empty all containers (for check in destructor)
  m_mapObject.clear();
  m_listObject.clear();
}

// Recursive function to delete all object windows. Does not delete views since
// they are destroyed by frame.
void CVisualFramework::execDestroy(CVisualObject *pObject)
{
  if (pObject->m_pWnd && ::IsWindow(pObject->m_pWnd->m_hWnd)) {
    if (pObject->m_pWnd->IsKindOf(RUNTIME_CLASS(CTabWnd))) {
      CVisualObject *pObj;
      VISUALOBJECTLIST::iterator it;
      for (it = pObject->m_listObject.begin(); it != pObject->m_listObject.end(); it++) {
        pObj = *it;
        execDestroy(pObj);
      }
      pObject->Destroy(TRUE);
    } else if (pObject->m_pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd))) {
      CVisualObject *pObj;
      VISUALOBJECTLIST::iterator it;
      for (it = pObject->m_listObject.begin(); it != pObject->m_listObject.end(); it++) {
        pObj = *it;
        execDestroy(pObj);
      }
      pObject->Destroy(TRUE);
    } 
  }
}

// Create specified object and all its childs
BOOL CVisualFramework::execCreate(CWnd *pWnd, CVisualObject *pObject)
{
  ASSERT(pWnd);
  ASSERT(pObject);
  
  BOOL rc = FALSE;
  switch (pObject->m_nObjectType) {
  case (CVisualObject::OT_SPLITTER): 
    rc = execCreateSplitter(pWnd,pObject);
    break;
  case (CVisualObject::OT_SPLITTERVIEW): 
    rc = execCreateSplitterView(pWnd, pObject);
    break;
  case (CVisualObject::OT_SPLITTERSPLITTER):
    rc = execCreateSplitterSplitter(pWnd, pObject);
    break;
  case (CVisualObject::OT_TAB):
    rc = execCreateTabWnd(pWnd, pObject);
    break;
  case (CVisualObject::OT_TABVIEW):
    rc = execCreateTabView(pWnd, pObject);
    break;
  case (CVisualObject::OT_VIEW):
    rc = execCreateView(pWnd, pObject);
    break;
  }
  return rc;
}

// Create a simple view 
BOOL CVisualFramework::execCreateView(CWnd *pWnd, CVisualObject *pObject)
{
  ASSERT(pWnd);
  ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)));
  ASSERT(pObject);
  ASSERT(pObject->m_pContext);
  ASSERT(pObject->m_pRuntimeClass);

  CFrameWnd *pFrame = (CFrameWnd*)pWnd;
  pObject->m_pContext->m_pNewViewClass = pObject->m_pRuntimeClass;
  pObject->m_pWnd = pFrame->CreateView(pObject->m_pContext);
  ASSERT(pObject->m_pWnd);
  ASSERT(::IsWindow(pObject->m_pWnd->m_hWnd));
  
  pObject->m_pParent = pFrame;
  setTabWndProperties(pObject);
  
  return TRUE;
}

// Create a view within a tab window
BOOL CVisualFramework::execCreateTabView(CWnd *pWnd, CVisualObject *pObject)
{
  ASSERT(pWnd);
  ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CTabWnd)));
  ASSERT(pObject);
  ASSERT(pObject->m_pContext);
  ASSERT(pObject->m_pRuntimeClass);
  ASSERT(!pObject->m_strTitle.IsEmpty());

  CTabWnd *pTab = (CTabWnd*)pWnd;
  CTabItem *pItem;
  pItem = pTab->CreatePane(pObject->m_strTitle, pObject->m_pRuntimeClass, 
                            pObject->m_pContext);
  pItem->SetIcon(pObject->GetIcon());
  ASSERT(pItem);
  pObject->m_pWnd = pItem->GetSafeWnd();
  ASSERT(pObject->m_pWnd);
  ASSERT(::IsWindow(pObject->m_pWnd->m_hWnd));
  pObject->m_pParent = pTab;
  setTabWndProperties(pObject);
  return TRUE;
}

// Create a splitter window
BOOL CVisualFramework::execCreateSplitter(CWnd *pWnd, CVisualObject *pObject)
{
  ASSERT(pWnd);
  ASSERT(pObject);

  // Cannot use pObject->m_pRuntimeClass->CreateObject() since splitters 
  // do not support dynamic creation
  pObject->m_pWnd = CreateSplitter(pObject->m_dwID);
  ASSERT(pObject->m_pWnd);
  ASSERT(pObject->m_pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)));

  if (pWnd->IsKindOf(RUNTIME_CLASS(CTabWnd))) {
    CTabWnd *pTab = (CTabWnd*)pWnd;
    CTabItem *pItem = pTab->CreatePane(pObject->m_strTitle, pObject->m_nRows, 
                                       pObject->m_nCols, pObject->m_pWnd);
    pItem->SetIcon(pObject->GetIcon());
    ASSERT(::IsWindow(pObject->m_pWnd->m_hWnd));
    pObject->m_pParent = pWnd;
  } else if (pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd))) {
    ((CSplitterWnd*)pObject->m_pWnd)->CreateStatic(pWnd,pObject->m_nRows,pObject->m_nCols);
    ASSERT(::IsWindow(pObject->m_pWnd->m_hWnd));
    pObject->m_pParent = pWnd;
  }
  setTabWndProperties(pObject);
  VISUALOBJECTLIST::iterator it;
  for (it = pObject->m_listObject.begin(); it != pObject->m_listObject.end(); it++) {
    execCreate(pObject->m_pWnd, *it);
  }
  return TRUE;
}

// Create a view within a splitter. Then create all childs of this view.
BOOL CVisualFramework::execCreateSplitterView(CWnd *pWnd, CVisualObject *pObject)
{
  ASSERT(pWnd);
  ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)));
  ASSERT(pObject);
  
  CSplitterWnd *pSplitter = (CSplitterWnd*)pWnd;
  pSplitter->CreateView(pObject->m_nRowIndex, pObject->m_nColIndex, 
                        pObject->m_pRuntimeClass, pObject->m_size, 
                        pObject->m_pContext);
  pObject->m_pWnd = pSplitter->GetPane(pObject->m_nRowIndex, pObject->m_nColIndex);
  ASSERT(pObject->m_pWnd);
  ASSERT(::IsWindow(pObject->m_pWnd->m_hWnd));
  pObject->m_pParent = pSplitter;
  setTabWndProperties(pObject);
  VISUALOBJECTLIST::iterator it;
  for (it = pObject->m_listObject.begin(); it != pObject->m_listObject.end(); it++) {
    execCreate(pObject->m_pWnd, *it);
  }
  return TRUE;
}

// Create a nested splitter window
BOOL CVisualFramework::execCreateSplitterSplitter(CWnd *pWnd, CVisualObject *pObject)
{
  ASSERT(pWnd);
  ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)));
  
  CSplitterWnd *pParent = (CSplitterWnd*)pWnd;
  // Cannot use pObject->m_pRuntimeClass->CreateObject() since splitters 
  // do not support dynamic creation
  pObject->m_pWnd = CreateSplitter(pObject->m_dwID);
  ASSERT(pObject->m_pWnd);
  ASSERT(pObject->m_pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)));
  CSplitterWnd *pSplitter = (CSplitterWnd*)pObject->m_pWnd;
  pSplitter->CreateStatic(pParent, pObject->m_nRows, pObject->m_nCols, 
                          WS_CHILD|WS_VISIBLE|WS_BORDER, 
                          pParent->IdFromRowCol(pObject->m_nRowIndex,pObject->m_nColIndex));
  ASSERT(::IsWindow(pSplitter->m_hWnd));
  pObject->m_pParent = pParent;
  VISUALOBJECTLIST::iterator it;
  for (it = pObject->m_listObject.begin(); it != pObject->m_listObject.end(); it++) {
    execCreate(pObject->m_pWnd, *it);
  }
  return TRUE;
}

// Create a tab window and all its childs (tabs)
BOOL CVisualFramework::execCreateTabWnd(CWnd *pWnd, CVisualObject *pObject)
{
  ASSERT(pWnd);
  ASSERT(pObject);
  
  if (pWnd->IsKindOf(RUNTIME_CLASS(CTabWnd))) {
    CTabWnd *pTab = (CTabWnd*)pWnd;
    CTabItem *pItem = pTab->CreatePane(pObject->m_strTitle, pObject->m_pRuntimeClass,
                                       pObject->m_pContext);
    pItem->SetIcon(pObject->GetIcon());
    ASSERT(pItem);
    pObject->m_pWnd = pItem->GetSafeWnd();
    ASSERT(pObject->m_pWnd);
    ASSERT(::IsWindow(pObject->m_pWnd->m_hWnd));
    pObject->m_pParent = pWnd;
    setTabWndProperties(pObject);
  } else {
    CRect rect;
    pObject->m_pWnd = (CWnd*)pObject->m_pRuntimeClass->CreateObject();
    ASSERT(pObject->m_pWnd);
    pObject->m_pParent = pWnd;
    pObject->m_pWnd->Create(NULL,_T(""),WS_VISIBLE|WS_CHILD,
                                rect,pWnd,TABWND_DEFAULT_ID);
    ASSERT(::IsWindow(pObject->m_pWnd->m_hWnd));
    setTabWndProperties(pObject);
  }
  CVisualObject *pObj;
  VISUALOBJECTLIST::iterator it;
  for (it = pObject->m_listObject.begin(); it != pObject->m_listObject.end(); it++) {
    pObj = *it;
    execCreate(pObject->m_pWnd, pObj);
  }
  return TRUE;
}

// Set properties of tab window
void CVisualFramework::setTabWndProperties(CVisualObject *pObject)
{
  ASSERT(pObject);
  ASSERT(pObject->m_pWnd != NULL);

  // If this is a tab window then set the position of tabs
  if (pObject->m_pWnd->IsKindOf(RUNTIME_CLASS(CTabWnd))) {
    CTabWnd *pTab = (CTabWnd*)pObject->m_pWnd;
    if (pObject->m_dwStyle & CVisualObject::TOS_TABTOP)
      pTab->SetTabPos(CTabWnd::TP_TOP);
    else if (pObject->m_dwStyle & CVisualObject::TOS_TABBOTTOM)
      pTab->SetTabPos(CTabWnd::TP_BOTTOM);
  }
  
  // If this is a pane within a tab then check if this pane
  // should be a selected pane
  ASSERT(pObject->m_pParent);
  if (pObject->m_pParent->IsKindOf(RUNTIME_CLASS(CTabWnd))) {
    CTabWnd *pTab = (CTabWnd*)pObject->m_pParent;
    if (pObject->m_dwStyle & CVisualObject::TOS_SELECTED) {
      int nIndex = pTab->GetTabIndex(pObject->m_pWnd);
      pTab->SetActivePane(nIndex);
    }
  }
}

// Get owner pointer (CFrameWnd derived class)
CWnd *CVisualFramework::GetWnd(void)
{
  return m_pOwner;
}

// Get safe owner pointer (CFrameWnd derived class)
CWnd *CVisualFramework::GetSafeWnd(void)
{
  if (m_pOwner && ::IsWindow(m_pOwner->m_hWnd))
    return m_pOwner;
  return NULL;
}

// Get window associated with the visual object specified with its id
// Can be any object (view, splitter or tab window)
CWnd *CVisualFramework::GetObject(DWORD dwId)
{
  CVisualObject *pObject = findObject(dwId);
  if (pObject == NULL)
    return NULL;
  return pObject->m_pWnd;
}

// Get ID associated with the visual object specified with its window pointer
// Can be any object (view, splitter or tab window)
DWORD CVisualFramework::GetObject(CWnd *pWnd)
{
  ASSERT(pWnd);
  CVisualObject *pObject = findObject(pWnd);
  if (pObject == NULL)
    return NULL;
  return pObject->m_dwID;
}

// Return a visual object with the specified id
CVisualObject *CVisualFramework::Get(DWORD dwId)
{
  return findObject(dwId);
}

// Return a visual object with the specified window
CVisualObject *CVisualFramework::Get(CWnd *pWnd)
{
  return findObject(pWnd);
}

// Returns an object that represents the currently active tab within the
// supplied tab window object. This may not be the active pane
CVisualObject *CVisualFramework::GetActiveTab(CVisualObject *pObject)
{
  ASSERT(pObject);
  ASSERT(pObject->m_pWnd);

  if (!pObject->m_pWnd->IsKindOf(RUNTIME_CLASS(CTabWnd)))
    return NULL;

  CTabWnd *pTab = (CTabWnd*)pObject->m_pWnd;
  int nIndex = pTab->GetTabIndex();
  CWnd *pWnd = pTab->GetTabWnd(nIndex);
  ASSERT(pWnd);
  
  return Get(pWnd);
}

// Set the active tab of the parent tab window. This will not activate the
// pane associated with the active tab.
BOOL CVisualFramework::SetActiveTab(CVisualObject *pObject)
{
  ASSERT(pObject);
  ASSERT(pObject->m_pWnd);
  ASSERT(pObject->m_pParent);

  if (!pObject->m_pParent->IsKindOf(RUNTIME_CLASS(CTabWnd)))
    return FALSE;

  CTabWnd *pTab = (CTabWnd*)pObject->m_pParent;
  int nIndex = pTab->GetTabIndex(pObject->m_pWnd);
  return pTab->SetActivePane(nIndex,FALSE);
}

// Returns TRUE if object is a tab within a tab window
BOOL CVisualFramework::IsTabPane(CVisualObject* pObject)
{
  ASSERT(pObject);
  ASSERT(pObject->m_pWnd);
  ASSERT(pObject->m_pParent);

  if (pObject->m_pParent->IsKindOf(RUNTIME_CLASS(CTabWnd)))
    return TRUE;
  return FALSE;
}

// Returns TRUE if object is a tab window
BOOL CVisualFramework::IsTabWindow(CVisualObject* pObject)
{
  ASSERT(pObject);
  ASSERT(pObject->m_pWnd);

  if (pObject->m_pWnd->IsKindOf(RUNTIME_CLASS(CTabWnd)))
    return TRUE;
  return FALSE;
}

// Returns TRUE if object is a pane within a splitter window
BOOL CVisualFramework::IsSplitterPane(CVisualObject* pObject)
{
  ASSERT(pObject);
  ASSERT(pObject->m_pWnd);
  ASSERT(pObject->m_pParent);

  if (pObject->m_pParent->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
    return TRUE;
  return FALSE;
}

// Returns TRUE if object is a pane within a splitter window
BOOL CVisualFramework::IsSplitterWindow(CVisualObject* pObject)
{
  ASSERT(pObject);
  ASSERT(pObject->m_pWnd);

  if (pObject->m_pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
    return TRUE;
  return FALSE;
}

// Returns TRUE if object is derived from CView
BOOL CVisualFramework::IsView(CVisualObject *pObject)
{
  ASSERT(pObject);
  ASSERT(pObject->m_pWnd);

  if (pObject->m_pWnd->IsKindOf(RUNTIME_CLASS(CView)))
    return TRUE;
  return FALSE;
}

// Get the count of visual objects
int CVisualFramework::GetCount(void)
{
  return (int)m_mapObject.size();
}

// Set font for complete framework
void CVisualFramework::SetFont(CFont *pFont)
{
  ASSERT(pFont);
  
  CVisualObject *pObject;
  VISUALOBJECTMAP::iterator mapit;
  for (mapit = m_mapObject.begin(); mapit != m_mapObject.end(); mapit ++) {
    pObject = mapit->second;
    if (pObject->m_pWnd->IsKindOf(RUNTIME_CLASS(CTabWnd))) {
      ((CTabWnd*)pObject->m_pWnd)->SetFont(pFont);
    } else {
      pObject->m_pWnd->SetFont(pFont);
    }
  }
}

// Enable/disable CtrlTab for tab window
void CVisualFramework::EnableCtrlTab(BOOL bEnable)
{
  // If framework is used in an MDI application, then Ctrl+Tab is used to switch
  // among open windows. If it is enabled, Ctrl+Tab will then switch among 
  // tab panes within the MDI child frame window (this disables default Ctrl+Tab
  // for MDI windows).
  // Ctrl+Tab works only if CWinApp derived class overloads PreTranslateMessage
  // and calles ProcessMessage() of the active CVisualFramework object
  m_bEnableCtrlTab = bEnable;
}

// Since CSplitterWnd does not support dynamic creation, this is a chance for
// derived class to supply CSplitterWnd derived class instead of CSplitterWnd
CSplitterWnd *CVisualFramework::CreateSplitter(DWORD dwId)
{
  return new CSplitterWnd;
}

// Set focus to visual object 
BOOL CVisualFramework::SetActivePane(CVisualObject *pObject)
{
  ASSERT(pObject);

  // Cannot set focus to splitter or tab window
  if (!pObject->CanFocus())
    return FALSE;

  // Cannot set focus to disabled window
  BOOL bEnabled;
  if (pObject->IsEnabled(bEnabled) && !bEnabled)
    return FALSE;

  // Build a list that walks thru the object hierarchy from specified 
  // object to the root
  VISUALOBJECTLIST list;
  CVisualObject *pObj = pObject;
  while (pObj) {
    list.insert(list.end(),pObj);
    pObj = pObj->m_pOwner;
  }
  
  // Reverse the list so that we can walk from root to the desired object
  list.reverse();
  
  // Now, walk thru the list and set focus as desired
  VISUALOBJECTLIST::iterator it;
  for (it = list.begin(); it != list.end(); it ++) {
    pObj = *it;
    if (pObj->m_pOwner && pObj->m_pOwner->m_pWnd->IsKindOf(RUNTIME_CLASS(CTabWnd))) {
      CTabWnd *pTab = (CTabWnd*)pObj->m_pOwner->m_pWnd;
      int nIndex = pTab->GetTabIndex(pObj->m_pWnd);
      if (!pTab->SetActivePane(nIndex)) 
        return FALSE;
    }
  }

  // Update framework owner
  CFrameWnd *pFrame = (CFrameWnd*)m_pOwner;
  pFrame->SetActiveView((CView*)pObject->m_pWnd);

  return TRUE;
}

// Return a pointer to visual object that represents the currently active pane
CVisualObject *CVisualFramework::GetActivePane(void)
{
  CFrameWnd *pFrame = (CFrameWnd*)m_pOwner;
  ASSERT(pFrame);
  CView *pView = pFrame->GetActiveView();
  ASSERT(pView);
  return findObject(pView);
}

// Enable/disable a view. Returns TRUE if sucessful
BOOL CVisualFramework::Enable(CVisualObject *pObject, BOOL bEnable)
{
  ASSERT(pObject);
  ASSERT(pObject->m_pWnd);

  if (pObject->m_pWnd->IsKindOf(RUNTIME_CLASS(CView))) {
    pObject->m_bEnabled = bEnable;
    pObject->m_pWnd->EnableWindow(bEnable);
    if (pObject->m_pParent->IsKindOf(RUNTIME_CLASS(CTabWnd))) {
      CTabWnd *pTab = (CTabWnd*)pObject->m_pParent;
      int nIndex = pTab->GetTabIndex(pObject->m_pWnd);
      pTab->Enable(nIndex, bEnable);
    }
    return TRUE;
  }
  return FALSE;
}

// Enable/disable a tab
BOOL CVisualFramework::EnableTab(CVisualObject *pObject, BOOL bEnable)
{
  ASSERT(pObject);
  ASSERT(pObject->m_pWnd);
  ASSERT(pObject->m_pParent);

  // Check if parent is a tab window
  if (pObject->m_pParent->IsKindOf(RUNTIME_CLASS(CTabWnd))) {
    CTabWnd *pTab = (CTabWnd*)pObject->m_pParent;
    int nIndex = pTab->GetTabIndex(pObject->m_pWnd);
    if (nIndex == pTab->GetTabIndex())
      return FALSE;
    pTab->EnableTab(nIndex, bEnable);
    pObject->m_bEnabled = bEnable;
    return TRUE;
  }
  return FALSE;
}

// Show/hide a tab
BOOL CVisualFramework::ShowTab(CVisualObject *pObject, BOOL bShow)
{
  ASSERT(pObject);
  ASSERT(pObject->m_pWnd);
  ASSERT(pObject->m_pParent);

  if (!pObject->m_pParent->IsKindOf(RUNTIME_CLASS(CTabWnd)))
    return FALSE;

  CTabWnd *pTab = (CTabWnd*)pObject->m_pParent;
  int nIndex = pTab->GetTabIndex(pObject->m_pWnd);
  if (nIndex == pTab->GetTabIndex())
    return FALSE;

  pTab->ShowTab(nIndex, bShow);
  return TRUE;
}

// Is object enabled. Returns FALSE if this is not a valid call for the supplied
// object. If return code is TRUE, check bEnabled
BOOL CVisualFramework::IsEnabled(CVisualObject *pObject, BOOL& bEnabled)
{
  ASSERT(pObject);
  ASSERT(pObject->m_pWnd);

  bEnabled = pObject->m_bEnabled;
  return TRUE;
  /*
  if (pObject->m_pWnd->IsKindOf(RUNTIME_CLASS(CView))) {
    bEnabled = pObject->m_pWnd->IsWindowEnabled();
    return TRUE;
  }
  return FALSE;
  */
}

// Is tab enabled. Returns FALSE if this is not a valid call for the supplied
// object. If return code is TRUE, check bEnabled
BOOL CVisualFramework::IsTabEnabled(CVisualObject *pObject, BOOL& bEnabled)
{
  ASSERT(pObject);
  ASSERT(pObject->m_pWnd);
  ASSERT(pObject->m_pParent);

  if (pObject->m_pParent->IsKindOf(RUNTIME_CLASS(CTabWnd))) {
    CTabWnd *pTab = (CTabWnd*)pObject->m_pParent;
    int nIndex = pTab->GetTabIndex(pObject->m_pWnd);
    bEnabled = pTab->IsTabEnabled(nIndex);
    return TRUE;
  }
  return FALSE;
}

// Is tab visible. Returns FALSE if this is not a valid call for the supplied
// object. If return code is TRUE then check bVisible.
BOOL CVisualFramework::IsTabVisible(CVisualObject *pObject, BOOL& bVisible)
{
  ASSERT(pObject);
  ASSERT(pObject->m_pWnd);
  ASSERT(pObject->m_pParent);

  if (pObject->m_pParent->IsKindOf(RUNTIME_CLASS(CTabWnd))) {
    CTabWnd *pTab = (CTabWnd*)pObject->m_pParent;
    int nIndex = pTab->GetTabIndex(pObject->m_pWnd);
    bVisible = pTab->IsTabVisible(nIndex);
    return TRUE;
  }
  return FALSE;
}

// This should be called from CWinApp derived PreTranslateMessage to handle
// any framework related messages
BOOL CVisualFramework::ProcessMessage(MSG *pMsg)
{
  ASSERT(pMsg);
  if (pMsg->message == WM_KEYDOWN) {
    // Handle Ctrl+Tab for tab windows
    if (m_bEnableCtrlTab) {
      if ((pMsg->wParam == VK_TAB) && (::GetAsyncKeyState(VK_CONTROL) != 0)) {
        CWnd *pWnd = CWnd::FromHandle(pMsg->hwnd);
        ASSERT(pWnd);
        if (pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
          return FALSE;
        // If we are in form view then pWnd is a control
        while (pWnd && !pWnd->IsKindOf(RUNTIME_CLASS(CView)))
          pWnd = pWnd->GetParent();
        // Find object for this window
        ASSERT(pWnd);
        CVisualObject *pObject = findObject(pWnd);
        ASSERT(pObject != NULL);
        while (pObject && !pObject->m_pWnd->IsKindOf(RUNTIME_CLASS(CTabWnd)))
          pObject = pObject->m_pOwner;
        if (pObject) {
          CTabWnd *pTab = (CTabWnd*)pObject->m_pWnd;
          BOOL bShift = (::GetAsyncKeyState(VK_SHIFT) != 0);
          int nIndex = pTab->GetTabIndex();
          int nNdx = nIndex;
          // Switch to new pane (skip invisible and disabled)
          do {
            if (bShift) {
              // Does not work
              nNdx--;
              if (nNdx < 0)
                nNdx = pTab->GetTabCount()-1;
            } else {
              nNdx ++;
              if (nNdx == pTab->GetTabCount())
                nNdx = 0;
            }
          } while (!pTab->SetActivePane(nNdx) && (nNdx != nIndex));
          return TRUE;
        }
      }
    }
  } else if (pMsg->message == WM_SYSKEYDOWN) {
    // Handle hot keys for views (if defined)
    CVisualObject *pObject;
    VISUALOBJECTMAP::iterator mapit;
    for (mapit = m_mapObject.begin(); mapit != m_mapObject.end(); mapit ++) {
      pObject = mapit->second;
      if (pObject->m_cHotKey == pMsg->wParam) {
        if (SetActivePane(pObject))
          return TRUE;
      }
    }
  }

  return FALSE;
}

BOOL CVisualFramework::OnCmdMsg(UINT nID, int nCode, void* pExtra, 
                             AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CCmdTarget::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

