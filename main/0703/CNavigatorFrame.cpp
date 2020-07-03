// CNavigatorFrame.cpp: 구현 파일
//

#include "pch.h"
#include "MFCparam.h"
#include "CNavigatorFrame.h"


// CNavigatorFrame

IMPLEMENT_DYNCREATE(CNavigatorFrame, CMDIChildWnd)

CNavigatorFrame::CNavigatorFrame()
{

}

CNavigatorFrame::~CNavigatorFrame()
{
}


BEGIN_MESSAGE_MAP(CNavigatorFrame, CMDIChildWnd)

	ON_WM_CREATE()
	//ON_WM_SIZE()
END_MESSAGE_MAP()


// CNavigatorFrame 메시지 처리기

int CNavigatorFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	CMenu* p_menu = this->GetSystemMenu(FALSE);
	p_menu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);  // X 무효화
	return 0;
}


BOOL CNavigatorFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	cs.style &= ~(LONG)FWS_ADDTOTITLE;
	cs.lpszName = _T("Navigator");
	return CMDIChildWnd::PreCreateWindow(cs);
}


//void CNavigatorFrame::OnSize(UINT nType, int cx, int cy)
//{
//	CMDIChildWnd::OnSize(nType, cx, cy);
//
//	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
//	if (pMain->c_Navi.cx != cx)
//		pMain->s_Navi = TRUE;
//
//	pMain->s_Option = FALSE;
//	pMain->s_Viewer = FALSE;
//	//if (pMain->s_Navi)
//	//{
//
//	//	pMain->c_Navi.cx = cx;
//	//	pMain->c_Navi.cy = cy;
//
//
//	//	pMain->c_Option.cx = pMain->m_rect.right - (cx + 15);
//
//	//	pMain->c_Navi.cx = pMain->m_rect.right - (cx + 15);
//
//
//
//	//	theApp.pOptionFrame->SetWindowPos(this, pMain->c_Option.sx, pMain->c_Option.sy, pMain->c_Option.cx, pMain->c_Option.cy, SWP_NOREDRAW | SWP_NOZORDER | SWP_NOACTIVATE);
//	//	theApp.pNavigatorFrame->SetWindowPos(this, pMain->c_Navi.sx, pMain->c_Navi.sy, pMain->c_Navi.cx, pMain->c_Navi.cy, SWP_NOREDRAW | SWP_NOZORDER | SWP_NOACTIVATE);
//	//}
//}
