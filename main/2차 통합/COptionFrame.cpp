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
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	cs.style &= ~(LONG)FWS_ADDTOTITLE;

	return CMDIChildWnd::PreCreateWindow(cs);
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
