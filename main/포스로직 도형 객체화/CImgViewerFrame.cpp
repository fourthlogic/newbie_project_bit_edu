
// ChildFrm.cpp: CChildFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "MFCparam.h"

#include "CImgViewerFrame.h"


// CChildFrame

IMPLEMENT_DYNCREATE(CImgViewerFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CImgViewerFrame, CMDIChildWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CChildFrame 생성/소멸

CImgViewerFrame::CImgViewerFrame() noexcept
{
}

CImgViewerFrame::~CImgViewerFrame()
{
}


BOOL CImgViewerFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;
	cs.style &= ~(LONG)FWS_ADDTOTITLE;

	return TRUE;
}

// CChildFrame 진단

#ifdef _DEBUG
void CImgViewerFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CImgViewerFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}
#endif //_DEBUG

// CChildFrame 메시지 처리기


int CImgViewerFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CMenu* p_menu = this->GetSystemMenu(FALSE);
	p_menu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED); // X 무효화

	return 0;
}