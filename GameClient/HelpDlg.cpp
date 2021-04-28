// HelpDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GameClient.h"
#include "HelpDlg.h"
#include ".\helpdlg.h"


// CHelpDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CHelpDlg, CDialog)
CHelpDlg::CHelpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHelpDlg::IDD, pParent)
{
}

CHelpDlg::~CHelpDlg()
{
}

void CHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COLMYUSER, m_ctColMyUser);
}


BEGIN_MESSAGE_MAP(CHelpDlg, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CHelpDlg 메시지 처리기입니다.

BOOL CHelpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CHelpDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.
}
