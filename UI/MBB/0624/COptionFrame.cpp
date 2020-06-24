// COptionFrame.cpp: 구현 파일
//

#include "pch.h"
#include "MFCparam.h"
#include "COptionFrame.h"

// COptionFrame

IMPLEMENT_DYNCREATE(COptionFrame, CMDIChildWnd)

COptionFrame::COptionFrame()
{

}

COptionFrame::~COptionFrame()
{
}


BEGIN_MESSAGE_MAP(COptionFrame, CMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// COptionFrame 메시지 처리기


BOOL COptionFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (!CMDIChildWnd::PreCreateWindow(cs))
		return FALSE;

	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	cs.style &= ~WS_THICKFRAME & ~WS_MAXIMIZEBOX; //크기조절 기능 비활성화 
	cs.cx = pMain->c_Option.cx;
	cs.cy = pMain->c_Option.cy;

	return TRUE;
	
	
	//cs.style &= ~(LONG)FWS_ADDTOTITLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	//cs.style &= ~(LONG)FWS_ADDTOTITLE;

	//return CMDIChildWnd::PreCreateWindow(cs);
}






int COptionFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	CMenu* p_menu = this->GetSystemMenu(FALSE); 
	p_menu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);  // X 무효화

	return 0;
}


	//CMDIChildWnd::OnSize(nType, cx, cy);

	//CMainFrame * pMain = (CMainFrame*)AfxGetMainWnd();

	//pMain->c_Option.sx = 0;
	//pMain->c_Option.sy = 0;
	//pMain->c_Option.cx = cx;
	//pMain->c_Option.cy = cy;

	//pMain->c_Navi.sx = pMain->c_Option.sx;
	//pMain->c_Navi.sy = cy + 38;
	//pMain->c_Navi.cx = cx + 15;
	//pMain->c_Navi.cy = pMain->m_rect.bottom - cy;

	//pMain->c_ImgView.sx = cx + 15;
	//pMain->c_ImgView.sy = 0;
	//pMain->c_ImgView.cx = pMain->m_rect.right - (cx + 15);
	//pMain->c_ImgView.cy = pMain->m_rect.bottom;

	//if(pMain->start)
	//	pMain->OnSize(1, pMain->m_rect.right, pMain->m_rect.bottom);


	//if (pMain->c_Option.cx != cx)
	//	pMain->s_Option = TRUE;

	//pMain->s_Navi = FALSE;
	//pMain->s_Viewer = FALSE;

	//if (theApp.pNavigatorFrame != nullptr && theApp.pImgViewerFrame != nullptr && pMain->s_Option)
	//{
	//	//HDWP hdwp = ::BeginDeferWindowPos(2);
	//	//::DeferWindowPos(hdwp, theApp.pNavigatorFrame->GetSafeHwnd(), HWND_TOP
	//	//	, 0, cy + 38, cx + 15, pMain->m_rect.bottom - cy
	//	//	, SWP_NOZORDER | SWP_SHOWWINDOW);
	//	//::DeferWindowPos(hdwp, theApp.pImgViewerFrame->GetSafeHwnd(), HWND_TOP
	//	//	, cx + 15, 0, pMain->m_rect.right - (cx + 15), pMain->m_rect.bottom
	//	//	, SWP_NOZORDER | SWP_SHOWWINDOW);
	//	//::EndDeferWindowPos(hdwp);
	//	//theApp.pNavigatorFrame->MoveWindow(0, cy+38, cx+15, pMain->m_rect.bottom - cy, 0);
	//	//theApp.pImgViewerFrame->MoveWindow(cx+15, 0, pMain->m_rect.right - (cx + 15), pMain->m_rect.bottom, 0);


	//	pMain->c_Option.sx = 0;
	//	pMain->c_Option.sy = 0;
	//	pMain->c_Option.cx = cx;
	//	pMain->c_Option.cy = cy;

	//	pMain->c_Navi.sx = pMain->c_Option.sx;
	//	pMain->c_Navi.sy = cy + 38;
	//	pMain->c_Navi.cx = cx + 15;
	//	pMain->c_Navi.cy = pMain->m_rect.bottom - cy;

	//	pMain->c_ImgView.sx = cx + 15;
	//	pMain->c_ImgView.sy = 0;
	//	pMain->c_ImgView.cx = pMain->m_rect.right - (cx + 15);
	//	pMain->c_ImgView.cy = pMain->m_rect.bottom;
	//	//theApp.pNavigatorFrame->MoveWindow(pMain->c_Navi.sx, pMain->c_Navi.sy, pMain->c_Navi.cx, pMain->c_Navi.cy, 0);
	//	//theApp.pImgViewerFrame->MoveWindow(pMain->c_ImgView.sx, pMain->c_ImgView.sy, pMain->c_ImgView.cx, pMain->c_ImgView.cy, 0);

	//	theApp.pNavigatorFrame->SetWindowPos(this, pMain->c_Navi.sx, pMain->c_Navi.sy, pMain->c_Navi.cx, pMain->c_Navi.cy, SWP_NOREDRAW | SWP_NOZORDER| SWP_NOACTIVATE);
	//	theApp.pImgViewerFrame->SetWindowPos(this, pMain->c_ImgView.sx, pMain->c_ImgView.sy, pMain->c_ImgView.cx, pMain->c_ImgView.cy, SWP_NOREDRAW | SWP_NOZORDER | SWP_NOACTIVATE);
	//}


void COptionFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIChildWnd::OnSize(nType, cx, cy);

	
}
