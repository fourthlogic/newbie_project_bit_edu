
// ChildFrm.cpp: CChildFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "MFCparam.h"

#include "CImgViewerFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CImgViewerFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CImgViewerFrame, CMDIChildWnd)
END_MESSAGE_MAP()

// CChildFrame 생성/소멸

CImgViewerFrame::CImgViewerFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CImgViewerFrame::~CImgViewerFrame()
{
}


BOOL CImgViewerFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서 Window 클래스 또는 스타일을 수정합니다.
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
