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
