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
	ON_WM_PAINT()
	ON_WM_SIZE()
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


void CNavigatorView::OnPaint()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CPaintDC dc(this); 
	//m_background = pMain->pImgViewerView->m_background;


	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CDC* pDC = GetDC();
	CBitmap* oldbitmap, * oldbitmap2;	//원본 오브젝트 저장

	//

	if(asd != false)
	{
		memDC.CreateCompatibleDC(pDC);
		mdcOffScreen.CreateCompatibleDC(pDC);

		bmpOffScreen.CreateCompatibleBitmap(pDC, clientWidth, clientHeight);

		oldbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);
		oldbitmap2 = memDC.SelectObject(&m_background);

		mdcOffScreen.SetStretchBltMode(HALFTONE);
		mdcOffScreen.StretchBlt(0, 0, clientWidth, clientHeight, &memDC,
			0, 0, bmpWidth, bmpHeight, SRCCOPY);

		pDC->SetStretchBltMode(HALFTONE);
		pDC->StretchBlt(0, 0, clientWidth, clientHeight, &mdcOffScreen,
			0, 0, clientWidth, clientHeight, SRCCOPY);

		memDC.SelectObject(oldbitmap2);
		memDC.DeleteDC();
		mdcOffScreen.SelectObject(oldbitmap);
		mdcOffScreen.DeleteDC();
		bmpOffScreen.DeleteObject();
		oldbitmap->DeleteObject();
		oldbitmap2->DeleteObject();
	}
}


void CNavigatorView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	clientWidth = cx;
	clientHeight = cy;
}


void CNavigatorView::OnFileOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CImgViewerView* pView = pMain->pImgViewerView;

	asd = true;
	m_background.Detach();
	m_background.Attach(pMain->pImgViewerView->m_background);
	m_background.GetObject(sizeof(BITMAP), &bmp);

	bmpWidth = bmp.bmWidth;
	bmpHeight = bmp.bmHeight;

	Invalidate();
}
