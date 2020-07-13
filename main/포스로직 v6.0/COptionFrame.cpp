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
END_MESSAGE_MAP()


// COptionFrame 메시지 처리기


BOOL COptionFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CMDIChildWnd::PreCreateWindow(cs))
		return FALSE;

	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	cs.style &= ~WS_THICKFRAME & ~WS_MAXIMIZEBOX & ~(LONG)FWS_ADDTOTITLE; //크기조절 기능 비활성화 
	cs.cx = pMain->c_Option.cx;
	cs.cy = pMain->c_Option.cy;

	return TRUE;
}






int COptionFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CMenu* p_menu = this->GetSystemMenu(FALSE); 
	p_menu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);  // X 무효화

	return 0;
}