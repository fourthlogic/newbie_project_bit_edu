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
END_MESSAGE_MAP()


// COptionFrame 메시지 처리기


BOOL COptionFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CMDIChildWnd::PreCreateWindow(cs);
}




