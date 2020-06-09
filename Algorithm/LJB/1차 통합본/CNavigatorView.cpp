// CNavigatorView.cpp: 구현 파일
//

#include "pch.h"
#include "MFCparam.h"
#include "CNavigatorView.h"


// CNavigatorView

IMPLEMENT_DYNCREATE(CNavigatorView, CView)

CNavigatorView::CNavigatorView()
{

}

CNavigatorView::~CNavigatorView()
{
}

BEGIN_MESSAGE_MAP(CNavigatorView, CView)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_COMMAND(ID_FILE_OPEN, &CNavigatorView::OnFileOpen)
END_MESSAGE_MAP()


// CNavigatorView 그리기

void CNavigatorView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// CNavigatorView 진단

#ifdef _DEBUG
void CNavigatorView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CNavigatorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CNavigatorView 메시지 처리기


int CNavigatorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.


	return 0;
}


void CNavigatorView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CView::OnPaint()을(를) 호출하지 마십시오.
}


void CNavigatorView::OnFileOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CImgViewerView* pView = pMain->pImgViewerView;

}
