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
	GetParent()->SetWindowText(_T("네비"));
	return 0;
}
