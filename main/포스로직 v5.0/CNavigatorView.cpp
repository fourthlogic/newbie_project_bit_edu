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
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CNavigatorView 그리기

void CNavigatorView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
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
	CPaintDC dc(this);

	CDC* pDC = GetDC();
	CBitmap* oldbitmap, * oldbitmap2;	//원본 오브젝트 저장

	if (theApp.pImgViewerView->m_Algorithm.isReady())
	{
		memDC.CreateCompatibleDC(pDC);
		mdcOffScreen.CreateCompatibleDC(pDC);

		bmpOffScreen.CreateCompatibleBitmap(pDC, clientWidth, clientHeight);

		oldbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);
		oldbitmap2 = memDC.SelectObject(&m_background);


		if ((bmpWidth * ((float)clientHeight / (float)bmpHeight)) <= clientWidth)
		{
			mdcOffScreen.SetStretchBltMode(HALFTONE);
			mdcOffScreen.StretchBlt(clientWidth / 2 - (bmpWidth * ((float)clientHeight / (float)bmpHeight)) / 2, 0, bmpWidth * ((float)clientHeight / (float)bmpHeight), clientHeight, &memDC,
				0, 0, bmpWidth, bmpHeight, SRCCOPY);

			pDC->SetStretchBltMode(HALFTONE);
			pDC->StretchBlt(0, 0, clientWidth, clientHeight, &mdcOffScreen,
				0, 0, clientWidth, clientHeight, SRCCOPY);
		}
		else
		{
			mdcOffScreen.SetStretchBltMode(HALFTONE);
			mdcOffScreen.StretchBlt(0, clientHeight / 2 - (bmpHeight * ((float)clientWidth / (float)bmpWidth)) / 2, clientWidth, bmpHeight * ((float)clientWidth / (float)bmpWidth), &memDC,
				0, 0, bmpWidth, bmpHeight, SRCCOPY);

			pDC->SetStretchBltMode(HALFTONE);
			pDC->StretchBlt(0, 0, clientWidth, clientHeight, &mdcOffScreen,
				0, 0, clientWidth, clientHeight, SRCCOPY);
		}

		CBrush brush;
		CPen hpen(PS_SOLID, 1, RGB(255, 255, 0));
		CPen* oldPen = pDC->SelectObject(&hpen);
		brush.CreateStockObject(NULL_BRUSH);
		CBrush* pOldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect);
		pDC->SelectObject(oldPen);

		memDC.SelectObject(oldbitmap2);
		memDC.DeleteDC();
		mdcOffScreen.SelectObject(oldbitmap);

		mdcOffScreen.DeleteDC();
		bmpOffScreen.DeleteObject();
		hpen.DeleteObject();

		oldbitmap->DeleteObject();
		oldbitmap2->DeleteObject();

		isNavigator_Paint = true;
	}
}


void CNavigatorView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	clientWidth = cx;
	clientHeight = cy;

	if (isNavigator_Paint == true)
	{
		DrawNavigatorRect();
	}
}


void CNavigatorView::OnFileOpen()
{
	m_background.Detach();
	m_background.Attach(theApp.pImgViewerView->m_background);
	m_background.GetObject(sizeof(BITMAP), &bmp);

	bmpWidth = bmp.bmWidth;
	bmpHeight = bmp.bmHeight;

	Invalidate();
}

void CNavigatorView::GetRectPos(float x, float y, float Width, float Height)
{
	imgViewer_x = x;
	imgViewer_y = y;
	imgViewer_Width = Width;
	imgViewer_Height = Height;

	DrawNavigatorRect();
}

void CNavigatorView::DrawNavigatorRect()
{
	//세로가 짧을때
	if ((bmpWidth * ((float)clientHeight / (float)bmpHeight)) <= clientWidth)
	{
		Rect_x = (bmpWidth * ((float)clientHeight / (float)bmpHeight)) * (imgViewer_x / bmpWidth) + (clientWidth / 2 - (bmpWidth * ((float)clientHeight / (float)bmpHeight)) / 2);
		Rect_y = clientHeight * (imgViewer_y / bmpHeight);
		Rect_Width = (bmpWidth * ((float)clientHeight / (float)bmpHeight)) * (imgViewer_Width / bmpWidth);
		Rect_Height = clientHeight * (imgViewer_Height / bmpHeight);
	}

	//가로가 짧을때
	else
	{
		Rect_x = clientWidth * (imgViewer_x / bmpWidth);
		Rect_y = (bmpHeight * ((float)clientWidth / (float)bmpWidth)) * (imgViewer_y / bmpHeight) + (clientHeight / 2 - (bmpHeight * ((float)clientWidth / (float)bmpWidth)) / 2);
		Rect_Width = clientWidth * (imgViewer_Width / bmpWidth);
		Rect_Height = (bmpHeight * ((float)clientWidth / (float)bmpWidth)) * (imgViewer_Height / bmpHeight);
	}


	rect.left = Rect_x;
	rect.top = Rect_y;
	rect.right = Rect_x + Rect_Width;
	rect.bottom = Rect_y + Rect_Height;

	Invalidate(false);
	UpdateWindow();
}

BOOL CNavigatorView::OnEraseBkgnd(CDC* pDC)
{
	if (isNavigator_Paint != true)
	{
		CBrush backBrush(RGB(255, 255, 255));

		CBrush* pOldBrush = pDC->SelectObject(&backBrush);
		CRect rect; pDC->GetClipBox(&rect);
		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
		pDC->SelectObject(pOldBrush);

		backBrush.DeleteObject();
	}

	Invalidate(false);
	UpdateWindow();

	return false;
}


void CNavigatorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_sPt = point;
	Navigator2imgViewer();

	SetCapture();

	CView::OnLButtonDown(nFlags, point);
}

void CNavigatorView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (nFlags & MK_LBUTTON)
	{
		m_sPt = point;
		Navigator2imgViewer();
	}

	CView::OnMouseMove(nFlags, point);
}

void CNavigatorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_sPt = point;
	Navigator2imgViewer();

	ReleaseCapture();

	CView::OnLButtonUp(nFlags, point);
}

void CNavigatorView::Navigator2imgViewer()
{
	if ((bmpWidth * ((float)clientHeight / (float)bmpHeight)) <= clientWidth)
	{
		client_x = (m_sPt.x - ((float)clientWidth / 2 - (bmpWidth * ((float)clientHeight / (float)bmpHeight)) / 2)) / ((float)clientWidth - 2 * ((float)clientWidth / 2 - (bmpWidth * ((float)clientHeight / (float)bmpHeight)) / 2));
		client_y = m_sPt.y / (float)clientHeight;
	}
	else
	{
		client_x = m_sPt.x / (float)clientWidth;
		client_y = (m_sPt.y - ((float)clientHeight / 2 - (bmpHeight * ((float)clientWidth / (float)bmpWidth)) / 2)) / ((float)clientHeight - 2 * ((float)clientHeight / 2 - (bmpHeight * ((float)clientWidth / (float)bmpWidth)) / 2));
	}

	theApp.pImgViewerView->GetImgPos(client_x, client_y);
}


void CNavigatorView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)); //icon 변경
	GetParent()->SetIcon(hIcon, FALSE); //icon 셋팅
}
