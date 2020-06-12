
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
#define IsCTRLPressed()  ( 0x8000 ==(GetKeyState(VK_CONTROL) & 0x8000 ))

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_DRAW_POINT, &CImgViewerView::OnDrawPoint)
	ON_COMMAND(ID_DRAW_LINE, &CImgViewerView::OnDrawLine)
	ON_COMMAND(ID_DRAW_ELLPSE, &CImgViewerView::OnDrawEllpse)
	ON_COMMAND(ID_DRAW_RECT, &CImgViewerView::OnDrawRect)
	ON_COMMAND(ID_MODE_DRAW, &CImgViewerView::OnModeDraw)
	ON_COMMAND(ID_MODE_SELECT, &CImgViewerView::OnModeSelect)
	ON_COMMAND(ID_CONTEXT_COPY, &CImgViewerView::OnContextCopy)
	ON_COMMAND(ID_CONTEXT_PASTE, &CImgViewerView::OnContextPaste)
	ON_COMMAND(ID_CONTEXT_DELETE, &CImgViewerView::OnContextDelete)
	ON_COMMAND(ID_CONTEXT_LINECOLOR, &CImgViewerView::OnContextLinecolor)
	ON_COMMAND(ID_FILE_SAVE_WITHSHAPE, &CImgViewerView::OnFileSaveWithshape)
	ON_COMMAND(ID_FILE_SAVE_ONLYIMG, &CImgViewerView::OnFileSaveOnlyimg)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_COMMAND(ID_SELECT_LW, &CImgViewerView::OnSelectLw)
	ON_COMMAND(ID_SELECT_COLOR, &CImgViewerView::OnSelectColor)
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
	GetParent()->SetWindowText(_T("이미지 뷰"));

}

void CImgViewerView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CWnd::OnMouseMove(nFlags, point);
	// 좌표 업데이트
	mov_Pt.x = (z_pos.x + (m_ePt.x + point.x) / PWidth)  - start_pos.x;
	mov_Pt.y = (z_pos.y + (m_ePt.y + point.y) / PHeight) - start_pos.y;
	m_pos = point;
	int i = 0;
	//패닝
	if (nFlags & MK_MBUTTON)
	{
		// A영역의 사각형 그리기 //m_sPt는 기존 포인트, point는 이동
		if (start_pos.x == 0)
		{
			if (m_sPt.x < point.x) // 오른쪽으로 끌었을때
			{
				m_ePt.x -= point.x - m_sPt.x;

				if (m_ePt.x <= 0)
				{
					i = 1 + ((0 - m_ePt.x) / PWidth);
					z_pos.x -= i;
					m_ePt.x += i * PWidth;

					/*if (z_pos.x > 0)
					{
						i = 1 + ((0 - m_ePt.x) / PWidth);
						z_pos.x -= i;
						m_ePt.x += i * PWidth;
					}*/
				}
			}
			else // 왼쪽으로 끌었을때
			{
				m_ePt.x += m_sPt.x - point.x;

				if (m_ePt.x + m_bgRect.right - (PWidth * (int)(zoomWidth + 2)) >= 0)
				{
					i = 1 + (m_ePt.x + m_bgRect.right - (PWidth * (int)(zoomWidth + 2))) / PWidth;
					z_pos.x += i;
					m_ePt.x -= i * PWidth;

				}
			}
		}


		if (start_pos.y == 0)
		{
			if (m_sPt.y > point.y) // 위로 올렸을때
			{
				m_ePt.y += m_sPt.y - point.y;

				if (m_ePt.y + m_bgRect.bottom - (PHeight * (int)(zoomHeight + 2)) >= 0)
				{
					i = 1 + (m_ePt.y + m_bgRect.bottom - (PHeight * (int)(zoomHeight + 2))) / PHeight;
					z_pos.y += i;
					m_ePt.y -= i * PHeight;
				}

			}
			else // 아래로 내렸을때
			{
				m_ePt.y -= point.y - m_sPt.y;
				if (m_ePt.y < 0)
				{
					i = 1 + ((0 - m_ePt.y) / PHeight);
					z_pos.y -= i;
					m_ePt.y += i * PHeight;

					/*if (z_pos.y > 0)
					{
						i = 1 + ((0 - m_ePt.y) / PHeight);
						z_pos.y -= i;
						m_ePt.y += i * PHeight;
					}*/
				}
			}
		}
		// B영역의 사각형 그리기


		//---------------------------
		if (m_ePt.x + m_bgRect.right > PWidth * (int)(zoomWidth + 2))
		{
			z_pos.x += 1;
			m_ePt.x -= PWidth;
		}

		if (m_ePt.y + m_bgRect.bottom > PHeight * (int)(zoomHeight + 2))
		{
			z_pos.y += 1;
			m_ePt.y -= PHeight;
		}
		//----------------------------

		if (m_ePt.x < 0)
			m_ePt.x = 0;
		if (m_ePt.y < 0)
			m_ePt.y = 0;

		m_sPt = point;
		//----------------------------------------------------------------
		imgViewer2Navigator();

		Invalidate(FALSE);
		UpdateWindow();
	}

	if (MK_LBUTTON == nFlags && drawID == TRUE) // 마우스를 클릭하여 드래그일 동안만 
		Invalidate(FALSE);
	// 크기 조정
	else if (MK_LBUTTON == nFlags && (EdgeSelect == TRUE))
	{
		int o, p;
		int xx, yy;
		//CPoint TLX;
		xx = d_sPt.x, yy = d_sPt.y;
		d_sPt.x = (z_pos.x + (m_ePt.x + point.x) / PWidth) - start_pos.x;
		d_sPt.y = (z_pos.y + (m_ePt.y + point.y) / PHeight) - start_pos.y;
		o = (d_sPt.x - xx);
		p = (d_sPt.y - yy);
		double theta = data[SelectIndex].theta;

		RectCount = data[SelectIndex].pts.size();

		// 길이
		if (data[SelectIndex].shapeType == DrawMode::DLine)
		{
			data[SelectIndex].pts[EdgeIndex].x += o;
			data[SelectIndex].pts[EdgeIndex].y += p;
		}
		// 삼각형
		else if (data[SelectIndex].shapeType == DrawMode::DTriangle)
		{
			data[SelectIndex].pts[EdgeIndex].x += o;
			data[SelectIndex].pts[EdgeIndex].y += p;
		}
		// 크로스 헤어
		else if (data[SelectIndex].shapeType == DrawMode::DCrossHair)
		{
			if (EdgeIndex / 2 == 0)
			{
				data[SelectIndex].pts[EdgeIndex].x += o;
				data[SelectIndex].pts[EdgeIndex].y += p;
				data[SelectIndex].pts[EdgeIndex + 1].x -= o;
				data[SelectIndex].pts[EdgeIndex + 1].y -= p;
			}
			else
			{
				data[SelectIndex].pts[EdgeIndex].x += o;
				data[SelectIndex].pts[EdgeIndex].y += p;
				data[SelectIndex].pts[EdgeIndex - 1].x -= o;
				data[SelectIndex].pts[EdgeIndex - 1].y -= p;
			}

		}
		//사각형 원
		else
		{
			data[SelectIndex].pts[EdgeIndex].x += o;
			data[SelectIndex].pts[EdgeIndex].y += p;

			if (data[SelectIndex].theta == 0)
			{
				if (EdgeIndex == 0)
				{
					data[SelectIndex].pts[3].x += o;
					data[SelectIndex].pts[1].y += p;
				}
				else if (EdgeIndex == 1)
				{

					data[SelectIndex].pts[2].x += o;
					data[SelectIndex].pts[0].y += p;
				}
				else if (EdgeIndex == 2)
				{
					data[SelectIndex].pts[1].x += o;
					data[SelectIndex].pts[3].y += p;
				}
				else if (EdgeIndex == 3)
				{
					data[SelectIndex].pts[0].x += o;
					data[SelectIndex].pts[2].y += p;
				}
			}
			else
			{
				if (EdgeIndex == 0)
				{
					data[SelectIndex].pts[1] = Intersection(data[SelectIndex].pts[0], data[SelectIndex].pts[1], data[SelectIndex].pts[2]);
					data[SelectIndex].pts[3] = Intersection(data[SelectIndex].pts[0], data[SelectIndex].pts[3], data[SelectIndex].pts[2]);
				}
				else if (EdgeIndex == 1)
				{
					data[SelectIndex].pts[0] = Intersection(data[SelectIndex].pts[1], data[SelectIndex].pts[0], data[SelectIndex].pts[3]);
					data[SelectIndex].pts[2] = Intersection(data[SelectIndex].pts[1], data[SelectIndex].pts[2], data[SelectIndex].pts[3]);
				}
				else if (EdgeIndex == 2)
				{
					data[SelectIndex].pts[1] = Intersection(data[SelectIndex].pts[2], data[SelectIndex].pts[1], data[SelectIndex].pts[0]);
					data[SelectIndex].pts[3] = Intersection(data[SelectIndex].pts[2], data[SelectIndex].pts[3], data[SelectIndex].pts[0]);
				}
				else if (EdgeIndex == 3)
				{
					data[SelectIndex].pts[0] = Intersection(data[SelectIndex].pts[3], data[SelectIndex].pts[0], data[SelectIndex].pts[1]);
					data[SelectIndex].pts[2] = Intersection(data[SelectIndex].pts[3], data[SelectIndex].pts[2], data[SelectIndex].pts[1]);
				}
			}
		}

		// 꼭지점 업데이트
		SelectShapeUpdate();
		Invalidate(FALSE);
	}
	// 회전 클릭
	else if (MK_LBUTTON == nFlags && (rotateID == TRUE))
	{
		Point2d center = data[SelectIndex].Center;
		Point2d Rotate = data[SelectIndex].Rotate;
		vector<Point2d> pts = data[SelectIndex].pts; // 복사 -> 이동 -> R복사
		Point2d pt;
		d_sPt.x = (z_pos.x + (m_ePt.x + point.x) / PWidth) - start_pos.x;
		d_sPt.y = (z_pos.y + (m_ePt.y + point.y) / PHeight) - start_pos.y;

		double theta = atan2((Rotate.x - center.x), (Rotate.y - center.y)) - atan2((d_sPt.x - center.x), (d_sPt.y - center.y));
		for (int i = 0; i < pts.size(); i++)
		{

			pt.x = cvRound((pts[i].x - center.x) * cos(theta) - (pts[i].y - center.y) * sin(theta) + center.x);
			pt.y = cvRound((pts[i].x - center.x) * sin(theta) + (pts[i].y - center.y) * cos(theta) + center.y);
			pts[i] = pt;
		}
		data[SelectIndex].RotatePts = pts;
		data[SelectIndex].R_theta = theta + data[SelectIndex].theta;
		Invalidate(FALSE);
	}
	// 이동
	else if (MK_LBUTTON == nFlags && (selectID == TRUE))
	{
		int o, p;
		int xx, yy;

		xx = d_sPt.x, yy = d_sPt.y;
		d_sPt.x = (z_pos.x + (m_ePt.x + point.x) / PWidth) - start_pos.x;
		d_sPt.y = (z_pos.y + (m_ePt.y + point.y) / PHeight) - start_pos.y;

		o = (d_sPt.x - xx);
		p = (d_sPt.y - yy);

		CPoint pt;
		RectCount = data[SelectIndex].pts.size();
		for (int i = 0; i < RectCount; i++)
		{
			pt.x = data[SelectIndex].RotatePts[i].x += o;
			pt.y = data[SelectIndex].RotatePts[i].y += p;
		}
		UpdateWindow();
		Invalidate(FALSE);
	}

}

void CImgViewerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_bgRect.left = 0;
	m_bgRect.top = 0;
	m_bgRect.right = cx;
	m_bgRect.bottom = cy;

	zoomWidth = cx * rectScale;
	zoomHeight = cy * rectScale;

	if (m_Bitmap.bmWidth * zoomView <= m_bgRect.right)
	{
		start_pos.x = m_bgRect.right / 2 - (m_Bitmap.bmWidth * zoomView) / 2;
		z_pos.x = 0;
	}
	else
	{
		start_pos.x = 0;
	}

	if (m_Bitmap.bmHeight * zoomView <= m_bgRect.bottom)
	{
		start_pos.y = m_bgRect.bottom / 2 - (m_Bitmap.bmHeight * zoomView) / 2;
		z_pos.y = 0;
	}
	else
	{
		start_pos.y = 0;
	}

	if (z_pos.x + zoomWidth + 5 >= m_Bitmap.bmWidth && zoomWidth <= m_Bitmap.bmWidth)
	{
		z_pos.x = m_Bitmap.bmWidth - zoomWidth - 5;
		m_ePt.x = PWidth * (int)(zoomWidth + 2) - m_bgRect.right;
	}


	if (z_pos.y + zoomHeight + 1 >= m_Bitmap.bmHeight && zoomHeight <= m_Bitmap.bmHeight)
	{
		z_pos.y = m_Bitmap.bmHeight - zoomHeight - 1;
		m_ePt.y = PHeight * (int)(zoomHeight + 2) - m_bgRect.bottom;
	}

	if (m_Algorithm.isReady())
	{
		imgViewer2Navigator();
	}

	Invalidate(FALSE);
}

BOOL CImgViewerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	float zoomScale = 1;
	if (zDelta <= 0)// 휠 내릴때
	{
		zoomScale = 1.25f;
		zoomView *= 0.8f;
		rectScale *= 1.25f;
	}
	else// 휠 올릴때
	{
		zoomScale = 0.8f;
		zoomView *= 1.25f;
		rectScale *= 0.8f;
	}

	before_Image_pos.x = z_pos.x + (m_ePt.x + m_pos.x) / PWidth;
	before_Image_pos.y = z_pos.y + (m_ePt.y + m_pos.y) / PHeight;

	before_Pixel_pos.x = (((m_ePt.x + m_pos.x) / PWidth - (int)((m_ePt.x + m_pos.x) / PWidth)));
	before_Pixel_pos.y = (((m_ePt.y + m_pos.y) / PHeight - (int)((m_ePt.y + m_pos.y) / PHeight)));

	//왼쪽위 꼭짓점
	z_pos.x += round(((((float)m_ePt.x + (float)m_pos.x) / (PWidth * (int)(zoomWidth + 2))) * zoomWidth)
		- ((((float)m_ePt.x + (float)m_pos.x) / (PWidth * (int)(zoomWidth + 2))) * (zoomWidth * zoomScale)));
	z_pos.y += round(((((float)m_ePt.y + (float)m_pos.y) / (PHeight * (int)(zoomHeight + 2))) * zoomHeight)
		- ((((float)m_ePt.y + (float)m_pos.y) / (PHeight * (int)(zoomHeight + 2))) * (zoomHeight * zoomScale)));

	//원본에서 출력될 길이
	zoomWidth = zoomWidth * zoomScale;
	zoomHeight = zoomHeight * zoomScale;

	printWidth = zoomWidth * zoomView;
	printHeight = zoomHeight * zoomView;

	//픽셀 크기
	PWidth = (printWidth) / (zoomWidth);
	PHeight = (printHeight) / (zoomHeight);

	after_Image_pos.x = z_pos.x + (m_ePt.x + m_pos.x) / PWidth;
	after_Image_pos.y = z_pos.y + (m_ePt.y + m_pos.y) / PHeight;

	after_Pixel_pos.x = (((m_ePt.x + m_pos.x) / PWidth - (int)((m_ePt.x + m_pos.x) / PWidth)));
	after_Pixel_pos.y = (((m_ePt.y + m_pos.y) / PHeight - (int)((m_ePt.y + m_pos.y) / PHeight)));

	if (before_Image_pos.x - (int)(after_Image_pos.x) != 0)
	{
		z_pos.x += before_Image_pos.x - (int)(after_Image_pos.x);
		m_ePt.x += (before_Pixel_pos.x * PWidth) - (after_Pixel_pos.x * PWidth);
	}

	if (before_Image_pos.y - (after_Image_pos.y) != 0)
	{
		z_pos.y += before_Image_pos.y - (int)(after_Image_pos.y);
		m_ePt.y += (before_Pixel_pos.y * PHeight) - (after_Pixel_pos.y * PHeight);
	}


	//-----------------------------------------------
	if (m_Bitmap.bmWidth * zoomView <= m_bgRect.right)
	{
		start_pos.x = m_bgRect.right / 2 - (m_Bitmap.bmWidth * zoomView) / 2;
		z_pos.x = 0;
	}
	else
	{
		start_pos.x = 0;
	}

	if (m_Bitmap.bmHeight * zoomView <= m_bgRect.bottom)
	{
		start_pos.y = m_bgRect.bottom / 2 - (m_Bitmap.bmHeight * zoomView) / 2;
		z_pos.y = 0;
	}
	else
	{
		start_pos.y = 0;
	}
	//-----------------------------------
	if (m_ePt.x >= PWidth)
	{
		z_pos.x += 1;
		m_ePt.x -= PWidth;
	}
	if (m_ePt.y >= PHeight)
	{
		z_pos.y += 1;
		m_ePt.y -= PHeight;
	}
	//-----------------------------------
	if (m_ePt.x < 0 && z_pos.x > 0)
	{
		z_pos.x -= 1;
		m_ePt.x += PWidth;
	}
	if (m_ePt.y < 0 && z_pos.y > 0)
	{
		z_pos.y -= 1;
		m_ePt.y += PHeight;
	}

	//-----------------------------------
	if (m_ePt.x >= PWidth)
	{
		z_pos.x -= m_ePt.x / PWidth;
		m_ePt.x = m_ePt.x - PWidth * (int)(m_ePt.x / PWidth);
	}

	if (m_ePt.y >= PHeight)
	{
		z_pos.y -= m_ePt.y / PHeight;
		m_ePt.y = m_ePt.y - PHeight * (int)(m_ePt.y / PHeight);
	}
	//---------------------------------

	if (z_pos.x + zoomWidth + 5 >= m_Bitmap.bmWidth && zoomWidth <= m_Bitmap.bmWidth)
	{
		z_pos.x = m_Bitmap.bmWidth - zoomWidth - 5;
		m_ePt.x = PWidth * (int)(zoomWidth + 2) - m_bgRect.right;
	}


	if (z_pos.y + zoomHeight + 1 >= m_Bitmap.bmHeight && zoomHeight <= m_Bitmap.bmHeight)
	{
		z_pos.y = m_Bitmap.bmHeight - zoomHeight - 1;
		m_ePt.y = PHeight * (int)(zoomHeight + 2) - m_bgRect.bottom;
	}

	//----------------------------------
	if (z_pos.x <= 0)
		z_pos.x = 0;
	if (z_pos.y <= 0)
		z_pos.y = 0;

	//---------------------------------------------------------
	imgViewer2Navigator();
	//---------------------------------------------------------

	Invalidate(FALSE);
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CImgViewerView::OnMButtonDown(UINT nFlags, CPoint point)
{

	CView::OnMButtonDown(nFlags, point);

	SetCapture();
	m_sPt = point;
}

void CImgViewerView::OnMButtonUp(UINT nFlags, CPoint point)
{
	CView::OnMButtonUp(nFlags, point);

	if (m_ePt.x >= PWidth)
	{
		z_pos.x += 1;
		m_ePt.x -= PWidth;
	}
	if (m_ePt.y >= PHeight)
	{
		z_pos.y += 1;
		m_ePt.y -= PHeight;
	}

	if (z_pos.x < 0)
		z_pos.x = 0;
	if (z_pos.y < 0)
		z_pos.y = 0;

	if (z_pos.x + zoomWidth + 5 >= m_Bitmap.bmWidth && zoomWidth <= m_Bitmap.bmWidth)
	{
		z_pos.x = m_Bitmap.bmWidth - zoomWidth - 5;
		m_ePt.x = PWidth * (int)(zoomWidth + 2) - m_bgRect.right;
	}


	if (z_pos.y + zoomHeight + 1 >= m_Bitmap.bmHeight && zoomHeight <= m_Bitmap.bmHeight)
	{
		z_pos.y = m_Bitmap.bmHeight - zoomHeight - 1;
		m_ePt.y = PHeight * (int)(zoomHeight + 2) - m_bgRect.bottom;
	}

	//-------------------------------------------------------
	imgViewer2Navigator();
	//---------------------------------------------------------

	ReleaseCapture();
	Invalidate(FALSE);
}

void CImgViewerView::OnPaint()
{
	CPaintDC pDC(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	CBitmap* oldbitmap, * oldbitmap2;	//원본 오브젝트 저장

	m_background.Detach();
	if (!m_background.Attach(::CopyImage(result_bmp, IMAGE_BITMAP, 0, 0, 0)))
		return;

	memDC.CreateCompatibleDC(&pDC);
	mdcOffScreen.CreateCompatibleDC(&pDC);

	bmpOffScreen.CreateCompatibleBitmap(&pDC, PWidth * (int)(zoomWidth + 2), PHeight * (int)(zoomHeight + 2));

	oldbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);
	oldbitmap2 = memDC.SelectObject(&m_background);


	// 그림 그리기
	if (drawShapeID == TRUE) {
		draw(&memDC);
	}
	if (selectID == TRUE)
	{
		SelectDrawShape(&memDC, data[SelectIndex]);
		// 그림 복원
		for (int i = 0; i < data.GetSize(); i++)
		{
			if (i == SelectIndex)
				continue;
			drawShape(&memDC, data[i]);
		}
	}
	else
	{
		// 그림 복원
		for (int i = 0; i < data.GetSize(); i++)
			drawShape(&memDC, data[i]);
	}


	mdcOffScreen.SetStretchBltMode(HALFTONE);
	mdcOffScreen.StretchBlt(start_pos.x, start_pos.y, PWidth * ((int)zoomWidth + 2), PHeight * ((int)zoomHeight + 2), &memDC,z_pos.x, z_pos.y, zoomWidth + 2, zoomHeight + 2, SRCCOPY);

	PrintText(&mdcOffScreen);

	pDC.SetStretchBltMode(HALFTONE);
	pDC.StretchBlt(0, 0, m_bgRect.right, m_bgRect.bottom, &mdcOffScreen,m_ePt.x, m_ePt.y, m_bgRect.right, m_bgRect.bottom, SRCCOPY);

	memDC.SelectObject(oldbitmap2);
	memDC.DeleteDC();
	mdcOffScreen.SelectObject(oldbitmap);
	mdcOffScreen.DeleteDC();
	bmpOffScreen.DeleteObject();
	oldbitmap->DeleteObject();
	oldbitmap2->DeleteObject();
}

void CImgViewerView::OnFileOpen()
{
	if (!m_Algorithm.SelectImage())
		return;

	result_mat = m_Algorithm.GetSourceImage();
	result_bmp = m_Algorithm.MatToBitmap(result_mat);

	m_background.Detach();
	m_background.Attach(result_bmp);
	m_background.GetBitmap(&m_Bitmap);

	zoomWidth = m_bgRect.right;
	zoomHeight = m_bgRect.bottom;

	m_ePt.x = 0;
	m_ePt.y = 0;

	z_pos.x = 0;
	z_pos.y = 0;

	PWidth = 1;
	PHeight = 1;

	zoomView = 1;
	rectScale = 1;
	printWidth = m_Bitmap.bmWidth;
	printHeight = m_Bitmap.bmHeight;

	PWidth = 1;
	PHeight = 1;

	if (m_Bitmap.bmWidth * zoomView <= m_bgRect.right)
	{
		start_pos.x = m_bgRect.right / 2 - (m_Bitmap.bmWidth * zoomView) / 2;
		z_pos.x = 0;
	}
	else
	{
		start_pos.x = 0;
	}

	if (m_Bitmap.bmHeight * zoomView <= m_bgRect.bottom)
	{
		start_pos.y = m_bgRect.bottom / 2 - (m_Bitmap.bmHeight * zoomView) / 2;
		z_pos.y = 0;
	}
	else
	{
		start_pos.y = 0;
	}

	Invalidate(FALSE);
}

void CImgViewerView::paraChanged() // 이미지 처리 및 버퍼에 붙이기
{
	if (!m_Algorithm.isReady())
		return;

	m_Algorithm.Run();
	result_mat = m_Algorithm.GetResultImage();
	result_bmp = m_Algorithm.MatToBitmap(result_mat);

	m_background.Detach();
	m_background.Attach(result_bmp);
	m_background.GetBitmap(&m_Bitmap);

	Invalidate(FALSE);
}

void CImgViewerView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu popup;
	CMenu* pMenu;

	popup.LoadMenuW(IDR_MENU_CONTEXT);
	pMenu = popup.GetSubMenu(0);

	pMenu->TrackPopupMenu(TPM_LEFTALIGN || TPM_RIGHTBUTTON, point.x, point.y, this);
}


void CImgViewerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMainFrame* frame = (CMainFrame*)AfxGetMainWnd();
	SetCapture();
	// Down 버튼 저장
	d_sPt.x = (z_pos.x + (m_ePt.x + point.x) / PWidth) - start_pos.x;
	d_sPt.y = (z_pos.y + (m_ePt.y + point.y) / PHeight) - start_pos.y;
	// darw 상태 검사
	if (drawID == TRUE && drawStyle != DrawMode::None) {
		drawShapeID = TRUE;
		return;
	}
	// 엣지 클릭 검사 // 회전 선 클릭 검사
	if (selectID == TRUE) {

		//엣지 클릭 검사
		for (int i = 0; i < RectCount; i++)
		{
			CRect& rect = data[SelectIndex].Rect[i];
			if (d_sPt.x >= rect.left && d_sPt.x <= rect.right && d_sPt.y >= rect.top && d_sPt.y <= rect.bottom)
			{
				EdgeSelect = TRUE;
				EdgeIndex = i;
				Invalidate(FALSE);
				return;
			}
		}

		// 회전 선 클릭 검사
		CRect centRC;

		Point2d Center = data[SelectIndex].Center;
		Point2d Rotate = data[SelectIndex].Rotate;

		centRC.left = min(Rotate.x, Center.x) - 10;
		centRC.right = max(Rotate.x, Center.x) + 10;
		centRC.top = min(Rotate.y, Center.y) - 10;
		centRC.bottom = max(Rotate.y, Center.y) + 10;
		if (d_sPt.x >= centRC.left && d_sPt.x <= centRC.right && d_sPt.y >= centRC.top && d_sPt.y <= centRC.bottom) {
			rotateID = TRUE;
			Invalidate(FALSE);
			return;
		}
	}

	// 이미지 객체 클릭 검사
	for (int i = 0; i < data.GetSize(); i++)
	{
		int zIndex = zOrder[i];
		if (data[zOrder[i]].shapeType != DrawMode::DLine) {
			if (isContainPolygon(CPoint(d_sPt.x, d_sPt.y), data[zOrder[i]].pts))
			{
				Point pt;
				RectCount = data[zOrder[i]].pts.size();
				for (int j = 0; j < RectCount; j++)
				{
					pt.x = data[zIndex].pts[j].x;
					pt.y = data[zIndex].pts[j].y;
					data[zIndex].Rect[j].SetRect(pt.x - 5, pt.y - 5, pt.x + 5, pt.y + 5);
				}

				selectID = TRUE;
				temp = data[zOrder[i]];
				zOrder.insert(zOrder.begin(), zOrder[i]);
				zOrder.erase(zOrder.begin() + i + 1);
				SelectIndex = zOrder[0];

				Invalidate(FALSE);
				return;
			}
		}
		else if (data[zIndex].shapeType == DrawMode::DLine)
		{
			vector<Point2d> linePts;
			double A, B;
			A = (data[zIndex].pts[0].y - data[zIndex].pts[1].y);
			B = (data[zIndex].pts[0].x - data[zIndex].pts[1].x);

			if (A == 0)
			{
				linePts.push_back(Point2d(data[zIndex].pts[0].x, data[zIndex].pts[0].y - 5));
				linePts.push_back(Point2d(data[zIndex].pts[0].x, data[zIndex].pts[0].y + 5));
				linePts.push_back(Point2d(data[zIndex].pts[1].x, data[zIndex].pts[1].y + 5));
				linePts.push_back(Point2d(data[zIndex].pts[1].x, data[zIndex].pts[1].y - 5));
			}
			else if (B == 0)
			{
				linePts.push_back(Point2d(data[zIndex].pts[0].x - 5, data[zIndex].pts[0].y));
				linePts.push_back(Point2d(data[zIndex].pts[0].x + 5, data[zIndex].pts[0].y));
				linePts.push_back(Point2d(data[zIndex].pts[1].x + 5, data[zIndex].pts[1].y));
				linePts.push_back(Point2d(data[zIndex].pts[1].x - 5, data[zIndex].pts[1].y));
			}
			else
			{
				A = A / B;
				B = -A * data[zIndex].pts[0].x + data[zIndex].pts[0].y;
				linePts.push_back(Point2d(data[zIndex].pts[0].x, data[zIndex].pts[0].x * A + B - 5));
				linePts.push_back(Point2d(data[zIndex].pts[0].x, data[zIndex].pts[0].x * A + B + 5));
				linePts.push_back(Point2d(data[zIndex].pts[1].x, data[zIndex].pts[1].x * A + B + 5));
				linePts.push_back(Point2d(data[zIndex].pts[1].x, data[zIndex].pts[1].x * A + B - 5));
			}

			if (isContainPolygon(CPoint(d_sPt.x, d_sPt.y), linePts))
			{
				CPoint pt;
				RectCount = data[zIndex].pts.size();
				for (int j = 0; j < RectCount; j++)
				{
					pt.x = data[zIndex].pts[j].x;
					pt.y = data[zIndex].pts[j].y;
					data[zIndex].Rect[j].SetRect(pt.x - 5, pt.y - 5, pt.x + 5, pt.y + 5);
				}

				penID = TRUE;
				selectID = TRUE;

				temp = data[zOrder[i]];
				zOrder.insert(zOrder.begin(), zOrder[i]);
				zOrder.erase(zOrder.begin() + i + 1);
				SelectIndex = zOrder[0];
				Invalidate(FALSE);
				return;
			}
		}
	}


	// 없을 경우 초기화
	SelectIndex = -1;
	selectID = FALSE;
	CView::OnLButtonDown(nFlags, point);
}

void CImgViewerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	//CMainFrame* frame = (CMainFrame*)AfxGetMainWnd();

	// 도형그리기
	if (drawShapeID == TRUE)  // 도형 그리기 상태일 동안만
	{
		//if (drawStyle == DrawMode::None)
		//	return;

		shape.pts.clear();

		mov_Pt.x = (z_pos.x + (m_ePt.x + point.x) / PWidth)- start_pos.x;
		mov_Pt.y = (z_pos.y + (m_ePt.y + point.y) / PHeight) - start_pos.y;
		// 마우스 버트을 놓으면 각종 값을 shape 구조체 맴버변수에 저장
		shape.shapeType = drawStyle;  // 도형 콤보 상자에서 선택된 도형 스타일을 저장		

		if (shape.shapeType == DrawMode::DLine)
		{
			shape.pts.push_back(Point2d(d_sPt.x, d_sPt.y));
			shape.pts.push_back(Point2d(mov_Pt.x, mov_Pt.y));
		}
		else if (shape.shapeType == DrawMode::DEllipse)
		{
			shape.pts.push_back(Point2d(d_sPt.x, d_sPt.y));
			shape.pts.push_back(Point2d(mov_Pt.x, d_sPt.y));
			shape.pts.push_back(Point2d(mov_Pt.x, mov_Pt.y));
			shape.pts.push_back(Point2d(d_sPt.x, mov_Pt.y));
		}
		else if (shape.shapeType == DrawMode::DRectangle)
		{
			shape.pts.push_back(Point2d(d_sPt.x, d_sPt.y));
			shape.pts.push_back(Point2d(mov_Pt.x, d_sPt.y));
			shape.pts.push_back(Point2d(mov_Pt.x, mov_Pt.y));
			shape.pts.push_back(Point2d(d_sPt.x, mov_Pt.y));
		}
		else if (shape.shapeType == DrawMode::DTriangle)
		{
			shape.pts.push_back(Point2d((d_sPt.x + mov_Pt.x) / 2, d_sPt.y));
			shape.pts.push_back(Point2d(mov_Pt.x, mov_Pt.y));
			shape.pts.push_back(Point2d(d_sPt.x, mov_Pt.y));
		}
		else if (shape.shapeType == DrawMode::DCrossHair)
		{
			shape.pts.push_back(Point2d((d_sPt.x + mov_Pt.x) / 2, d_sPt.y));
			shape.pts.push_back(Point2d((d_sPt.x + mov_Pt.x) / 2, mov_Pt.y));
			shape.pts.push_back(Point2d(d_sPt.x, (d_sPt.y + mov_Pt.y) / 2));
			shape.pts.push_back(Point2d(mov_Pt.x, (d_sPt.y + mov_Pt.y) / 2));
		}
		Point2d pt(0, 0);
		shape.radin[0] = abs(d_sPt.x - mov_Pt.x) / 2;
		shape.radin[1] = abs(d_sPt.y - mov_Pt.y) / 2;
		for (int i = 0; i < shape.pts.size(); i++)
			pt += shape.pts[i];

		shape.RotatePts = shape.pts;
		shape.Center = pt / (int)shape.pts.size();
		shape.Rotate = shape.Center;
		shape.Rotate.y -= 15;
		shape.theta = 0;
		shape.R_theta = 0;
		shape.shapeColor = color; // 색상값
		shape.penWidth = 1; // 굵기 
		data.Add(shape);
		drawShapeID = FALSE;
		selectID = TRUE;

		zOrder.insert(zOrder.begin(), data.GetSize() - 1);
		SelectIndex = zOrder[0];

		RollbackInfo info;
		info.idx = SelectIndex;
		info.redoShape = data[SelectIndex];
		info.undoShape = shape;
		info.rollbackmode = RollBackMode::Create;
		if (rollbackIndex != -1)
			rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
		else if (rollback.size() != 0)
			rollback.erase(rollback.begin(), rollback.end());
		rollback.push_back(info);
		rollbackIndex = rollback.size() - 1;
	}
	// 횐전 업데이트
	else if (rotateID)
	{
		// 360 조정
		if (data[SelectIndex].R_theta >= (CV_PI * 2))
			data[SelectIndex].R_theta = data[SelectIndex].R_theta - (CV_PI * 2);
		// 회전각 업데이트
		data[SelectIndex].pts = data[SelectIndex].RotatePts;
		data[SelectIndex].theta = data[SelectIndex].R_theta;
		rotateID = FALSE;
		RectCount = data[SelectIndex].pts.size();
		for (int i = 0; i < RectCount; i++)
		{
			if (data[SelectIndex].pts[i] != temp.pts[i])
			{
				RollbackInfo info;
				info.idx = SelectIndex;
				info.redoShape = data[SelectIndex];
				info.undoShape = temp;
				info.rollbackmode = RollBackMode::Update;
				if (rollbackIndex != -1)
					rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
				rollback.push_back(info);
				rollbackIndex = rollback.size() - 1;
				break;
			}
		}
	}
	// 이동좌표 업데이트
	else if (selectID)
	{
		data[SelectIndex].pts = data[SelectIndex].RotatePts;
		RectCount = data[SelectIndex].pts.size();
		for (int i = 0; i < RectCount; i++)
		{
			if (data[SelectIndex].pts[i] != temp.pts[i])
			{
				RollbackInfo info;
				info.idx = SelectIndex;
				info.redoShape = data[SelectIndex];
				info.undoShape = temp;
				info.rollbackmode = RollBackMode::Update;
				if (rollbackIndex != -1)
					rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
				rollback.push_back(info);
				rollbackIndex = rollback.size() - 1;
				break;
			}
		}
	}

	EdgeSelect = FALSE;
	SelectShapeUpdate();
	Invalidate(FALSE);
	ReleaseCapture();
	CView::OnLButtonUp(nFlags, point);
}

void CImgViewerView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	p_pt.x = (z_pos.x + (m_ePt.x + point.x) / PWidth) - start_pos.x;
	p_pt.y = (z_pos.y + (m_ePt.y + point.y) / PHeight) - start_pos.y;
	CView::OnRButtonDown(nFlags, point);
}

void CImgViewerView::PrintText(CDC* pDC)
{
	COLORREF rgb;
	int cntWidth = 0, cntHeight = 0;

	if (PWidth >= 65)
	{
		for (int i = z_pos.x; i < z_pos.x + zoomWidth + 2; i++)
		{
			for (int j = z_pos.y; j < z_pos.y + zoomHeight + 2; j++)
			{
				rgb = memDC.GetPixel(i, j);
				int R = 0, G = 0, B = 0;

				R = GetRValue(rgb);
				G = GetGValue(rgb);
				B = GetBValue(rgb);

				int pixelvale = (R + G + B) / 3.0;



				pDC->SetBkMode(TRANSPARENT);

				if (pixelvale >= 130)
					pDC->SetTextColor(0x00000000);
				else
					pDC->SetTextColor(RGB(255, 255, 255));

				HBRUSH myBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
				HBRUSH oldBrush = (HBRUSH)pDC->SelectObject(myBrush);
				CRect rc(cntWidth * PWidth, cntHeight * PHeight, cntWidth * PWidth + PWidth, cntHeight * PHeight + PHeight);

				pDC->Rectangle(rc);

				CString strNum;

				CFont cFont;
				cFont.CreateFontW(PHeight / 5,                     // 글자높이
					PWidth / 5,                     // 글자너비
					0,                      // 출력각도
					0,                      // 기준 선에서의각도
					FW_NORMAL,              // 글자굵기
					FALSE,                  // Italic 적용여부
					FALSE,                  // 밑줄적용여부
					FALSE,                  // 취소선적용여부
					DEFAULT_CHARSET,       // 문자셋종류
					OUT_DEFAULT_PRECIS,    // 출력정밀도
					CLIP_DEFAULT_PRECIS,   // 클리핑정밀도
					DEFAULT_QUALITY,       // 출력문자품질
					DEFAULT_PITCH,         // 글꼴Pitch
					_T("맑은고딕")           // 글꼴
				);
				pDC->SelectObject(cFont);
				if (R == G && G == B)
				{
					strNum.Format(_T("%d"), pixelvale);

					pDC->DrawText(strNum, rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				}
				else
				{
					strNum.Format(_T("%d\n%d\n%d"), R, G, B);
					DrawTextEx(pDC, strNum, rc, DT_VCENTER | DT_CENTER);
				}

				pDC->SelectObject(oldBrush);
				DeleteObject(myBrush);
				DeleteObject(cFont);
				cntHeight++;
			}
			cntWidth++;
			cntHeight = 0;
		}
	}
}

void CImgViewerView::DrawTextEx(CDC* pDC, const CString& str, CRect rect, UINT nFormat)
{
	int    nLineCount = 0;

	if (str.GetLength())
	{
		nLineCount = 1;
		int nPos = -1;

		while ((nPos = str.Find('\n', nPos + 1)) > 0)
			nLineCount++;
	}

	CSize    szText = pDC->GetTextExtent(str);
	szText.cy = szText.cy * nLineCount;

	int    nHeight = rect.Height();
	if (nHeight < 0)
		nHeight = nHeight + szText.cy;
	else
		nHeight = nHeight - szText.cy;

	rect.top = rect.top + (nHeight / 2);

	pDC->DrawText(str, rect, nFormat);
}

void CImgViewerView::OnDrawPoint()
{
	drawStyle = DrawMode::DPoint;
	drawID = TRUE;
}

void CImgViewerView::OnDrawLine()
{
	drawStyle = DrawMode::DLine;
	drawID = TRUE;
}

void CImgViewerView::OnDrawEllpse()
{
	drawStyle = DrawMode::DEllipse;
	drawID = TRUE;
}

void CImgViewerView::OnDrawRect()
{
	drawStyle = DrawMode::DRectangle;
	drawID = TRUE;
}

void CImgViewerView::OnModeDraw()
{
	drawID = TRUE;
	penID = FALSE;
}

void CImgViewerView::OnModeSelect()
{
	drawID = FALSE;
	penID = TRUE;
}

void CImgViewerView::OnContextPaste()
{
	if (iscopy)
	{
		if (ctrl)
		{
			for (int i = 0; i < copyShape.pts.size(); i++)
			{
				copyShape.pts[i].x += 5;
				copyShape.pts[i].y += 5;
			}
			copyShape.RotatePts = copyShape.pts;
		}
		else
		{
			Point pt(p_pt.x - copyShape.Center.x, p_pt.y - copyShape.Center.y);
			// 가운데나 회전
			for (int i = 0; i < copyShape.pts.size(); i++)
			{
				copyShape.pts[i].x += pt.x;
				copyShape.pts[i].y += pt.y;
			}
			copyShape.RotatePts = copyShape.pts;
		}
		data.Add(copyShape);
		zOrder.insert(zOrder.begin(), data.GetSize() - 1);;
		SelectIndex = zOrder[0];
		RollbackInfo info;
		info.redoShape = data[SelectIndex];
		info.idx = SelectIndex;
		info.undoShape = shape;
		info.rollbackmode = RollBackMode::Create;
		if (rollbackIndex != -1)
			rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
		else if (rollback.size() != 0) {
			rollback.erase(rollback.begin(), rollback.end());
		}
		rollback.push_back(info);
		rollbackIndex = rollback.size() - 1;
		Invalidate(FALSE);
		ctrl = false;
	}
}


void CImgViewerView::OnContextCopy()
{
	if (selectID)
	{
		copyShape = data[SelectIndex];
		iscopy = true;
		OnModeSelect();
	}
}

void CImgViewerView::OnContextDelete()
{
	if (selectID)
	{
		RollbackInfo info;
		info.idx = SelectIndex;
		info.undoShape = data[SelectIndex];
		info.rollbackmode = RollBackMode::Delete;
		if (rollbackIndex != -1)
			rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
		else if (rollback.size() != 0) {
			rollback.erase(rollback.begin(), rollback.end());
		}
		for (int i = 1; i < zOrder.size(); i++) {
			if (zOrder[i] > zOrder[0])
				zOrder[i]--;
		}
		zOrder.erase(zOrder.begin());
		rollback.push_back(info);
		rollbackIndex = rollback.size() - 1;
		data.RemoveAt(SelectIndex);
		SelectIndex = -1;
		Invalidate(FALSE);
		selectID = FALSE;
	}
	return;
}

void CImgViewerView::OnContextLinecolor()
{
	if (SelectIndex != -1)
	{
		CColorDialog colorDlg;

		if (colorDlg.DoModal() == IDOK)
		{
			RollbackInfo info;
			info.idx = SelectIndex;
			info.undoShape = data[SelectIndex];
			data[SelectIndex].shapeColor = colorDlg.GetColor();
			info.redoShape = data[SelectIndex];
			info.rollbackmode = RollBackMode::Update;
			if (rollbackIndex != -1)
				rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
			else if (rollback.size() != 0) {
				rollback.erase(rollback.begin(), rollback.end());
			}
			rollback.push_back(info);
			rollbackIndex = rollback.size() - 1;
			Invalidate(FALSE);
		}
	}
}

BOOL CImgViewerView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (::GetKeyState(VK_CONTROL) < 0)
		{
			switch (pMsg->wParam)
			{
			case 99: case 67:	// ctrl - C
			{
				OnContextCopy();
				break;
			}
			case 118: case 86:	// ctrl - V
			{
				ctrl = true;
				OnContextPaste();
				break;
			}
			case 89: case 121:	// ctrl - Y
			{
				redo();
				break;
			}
			case 90: case 122:	//ctrl - Z
			{
				undo();
				break;
			}
			}
		}
		else if (::GetKeyState(VK_DELETE) < 0)
		{
			OnContextDelete();
		}
	}
	return CView::PreTranslateMessage(pMsg);
}

void CImgViewerView::undo() {
	if (rollbackIndex != -1) {
		switch (rollback[rollbackIndex].rollbackmode)
		{
		case RollBackMode::Create: {
			data.RemoveAt(rollback[rollbackIndex].idx);
			for (int i = 1; i < zOrder.size(); i++) {
				if (zOrder[i] > zOrder[0])
					zOrder[i]--;
			}
			zOrder.erase(zOrder.begin());
			rollbackIndex--;
			SelectIndex = -1;
			selectID = FALSE;
			Invalidate(FALSE);
			break;
		}
		case RollBackMode::Delete: {
			data.Add(rollback[rollbackIndex--].undoShape);
			zOrder.insert(zOrder.begin(), data.GetSize() - 1);
			SelectIndex = zOrder[0];
			break;
		}
		case RollBackMode::Update: {
			data[rollback[rollbackIndex].idx] = rollback[rollbackIndex].undoShape;
			SelectIndex = rollback[rollbackIndex].idx;
			rollbackIndex--;
			break;
		}
		default:
			break;
		}
		Invalidate(FALSE);
	}
}

void CImgViewerView::redo() {
	if (rollbackIndex + 1 < rollback.size())
	{
		if (rollback.size() == rollbackIndex) {
			int sibalsakiya;
		}
		switch (rollback[++rollbackIndex].rollbackmode)
		{
		case RollBackMode::Create: {
			data.Add(rollback[rollbackIndex].undoShape);
			zOrder.insert(zOrder.begin(), data.GetSize() - 1);
			SelectIndex = zOrder[0];
			break;
		}
		case RollBackMode::Delete: {

			int idx = -1;
			data.RemoveAt(rollback[rollbackIndex].idx);
			zOrder.erase(zOrder.begin());
			SelectIndex = -1;
			selectID = FALSE;
			Invalidate(FALSE);
			break;
		}
		case RollBackMode::Update: {
			data[rollback[rollbackIndex].idx] = rollback[rollbackIndex].redoShape;
			SelectIndex = rollback[rollbackIndex].idx;
			break;
		}
		default:
			break;
		}
		Invalidate(FALSE);
	}
}

void CImgViewerView::OnFileSaveWithshape()
{
	selectID = FALSE;
	SelectIndex = -1;
	Invalidate(FALSE);

	static TCHAR BASED_CODE szFilter[] = _T("이미지 파일(*.BMP, *.GIF, *.JPG, *PNG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.png;*.gif; |모든파일(*.*)|*.*||");

	CFileDialog fsDlg(FALSE, _T("png"), NULL, OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, szFilter, NULL);
	if (fsDlg.DoModal() == IDOK)
	{
		CString fileSave = fsDlg.GetPathName();  //파일경로 얻어와서

		CImage image;
		image.Attach(m_background);
		image.Save(fileSave, Gdiplus::ImageFormatJPEG);
	}
}

void CImgViewerView::OnFileSaveOnlyimg()
{
	static TCHAR BASED_CODE szFilter[] = _T("이미지 파일(*.BMP, *.GIF, *.JPG, *PNG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.png;*.gif; |모든파일(*.*)|*.*||");

	CFileDialog fsDlg(FALSE, _T("png"), NULL, OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, szFilter, NULL);
	if (fsDlg.DoModal() == IDOK)
	{
		CString fileSave = fsDlg.GetPathName();  //파일경로 얻어와서

		CImage image;
		m_background.Detach();
		m_background.Attach(::CopyImage(result_bmp, IMAGE_BITMAP, 0, 0, 0));
		image.Attach(m_background);
		image.Save(fileSave, Gdiplus::ImageFormatJPEG);
	}
}

int CImgViewerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	idx = 0;
	rollbackIndex = -1;
	SelectIndex = -1;
	ctrl = false;
	iscopy = false;
	RectCount = 0;
	drawID = FALSE;
	penID = FALSE;
	selectID = FALSE;
	EdgeSelect = FALSE;
	rotateID = FALSE;
	drawStyle = DrawMode::None;


	return 0;
}


BOOL CImgViewerView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (!m_Algorithm.isReady())
	{
		CBrush backBrush(RGB(255, 255, 255));

		CBrush* pOldBrush = pDC->SelectObject(&backBrush);
		CRect rect; pDC->GetClipBox(&rect);
		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
		pDC->SelectObject(pOldBrush);

		return TRUE;
	}

	return FALSE;
}

void CImgViewerView::imgViewer2Navigator()
{
	float x = z_pos.x + (m_ePt.x) / PWidth;
	float y = z_pos.y + (m_ePt.y) / PHeight;
	float Width = z_pos.x + (m_ePt.x + m_bgRect.right) / PWidth - x;
	float Height = z_pos.y + (m_ePt.y + m_bgRect.bottom) / PHeight - y;

	theApp.pNavigatorView->GetRectPos(x, y, Width, Height);
}

void CImgViewerView::GetImgPos(float Navigator_x, float Navigator_y)
{
	CPoint Navigator_pos;
	Navigator_pos.x = Navigator_x * m_Bitmap.bmWidth;
	Navigator_pos.y = Navigator_y * m_Bitmap.bmHeight;

	z_pos.x = Navigator_pos.x - zoomWidth / 2;
	z_pos.y = Navigator_pos.y - zoomHeight / 2;

	if (z_pos.x < 0)
		z_pos.x = 0;
	if (z_pos.y < 0)
		z_pos.y = 0;

	if (z_pos.x + zoomWidth + 5 >= m_Bitmap.bmWidth && zoomWidth <= m_Bitmap.bmWidth)
	{
		z_pos.x = m_Bitmap.bmWidth - zoomWidth - 5;
		m_ePt.x = PWidth * (int)(zoomWidth + 2) - m_bgRect.right;
	}

	if (z_pos.y + zoomHeight + 1 >= m_Bitmap.bmHeight && zoomHeight <= m_Bitmap.bmHeight)
	{
		z_pos.y = m_Bitmap.bmHeight - zoomHeight - 1;
		m_ePt.y = PHeight * (int)(zoomHeight + 2) - m_bgRect.bottom;
	}

	imgViewer2Navigator();
	Invalidate();
	UpdateWindow();
}


int CImgViewerView::isLeft(Point2d linePt1, Point2d linePt2, CPoint pos)
{
	return ((linePt2.x - linePt1.x) * (pos.y - linePt1.y) - (pos.x - linePt1.x) * (linePt2.y - linePt1.y));
}

int CImgViewerView::isContainPolygon(CPoint pos, vector<Point2d> vertices)
{
	int wideNum = 0;

	for (int i = 0; i < vertices.size(); i++) {
		int nextpos = (i + 1 >= vertices.size()) ? 0 : i + 1;
		if (vertices[i].y <= pos.y) {
			if (vertices[nextpos].y > pos.y)
				if (isLeft(vertices[i], vertices[nextpos], pos) > 0)
					++wideNum;
		}
		else {
			if (vertices[nextpos].y <= pos.y)
				if (isLeft(vertices[i], vertices[nextpos], pos) < 0)
					--wideNum;
		}
	}
	return wideNum;
}

void CImgViewerView::SelectShapeUpdate()
{
	if (selectID == FALSE)
		return;

	//  중심 업데이트
	Point2d pt(0, 0);
	RectCount = data[SelectIndex].pts.size();
	for (int i = 0; i < RectCount; i++)
		pt += data[SelectIndex].pts[i];

	data[SelectIndex].Center = pt / RectCount;
	data[SelectIndex].Rotate = data[SelectIndex].Center;
	data[SelectIndex].Rotate.y -= 15;

	// 사각 업데이트
	for (int i = 0; i < RectCount; i++)
	{
		pt = data[SelectIndex].pts[i];
		data[SelectIndex].Rect[i].SetRect(pt.x - 5, pt.y - 5, pt.x + 5, pt.y + 5);
	}

	// 각도 업데이트
	data[SelectIndex].RotatePts = data[SelectIndex].pts;
	data[SelectIndex].R_theta = data[SelectIndex].theta;

	if (data[SelectIndex].shapeType == DrawMode::DEllipse)
	{
		pt = data[SelectIndex].pts[0] - data[SelectIndex].pts[1];
		data[SelectIndex].radin[0] = sqrt(pow(pt.x, 2) + pow(pt.y, 2)) / 2;
		pt = data[SelectIndex].pts[1] - data[SelectIndex].pts[2];
		data[SelectIndex].radin[1] = sqrt(pow(pt.x, 2) + pow(pt.y, 2)) / 2;
	}

}

Point2d CImgViewerView::Intersection(Point2d& pt, Point2d& LinePt1, Point2d& LinePt2)
{
	Point2d result_2d;
	Point2d result;
	double A, B, t_A, t_B;
	if ((LinePt1.x - LinePt2.x) != 0 && LinePt1.y - LinePt2.y != 0) {
		A = (LinePt1.y - LinePt2.y) / (LinePt1.x - LinePt2.x);
		B = LinePt1.y - A * LinePt1.x;
		t_A = -1 / A;
		t_B = pt.y - t_A * pt.x;

		result_2d.x = (t_B - B) / (A - t_A);
		result_2d.y = result_2d.x * A + B;
		result.x = result_2d.x;
		result.y = result_2d.y;
		return result;
	}
	else if (LinePt1.x - LinePt2.x == 0) {
		result.x = LinePt1.x;
		result.y = pt.y;
		return result;
	}
	else if (LinePt1.y - LinePt2.y == 0) {
		result.x = pt.x;
		result.y = LinePt1.y;
		return result;
	}
}

// 처음 그리기
void CImgViewerView::draw(CDC* pDC)
{
	CPen* pen = new CPen(PS_SOLID, 1, RGB(0,255,0));
	CPen* oldPen;
	oldPen = pDC->SelectObject(pen);
	pDC->SelectStockObject(NULL_BRUSH);

	if (drawStyle == DrawMode::DLine) // 콤보상자에서 직선 선택시
	{
		pDC->MoveTo(d_sPt.x, d_sPt.y); pDC->LineTo(mov_Pt.x, mov_Pt.y);
	}
	else if (drawStyle == DrawMode::DRectangle) // 콤보상자에서 사각형 선택시
	{
		pDC->Rectangle(d_sPt.x, d_sPt.y, mov_Pt.x, mov_Pt.y);
	}
	else if (drawStyle == DrawMode::DEllipse) // 콤보상자에서 원 선택시
	{
		pDC->Ellipse(d_sPt.x, d_sPt.y, mov_Pt.x, mov_Pt.y);
	}
	else if (drawStyle == DrawMode::DTriangle) // 콤보상자에서 사각형 선택시
	{
		pDC->MoveTo((d_sPt.x + mov_Pt.x) / 2, d_sPt.y);
		pDC->LineTo(m_pos.x, mov_Pt.y);
		pDC->LineTo(d_sPt.x, mov_Pt.y);
		pDC->LineTo((d_sPt.x + mov_Pt.x) / 2, d_sPt.y);
	}
	else if (drawStyle == DrawMode::DCrossHair) // 콤보상자에서 사각형 선택시
	{
		pDC->MoveTo((d_sPt.x + mov_Pt.x) / 2, d_sPt.y);
		pDC->LineTo((d_sPt.x + mov_Pt.x) / 2, mov_Pt.y);
		pDC->MoveTo(d_sPt.x, (d_sPt.y + mov_Pt.y) / 2);
		pDC->LineTo(m_pos.x, (d_sPt.y + mov_Pt.y) / 2);
	}
	pDC->SelectObject(oldPen); // 이전 팬 선택		
	pen->DeleteObject();  // 생성한 펜 메모리에서 제거			
}

// 그림 복구
void CImgViewerView::drawShape(CDC* pDC, MyShape& shape)
{
	int shapeType = shape.shapeType;
	CPen* pen = new CPen(PS_SOLID, shape.penWidth, shape.shapeColor);
	CPen* oldPen;
	oldPen = pDC->SelectObject(pen);

	if (shapeType == DrawMode::DLine)
	{
		pDC->MoveTo(shape.pts[0].x, shape.pts[0].y);
		pDC->LineTo(shape.pts[1].x, shape.pts[1].y);
	}
	else if (shapeType == DrawMode::DEllipse) // 원 그리기
	{
		MyEllipseR(pDC, shape.Center, shape.radin[0], shape.radin[1], shape.theta, shape.shapeColor);
	}
	else if (shapeType == DrawMode::DRectangle)
	{
		pDC->MoveTo(shape.pts[0].x, shape.pts[0].y);
		pDC->LineTo(shape.pts[1].x, shape.pts[1].y);
		pDC->LineTo(shape.pts[2].x, shape.pts[2].y);
		pDC->LineTo(shape.pts[3].x, shape.pts[3].y);
		pDC->LineTo(shape.pts[0].x, shape.pts[0].y);
	}
	else if (shapeType == DrawMode::DTriangle)
	{
		pDC->MoveTo(shape.pts[0].x, shape.pts[0].y);
		pDC->LineTo(shape.pts[1].x, shape.pts[1].y);
		pDC->LineTo(shape.pts[2].x, shape.pts[2].y);
		pDC->LineTo(shape.pts[0].x, shape.pts[0].y);
	}
	else if (shapeType == DrawMode::DCrossHair)
	{
		pDC->MoveTo(shape.pts[0].x, shape.pts[0].y);
		pDC->LineTo(shape.pts[1].x, shape.pts[1].y);
		pDC->MoveTo(shape.pts[2].x, shape.pts[2].y);
		pDC->LineTo(shape.pts[3].x, shape.pts[3].y);
	}
	pDC->SelectObject(oldPen); // 이전 팬 선택		
	pen->DeleteObject();  // 생성한 펜 메모리에서 제거		
}

// 선택 그리기
void CImgViewerView::SelectDrawShape(CDC* pDC, MyShape& shape)
{
	// 선택 실선
	CPen* pen = new CPen(PS_DOT, shape.penWidth, BLACK_PEN);
	CPen* oldPen;
	Point2d Center = shape.Center;
	Point2d Rotate = shape.Rotate;
	oldPen = pDC->SelectObject(pen);
	if (shape.shapeType == DrawMode::DLine) // 콤보상자에서 직선 선택시
	{
		pDC->MoveTo(shape.pts[0].x, shape.pts[0].y);
		pDC->LineTo(shape.pts[1].x, shape.pts[1].y);
	}
	else if (shape.shapeType == DrawMode::DRectangle) // 콤보상자에서 사각형 선택시
	{
		pDC->MoveTo(shape.pts[0].x, shape.pts[0].y);
		pDC->LineTo(shape.pts[1].x, shape.pts[1].y);
		pDC->LineTo(shape.pts[2].x, shape.pts[2].y);
		pDC->LineTo(shape.pts[3].x, shape.pts[3].y);
		pDC->LineTo(shape.pts[0].x, shape.pts[0].y);
	}
	else if (shape.shapeType == DrawMode::DEllipse)
	{
		MyEllipsePS_DOT(pDC, shape.Center, shape.radin[0], shape.radin[1], shape.R_theta);
	}
	else if (shape.shapeType == DrawMode::DTriangle) // 콤보상자에서 사각형 선택시
	{
		pDC->MoveTo(shape.pts[0].x, shape.pts[0].y);
		pDC->LineTo(shape.pts[1].x, shape.pts[1].y);
		pDC->LineTo(shape.pts[2].x, shape.pts[2].y);
		pDC->LineTo(shape.pts[0].x, shape.pts[0].y);
	}
	else if (shape.shapeType == DrawMode::DCrossHair) // 콤보상자에서 크로스 헤어 선택시
	{
		pDC->MoveTo(shape.pts[0].x, shape.pts[0].y);
		pDC->LineTo(shape.pts[1].x, shape.pts[1].y);
		pDC->MoveTo(shape.pts[2].x, shape.pts[2].y);
		pDC->LineTo(shape.pts[3].x, shape.pts[3].y);
	}
	pDC->SelectObject(oldPen);
	pen->DeleteObject();

	// 확대 사각 박스 그리기
	pDC->SelectStockObject(WHITE_BRUSH);
	for (int i = 0; i < RectCount; i++)
		pDC->Rectangle(shape.Rect[i]);
	pDC->SelectStockObject(NULL_BRUSH);

	// 회전 직선 그리기
	pen = new CPen(PS_SOLID, 1, RGB(0, 0, 255));
	oldPen = pDC->SelectObject(pen);
	pDC->MoveTo(Center.x, Center.y);
	pDC->LineTo(Rotate.x, Rotate.y);
	pDC->SelectObject(oldPen);
	pen->DeleteObject();

	// 회전 선택
	if (rotateID == TRUE) {
		pen = new CPen(PS_SOLID, shape.penWidth, RGB(0, 255, 0));
		oldPen = pDC->SelectObject(pen);
		RectCount = shape.pts.size();
		if (shape.shapeType == DrawMode::DCrossHair)
		{
			pDC->MoveTo(shape.RotatePts[0].x, shape.RotatePts[0].y);
			pDC->LineTo(shape.RotatePts[1].x, shape.RotatePts[1].y);
			pDC->MoveTo(shape.RotatePts[2].x, shape.RotatePts[2].y);
			pDC->LineTo(shape.RotatePts[3].x, shape.RotatePts[3].y);
		}
		else if (shape.shapeType == DrawMode::DEllipse)
		{
			MyEllipseR(pDC, shape.Center, shape.radin[0], shape.radin[1], shape.R_theta, shape.shapeColor);
		}
		else
		{
			pDC->MoveTo(data[SelectIndex].RotatePts[0].x, data[SelectIndex].RotatePts[0].y);
			for (int i = 1; i < data[SelectIndex].RotatePts.size(); i++)
				pDC->LineTo(data[SelectIndex].RotatePts[i].x, data[SelectIndex].RotatePts[i].y);
			pDC->LineTo(data[SelectIndex].RotatePts[0].x, data[SelectIndex].RotatePts[0].y);
		}
		Rotate.x = d_sPt.x;
		Rotate.y = d_sPt.y;

		pDC->SelectObject(oldPen);
		pen->DeleteObject();
	}
	if (penID == TRUE) {
		pen = new CPen(PS_SOLID, shape.penWidth, RGB(0, 255, 0));
		oldPen = pDC->SelectObject(pen);
		RectCount = shape.pts.size();
		if (shape.RotatePts.size() > 0)
		{
			// 사각형 OR 직선
			if (shape.shapeType == DrawMode::DCrossHair)
			{
				pDC->MoveTo(shape.RotatePts[0].x, shape.RotatePts[0].y);
				pDC->LineTo(shape.RotatePts[1].x, shape.RotatePts[1].y);
				pDC->MoveTo(shape.RotatePts[2].x, shape.RotatePts[2].y);
				pDC->LineTo(shape.RotatePts[3].x, shape.RotatePts[3].y);
			}
			else if (shape.shapeType == DrawMode::DEllipse)
			{
				Point2d RotateCenter(0, 0);
				for (size_t i = 0; i < RectCount; i++)
				{
					RotateCenter.x += shape.RotatePts[i].x;
					RotateCenter.y += shape.RotatePts[i].y;
				}
				MyEllipseR(pDC, RotateCenter / RectCount, shape.radin[0], shape.radin[1], shape.theta, RGB(0, 255, 0));
			}
			else
			{
				pDC->MoveTo(data[SelectIndex].RotatePts[0].x, data[SelectIndex].RotatePts[0].y);
				for (int i = 1; i < data[SelectIndex].RotatePts.size(); i++)
					pDC->LineTo(data[SelectIndex].RotatePts[i].x, data[SelectIndex].RotatePts[i].y);
				pDC->LineTo(data[SelectIndex].RotatePts[0].x, data[SelectIndex].RotatePts[0].y);
			}

		}
		pDC->SelectObject(oldPen);
		pen->DeleteObject();
	}
}

void CImgViewerView::MyEllipseR(CDC* pDC, Point2d Center, int radinX, int radinY, double theta, COLORREF parm_color)
{
	// 직선 알고리즘 추가
	CPen* pen = new CPen(PS_SOLID, 1, parm_color);
	CPen* oldPen = pDC->SelectObject(pen);
	CPoint pt;
	CPoint Rotatept;
	pt.x = Center.x + int(sin(0 * CV_PI / 180) * radinX);
	pt.y = Center.y + int(cos(0 * CV_PI / 180) * radinY);
	Rotatept.x = cvRound((pt.x - Center.x) * cos(theta) - (pt.y - Center.y) * sin(theta) + Center.x);
	Rotatept.y = cvRound((pt.x - Center.x) * sin(theta) + (pt.y - Center.y) * cos(theta) + Center.y);
	pDC->SetPixel(Rotatept, parm_color);
	for (int i = 1; i < 360; i++) {
		pDC->MoveTo(Rotatept);
		pt.x = Center.x + int(sin(i * CV_PI / 180) * radinX);
		pt.y = Center.y + int(cos(i * CV_PI / 180) * radinY);

		Rotatept.x = cvRound((pt.x - Center.x) * cos(theta) - (pt.y - Center.y) * sin(theta) + Center.x);
		Rotatept.y = cvRound((pt.x - Center.x) * sin(theta) + (pt.y - Center.y) * cos(theta) + Center.y);

		pDC->LineTo(Rotatept);
		pDC->SetPixel(Rotatept, parm_color);
	}
	pDC->SelectObject(oldPen);
	pen->DeleteObject();
}

void CImgViewerView::MyEllipsePS_DOT(CDC* pDC, Point2d Center, int radinX, int radinY, double theta)
{
	// 직선 알고리즘 추가
	int cnt = 0;
	CPoint pt;
	CPoint Rotatept;

	CPen* pen1 = new CPen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* pen2 = new CPen(PS_SOLID, 1, RGB(255, 255, 255));
	CPen* oldPen;

	pt.x = Center.x + int(sin(0 * CV_PI / 180) * radinX);
	pt.y = Center.y + int(cos(0 * CV_PI / 180) * radinY);
	Rotatept.x = cvRound((pt.x - Center.x) * cos(theta) - (pt.y - Center.y) * sin(theta) + Center.x);
	Rotatept.y = cvRound((pt.x - Center.x) * sin(theta) + (pt.y - Center.y) * cos(theta) + Center.y);
	pDC->SetPixel(Rotatept, RGB(0, 0, 0));

	for (int i = 1; i < 360; i++) {
		if (i % 5 == 0)
			cnt++;
		if (cnt % 2 == 0) {
			pDC->MoveTo(Rotatept);
			pt.x = Center.x + int(sin(i * CV_PI / 180) * radinX);
			pt.y = Center.y + int(cos(i * CV_PI / 180) * radinY);

			Rotatept.x = cvRound((pt.x - Center.x) * cos(theta) - (pt.y - Center.y) * sin(theta) + Center.x);
			Rotatept.y = cvRound((pt.x - Center.x) * sin(theta) + (pt.y - Center.y) * cos(theta) + Center.y);

			oldPen = pDC->SelectObject(pen1);
			pDC->SelectStockObject(NULL_BRUSH);
			pDC->LineTo(Rotatept);
			pDC->SetPixel(Rotatept, RGB(0, 0, 0));
		}
		else
		{
			pDC->MoveTo(Rotatept);
			pt.x = Center.x + int(sin(i * CV_PI / 180) * radinX);
			pt.y = Center.y + int(cos(i * CV_PI / 180) * radinY);

			Rotatept.x = cvRound((pt.x - Center.x) * cos(theta) - (pt.y - Center.y) * sin(theta) + Center.x);
			Rotatept.y = cvRound((pt.x - Center.x) * sin(theta) + (pt.y - Center.y) * cos(theta) + Center.y);

			oldPen = pDC->SelectObject(pen2);
			pDC->SelectStockObject(NULL_BRUSH);
			pDC->LineTo(Rotatept);
			pDC->SetPixel(Rotatept, RGB(255, 255, 255));
		}
	}
	pDC->SelectObject(oldPen);
	pen1->DeleteObject();
	pen2->DeleteObject();
}

void CImgViewerView::OnClose()
{
	data.RemoveAll();
	CView::OnClose();
}


void CImgViewerView::OnSelectLw()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	
}


void CImgViewerView::OnSelectColor()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CColorDialog cDlg;
	if (cDlg.DoModal() == IDOK)
	{
		color = cDlg.GetColor();
	}
}
