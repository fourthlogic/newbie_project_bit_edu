
// MFCparamView.cpp: CMFCparamView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MFCparam.h"
#endif

#include "CImgViewerDoc.h"
#include "CImgViewerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCparamView

IMPLEMENT_DYNCREATE(CImgViewerView, CView)

BEGIN_MESSAGE_MAP(CImgViewerView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_MOUSEHWHEEL()
	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_PAINT()
	ON_COMMAND(ID_FILE_OPEN, &CImgViewerView::OnFileOpen)
END_MESSAGE_MAP()

// CMFCparamView 생성/소멸

CImgViewerView::CImgViewerView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.
}

CImgViewerView::~CImgViewerView()
{
}

BOOL CImgViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

	return CView::PreCreateWindow(cs);
}

// CMFCparamView 그리기

void CImgViewerView::OnDraw(CDC* /*pDC*/)
{
	CImgViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}

// CMFCparamView 진단

#ifdef _DEBUG
void CImgViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CImgViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImgViewerDoc* CImgViewerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImgViewerDoc)));
	return (CImgViewerDoc*)m_pDocument;
}
#endif //_DEBUG

// CMFCparamView 메시지 처리기

void CImgViewerView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	GetParent()->SetWindowText(_T("이미지 뷰"));

}


void CImgViewerView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnMouseMove(nFlags, point);
	m_pos = point;
	int i = 0;

	if (nFlags & MK_MBUTTON)
	{
		// A영역의 사각형 그리기 //m_sPt는 기존 포인트, point는 이동
		if (m_sPt.x < point.x) // 오른쪽으로 끌었을때
		{
			m_ePt.x -= point.x - m_sPt.x;

			if (m_ePt.x <= 0)
			{
				if (z_pos.x > 0)
				{
					i = 1 + ((0 - m_ePt.x) / (int)PWidth); //원본에서 움직여야되는 픽셀 수
					z_pos.x -= i;
					m_ePt.x += i * PWidth;
				}
			}
		}
		else // 왼쪽으로 끌었을때
		{
			m_ePt.x += m_sPt.x - point.x;

			if (m_ePt.x + m_bgRect.right - (m_Bitmap.bmWidth + PWidth) >= 0)
			{
				i = 1 + (m_ePt.x + m_bgRect.right - (m_Bitmap.bmWidth + PWidth)) / (int)PWidth;
				z_pos.x += i;
				m_ePt.x -= i * PWidth;
			}
		}
		// B영역의 사각형 그리기
		if (m_sPt.y > point.y) // 위로 올렸을때
		{
			m_ePt.y += m_sPt.y - point.y;

			if (m_ePt.y + m_bgRect.bottom - (m_Bitmap.bmHeight + PHeight) >= 0)
			{
				i = 1 + (m_ePt.y + m_bgRect.bottom - m_Bitmap.bmHeight + PHeight) / (int)PHeight;
				z_pos.y += i;
				m_ePt.y -= i * PHeight;
			}

		}
		else // 아래로 내렸을때
		{
			m_ePt.y -= point.y - m_sPt.y;
			if (m_ePt.y < 0)
			{
				i = 1 + ((0 - m_ePt.y) / (int)PHeight);
				z_pos.y -= i;
				m_ePt.y += i * PHeight;
			}
		}

		if (m_ePt.x < 0)
			m_ePt.x = 0;
		if (m_ePt.y < 0)
			m_ePt.y = 0;

		m_sPt = point;
		Invalidate(FALSE);
		UpdateWindow();
	}

	//if (MK_LBUTTON && drawID == TRUE) // 마우스를 클릭하여 드래그일 동안만 
	//{
	//	pts.x = (z_pos.x + (m_ePt.x + point.x) / PWidth) + 1;
	//	pts.y = (z_pos.y + (m_ePt.y + point.y) / PHeight) + 1;
	//	draw(pts); // 뷰에서 도형 그리기(draw) 함수 호출		
	//	// 마우스 드래그시의 좌표 값을 도형 끝 값에 다시 저장	
	//	//Invalidate();

	//	mov_x = (z_pos.x + (m_ePt.x + point.x) / PWidth) + 1;
	//	mov_y = (z_pos.y + (m_ePt.y + point.y) / PHeight) + 1;

	//	Invalidate();
	//}
}


void CImgViewerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_bgRect.left = 0;
	m_bgRect.top = 0;
	m_bgRect.right = cx;
	m_bgRect.bottom = cy;

	Invalidate();
}

BOOL CImgViewerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (zDelta <= 0)// 휠 내릴때
	{
		m_Zoom = 1.25f;
	}
	else// 휠 올릴때
	{
		m_Zoom = 0.8f;
	}

	//if(zoomWidth *= m_Zoom)
	z_pos.x += round(((((float)m_ePt.x + (float)m_pos.x) / (float)(PWidth * ((int)zoomWidth + 2))) * zoomWidth) - ((((float)m_ePt.x + (float)m_pos.x) / (float)(PWidth * ((int)zoomWidth + 2))) * (zoomWidth * m_Zoom)));
	z_pos.y += round(((((float)m_ePt.y + (float)m_pos.y) / (float)(PHeight * ((int)zoomHeight + 2))) * zoomHeight) - ((((float)m_ePt.y + (float)m_pos.y) / (float)(PHeight * ((int)zoomHeight + 2))) * (zoomHeight * m_Zoom)));

	if (m_ePt.x + m_bgRect.right > m_Bitmap.bmWidth + PWidth)
	{
		z_pos.x += 1;
		m_ePt.x -= PWidth;
	}

	if (m_ePt.y + m_bgRect.bottom > m_Bitmap.bmHeight + PHeight)
	{
		z_pos.y += 1;
		m_ePt.y -= PHeight;
	}

	zoomWidth *= m_Zoom;
	zoomHeight *= m_Zoom;

	PWidth = m_Bitmap.bmWidth / zoomWidth;
	PHeight = m_Bitmap.bmHeight / zoomHeight;

	Invalidate(FALSE);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CImgViewerView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnMButtonDown(nFlags, point);
	SetCapture();
	m_sPt = point;
}


void CImgViewerView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnMButtonUp(nFlags, point);
	ReleaseCapture();
}


void CImgViewerView::OnPaint()
{
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CView::OnPaint()을(를) 호출하지 마십시오.
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.
	CDC* pDC = GetDC();
	CString str;
	str.Format(_T("X = %d, Y = %d"), m_pos.x, m_pos.y);
	dc.TextOut(m_pos.x, m_pos.y, str);

	memDC.CreateCompatibleDC(pDC);
	mdcOffScreen.CreateCompatibleDC(pDC);

	bmpOffScreen.CreateCompatibleBitmap(pDC, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CXFULLSCREEN));

	oldbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);

	memDC.SelectObject(&m_background);

	mdcOffScreen.SetStretchBltMode(COLORONCOLOR);
	mdcOffScreen.StretchBlt(0, 0, PWidth * ((int)zoomWidth + 2), PHeight * ((int)zoomHeight + 2), &memDC,
		z_pos.x, z_pos.y, zoomWidth + 2, zoomHeight + 2, SRCCOPY);

	//PrintText(&mdcOffScreen);

	pDC->SetStretchBltMode(COLORONCOLOR);
	pDC->StretchBlt(0, 0, m_bgRect.right, m_bgRect.bottom, &mdcOffScreen,
		m_ePt.x, m_ePt.y, m_bgRect.right, m_bgRect.bottom, SRCCOPY);

	//draw(pDC, m_pos);
	memDC.DeleteDC();
	mdcOffScreen.SelectObject(oldbitmap);
	mdcOffScreen.DeleteDC();
	bmpOffScreen.DeleteObject();
}


void CImgViewerView::OnFileOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	COptionFormView* pView = pMain->pOptionView; // 설정 창 View 포인터

	if (!m_Algorithm.SelectImage())
		return;
	m_Algorithm.SetDistance(pView->m_nDist);
	m_Algorithm.SetCircleValue(pView->m_nRadMin, pView->m_nRadMax, pView->m_nBGV);
	m_Algorithm.Run();

	result_mat = m_Algorithm.GetResultImage();
	result_bmp = m_Algorithm.MatToBitmap(result_mat);

	m_background.Detach();
	m_background.Attach(result_bmp);
	m_background.GetBitmap(&m_Bitmap);

	zoomWidth = m_Bitmap.bmWidth;
	zoomHeight = m_Bitmap.bmHeight;

	m_ePt.x = 0;
	m_ePt.y = 0;

	z_pos.x = 1;
	z_pos.y = 1;

	m_Zoom = 1;

	PWidth = m_Bitmap.bmWidth / zoomWidth;
	PHeight = m_Bitmap.bmHeight / zoomHeight;

	Invalidate(FALSE);
}


void CImgViewerView::paraChanged() // 이미지 처리 및 버퍼에 붙이기
{
	// TODO: 여기에 구현 코드 추가.
	if (!m_Algorithm.isReady())
		return;

	m_Algorithm.Run();
	//m_Algorithm.ShowSrcImage();
	//m_Algorithm.ShowResultImage();
	result_mat = m_Algorithm.GetResultImage();
	//imshow("dst", m_Algorithm.GetResultImage());
	result_bmp = m_Algorithm.MatToBitmap(result_mat);

	m_background.Detach();
	m_background.Attach(result_bmp);
	m_background.GetBitmap(&m_Bitmap);

	Invalidate(FALSE);
}