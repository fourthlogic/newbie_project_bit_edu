
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
#include "CSelectLineWidth.h"
#define IsCTRLPressed()  ( 0x8000 ==(GetKeyState(VK_CONTROL) & 0x8000 ))
//콘솔 출력
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif


IMPLEMENT_DYNCREATE(CImgViewerView, CView)

BEGIN_MESSAGE_MAP(CImgViewerView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
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
	ON_COMMAND(ID_DRAW_TRI, &CImgViewerView::OnDrawTri)
	ON_COMMAND(ID_MODE_SELECT, &CImgViewerView::OnModeSelect)
	ON_COMMAND(ID_CONTEXT_COPY, &CImgViewerView::OnContextCopy)
	ON_COMMAND(ID_CONTEXT_PASTE, &CImgViewerView::OnContextPaste)
	ON_COMMAND(ID_CONTEXT_DELETE, &CImgViewerView::OnContextDelete)
	ON_COMMAND(ID_CONTEXT_LINECOLOR, &CImgViewerView::OnContextLinecolor)
	ON_COMMAND(ID_CONTEXT_LINEWIDTH, &CImgViewerView::OnContextLinewidth)
	ON_COMMAND(ID_FILE_SAVE_WITHSHAPE, &CImgViewerView::OnFileSaveWithshape)
	ON_COMMAND(ID_FILE_SAVE_ONLYIMG, &CImgViewerView::OnFileSaveOnlyimg)
	ON_COMMAND(ID_SELECT_LW, &CImgViewerView::OnSelectLw)
	ON_COMMAND(ID_SELECT_COLOR, &CImgViewerView::OnSelectColor)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

template <typename T>
BOOL IsContainVector(T data, vector<T> list) {
	for (T element : list) {
		if (element == data) {
			return true;
		}
	}
	return false;
}
template <typename T>
BOOL IsContainVector(T data, vector<T> list, int& idx) {
	for (int i = 0; i < list.size(); i++) {
		if (list[i] == data) {
			idx = i;
			return true;
		}
	}
	return false;
}

Point2d Perpendicular_intersection(Point2d pt, Point2d LinePt1, Point2d LinePt2)
{
	Point2d result;
	Matx<double, 2, 1> m;
	Matx22d m1;
	Matx21d m2(1, 1);
	double A, B, t_A, t_B;
	if ((LinePt1.x - LinePt2.x) != 0 && LinePt1.y - LinePt2.y != 0) {
		A = (LinePt1.y - LinePt2.y) / (LinePt1.x - LinePt2.x);
		B = LinePt1.y - A * LinePt1.x;
		t_A = -1 / A;
		t_B = pt.y - t_A * pt.x;

		m1 << A, -1, t_A, -1;
		m2 << B, t_B;
		m = -m1.inv() * m2;
		result.x = m.val[0];
		result.y = m.val[1];

	}
	else if (LinePt1.x == LinePt2.x)
	{
		result.x = pt.x;
		result.y = LinePt1.y;
	}
	else if (LinePt1.y == LinePt2.y)
	{
		result.x = LinePt1.x;
		result.y = pt.y;
	}
	return result;
}

template <typename T>
T my_greater(T num1, T num2, T& smaller) {
	if (num1 > num2) {
		smaller = num2;
		return num1;
	}
	smaller = num1;
	return num2;
}


Point2d intersection(Point2d p1, Point2d p2, Point2d p3, Point2d p4) {
	// Store the values for fast access and easy
	// equations-to-code conversion
	double x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
	double y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;

	double d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	// If d is zero, there is no intersection
	if (d == 0) return NULL;

	// Get the x and y
	double pre = (x1 * y2 - y1 * x2), post = (x3 * y4 - y3 * x4);
	double x = (pre * (x3 - x4) - (x1 - x2) * post) / d;
	double y = (pre * (y3 - y4) - (y1 - y2) * post) / d;

	// Check if the x and y coordinates are within both lines
	//if (x < min(x1, x2) || x > max(x1, x2) ||
	//	x < min(x3, x4) || x > max(x3, x4)) return NULL;
	//if (y < min(y1, y2) || y > max(y1, y2) ||
	//	y < min(y3, y4) || y > max(y3, y4)) return NULL;

	// Return the point of intersection
	return Point2d(x, y);
}


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
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)); //icon 변경
	GetParent()->SetIcon(hIcon, FALSE); //icon 셋팅

}

void CImgViewerView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CWnd::OnMouseMove(nFlags, point);
	m_nFlags = nFlags;
	// 좌표 업데이트
	u_img_pos.x = z_pos.x + (int)((m_ePt.x + point.x) / PWidth);
	u_img_pos.y = z_pos.y + (int)((m_ePt.y + point.y) / PHeight);

	u_pix_pos.x = (((m_ePt.x + point.x) / PWidth - (int)((m_ePt.x + point.x) / PWidth)));
	u_pix_pos.y = (((m_ePt.y + point.y) / PHeight - (int)((m_ePt.y + point.y) / PHeight)));

	u_pos = u_img_pos + u_pix_pos;

	m_pos.x = point.x;
	m_pos.y = point.y;

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
				}

				//이미지 왼쪽으로 나갈때
				if ((z_pos.x == (int)(-((m_bgRect.right / 5 * 4) / PWidth) - 1) && m_ePt.x <= PWidth * (1 - ((((m_bgRect.right / 5 * 4) / PWidth) - (int)((m_bgRect.right / 5 * 4) / PWidth))))) || z_pos.x < (int)(-((m_bgRect.right / 5 * 4) / PWidth) - 1))
				{
					z_pos.x = -((m_bgRect.right / 5 * 4) / PWidth) - 1;
					m_ePt.x = PWidth * (1 - ((((m_bgRect.right / 5 * 4) / PWidth) - (int)((m_bgRect.right / 5 * 4) / PWidth))));
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

				//이미지 오른쪽으로 나갈때(고쳐야댐)
				if ((z_pos.x == (int)(m_Bitmap.bmWidth - ((m_bgRect.right / 5 * 1) / PWidth)) && m_ePt.x >= PWidth * (1 - ((((m_bgRect.right / 5) / PWidth) - (int)((m_bgRect.right / 5) / PWidth))))) || z_pos.x > (int)(m_Bitmap.bmWidth - ((m_bgRect.right / 5) / PWidth)))
				{
					z_pos.x = m_Bitmap.bmWidth - ((m_bgRect.right / 5) / PWidth);
					m_ePt.x = PWidth * (1 - ((((m_bgRect.right / 5) / PWidth) - (int)((m_bgRect.right / 5) / PWidth))));
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

				//이미지 아래쪽으로 나갈때(고쳐야댐)
				if ((z_pos.y == (int)(m_Bitmap.bmHeight - ((m_bgRect.bottom / 5 * 1) / PHeight)) && m_ePt.y >= PHeight * (1 - ((((m_bgRect.bottom / 5) / PHeight) - (int)((m_bgRect.bottom / 5) / PHeight))))) || z_pos.y > (int)(m_Bitmap.bmHeight - ((m_bgRect.bottom / 5) / PHeight)))
				{
					z_pos.y = m_Bitmap.bmHeight - ((m_bgRect.bottom / 5) / PHeight);
					m_ePt.y = PHeight * (1 - ((((m_bgRect.bottom / 5) / PHeight) - (int)((m_bgRect.bottom / 5) / PHeight))));
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
				}

				//이미지 위쪽으로 나갈때
				if ((z_pos.y == (int)(-((m_bgRect.bottom / 5 * 4) / PHeight) - 1) && m_ePt.y <= PHeight * (1 - ((((m_bgRect.bottom / 5 * 4) / PHeight) - (int)((m_bgRect.bottom / 5 * 4) / PHeight))))) || z_pos.y < (int)(-((m_bgRect.bottom / 5 * 4) / PHeight) - 1))
				{
					z_pos.y = -((m_bgRect.bottom / 5 * 4) / PHeight) - 1;
					m_ePt.y = PHeight * (1 - ((((m_bgRect.bottom / 5 * 4) / PHeight) - (int)((m_bgRect.bottom / 5 * 4) / PHeight))));
				}
			}
		}


		//m_ePt가 Pixel보다 크거나 작아질때
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
		//-----------------------------

		m_sPt.x = point.x;
		m_sPt.y = point.y;
		//----------------------------------------------------------------
		imgViewer2Navigator();

		Invalidate(FALSE);
		UpdateWindow();
	}
	//복사
	if (ctrlSelect && IsCTRLPressed()) {
		double o, p;
		double xx, yy;

		xx = d_pos.x, yy = d_pos.y;

		o = (u_pos.x - xx);
		p = (u_pos.y - yy);

		d_pos.x += o;
		d_pos.y += p;

		u_pos.x += o;
		u_pos.y += p;
		if (ctrlCopyList.empty())
		{
			for (int i = 0; i < GroupList.size(); i++) {
				ctrlCopyList.push_back(data[GroupList[i]]);
			}
			return;
		}
		else
		{
			for (int j = 0; j < ctrlCopyList.size(); j++) {
				for (int i = 0; i < RectCount; i++)
				{
					ctrlCopyList[j].RotatePts[i].x += o;
					ctrlCopyList[j].RotatePts[i].y += p;
				}
			}
			UpdateWindow();
			Invalidate(FALSE);
			return;
		}
	}
	if (MK_LBUTTON == nFlags && drawID == TRUE) // 마우스를 클릭하여 드래그일 동안만 
		Invalidate(FALSE);

	// 크기 조정
	else if (MK_LBUTTON == nFlags && (EdgeSelect == TRUE))
	{
		double o, p;
		double xx, yy;
		//CPoint TLX;
		xx = d_pos.x, yy = d_pos.y;

		o = (u_pos.x - xx);
		p = (u_pos.y - yy);

		d_pos.x += o;
		d_pos.y += p;

		u_pos.x += o;
		u_pos.y += p;

		if (IsContainVector(SelectIndex, GroupList)) {
			for (int j = 0; j < GroupList.size(); j++) {
				double theta = data[GroupList[j]].theta;

				RectCount = data[GroupList[j]].pts.size();

				// 길이
				if (data[GroupList[j]].shapeType == DrawMode::DLine)
				{
					data[GroupList[j]].RotatePts[EdgeIndex].x += o;
					data[GroupList[j]].RotatePts[EdgeIndex].y += p;
				}
				// 삼각형
				else if (data[GroupList[j]].shapeType == DrawMode::DTriangle)
				{
					if (data[GroupList[j]].theta == 0) {

						if (EdgeIndex == 0)
						{
							data[GroupList[j]].RotatePts[2].x += o;
							data[GroupList[j]].RotatePts[0].y += p;
						}
						else if (EdgeIndex == 1)
						{

							data[GroupList[j]].RotatePts[1].x += o;
							data[GroupList[j]].RotatePts[0].y += p;
						}
						else if (EdgeIndex == 2)
						{
							data[GroupList[j]].RotatePts[1].x += o;
							data[GroupList[j]].RotatePts[1].y += p;

							data[GroupList[j]].RotatePts[2].y += p;

							data[GroupList[j]].RotatePts[0].x = (data[GroupList[j]].RotatePts[1].x + data[GroupList[j]].RotatePts[2].x) / 2;
						}
						else if (EdgeIndex == 3)
						{
							data[GroupList[j]].RotatePts[0].x += o;
							data[GroupList[j]].RotatePts[2].y += p;
						}
					}
				}
				//사각형 원
				else
				{
					data[GroupList[j]].RotatePts[EdgeIndex].x += o;
					data[GroupList[j]].RotatePts[EdgeIndex].y += p;

					if (data[GroupList[j]].theta == 0)
					{
						if (EdgeIndex == 0)
						{
							data[GroupList[j]].RotatePts[3].x += o;
							data[GroupList[j]].RotatePts[1].y += p;
						}
						else if (EdgeIndex == 1)
						{

							data[GroupList[j]].RotatePts[2].x += o;
							data[GroupList[j]].RotatePts[0].y += p;
						}
						else if (EdgeIndex == 2)
						{
							data[GroupList[j]].RotatePts[1].x += o;
							data[GroupList[j]].RotatePts[3].y += p;
						}
						else if (EdgeIndex == 3)
						{
							data[GroupList[j]].RotatePts[0].x += o;
							data[GroupList[j]].RotatePts[2].y += p;
						}
					}
					else
					{
						if (EdgeIndex == 0)
						{
							data[GroupList[j]].RotatePts[1] = Intersection(data[GroupList[j]].RotatePts[0], data[GroupList[j]].RotatePts[1], data[GroupList[j]].RotatePts[2]);
							data[GroupList[j]].RotatePts[3] = Intersection(data[GroupList[j]].RotatePts[0], data[GroupList[j]].RotatePts[3], data[GroupList[j]].RotatePts[2]);
						}
						else if (EdgeIndex == 1)
						{
							data[GroupList[j]].RotatePts[0] = Intersection(data[GroupList[j]].RotatePts[1], data[GroupList[j]].RotatePts[0], data[GroupList[j]].RotatePts[3]);
							data[GroupList[j]].RotatePts[2] = Intersection(data[GroupList[j]].RotatePts[1], data[GroupList[j]].RotatePts[2], data[GroupList[j]].RotatePts[3]);
						}
						else if (EdgeIndex == 2)
						{
							data[GroupList[j]].RotatePts[1] = Intersection(data[GroupList[j]].RotatePts[2], data[GroupList[j]].RotatePts[1], data[GroupList[j]].RotatePts[0]);
							data[GroupList[j]].RotatePts[3] = Intersection(data[GroupList[j]].RotatePts[2], data[GroupList[j]].RotatePts[3], data[GroupList[j]].RotatePts[0]);
						}
						else if (EdgeIndex == 3)
						{
							data[GroupList[j]].RotatePts[0] = Intersection(data[GroupList[j]].RotatePts[3], data[GroupList[j]].RotatePts[0], data[GroupList[j]].RotatePts[1]);
							data[GroupList[j]].RotatePts[2] = Intersection(data[GroupList[j]].RotatePts[3], data[GroupList[j]].RotatePts[2], data[GroupList[j]].RotatePts[1]);
						}
					}
				}

			}
		}
		else {
			double theta = data[SelectIndex].theta;

			RectCount = data[SelectIndex].RotatePts.size();

			// 길이
			if (data[SelectIndex].shapeType == DrawMode::DLine)
			{
				data[SelectIndex].RotatePts[EdgeIndex].x += o;
				data[SelectIndex].RotatePts[EdgeIndex].y += p;
			}
			// 삼각형
			else if (data[SelectIndex].shapeType == DrawMode::DTriangle)
			{
				if (true/*data[SelectIndex].theta == 0*/) {

					if (EdgeIndex == 0)
					{
						double dX, dY;
						double a, b, c = 1;
						dX = data[SelectIndex].RotatePts[0].x - data[SelectIndex].RotatePts[2].x;
						dY = data[SelectIndex].RotatePts[0].y - data[SelectIndex].RotatePts[2].y;
						/*if ((data[SelectIndex].RotatePts[0].x == data[SelectIndex].RotatePts[2].x || data[SelectIndex].RotatePts[0].y == data[SelectIndex].RotatePts[2].y)  &&
							(data[SelectIndex].RotatePts[0].x == data[SelectIndex].RotatePts[1].x || data[SelectIndex].RotatePts[0].y == data[SelectIndex].RotatePts[1].y)  &&
							(data[SelectIndex].RotatePts[2].x == data[SelectIndex].RotatePts[1].x || data[SelectIndex].RotatePts[2].y == data[SelectIndex].RotatePts[1].y)
							)
							return;*/
						if (dX == 0)
						{
							a = 0;

						}
						else if (dY == 0)
						{
							c = 0;
							a = -1;
						}
						else {
							a = dY / dX;
						}

						double dX2, dY2;
						double a2, b2, c2 = 1;
						dX2 = data[SelectIndex].RotatePts[0].x - data[SelectIndex].RotatePts[1].x;
						dY2 = data[SelectIndex].RotatePts[0].y - data[SelectIndex].RotatePts[1].y;

						if (dX2 == 0)
						{
							a2 = 0;

						}
						else if (dY2 == 0)
						{
							c2 = 0;
							a2 = -1;
						}
						else {
							a2 = dY2 / dX2;
						}

						data[SelectIndex].RotatePts[0].x += o;
						data[SelectIndex].RotatePts[0].y += p;
						b = -a * data[SelectIndex].RotatePts[0].x + c * data[SelectIndex].RotatePts[0].y;
						b2 = -a2 * data[SelectIndex].RotatePts[0].x + c2 * data[SelectIndex].RotatePts[0].y;


						if (a == 0)
							data[SelectIndex].RotatePts[2] = intersection(data[SelectIndex].RotatePts[0], Point2d((c - b) / a, 1), data[SelectIndex].RotatePts[1], data[SelectIndex].RotatePts[2]);
						else
							data[SelectIndex].RotatePts[2] = intersection(data[SelectIndex].RotatePts[0], Point2d(1, (a + b) / c), data[SelectIndex].RotatePts[1], data[SelectIndex].RotatePts[2]);
						if (a2 == 0)
							data[SelectIndex].RotatePts[1] = intersection(data[SelectIndex].RotatePts[0], Point2d((c2 - b2) / a2, 1), data[SelectIndex].RotatePts[1], data[SelectIndex].RotatePts[2]);
						else
							data[SelectIndex].RotatePts[1] = intersection(data[SelectIndex].RotatePts[0], Point2d(1, (a2 + b2) / c2), data[SelectIndex].RotatePts[1], data[SelectIndex].RotatePts[2]);
						double ddx = data[SelectIndex].pts[1].x - data[SelectIndex].RotatePts[1].x;
						double ddy = data[SelectIndex].pts[1].y - data[SelectIndex].RotatePts[1].y;
						for (int i = 0; i < 3; i++) {
							data[SelectIndex].RotatePts[i].x += ddx;
							data[SelectIndex].RotatePts[i].y += ddy;
						}
					}
					else if (EdgeIndex == 1)
					{

						double dX, dY;
						double a, b, c = 1;
						dX = data[SelectIndex].RotatePts[0].x - data[SelectIndex].RotatePts[1].x;
						dY = data[SelectIndex].RotatePts[0].y - data[SelectIndex].RotatePts[1].y;

						if (dX == 0)
						{
							a = 0;

						}
						else if (dY == 0)
						{
							c = 0;
							a = -1;
						}
						else {
							a = dY / dX;
						}

						double dX2, dY2;
						double a2, b2, c2 = 1;
						dX2 = data[SelectIndex].RotatePts[0].x - data[SelectIndex].RotatePts[2].x;
						dY2 = data[SelectIndex].RotatePts[0].y - data[SelectIndex].RotatePts[2].y;

						if (dX2 == 0)
						{
							a2 = 0;

						}
						else if (dY2 == 0)
						{
							c2 = 0;
							a2 = -1;
						}
						else {
							a2 = dY2 / dX2;
						}

						data[SelectIndex].RotatePts[0].x += o;
						data[SelectIndex].RotatePts[0].y += p;
						b = -a * data[SelectIndex].RotatePts[0].x + c * data[SelectIndex].RotatePts[0].y;
						b2 = -a2 * data[SelectIndex].RotatePts[0].x + c2 * data[SelectIndex].RotatePts[0].y;
						if (a == 0)
							data[SelectIndex].RotatePts[1] = intersection(data[SelectIndex].RotatePts[0], Point2d((c - b) / a, 1), data[SelectIndex].RotatePts[1], data[SelectIndex].RotatePts[2]);
						else
							data[SelectIndex].RotatePts[1] = intersection(data[SelectIndex].RotatePts[0], Point2d(1, (a + b) / c), data[SelectIndex].RotatePts[1], data[SelectIndex].RotatePts[2]);
						if (a2 == 0)
							data[SelectIndex].RotatePts[2] = intersection(data[SelectIndex].RotatePts[0], Point2d((c2 - b2) / a2, 1), data[SelectIndex].RotatePts[1], data[SelectIndex].RotatePts[2]);
						else
							data[SelectIndex].RotatePts[2] = intersection(data[SelectIndex].RotatePts[0], Point2d(1, (a2 + b2) / c2), data[SelectIndex].RotatePts[1], data[SelectIndex].RotatePts[2]);
						double ddx = data[SelectIndex].pts[2].x - data[SelectIndex].RotatePts[2].x;
						double ddy = data[SelectIndex].pts[2].y - data[SelectIndex].RotatePts[2].y;
						for (int i = 0; i < 3; i++) {
							data[SelectIndex].RotatePts[i].x += ddx;
							data[SelectIndex].RotatePts[i].y += ddy;
						}
					}
					else if (EdgeIndex == 2)
					{
						data[SelectIndex].RotatePts[1].x += o;
						data[SelectIndex].RotatePts[1].y += p;

						data[SelectIndex].RotatePts[2].y += p;

						data[SelectIndex].RotatePts[0].x = (data[SelectIndex].RotatePts[1].x + data[SelectIndex].RotatePts[2].x) / 2;
					}
					else if (EdgeIndex == 3)
					{
						data[SelectIndex].RotatePts[0].x += o;
						data[SelectIndex].RotatePts[2].y += p;
					}
				}
			}
			//사각형 원
			else
			{

				data[SelectIndex].RotatePts[EdgeIndex].x += o;
				data[SelectIndex].RotatePts[EdgeIndex].y += p;

				if (data[SelectIndex].theta == 0)
				{
					if (EdgeIndex == 0)
					{
						data[SelectIndex].RotatePts[3].x += o;
						data[SelectIndex].RotatePts[1].y += p;
					}
					else if (EdgeIndex == 1)
					{

						data[SelectIndex].RotatePts[2].x += o;
						data[SelectIndex].RotatePts[0].y += p;
					}
					else if (EdgeIndex == 2)
					{
						data[SelectIndex].RotatePts[1].x += o;
						data[SelectIndex].RotatePts[3].y += p;
					}
					else if (EdgeIndex == 3)
					{
						data[SelectIndex].RotatePts[0].x += o;
						data[SelectIndex].RotatePts[2].y += p;
					}
				}
				else
				{
					if (EdgeIndex == 0)
					{
						data[SelectIndex].RotatePts[1] = Intersection(data[SelectIndex].RotatePts[0], data[SelectIndex].RotatePts[1], data[SelectIndex].RotatePts[2]);
						data[SelectIndex].RotatePts[3] = Intersection(data[SelectIndex].RotatePts[0], data[SelectIndex].RotatePts[3], data[SelectIndex].RotatePts[2]);
					}
					else if (EdgeIndex == 1)
					{
						data[SelectIndex].RotatePts[0] = Intersection(data[SelectIndex].RotatePts[1], data[SelectIndex].RotatePts[0], data[SelectIndex].RotatePts[3]);
						data[SelectIndex].RotatePts[2] = Intersection(data[SelectIndex].RotatePts[1], data[SelectIndex].RotatePts[2], data[SelectIndex].RotatePts[3]);
					}
					else if (EdgeIndex == 2)
					{
						data[SelectIndex].RotatePts[1] = Intersection(data[SelectIndex].RotatePts[2], data[SelectIndex].RotatePts[1], data[SelectIndex].RotatePts[0]);
						data[SelectIndex].RotatePts[3] = Intersection(data[SelectIndex].RotatePts[2], data[SelectIndex].RotatePts[3], data[SelectIndex].RotatePts[0]);
					}
					else if (EdgeIndex == 3)
					{
						data[SelectIndex].RotatePts[0] = Intersection(data[SelectIndex].RotatePts[3], data[SelectIndex].RotatePts[0], data[SelectIndex].RotatePts[1]);
						data[SelectIndex].RotatePts[2] = Intersection(data[SelectIndex].RotatePts[3], data[SelectIndex].RotatePts[2], data[SelectIndex].RotatePts[1]);
					}
				}
			}

		}
		// 꼭지점 업데이트
		//SelectShapeUpdate();
		Invalidate(FALSE);
	}
	// 회전 클릭
	else if (MK_LBUTTON == nFlags && (rotateID == TRUE))
	{
		Point2d center = data[SelectIndex].Center;
		Point2d Rotate = data[SelectIndex].Rotate;
		Point2d e_center;
		if (data[SelectIndex].shapeType != DrawMode::DTriangle)
			e_center = data[SelectIndex].pts[EdgeIndex];
		else {
			if (EdgeIndex == 2 || EdgeIndex == 3) {
				e_center = data[SelectIndex].pts[EdgeIndex - 1];
			}
			else {
				double dX, dY;
				double a, b, c = 1;
				dX = data[SelectIndex].pts[1].x - data[SelectIndex].pts[2].x;
				dY = data[SelectIndex].pts[1].y - data[SelectIndex].pts[2].y;

				if (dX == 0)
				{
					a = 0;

				}
				else if (dY == 0)
				{
					c = 0;
					a = -1;
				}
				else {
					a = dY / dX;
				}
				b = -a * data[SelectIndex].pts[0].x + c * data[SelectIndex].pts[0].y;

				if (EdgeIndex == 0) {
					if (a != 0) {
						e_center = Perpendicular_intersection(data[SelectIndex].pts[2], data[SelectIndex].pts[0], Point2d((c - b) / a, 1));
					}
					else {

						e_center = Perpendicular_intersection(data[SelectIndex].pts[2], data[SelectIndex].pts[0], Point2d(1, (a + b) / c));
					}
				}
				else {
					if (a != 0) {

						e_center = Perpendicular_intersection(data[SelectIndex].pts[1], data[SelectIndex].pts[0], Point2d((c - b) / a, 1));
					}
					else {
						e_center = Perpendicular_intersection(data[SelectIndex].pts[1], data[SelectIndex].pts[0], Point2d(1, (a + b) / c));
					}
				}
			}
		}

		double theta;
		theta = atan2((d_pos.x - e_center.x), (d_pos.y - e_center.y)) - atan2((u_pos.x - e_center.x), (u_pos.y - e_center.y));
				

		if (IsContainVector(SelectIndex, GroupList)) {
			for (int j = 0; j < GroupList.size(); j++) {
				vector<Point2d> pts = data[GroupList[j]].pts; // 복사 -> 이동 -> R복사
				Point2d pt;
				for (int i = 0; i < pts.size(); i++)
				{
					pt.x = ((pts[i].x - data[GroupList[j]].Center.x) * cos(theta) - (pts[i].y - data[GroupList[j]].Center.y) * sin(theta) + data[GroupList[j]].Center.x);
					pt.y = ((pts[i].x - data[GroupList[j]].Center.x) * sin(theta) + (pts[i].y - data[GroupList[j]].Center.y) * cos(theta) + data[GroupList[j]].Center.y);
					
					//pt.x = ((pts[i].y - data[GroupList[j]].Center.y) * sin(theta) + (pts[i].x - data[GroupList[j]].Center.x) * cos(theta) + data[GroupList[j]].Center.x);
					//pt.y = ((pts[i].y - data[GroupList[j]].Center.y) * cos(theta) - (pts[i].x - data[GroupList[j]].Center.x) * sin(theta) + data[GroupList[j]].Center.y);
					pts[i] = pt;
				}
				data[GroupList[j]].RotatePts = pts;
				data[GroupList[j]].R_theta = theta + data[GroupList[j]].theta;
			}
		}
		else {
			vector<Point2d> pts = data[SelectIndex].pts; // 복사 -> 이동 -> R복사
			Point2d pt;
			for (int i = 0; i < pts.size(); i++)
			{
				pt.x = ((pts[i].x - center.x) * cos(theta) - (pts[i].y - center.y) * sin(theta) + center.x);
				pt.y = ((pts[i].x - center.x) * sin(theta) + (pts[i].y - center.y) * cos(theta) + center.y);
				
				//pt.x = ((pts[i].y - center.y) * sin(theta) + (pts[i].x - center.x) * cos(theta) + center.x);
				//pt.y = ((pts[i].y - center.y) * cos(theta) - (pts[i].x - center.x) * sin(theta) + center.y);
				pts[i] = pt;
			}
			data[SelectIndex].RotatePts = pts;
			data[SelectIndex].R_theta = theta + data[SelectIndex].theta;
		}
		Invalidate(FALSE);
	}
	// 이동
	else if (MK_LBUTTON == nFlags && (selectID == TRUE))
	{

		double o, p;
		double xx, yy;

		xx = d_pos.x, yy = d_pos.y;

		o = (u_pos.x - xx);
		p = (u_pos.y - yy);

		d_pos.x += o;
		d_pos.y += p;

		u_pos.x += o;
		u_pos.y += p;
		
		if (IsContainVector(SelectIndex, GroupList)) {
			for (int j = 0; j < GroupList.size(); j++) {
				if (GroupList[j] == SelectIndex)
					continue;
				RectCount = data[GroupList[j]].pts.size();
				for (int i = 0; i < RectCount; i++)
				{
					data[GroupList[j]].RotatePts[i].x += o;
					data[GroupList[j]].RotatePts[i].y += p;
				}
			}
		}
		Point2d pt;
		RectCount = data[SelectIndex].pts.size();
		for (int i = 0; i < RectCount; i++)
		{
			data[SelectIndex].RotatePts[i].x += o;
			data[SelectIndex].RotatePts[i].y += p;
		}
		UpdateWindow();
		Invalidate(FALSE);
	}
	else if (MK_LBUTTON == nFlags && penID)
		Invalidate(FALSE);
	else if (IsCTRLPressed())
		Invalidate(FALSE);

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

	//---------------------
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

	before_Image_pos.x = z_pos.x + (int)((m_ePt.x + m_pos.x) / PWidth);
	before_Image_pos.y = z_pos.y + (int)((m_ePt.y + m_pos.y) / PHeight);

	before_Pixel_pos.x = (((m_ePt.x + m_pos.x) / PWidth - (int)((m_ePt.x + m_pos.x) / PWidth)));
	before_Pixel_pos.y = (((m_ePt.y + m_pos.y) / PHeight - (int)((m_ePt.y + m_pos.y) / PHeight)));


	//왼쪽위 꼭짓점
	z_pos.x += (((m_ePt.x + m_pos.x) / (int)(PWidth * ((int)zoomWidth + 2))) * zoomWidth)
		- (((m_ePt.x + m_pos.x) / (int)(PWidth * ((int)zoomWidth + 2))) * (zoomWidth * zoomScale));
	z_pos.y += (((m_ePt.y + m_pos.y) / (int)(PHeight * ((int)zoomHeight + 2))) * zoomHeight)
		- (((m_ePt.y + m_pos.y) / (int)(PHeight * ((int)zoomHeight + 2))) * (zoomHeight * zoomScale));



	//원본에서 출력될 길이
	zoomWidth = zoomWidth * zoomScale;
	zoomHeight = zoomHeight * zoomScale;

	printWidth = zoomWidth * zoomView;
	printHeight = zoomHeight * zoomView;

	//픽셀 크기
	PWidth = (printWidth) / (zoomWidth);
	PHeight = (printHeight) / (zoomHeight);

	after_Image_pos.x = z_pos.x + (int)((m_ePt.x + m_pos.x) / PWidth);
	after_Image_pos.y = z_pos.y + (int)((m_ePt.y + m_pos.y) / PHeight);

	after_Pixel_pos.x = (((m_ePt.x + m_pos.x) / PWidth - (int)((m_ePt.x + m_pos.x) / PWidth)));
	after_Pixel_pos.y = (((m_ePt.y + m_pos.y) / PHeight - (int)((m_ePt.y + m_pos.y) / PHeight)));

	if (before_Image_pos.x - (after_Image_pos.x) != 0)
	{
		z_pos.x += before_Image_pos.x - (after_Image_pos.x);
	}
	if (before_Pixel_pos.x != after_Pixel_pos.x)
	{
		m_ePt.x += (before_Pixel_pos.x * PWidth) - (after_Pixel_pos.x * PWidth);
	}

	if (before_Image_pos.y - (after_Image_pos.y) != 0)
	{
		z_pos.y += before_Image_pos.y - (after_Image_pos.y);
	}
	if (before_Pixel_pos.y != after_Pixel_pos.y)
	{
		m_ePt.y += (before_Pixel_pos.y * PHeight) - (after_Pixel_pos.y * PHeight);
	}
	//----------------------------------
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


	//-----------------------------------------------

	//이미지 왼쪽으로 나갈때
	if ((z_pos.x == (int)(-((m_bgRect.right / 5 * 4) / PWidth) - 1) && m_ePt.x <= PWidth * (1 - ((((m_bgRect.right / 5 * 4) / PWidth) - (int)((m_bgRect.right / 5 * 4) / PWidth))))) || z_pos.x < (int)(-((m_bgRect.right / 5 * 4) / PWidth) - 1))
	{
		z_pos.x = -((m_bgRect.right / 5 * 4) / PWidth) - 1;
		m_ePt.x = PWidth * (1 - ((((m_bgRect.right / 5 * 4) / PWidth) - (int)((m_bgRect.right / 5 * 4) / PWidth))));
	}

	//이미지 오른쪽으로 나갈때
	if ((z_pos.x == (int)(m_Bitmap.bmWidth - ((m_bgRect.right / 5 * 1) / PWidth)) && m_ePt.x >= PWidth * (1 - ((((m_bgRect.right / 5) / PWidth) - (int)((m_bgRect.right / 5) / PWidth))))) || z_pos.x > (int)(m_Bitmap.bmWidth - ((m_bgRect.right / 5) / PWidth)))
	{
		z_pos.x = m_Bitmap.bmWidth - ((m_bgRect.right / 5) / PWidth);
		m_ePt.x = PWidth * (1 - ((((m_bgRect.right / 5) / PWidth) - (int)((m_bgRect.right / 5) / PWidth))));
	}

	//이미지 아래쪽으로 나갈때
	if ((z_pos.y == (int)(m_Bitmap.bmHeight - ((m_bgRect.bottom / 5 * 1) / PHeight)) && m_ePt.y >= PHeight * (1 - ((((m_bgRect.bottom / 5) / PHeight) - (int)((m_bgRect.bottom / 5) / PHeight))))) || z_pos.y > (int)(m_Bitmap.bmHeight - ((m_bgRect.bottom / 5) / PHeight)))
	{
		z_pos.y = m_Bitmap.bmHeight - ((m_bgRect.bottom / 5) / PHeight);
		m_ePt.y = PHeight * (1 - ((((m_bgRect.bottom / 5) / PHeight) - (int)((m_bgRect.bottom / 5) / PHeight))));
	}

	//이미지 위쪽으로 나갈때
	if ((z_pos.y == (int)(-((m_bgRect.bottom / 5 * 4) / PHeight) - 1) && m_ePt.y <= PHeight * (1 - ((((m_bgRect.bottom / 5 * 4) / PHeight) - (int)((m_bgRect.bottom / 5 * 4) / PHeight))))) || z_pos.y < (int)(-((m_bgRect.bottom / 5 * 4) / PHeight) - 1))
	{
		z_pos.y = -((m_bgRect.bottom / 5 * 4) / PHeight) - 1;
		m_ePt.y = PHeight * (1 - ((((m_bgRect.bottom / 5 * 4) / PHeight) - (int)((m_bgRect.bottom / 5 * 4) / PHeight))));
	}
	//-----------------------------------

	if (m_ePt.x < 0)
	{
		z_pos.x -= 1;
		m_ePt.x += PWidth;
	}
	if (m_ePt.y < 0)
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
	m_sPt.x = point.x;
	m_sPt.y = point.y;
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

	if (PWidth > 1)
		mdcOffScreen.SetStretchBltMode(COLORONCOLOR);
	else
		mdcOffScreen.SetStretchBltMode(HALFTONE);
	mdcOffScreen.StretchBlt(0, 0, PWidth * ((int)zoomWidth + 2), PHeight * ((int)zoomHeight + 2), &memDC, z_pos.x, z_pos.y, zoomWidth + 2, zoomHeight + 2, SRCCOPY);

	PrintText(&mdcOffScreen);

	if (CircleCenter.size() > 0) {
		DrawCircle(&mdcOffScreen, CircleCenter);
		DrawCirLines(&mdcOffScreen, CirLinePoints);
		DrawCirPoint(&mdcOffScreen, IntersectionPoint);
	}

	// 그림 그리기
	if (drawShapeID == TRUE) {
		draw(&mdcOffScreen);
	}
	if (penID) {
		draw(&mdcOffScreen);
	}
	for (int i = 0; i < GroupList.size(); i++) {
		SelectDrawShape(&mdcOffScreen, data[GroupList[i]]);
	}
	if (selectID == TRUE || !GroupList.empty())
	{
		if (selectID)
			SelectDrawShape(&mdcOffScreen, data[SelectIndex]);
		// 그림 복원
		for (int i = 0; i < data.GetSize(); i++)
		{
			if (i == SelectIndex)
				continue;
			if (IsContainVector(i, GroupList))
				continue;
			drawShape(&mdcOffScreen, data[i]);
		}
	}
	else
	{
		// 그림 복원
		for (int i = 0; i < data.GetSize(); i++)
			drawShape(&mdcOffScreen, data[i]);
	}

	pDC.SetStretchBltMode(HALFTONE);
	pDC.StretchBlt(0, 0, m_bgRect.right, m_bgRect.bottom, &mdcOffScreen, m_ePt.x, m_ePt.y, m_bgRect.right, m_bgRect.bottom, SRCCOPY);

	if (Save_Shape) {
		CImage image;
		image.Attach(m_background);
		image.Save(fileSave, Gdiplus::ImageFormatJPEG);
		Save_Shape = FALSE;
	}

	m_background.DeleteObject();
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
	CircleCenter.clear();
	CirLinePoints.clear();
	IntersectionPoint = { 0, 0 };
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
		z_pos.x = -(m_bgRect.right / 2 - (m_Bitmap.bmWidth * zoomView) / 2);
	}
	else
	{
		z_pos.x = 0;
	}

	if (m_Bitmap.bmHeight * zoomView <= m_bgRect.bottom)
	{
		z_pos.y = -(m_bgRect.bottom / 2 - (m_Bitmap.bmHeight * zoomView) / 2);
	}
	else
	{
		z_pos.y = 0;
	}
	theApp.pNavigatorView->OnFileOpen();
	Invalidate(FALSE);
}

void CImgViewerView::paraChanged() // 이미지 처리 및 버퍼에 붙이기
{
	if (!m_Algorithm.isReady())
		return;

	m_Algorithm.Run();
	CircleCenter = m_Algorithm.GetCirclePoint();
	CirLinePoints = m_Algorithm.GetCirLinePoints();
	IntersectionPoint = m_Algorithm.GetIntersectionPoint();
	//result_mat = m_Algorithm.GetResultImage();
	//result_bmp = m_Algorithm.MatToBitmap(result_mat);
	//
	//m_background.Detach();
	//m_background.Attach(result_bmp);
	//m_background.GetBitmap(&m_Bitmap);

	Invalidate(FALSE);
}

void CImgViewerView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu popup;
	if (result_mat.empty()) {
		popup.LoadMenuW(IDR_MAINFRAME);
		pMenu = popup.GetSubMenu(0);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN || TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	}
	else if ((penID) || !GroupList.empty())
	{
		popup.LoadMenuW(IDR_MENU_CONTEXT);
		pMenu = popup.GetSubMenu(0);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN || TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	}
	else
	{
		popup.LoadMenuW(IDR_MENU_IMAGEVIEW);
		pMenu = popup.GetSubMenu(1);

		pMenu->TrackPopupMenu(TPM_LEFTALIGN || TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	}
}


void CImgViewerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMainFrame* frame = (CMainFrame*)AfxGetMainWnd();
	SetCapture();
	m_nFlags = nFlags;
	// Down 버튼 저장

	d_sPt.x = point.x;
	d_sPt.y = point.y;

	d_img_pos.x = z_pos.x + (int)((m_ePt.x + point.x) / PWidth);
	d_img_pos.y = z_pos.y + (int)((m_ePt.y + point.y) / PHeight);

	d_pix_pos.x = (((m_ePt.x + point.x) / PWidth - (int)((m_ePt.x + point.x) / PWidth)));
	d_pix_pos.y = (((m_ePt.y + point.y) / PHeight - (int)((m_ePt.y + point.y) / PHeight)));
	
	d_pos = d_img_pos + d_pix_pos;

	// darw 상태 검사
	if (drawID == TRUE && drawStyle != DrawMode::None) {
		drawShapeID = TRUE;
		return;
	}
	// 엣지 클릭 검사
	// 회전 선 클릭 검사
	if (!GroupList.empty()) {
		for (int j = 0; j < GroupList.size(); j++) {
			RectCount = data[GroupList[j]].pts.size();
			if (data[GroupList[j]].shapeType == DrawMode::DTriangle)
				RectCount++;
			for (int i = 0; i < RectCount; i++)
			{
				vector<vector<Point2d>> tedge, redge;
				tedge.resize(data[GroupList[j]].edge.size());
				redge.resize(data[GroupList[j]].R_edge.size());
				for (int a = 0; a < data[GroupList[j]].edge.size(); a++) {
					tedge[i].push_back(Point2d(data[GroupList[j]].edge[i][a].x - m_ePt.x, data[GroupList[j]].edge[i][a].y - m_ePt.y));
					redge[i].push_back(Point2d(data[GroupList[j]].R_edge[i][a].x - m_ePt.x, data[GroupList[j]].R_edge[i][a].y - m_ePt.y));
				}

				if (isContainPolygon(point, tedge[i])) {
					EdgeSelect = TRUE;
					EdgeIndex = i;
					Invalidate(FALSE);
					return;
				}
				else if (isContainPolygon(point, redge[i])) {
					rotatePts = Point2d(d_pos.x, d_pos.y);
					rotateID = TRUE;
					EdgeIndex = i;
					Invalidate(FALSE);
					return;
				}
			}
		}
	}
	if (selectID == TRUE) {
		if (!GroupList.empty()) {
			for (int j = 0; j < GroupList.size(); j++) {
				int rcCount = data[GroupList[j]].pts.size();
				for (int i = 0; i < rcCount; i++)
				{
					/*CRect& rect = data[GroupList[j]].Rect[i];
					if (d_pos.x >= rect.left && d_pos.x <= rect.right && d_pos.y >= rect.top && d_pos.y <= rect.bottom)
					{
						EdgeSelect = TRUE;
						EdgeIndex = i;
						Invalidate(FALSE);
						return;
					}*/
				}
			}
		}
		else {
			//엣지 클릭 검사
			RectCount = data[SelectIndex].pts.size();
			if (data[SelectIndex].shapeType == DrawMode::DTriangle)
				RectCount++;
			//엣지 클릭 검사
			for (int i = 0; i < RectCount; i++)
			{
				vector<vector<Point2d>> tedge, redge;
				tedge.resize(data[SelectIndex].edge.size());
				redge.resize(data[SelectIndex].R_edge.size());
				for (int a = 0; a < data[SelectIndex].edge.size(); a++) {
					tedge[i].push_back(Point2d(data[SelectIndex].edge[i][a].x - m_ePt.x, data[SelectIndex].edge[i][a].y - m_ePt.y));
					redge[i].push_back(Point2d(data[SelectIndex].R_edge[i][a].x - m_ePt.x, data[SelectIndex].R_edge[i][a].y - m_ePt.y));
				}

				if (isContainPolygon(point, tedge[i])) {
					EdgeSelect = TRUE;
					EdgeIndex = i;
					Invalidate(FALSE);
					return;
				}
				else if (isContainPolygon(point, redge[i])) {
					rotatePts = Point2d(d_pos.x, d_pos.y);
					rotateID = TRUE;
					EdgeIndex = i;
					Invalidate(FALSE);
					return;
				}
			}
		}
	}

	// 이미지 객체 클릭 검사
	for (int i = 0; i < data.GetSize(); i++)
	{
		int zIndex = zOrder[i];
		if (data[zOrder[i]].shapeType != DrawMode::DLine) {
			BOOL isInShape = FALSE;
			if (data[zOrder[i]].shapeType == DrawMode::DEllipse) {
				double rad = fabs((data[zOrder[i]].pts[0].x - data[zOrder[i]].pts[1].x) / 2.f);
				if (sqrt(pow(data[zOrder[i]].Center.x - d_pos.x, 2) + pow(data[zOrder[i]].Center.y - d_pos.y, 2)) <= rad)
					isInShape = TRUE;
			}
			else if (isContainPolygon(Point2d(d_pos.x, d_pos.y), data[zOrder[i]].pts, data[zOrder[i]].shapeType)) {
				isInShape = TRUE;
			}
			if (isInShape)
			{
				Point pt;
				RectCount = data[zOrder[i]].pts.size();
				for (int j = 0; j < RectCount; j++)
				{
					pt.x = data[zIndex].pts[j].x;
					pt.y = data[zIndex].pts[j].y;
					//data[zIndex].Rect[j].SetRect(pt.x , pt.y , pt.x , pt.y );
				}

				if (selectID && IsCTRLPressed() && SelectIndex != zOrder[i] && GroupList.empty()) {
					GroupList.push_back(SelectIndex);
					GroupList.push_back(zOrder[i]);
				}

				temp = data[zOrder[i]];
				zOrder.insert(zOrder.begin(), zOrder[i]);
				zOrder.erase(zOrder.begin() + i + 1);
				if (IsCTRLPressed() && !IsContainVector(zOrder[0], GroupList)) {
					GroupList.push_back(zOrder[0]);
				}
				else if (IsCTRLPressed()) {
					if (GroupList.empty() && SelectIndex == zOrder[0])
						ctrlSelect = TRUE;
					else if (IsContainVector(zOrder[0], GroupList)) {
						SelectIndex = zOrder[0];
						ctrlSelect = TRUE;
						selectID = FALSE;
						return;
					}
				}
				else if (!IsCTRLPressed() && !IsContainVector(zOrder[0], GroupList)) {
					GroupList.clear();
				}
				selectID = TRUE;
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

			if (isContainPolygon(Point2d(d_pos.x, d_pos.y), linePts))
			{
				CPoint pt;
				RectCount = data[zIndex].pts.size();
				for (int j = 0; j < RectCount; j++)
				{
					pt.x = data[zIndex].pts[j].x;
					pt.y = data[zIndex].pts[j].y;
					//data[zIndex].Rect[j].SetRect(pt.x , pt.y , pt.x , pt.y );
				}

				penID = TRUE;
				if (selectID && IsCTRLPressed() && SelectIndex != zOrder[i] && GroupList.empty()) {
					GroupList.push_back(SelectIndex);
					GroupList.push_back(zOrder[i]);
				}

				temp = data[zOrder[i]];
				zOrder.insert(zOrder.begin(), zOrder[i]);
				zOrder.erase(zOrder.begin() + i + 1);
				if (IsCTRLPressed() && !IsContainVector(zOrder[0], GroupList)) {
					GroupList.push_back(zOrder[0]);
				}
				else if (!IsCTRLPressed() && !IsContainVector(zOrder[0], GroupList)) {
					GroupList.clear();
				}

				selectID = TRUE;
				SelectIndex = zOrder[0];
				Invalidate(FALSE);
				return;
			}
		}
	}
	if (IsCTRLPressed() && selectID) {
		selectID = FALSE;
		return;
	}
	selectID = FALSE;
	// 없을 경우 초기화
	SelectIndex = -1;
	CView::OnLButtonDown(nFlags, point);
}

void CImgViewerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	//CMainFrame* frame = (CMainFrame*)AfxGetMainWnd();

	ReleaseCapture();
	m_nFlags = nFlags;

	//복사
	if (ctrlSelect && IsCTRLPressed()) {
		if (!ctrlCopyList.empty()) {
			RollbackInfo gr;
			gr.rollbackmode = RollBackMode::GroupStart;
			if (rollbackIndex != -1)
				rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
			else if (rollback.size() != 0) {
				rollback.erase(rollback.begin(), rollback.end());
			}
			rollback.push_back(gr);
			int groupIndex = data.GetSize();
			for (int j = 0; j < ctrlCopyList.size(); j++) {
				ctrlCopyList[j].pts = ctrlCopyList[j].RotatePts;
				data.Add(ctrlCopyList[j]);
				zOrder.insert(zOrder.begin(), data.GetSize() - 1);;
				SelectIndex = zOrder[0];
				RollbackInfo info;
				info.redoShape = data[SelectIndex];
				info.idx = SelectIndex;
				info.undoShape = data[SelectIndex];
				info.rollbackmode = RollBackMode::Create;

				rollback.push_back(info);
			}
			ctrlCopyList.clear();
			GroupList.clear();
			for (int i = groupIndex; i < data.GetSize(); i++) {
				GroupList.push_back(i);
			}
			gr.rollbackmode = RollBackMode::GroupEnd;
			rollback.push_back(gr);
			rollbackIndex = rollback.size() - 1;
		}
		ctrlSelect = FALSE;

		return;
	}
	// 도형그리기
	if (drawShapeID == TRUE)  // 도형 그리기 상태일 동안만
	{
		//if (drawStyle == DrawMode::None)
		//   return;

		shape.pts.clear();

		mov_Pt.x = point.x;
		mov_Pt.y = point.y;

		// 마우스 버트을 놓으면 각종 값을 shape 구조체 맴버변수에 저장
		shape.shapeType = drawStyle;  // 도형 콤보 상자에서 선택된 도형 스타일을 저장      
		if (shape.shapeType == DrawMode::DPoint)
		{
			shape.pts.push_back(Point2d(d_pos.x, d_pos.y));
		}
		else if (shape.shapeType == DrawMode::DLine)
		{
			shape.pts.push_back(Point2d(d_pos.x, d_pos.y));
			shape.pts.push_back(Point2d(u_pos.x, u_pos.y));
		}
		else if (shape.shapeType == DrawMode::DEllipse)
		{
			double width = 0, height = 0;
			//가로가 길 경우
			if (abs(u_pos.x - d_pos.x) > abs(u_pos.y - d_pos.y))
			{
				width = ((u_pos.x - d_pos.x) / 2) - ((u_pos.y - d_pos.y) / 2);
				d_pos.x += width;
				u_pos.x -= width;
				
				shape.pts.push_back(Point2d(d_pos.x, d_pos.y));		//0
				shape.pts.push_back(Point2d(u_pos.x, d_pos.y));		//1
				shape.pts.push_back(Point2d(u_pos.x, u_pos.y));		//2
				shape.pts.push_back(Point2d(d_pos.x, u_pos.y));		//3
			}
			//세로가 길 경우
			else
			{
				height = ((u_pos.y - d_pos.y) / 2) - ((u_pos.x - d_pos.x) / 2);
				d_pos.y += height;
				u_pos.y -= height;

				shape.pts.push_back(Point2d(d_pos.x, d_pos.y));		//0
				shape.pts.push_back(Point2d(u_pos.x, d_pos.y));		//1
				shape.pts.push_back(Point2d(u_pos.x, u_pos.y));		//2
				shape.pts.push_back(Point2d(d_pos.x, u_pos.y));		//3
			}
			//shape.pts.push_back(Point2d(d_pos.x, d_pos.y));
			//shape.pts.push_back(Point2d(u_pos.x, d_pos.y));
			//shape.pts.push_back(Point2d(u_pos.x, u_pos.y));
			//shape.pts.push_back(Point2d(d_pos.x, u_pos.y));
		}
		else if (shape.shapeType == DrawMode::DRectangle)
		{
			shape.pts.push_back(Point2d(d_pos.x, d_pos.y));		//0
			shape.pts.push_back(Point2d(u_pos.x, d_pos.y));		//1
			shape.pts.push_back(Point2d(u_pos.x, u_pos.y));		//2
			shape.pts.push_back(Point2d(d_pos.x, u_pos.y));		//3
		}
		else if (shape.shapeType == DrawMode::DTriangle)
		{
			double minX, minY, maxX, maxY;
			maxX = my_greater(d_pos.x, u_pos.x, minX);
			maxY = my_greater(d_pos.y, u_pos.y, minY);

			shape.pts.push_back(Point2d((minX + maxX) / 2, minY));
			shape.pts.push_back(Point2d(maxX, maxY));
			shape.pts.push_back(Point2d(minX, maxY));

		}
		Point2d pt(0, 0);
		shape.radin[0] = abs(pts_0.x - pts_2.x) / 2;
		shape.radin[1] = abs(pts_0.y - pts_2.y) / 2;
		for (int i = 0; i < shape.pts.size(); i++)
			pt += shape.pts[i];

		double xi, xd;
		double yi, yd;
		vector<Point2d> pts(4);
		if (shape.shapeType == DrawMode::DTriangle) {
			for (int i = 0; i < 4; i++) {
				if (i == 2 || i == 3) {

					xd = modf(shape.pts[i - 1].x, &xi);
					yd = modf(shape.pts[i - 1].y, &yi);
				}
				else {
					double dX, dY;
					double a, b, c = 1;
					dX = shape.pts[1].x - shape.pts[2].x;
					dY = shape.pts[1].y - shape.pts[2].y;

					if (dX == 0)
					{
						a = 0;

					}
					else if (dY == 0)
					{
						c = 0;
						a = -1;
					}
					else {
						a = dY / dX;
					}
					b = -a * shape.pts[0].x + c * shape.pts[0].y;
					if (EdgeIndex == 0) {
						if (a != 0) {
							xd = modf(Intersection(shape.pts[2], shape.pts[0], Point2d((c - b) / a, 1)).x, &xi);
							yd = modf(Intersection(shape.pts[2], shape.pts[0], Point2d((c - b) / a, 1)).y, &yi);
						}
						else {
							xd = modf(Intersection(shape.pts[2], shape.pts[0], Point2d(1, (a + b) / c)).x, &xi);
							yd = modf(Intersection(shape.pts[2], shape.pts[0], Point2d(1, (a + b) / c)).y, &yi);
						}
					}
					else if (EdgeIndex == 1) {
						if (a != 0) {
							Point2d test = Intersection(shape.pts[1], shape.pts[0], Point2d((c - b) / a, 1));
							xd = modf(Intersection(shape.pts[1], shape.pts[0], Point2d((c - b) / a, 1)).x, &xi);
							yd = modf(Intersection(shape.pts[1], shape.pts[0], Point2d((c - b) / a, 1)).y, &yi);
						}
						else {
							Point2d test = Intersection(shape.pts[1], shape.pts[0], Point2d(1, (a + b) / c));
							xd = modf(Intersection(shape.pts[1], shape.pts[0], Point2d(1, (a + b) / c)).x, &xi);
							yd = modf(Intersection(shape.pts[1], shape.pts[0], Point2d(1, (a + b) / c)).y, &yi);

						}
					}
				}

				pts[i].x = (PWidth * ((int)xi - z_pos.x)) + (PWidth * xd);
				pts[i].y = (PHeight * ((int)yi - z_pos.y)) + (PHeight * yd);
			}
		}
		else {
			for (int i = 0; i < shape.pts.size(); i++) {
				xd = modf(shape.pts[i].x, &xi);
				yd = modf(shape.pts[i].y, &yi);

				pts[i].x = (PWidth * ((int)xi - z_pos.x)) + (PWidth * xd);
				pts[i].y = (PHeight * ((int)yi - z_pos.y)) + (PHeight * yd);
			}
		}
		if (shape.shapeType == DrawMode::DTriangle) {
			shape.edge.resize(shape.pts.size() + 1);
			shape.R_edge.resize(shape.pts.size() + 1);
		}
		else {
			shape.edge.resize(shape.pts.size());
			shape.R_edge.resize(shape.pts.size());
		}
		for (int i = 0; i < shape.pts.size(); i++) {
			
			shape.edge[i].resize(4);
			shape.edge[i].push_back(Point2d(pts[i].x - 8, pts[i].y - 8));
			shape.edge[i].push_back(Point2d(pts[i].x + 8, pts[i].y - 8));
			shape.edge[i].push_back(Point2d(pts[i].x + 8, pts[i].y + 8));
			shape.edge[i].push_back(Point2d(pts[i].x - 8, pts[i].y + 8));


			shape.R_edge[i].resize(4);

			if (shape.shapeType == DrawMode::DLine) {
				if (i == 0) {
					shape.R_edge[i].push_back(Point2d(pts[i].x - 15, pts[i].y - 15));
					shape.R_edge[i].push_back(Point2d(pts[i].x + 8, pts[i].y - 15));
					shape.R_edge[i].push_back(Point2d(pts[i].x + 8, pts[i].y + 8));
					shape.R_edge[i].push_back(Point2d(pts[i].x - 15, pts[i].y + 8));
				}
				else if (i == 1) {
					shape.R_edge[i].push_back(Point2d(pts[i].x - 8, pts[i].y - 8));
					shape.R_edge[i].push_back(Point2d(pts[i].x + 15, pts[i].y - 8));
					shape.R_edge[i].push_back(Point2d(pts[i].x + 15, pts[i].y + 15));
					shape.R_edge[i].push_back(Point2d(pts[i].x - 8, pts[i].y + 15));
				}
			}
			else
			{
				if (i == 0) {
					shape.R_edge[i].push_back(Point2d(pts[i].x - 15, pts[i].y - 15));
					shape.R_edge[i].push_back(Point2d(pts[i].x + 8, pts[i].y - 15));
					shape.R_edge[i].push_back(Point2d(pts[i].x + 8, pts[i].y + 8));
					shape.R_edge[i].push_back(Point2d(pts[i].x - 15, pts[i].y + 8));
				}
				else if (i == 1) {
					shape.R_edge[i].push_back(Point2d(pts[i].x - 8, pts[i].y - 15));
					shape.R_edge[i].push_back(Point2d(pts[i].x + 15, pts[i].y - 15));
					shape.R_edge[i].push_back(Point2d(pts[i].x + 15, pts[i].y + 8));
					shape.R_edge[i].push_back(Point2d(pts[i].x - 8, pts[i].y + 8));
				}
				else if (i == 2) {
					shape.R_edge[i].push_back(Point2d(pts[i].x - 8, pts[i].y - 8));
					shape.R_edge[i].push_back(Point2d(pts[i].x + 15, pts[i].y - 8));
					shape.R_edge[i].push_back(Point2d(pts[i].x + 15, pts[i].y + 15));
					shape.R_edge[i].push_back(Point2d(pts[i].x - 8, pts[i].y + 15));
				}
				else if (i == 3) {
					shape.R_edge[i].push_back(Point2d(pts[i].x - 15, pts[i].y - 8));
					shape.R_edge[i].push_back(Point2d(pts[i].x + 8, pts[i].y - 8));
					shape.R_edge[i].push_back(Point2d(pts[i].x + 8, pts[i].y + 15));
					shape.R_edge[i].push_back(Point2d(pts[i].x - 15, pts[i].y + 15));
				}

			}
		}

		shape.RotatePts = shape.pts;
		shape.Center = pt / (int)shape.pts.size();
		shape.Rotate = shape.Center;
		shape.Rotate.y -= 15;
		shape.theta = 0;
		shape.R_theta = 0;
		shape.shapeColor = color; // 색상값
		shape.penWidth = l_width; // 굵기 
		data.Add(shape);
		drawShapeID = FALSE;
		selectID = FALSE;

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

	// 회전 업데이트
	else if (rotateID)
	{
		if (IsContainVector(SelectIndex, GroupList)) {
			// 360 조정
			RollbackInfo gr;
			gr.rollbackmode = RollBackMode::GroupStart;
			if (rollbackIndex != -1)
				rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
			rollback.push_back(gr);
			for (int j = 0; j < GroupList.size(); j++) {
				
				rotateID = FALSE;
				RectCount = data[GroupList[j]].pts.size();
				for (int i = 0; i < RectCount; i++)
				{
					if (data[GroupList[j]].pts[i] != data[GroupList[j]].RotatePts[i])
					{
						RollbackInfo info;
						info.idx = GroupList[j];
						info.redoShape = data[GroupList[j]];
						info.undoShape = data[GroupList[j]];
						info.rollbackmode = RollBackMode::Update;
						info.undoShape.pts = data[GroupList[j]].pts;
						info.redoShape.pts = data[GroupList[j]].RotatePts;
						info.undoShape.theta = data[GroupList[j]].theta;
						info.redoShape.theta = data[GroupList[j]].R_theta;
						if (data[GroupList[j]].R_theta >= (CV_PI * 2))
							data[GroupList[j]].R_theta = data[GroupList[j]].R_theta - (CV_PI * 2);
						// 회전각 업데이트
						data[GroupList[j]].pts = data[GroupList[j]].RotatePts;
						data[GroupList[j]].theta = data[GroupList[j]].R_theta;
						rollback.push_back(info);
						break;
					}
				}
			}
			gr.rollbackmode = RollBackMode::GroupEnd;
			rollback.push_back(gr);
			rollbackIndex = rollback.size() - 1;
		}
		else {
			if (data[SelectIndex].R_theta >= (CV_PI * 2))
				data[SelectIndex].R_theta = data[SelectIndex].R_theta - (CV_PI * 2);
			// 회전각 업데이트
			rotateID = FALSE;
			RectCount = data[SelectIndex].pts.size();

			RollbackInfo info;
			info.idx = SelectIndex;
			info.redoShape = data[SelectIndex];
			info.undoShape = temp;
			info.rollbackmode = RollBackMode::Update;

			info.undoShape.pts = data[SelectIndex].pts;
			info.redoShape.pts = data[SelectIndex].RotatePts;
			info.undoShape.theta = data[SelectIndex].theta;
			info.redoShape.theta = data[SelectIndex].R_theta;

			if (rollbackIndex != -1)
				rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
			rollback.push_back(info);
			rollbackIndex = rollback.size() - 1;
			data[SelectIndex].pts = data[SelectIndex].RotatePts;
			data[SelectIndex].theta = data[SelectIndex].R_theta;
		}
	}
	// 이동좌표 업데이트
	else if (selectID)
	{
		if (IsContainVector(SelectIndex, GroupList)) {
			RollbackInfo gr;
			gr.rollbackmode = RollBackMode::GroupStart;
			if (rollbackIndex != -1)
				rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
			rollback.push_back(gr);
			for (int j = 0; j < GroupList.size(); j++) {
				RectCount = data[GroupList[j]].pts.size();
				for (int i = 0; i < RectCount; i++)
				{
					if (data[GroupList[j]].pts[i] != data[GroupList[j]].RotatePts[i])
					{
						RollbackInfo info;
						info.idx = GroupList[j];
						info.redoShape = data[GroupList[j]];
						info.undoShape = data[GroupList[j]];
						info.redoShape.pts = data[GroupList[j]].RotatePts;
						info.undoShape.pts = data[GroupList[j]].pts;
						info.rollbackmode = RollBackMode::Update;
						rollback.push_back(info);
						data[GroupList[j]].pts = data[GroupList[j]].RotatePts;
						break;
					}
				}
			}
			gr.rollbackmode = RollBackMode::GroupEnd;
			rollback.push_back(gr);
			rollbackIndex = rollback.size() - 1;
		}
		else {
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
	}
	//그룹 선택
	else if (penID) {
		vector<Point2d> vertices;
		if (start_pos.x == 0)

		vertices.push_back(Point2d(d_pos.x, d_pos.y));
		vertices.push_back(Point2d(u_pos.x, d_pos.y));
		vertices.push_back(Point2d(u_pos.x, u_pos.y));
		vertices.push_back(Point2d(d_pos.x, u_pos.y));

		if (IsCTRLPressed())
		{
			if (ctrlSelect) {
				int localIdx;
				if (IsContainVector(SelectIndex, GroupList, localIdx)) {
					GroupList.erase(GroupList.begin() + localIdx);
					SelectIndex = -1;
					ctrlSelect = FALSE;
				}
			}
			else if (SelectIndex != -1 && GroupList.empty()) {
				GroupList.push_back(SelectIndex);
				SelectIndex = -1;
			}
		}
		else
		{
			GroupList.clear();
		}
		for (int i = 0; i < data.GetSize(); i++)
		{
			if (IsContainVector(i, GroupList))
				continue;


			if (data[i].shapeType != DrawMode::DEllipse) {
				if (d_pos.x - u_pos.x > 0) {
					int localCount = 0;
					for (int j = 0; j < data[i].pts.size(); j++)
					{
						if (isContainPolygon(Point2d(data[i].pts[j].x, data[i].pts[j].y), vertices))
						{
							localCount++;
						}
					}
					if (localCount == data[i].pts.size())
						GroupList.push_back(i);
				}
				else if (d_pos.x - u_pos.x < 0) {
					if (overlap(vertices, data[i].pts)) {
						GroupList.push_back(i);
						continue;
					}
				}
			}
			else {
				if (d_pos.x - u_pos.x > 0) {
					int localCount = 0;
					for (int j = 0; j < data[i].pts.size(); j++) {
						if (!isContainPolygon(data[i].pts[j], vertices))
							localCount--;
					}
					if (localCount == 0)
						GroupList.push_back(i);

				}
				else if (d_pos.x - u_pos.x < 0) {
					if (isContainPolygon(data[i].Center, vertices)) {
						GroupList.push_back(i);
						continue;
					}
					for (int j = 0; j < vertices.size(); j++) {
						int a = j, b = j + 1;
						if (j == vertices.size() - 1)
							b = 0;
						if (lineCircleIntersection(vertices[a], vertices[b], data[i])) {
							GroupList.push_back(i);
							break;
						}
					}
				}
			}
		}

		if (!GroupList.empty()) {
			EdgeSelect = FALSE;
			Invalidate(FALSE);
			ReleaseCapture();
			shape.pts.clear();
			SelectIndex = GroupList[0];
			selectID = TRUE;
			SelectShapeUpdate();
			return;
		}
	}
	ctrlSelect = FALSE;
	EdgeSelect = FALSE;
	SelectShapeUpdate();
	Invalidate(FALSE);
	CView::OnLButtonUp(nFlags, point);
}

void CImgViewerView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (start_pos.x == 0)
	{
		p_pt.x = (z_pos.x + (m_ePt.x + point.x) / PWidth);
	}
	else
	{
		p_pt.x = m_Bitmap.bmWidth * ((point.x - start_pos.x) / (m_Bitmap.bmWidth * zoomView - 4));
	}
	if (start_pos.y == 0)
	{
		p_pt.y = (z_pos.y + (m_ePt.y + point.y) / PHeight);
	}
	else
	{
		p_pt.y = m_Bitmap.bmHeight * ((point.y - start_pos.y) / (m_Bitmap.bmHeight * zoomView - 4));
	}

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
	GroupList.clear();
	drawID = TRUE;
	penID = FALSE;
}

void CImgViewerView::OnDrawLine()
{
	drawStyle = DrawMode::DLine;
	GroupList.clear();
	drawID = TRUE;
	penID = FALSE;
}

void CImgViewerView::OnDrawEllpse()
{
	drawStyle = DrawMode::DEllipse;
	drawID = TRUE;
	GroupList.clear();
	penID = FALSE;
}

void CImgViewerView::OnDrawRect()
{
	drawStyle = DrawMode::DRectangle;
	GroupList.clear();
	drawID = TRUE;
	penID = FALSE;
}

void CImgViewerView::OnDrawTri()
{
	drawStyle = DrawMode::DTriangle;
	GroupList.clear();
	drawID = TRUE;
	penID = FALSE;
}

void CImgViewerView::OnModeSelect()
{
	drawStyle = DrawMode::None;
	drawID = FALSE;
	penID = TRUE;
}

void CImgViewerView::OnContextCopy()
{
	copyList.clear();
	ctrlv = 1;
	if (!GroupList.empty()) {
		for (int i = 0; i < GroupList.size(); i++) {
			CopyShape temp;
			temp.copy = data[GroupList[i]];
			temp.pts = data[GroupList[i]].pts;
			copyList.push_back(temp);
		}
		iscopy = true;
	}
	else if (selectID)
	{
		copyShape.copy = data[SelectIndex];
		copyShape.pts = data[SelectIndex].pts;
		iscopy = true;
		OnModeSelect();
	}
	else
		iscopy = false;
}

void CImgViewerView::OnContextPaste()
{
	if (iscopy)
	{
		if (ctrl)
		{
			if (!copyList.empty()) {
				for (int j = 0; j < copyList.size(); j++) {
					for (int i = 0; i < copyList[j].pts.size(); i++)
					{
						copyList[j].copy.pts[i].x = copyList[j].pts[i].x + (ctrlv * 5);
						copyList[j].copy.pts[i].y = copyList[j].pts[i].y + (ctrlv * 5);
					}
					copyList[j].copy.RotatePts = copyList[j].copy.pts;
				}
			}
			else {
				for (int i = 0; i < copyShape.pts.size(); i++)
				{
					copyShape.copy.pts[i].x = copyShape.pts[i].x + (ctrlv * 5);
					copyShape.copy.pts[i].y = copyShape.pts[i].y + (ctrlv * 5);
				}
				copyShape.copy.RotatePts = copyShape.copy.pts;
			}
			ctrlv++;
		}
		else
		{

			if (!copyList.empty()) {
					Point pt(p_pt.x - copyList[0].pts[0].x, p_pt.y - copyList[0].pts[0].y);
				for (int j = 0; j < copyList.size(); j++) {
					// 가운데나 회전
					for (int i = 0; i < copyList[j].pts.size(); i++)
					{
						copyList[j].copy.pts[i].x = copyList[j].pts[i].x+ pt.x;
						copyList[j].copy.pts[i].y = copyList[j].pts[i].y + pt.y;
					}
					copyList[j].copy.RotatePts = copyList[j].copy.pts;
				}
			}
			else {
				Point pt(p_pt.x - copyShape.copy.Center.x, p_pt.y - copyShape.copy.Center.y);
				// 가운데나 회전
				for (int i = 0; i < copyShape.pts.size(); i++)
				{
					copyShape.copy.pts[i].x = copyShape.pts[i].x + pt.x;
					copyShape.copy.pts[i].y = copyShape.pts[i].y + pt.y;
				}
				copyShape.copy.RotatePts = copyShape.copy.pts;
			}
		}
		if (!copyList.empty()) {
			RollbackInfo gr;
			gr.rollbackmode = RollBackMode::GroupStart;
			if (rollbackIndex != -1)
				rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
			else if (rollback.size() != 0) {
				rollback.erase(rollback.begin(), rollback.end());
			}
			rollback.push_back(gr);
			int groupIndex = data.GetSize();
			for (int j = 0; j < copyList.size(); j++) {
				data.Add(copyList[j].copy);
				zOrder.insert(zOrder.begin(), data.GetSize() - 1);;
				SelectIndex = zOrder[0];
				RollbackInfo info;
				info.redoShape = data[SelectIndex];
				info.idx = SelectIndex;
				info.undoShape = data[SelectIndex];
				info.rollbackmode = RollBackMode::Create;

				rollback.push_back(info);
			}
			GroupList.clear();
			for (int i = groupIndex; i < data.GetSize(); i++) {
				GroupList.push_back(i);
			}
			gr.rollbackmode = RollBackMode::GroupEnd;
			rollback.push_back(gr);
			rollbackIndex = rollback.size() - 1;
		}
		else {
			data.Add(copyShape.copy);
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
		}
		SelectShapeUpdate();
		Invalidate(FALSE);
		ctrl = false;
	}
}



void CImgViewerView::OnContextDelete()
{
	if (!GroupList.empty()) {
		sort(GroupList.begin(), GroupList.end(), greater<int>());
		RollbackInfo gr;
		gr.rollbackmode = RollBackMode::GroupStart;
		if (rollbackIndex != -1)
			rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
		else if (rollback.size() != 0) {
			rollback.erase(rollback.begin(), rollback.end());
		}
		rollback.push_back(gr);
		for (int j = 0; j < GroupList.size(); j++) {
			RollbackInfo info;
			info.idx = GroupList[j];
			info.undoShape = data[GroupList[j]];
			info.rollbackmode = RollBackMode::Delete;

			for (int i = 1; i < zOrder.size(); i++) {
				if (zOrder[i] > zOrder[0])
					zOrder[i]--;
			}
			zOrder.erase(zOrder.begin());
			rollback.push_back(info);
			data.RemoveAt(GroupList[j]);
		}
		gr.rollbackmode = RollBackMode::GroupEnd;
		rollback.push_back(gr);
		rollbackIndex = rollback.size() - 1;
		GroupList.clear();
		SelectIndex = -1;
		Invalidate(FALSE);
		selectID = FALSE;
	}
	else if (selectID)
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
	if (SelectIndex != -1 ||!GroupList.empty())
	{
		CColorDialog colorDlg;

		if (colorDlg.DoModal() == IDOK)
		{
			if (!GroupList.empty()) {
				if (rollbackIndex != -1)
					rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
				else if (rollback.size() != 0) {
					rollback.erase(rollback.begin(), rollback.end());
				}
				RollbackInfo gr;
				gr.rollbackmode = RollBackMode::GroupStart;
				rollback.push_back(gr);
				for (int i = 0; i < GroupList.size(); i++) {
					RollbackInfo info;
					info.idx = GroupList[i];
					info.undoShape = data[GroupList[i]];
					data[GroupList[i]].shapeColor = colorDlg.GetColor();
					info.redoShape = data[GroupList[i]];
					info.rollbackmode = RollBackMode::Update;
					
					rollback.push_back(info);
				}
				gr.rollbackmode = RollBackMode::GroupEnd;
				rollback.push_back(gr);
				rollbackIndex = rollback.size() - 1;
				Invalidate(FALSE);
			}
			else {
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
}

void CImgViewerView::OnContextLinewidth()
{
	if (SelectIndex != -1 || !GroupList.empty())
	{
		CSelectLineWidth lwDlg;

		if (lwDlg.DoModal() == IDOK)
		{
			if (!GroupList.empty()) {
				if (rollbackIndex != -1)
					rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
				else if (rollback.size() != 0) {
					rollback.erase(rollback.begin(), rollback.end());
				}
				RollbackInfo gr;
				gr.rollbackmode = RollBackMode::GroupStart;
				rollback.push_back(gr);
				for (int i = 0; i < GroupList.size(); i++) {
					RollbackInfo info;
					info.idx = GroupList[i];
					info.undoShape = data[GroupList[i]];
					data[GroupList[i]].penWidth = lwDlg.l_width;
					info.redoShape = data[GroupList[i]];
					info.rollbackmode = RollBackMode::Update;

					rollback.push_back(info);
				}
				gr.rollbackmode = RollBackMode::GroupEnd;
				rollback.push_back(gr);
				rollbackIndex = rollback.size() - 1;
				Invalidate(FALSE);
			}
			else {
				RollbackInfo info;
				info.idx = SelectIndex;
				info.undoShape = data[SelectIndex];
				data[SelectIndex].penWidth = lwDlg.l_width;
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
			case 97: case 65:	//ctrl - A
			{
				drawID = FALSE;
				penID = TRUE;
				GroupList.clear();
				for (int i = 0; i < data.GetSize(); i++) {
					GroupList.push_back(i);
				}
				SelectShapeUpdate();
				Invalidate();
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
			GroupList.clear();
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
		case RollBackMode::GroupEnd: {
			rollbackIndex--;
			while (rollback[rollbackIndex].rollbackmode != RollBackMode::GroupStart)
			{
				undo();
			}
			rollbackIndex--;
			SelectShapeUpdate();
			Invalidate(FALSE);
			break;
		}
		default:
			break;
		}
		if (GroupList.empty())
			Invalidate(FALSE);
	}
}

void CImgViewerView::redo() {
	if (rollbackIndex + 1 < rollback.size())
	{
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
		case RollBackMode::GroupStart: {
			while (rollback[rollbackIndex].rollbackmode != RollBackMode::GroupEnd)
			{
				redo();
			}
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
	static TCHAR BASED_CODE szFilter[] = _T("이미지 파일(*.BMP, *.GIF, *.JPG, *PNG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.png;*.gif; |모든파일(*.*)|*.*||");

	CFileDialog fsDlg(FALSE, _T("png"), NULL, OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, szFilter, NULL);
	if (fsDlg.DoModal() == IDOK)
	{
		fileSave = fsDlg.GetPathName();  //파일경로 얻어와서
		selectID = FALSE;
		GroupList.clear();
		Save_Shape = TRUE;
		Invalidate(FALSE);
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
	Save_Shape = FALSE;
	drawStyle = DrawMode::None;

	return 0;
}


BOOL CImgViewerView::OnEraseBkgnd(CDC* pDC)
{
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
	double x = z_pos.x + (m_ePt.x) / PWidth;
	double y = z_pos.y + (m_ePt.y) / PHeight;
	double Width = z_pos.x + (m_ePt.x + m_bgRect.right) / PWidth - x;
	double Height = z_pos.y + (m_ePt.y + m_bgRect.bottom) / PHeight - y;

	theApp.pNavigatorView->GetRectPos(x, y, Width, Height);
}

void CImgViewerView::GetImgPos(double Navigator_x, double Navigator_y)
{
	CPoint Navigator_pos;
	Navigator_pos.x = Navigator_x * m_Bitmap.bmWidth;
	Navigator_pos.y = Navigator_y * m_Bitmap.bmHeight;

	z_pos.x = Navigator_pos.x - zoomWidth / 2;
	z_pos.y = Navigator_pos.y - zoomHeight / 2;

	/*if (z_pos.x < 0)
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
	}*/

	if ((z_pos.x == (int)(-((m_bgRect.right / 5 * 4) / PWidth) - 1) && m_ePt.x <= PWidth * (1 - ((((m_bgRect.right / 5 * 4) / PWidth) - (int)((m_bgRect.right / 5 * 4) / PWidth))))) || z_pos.x < (int)(-((m_bgRect.right / 5 * 4) / PWidth) - 1))
	{
		z_pos.x = -((m_bgRect.right / 5 * 4) / PWidth) - 1;
		m_ePt.x = PWidth * (1 - ((((m_bgRect.right / 5 * 4) / PWidth) - (int)((m_bgRect.right / 5 * 4) / PWidth))));
	}

	//이미지 오른쪽으로 나갈때(고쳐야댐)
	if ((z_pos.x == (int)(m_Bitmap.bmWidth - ((m_bgRect.right / 5 * 1) / PWidth)) && m_ePt.x >= PWidth * (1 - ((((m_bgRect.right / 5) / PWidth) - (int)((m_bgRect.right / 5) / PWidth))))) || z_pos.x > (int)(m_Bitmap.bmWidth - ((m_bgRect.right / 5) / PWidth)))
	{
		z_pos.x = m_Bitmap.bmWidth - ((m_bgRect.right / 5) / PWidth);
		m_ePt.x = PWidth * (1 - ((((m_bgRect.right / 5) / PWidth) - (int)((m_bgRect.right / 5) / PWidth))));
	}

	//이미지 아래쪽으로 나갈때(고쳐야댐)
	if ((z_pos.y == (int)(m_Bitmap.bmHeight - ((m_bgRect.bottom / 5 * 1) / PHeight)) && m_ePt.y >= PHeight * (1 - ((((m_bgRect.bottom / 5) / PHeight) - (int)((m_bgRect.bottom / 5) / PHeight))))) || z_pos.y > (int)(m_Bitmap.bmHeight - ((m_bgRect.bottom / 5) / PHeight)))
	{
		z_pos.y = m_Bitmap.bmHeight - ((m_bgRect.bottom / 5) / PHeight);
		m_ePt.y = PHeight * (1 - ((((m_bgRect.bottom / 5) / PHeight) - (int)((m_bgRect.bottom / 5) / PHeight))));
	}

	//이미지 위쪽으로 나갈때
	if ((z_pos.y == (int)(-((m_bgRect.bottom / 5 * 4) / PHeight) - 1) && m_ePt.y <= PHeight * (1 - ((((m_bgRect.bottom / 5 * 4) / PHeight) - (int)((m_bgRect.bottom / 5 * 4) / PHeight))))) || z_pos.y < (int)(-((m_bgRect.bottom / 5 * 4) / PHeight) - 1))
	{
		z_pos.y = -((m_bgRect.bottom / 5 * 4) / PHeight) - 1;
		m_ePt.y = PHeight * (1 - ((((m_bgRect.bottom / 5 * 4) / PHeight) - (int)((m_bgRect.bottom / 5 * 4) / PHeight))));
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

int CImgViewerView::isLeft(Point2d linePt1, Point2d linePt2, Point2d pos)
{
	return ((linePt2.x - linePt1.x) * (pos.y - linePt1.y) - (pos.x - linePt1.x) * (linePt2.y - linePt1.y));
}

int CImgViewerView::isContainPolygon(Point2d pos, vector<Point2d> vertices)
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
	if (!GroupList.empty()) {
		for (int j = 0; j < GroupList.size(); j++) {
			//  중심 업데이트
			Point2d pt(0, 0);
			RectCount = data[GroupList[j]].pts.size();
			for (int i = 0; i < RectCount; i++)
				pt += data[GroupList[j]].pts[i];

			data[GroupList[j]].Center = pt / RectCount;
			data[GroupList[j]].Rotate = data[GroupList[j]].Center;
			data[GroupList[j]].Rotate.y -= 15;

			// 사각 업데이트
			for (int i = 0; i < RectCount; i++)
			{
				pt = data[GroupList[j]].pts[i];
				//data[GroupList[j]].Rect[i].SetRect(pt.x , pt.y , pt.x , pt.y );
			}

			// 각도 업데이트
			data[GroupList[j]].RotatePts = data[GroupList[j]].pts;
			data[GroupList[j]].R_theta = data[GroupList[j]].theta;

			if (data[GroupList[j]].shapeType == DrawMode::DEllipse)
			{
				pt = data[GroupList[j]].pts[0] - data[GroupList[j]].pts[1];
				data[GroupList[j]].radin[0] = sqrt(pow(pt.x, 2) + pow(pt.y, 2)) / 2;
				pt = data[GroupList[j]].pts[1] - data[GroupList[j]].pts[2];
				data[GroupList[j]].radin[1] = sqrt(pow(pt.x, 2) + pow(pt.y, 2)) / 2;
			}
		}
	}
	else {
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
			//data[SelectIndex].Rect[i].SetRect(pt.x , pt.y , pt.x , pt.y );
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
	//RectCount = data[SelectIndex].pts.size();
	//int tempCount;
	//if (data[SelectIndex].theta < CV_PI / 2)
	//	tempCount = 0;
	//else if (data[SelectIndex].theta < CV_PI)
	//	tempCount = 1;
	//else if (data[SelectIndex].theta < CV_PI * 3 / 2)
	//	tempCount = 2;
	//else if (data[SelectIndex].theta < CV_PI * 2)
	//	tempCount = 3;

	//vector<Point2d> temp = data[SelectIndex].pts;
	//for (int i = 0; i < RectCount; i++)
	//{
	//	int Index = i + tempCount;
	//	if (Index > RectCount)
	//		Index -= RectCount;
	//	temp[Index] = data[SelectIndex].pts[i];
	//}
	//data[SelectIndex].pts = temp;
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
int CImgViewerView::isContainPolygon(Point2d pos, vector<Point2d> vertices, int shapeType)
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

BOOL CImgViewerView::polygon_points_inside(vector<Point2d> rc, vector<Point2d> V) {

	for (int i = 0; i < rc.size(); i++) {
		if (isContainPolygon(rc[i], V)) {
			return TRUE;
		}
	}
	for (int i = 0; i < V.size(); i++) {
		if (isContainPolygon(V[i], rc)) {
			return TRUE;
		}
	}
	return FALSE;
}


BOOL CImgViewerView::lineCircleIntersection(Point2d AP1, Point2d AP2, MyShape& sh) {
	double rad = fabs((sh.pts[0].x - sh.pts[1].x) / 2.f);
	if (sqrt(pow(sh.Center.x - AP1.x, 2) + pow(sh.Center.y - AP1.y, 2)) <= rad)
		return TRUE;
	if (sqrt(pow(sh.Center.x - AP2.x, 2) + pow(sh.Center.y - AP2.y, 2)) <= rad)
		return TRUE;
	double a = 1;
	double c = 1;
	if (AP1.x == AP2.x) {
		c = 0;

	}
	if (AP1.y == AP2.y) {
		a = 0;
	}

	double b = AP1.y * c - AP1.x * a;

	double d = fabs(a * sh.Center.x - sh.Center.y * c + b) / sqrt(a * a + c * c);
	if (d <= rad) {
		double min, max;
		max = my_greater(AP1.y, AP2.y, min);
		if ((min <= sh.Center.y && max >= sh.Center.y))
			return TRUE;
		max = my_greater(AP1.x, AP2.x, min);
		if ((min <= sh.Center.x && max >= sh.Center.x))
			return TRUE;
	}
	return FALSE;

}

BOOL CImgViewerView::CImgViewerView::polygon_edges_overlap(vector<Point2d> rc, vector<Point2d> V) {
	for (int i = 0; i < rc.size(); i += 1) {
		for (int j = 0; j < V.size(); j += 1) {
			int a = i, b = i + 1, c = j, d = j + 1;
			if (i == rc.size() - 1)
				b = 0;
			if (j == V.size() - 1)
				d = 0;
			if (lineLineIntersection(rc[a], rc[b], V[c], V[d])) {
				return TRUE;
			}
		}

	}

	return FALSE;
}

BOOL CImgViewerView::lineLineIntersection(Point2d AP1, Point2d AP2, Point2d BP1, Point2d BP2)
{
	double t;
	double s;
	double under = (BP2.y - BP1.y) * (AP2.x - AP1.x) - (BP2.x - BP1.x) * (AP2.y - AP1.y);
	if (under == 0) return false;

	double _t = (BP2.x - BP1.x) * (AP1.y - BP1.y) - (BP2.y - BP1.y) * (AP1.x - BP1.x);
	double _s = (AP2.x - AP1.x) * (AP1.y - BP1.y) - (AP2.y - AP1.y) * (AP1.x - BP1.x);

	t = _t / under;
	s = _s / under;

	if (t < 0.0 || t>1.0 || s < 0.0 || s>1.0) return false;
	if (_t == 0 && _s == 0) return false;


	return true;

}

// 처음 그리기
void CImgViewerView::draw(CDC* pDC)
{
	if (!(m_nFlags & MK_LBUTTON)) {
		return;
	}
	CPen* pen = new CPen(PS_SOLID, 1, RGB(0, 255, 0));
	CPen* oldPen;
	oldPen = pDC->SelectObject(pen);
	pDC->SelectStockObject(NULL_BRUSH);

	double dxi, dxd;
	double dyi, dyd;
	double uxi, uxd;
	double uyi, uyd;
	Point2d d_pts, d_pte;
	if (drawStyle == DrawMode::DLine) // 콤보상자에서 직선 선택시
	{
		dxd = modf(d_pos.x, &dxi);
		dyd = modf(d_pos.y, &dyi);
		uxd = modf(u_pos.x, &uxi);
		uyd = modf(u_pos.y, &uyi);

		pts_0.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd);
		pts_0.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd);
		pts_1.x = (PWidth * ((int)uxi - z_pos.x)) + (PWidth * uxd);
		pts_1.y = (PHeight * ((int)uyi - z_pos.y)) + (PHeight * uyd);

		pDC->MoveTo(pts_0.x, pts_0.y);
		pDC->LineTo(pts_1.x, pts_1.y);

		//pDC->MoveTo(d_sPt.x, d_sPt.y); pDC->LineTo(mov_Pt.x, mov_Pt.y);
	}
	else if (drawStyle == DrawMode::DRectangle) // 콤보상자에서 사각형 선택시
	{
		dxd = modf(d_pos.x, &dxi);
		dyd = modf(d_pos.y, &dyi);
		uxd = modf(u_pos.x, &uxi);
		uyd = modf(u_pos.y, &uyi);


		d_pts.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd);
		d_pts.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd);
		d_pte.x = (PWidth * ((int)uxi - z_pos.x)) + (PWidth * uxd);
		d_pte.y = (PHeight * ((int)uyi - z_pos.y)) + (PHeight * uyd);

		pDC->Rectangle(d_pts.x, d_pts.y, d_pte.x, d_pte.y);
	}
	else if (drawStyle == DrawMode::DEllipse) // 콤보상자에서 원 선택시
	{
		Point2d tmp1(d_pos);
		Point2d tmp2(u_pos);
		double width = 0, height = 0;

		if (abs(u_pos.x - d_pos.x) > abs(u_pos.y - d_pos.y))
		{
			width = ((u_pos.x - d_pos.x) / 2) - ((u_pos.y - d_pos.y) / 2);
			tmp1.x += width;
			tmp2.x -= width;
		}
		else
		{
			height = ((u_pos.y - d_pos.y) / 2) - ((u_pos.x - d_pos.x) / 2);
			tmp1.y += height;
			tmp2.y -= height;
		}

		dxd = modf(tmp1.x, &dxi);
		dyd = modf(tmp1.y, &dyi);
		uxd = modf(tmp2.x, &uxi);
		uyd = modf(tmp2.y, &uyi);

		pts_0.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd);
		pts_0.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd);
		pts_1.x = (PWidth * ((int)uxi - z_pos.x)) + (PWidth * uxd);
		pts_1.y = (PHeight * ((int)uyi - z_pos.y)) + (PHeight * uyd);
		
		pDC->Ellipse(pts_0.x, pts_0.y, pts_1.x, pts_1.y);

		//pDC->Ellipse(pts_0.x, pts_0.y, pts_1.x, pts_1.y);
	}
	else if (drawStyle == DrawMode::DTriangle) // 콤보상자에서 사각형 선택시
	{
		double minX, minY, maxX, maxY;
		maxX = my_greater(d_pos.x, u_pos.x, minX);
		maxY = my_greater(d_pos.y, u_pos.y, minY);
		pDC->MoveTo((minX + maxX) / 2, minY);
		pDC->LineTo(maxX, maxY);
		pDC->LineTo(minX, maxY);
		pDC->LineTo((minX + maxX) / 2, minY);
	}

	else if (penID && selectID==FALSE && rotateID == FALSE && drawShapeID == FALSE) {


		dxd = modf(d_pos.x, &dxi);
		dyd = modf(d_pos.y, &dyi);
		uxd = modf(u_pos.x, &uxi);
		uyd = modf(u_pos.y, &uyi);


		d_pts.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd);
		d_pts.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd);
		d_pte.x = (PWidth * ((int)uxi - z_pos.x)) + (PWidth * uxd);
		d_pte.y = (PHeight * ((int)uyi - z_pos.y)) + (PHeight * uyd);

		pDC->Rectangle(d_pts.x, d_pts.y, d_pte.x, d_pte.y);
		//pDC->Rectangle(pts_0.x, pts_0.y, pts_2.x, pts_2.y);
	}
	pDC->SelectObject(oldPen); // 이전 팬 선택		
	pen->DeleteObject();  // 생성한 펜 메모리에서 제거			
	delete pen;
}

// 그림 복구
void CImgViewerView::drawShape(CDC* pDC, MyShape& shape)
{
	int shapeType = shape.shapeType;
	CPen* pen;
	if(shape.shapeType==DrawMode::DPoint)
		pen = new CPen(PS_SOLID, 3, shape.shapeColor);
	else
		pen = new CPen(PS_SOLID, shape.penWidth, shape.shapeColor);
	CPen* oldPen;
	oldPen = pDC->SelectObject(pen);
	pDC->SelectStockObject(NULL_BRUSH);

	double dxi, dxd;
	double dyi, dyd;
	double uxi, uxd;
	double uyi, uyd;

	double xi_0, xd_0;
	double yi_0, yd_0;
	double xi_1, xd_1;
	double yi_1, yd_1;
	double xi_2, xd_2;
	double yi_2, yd_2;
	double xi_3, xd_3;
	double yi_3, yd_3;


	if (shapeType == DrawMode::DPoint)
	{
		dxd = modf(shape.RotatePts[0].x, &dxi);
		dyd = modf(shape.RotatePts[0].y, &dyi);
		pts_0.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd) - PWidth * 5;
		pts_0.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd)-PHeight*5;
		pts_1.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd)+ PWidth*5;
		pts_1.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd)- PWidth*5;
		pts_2.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd)+ PWidth*5;
		pts_2.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd)+ PWidth*5;
		pts_3.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd)- PWidth*5;
		pts_3.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd)+ PWidth*5;

		pDC->MoveTo(pts_0.x, pts_0.y);
		pDC->LineTo(pts_2.x, pts_2.y);
		pDC->MoveTo(pts_1.x, pts_1.y);
		pDC->LineTo(pts_3.x, pts_3.y);
	}
	else if (shapeType == DrawMode::DLine)
	{
		dxd = modf(shape.RotatePts[0].x, &dxi);
		dyd = modf(shape.RotatePts[0].y, &dyi);
		uxd = modf(shape.RotatePts[1].x, &uxi);
		uyd = modf(shape.RotatePts[1].y, &uyi);

		pts_0.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd);
		pts_0.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd);
		pts_1.x = (PWidth * ((int)uxi - z_pos.x)) + (PWidth * uxd);
		pts_1.y = (PHeight * ((int)uyi - z_pos.y)) + (PHeight * uyd);

		pDC->MoveTo(pts_0.x, pts_0.y);
		pDC->LineTo(pts_1.x, pts_1.y);
	}
	else if (shapeType == DrawMode::DEllipse) // 원 그리기
	{
		Point2d tmp1(shape.RotatePts[0]);
		Point2d tmp2(shape.RotatePts[2]);
		double width = 0, height = 0;

		if (abs(tmp2.x - tmp1.x) > abs(tmp2.y - tmp1.y))
		{
			width = ((tmp2.x - tmp1.x) / 2) - ((tmp2.y - tmp1.y) / 2);
			tmp1.x += width;
			tmp2.x -= width;
		}
		else
		{
			height = ((tmp2.y - tmp1.y) / 2) - ((tmp2.x - tmp1.x) / 2);
			tmp1.y += height;
			tmp2.y -= height;
		}

		dxd = modf(tmp1.x, &dxi);
		dyd = modf(tmp1.y, &dyi);
		uxd = modf(tmp2.x, &uxi);
		uyd = modf(tmp2.y, &uyi);

		pts_0.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd);
		pts_0.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd);
		pts_1.x = (PWidth * ((int)uxi - z_pos.x)) + (PWidth * uxd);
		pts_1.y = (PHeight * ((int)uyi - z_pos.y)) + (PHeight * uyd);

		pDC->Ellipse(pts_0.x, pts_0.y, pts_1.x, pts_1.y);
		//MyEllipseR(pDC, shape.Center, shape.radin[0], shape.radin[1], shape.theta, shape.shapeColor);
	}
	else if (shapeType == DrawMode::DRectangle)
	{
		xd_0 = modf(shape.pts[0].x, &xi_0);
		yd_0 = modf(shape.pts[0].y, &yi_0);
		xd_1 = modf(shape.pts[1].x, &xi_1);
		yd_1 = modf(shape.pts[1].y, &yi_1);
		xd_2 = modf(shape.pts[2].x, &xi_2);
		yd_2 = modf(shape.pts[2].y, &yi_2);
		xd_3 = modf(shape.pts[3].x, &xi_3);
		yd_3 = modf(shape.pts[3].y, &yi_3);

		pts_0.x = (PWidth * ((int)xi_0 - z_pos.x)) + (PWidth * xd_0);
		pts_0.y = (PHeight * ((int)yi_0 - z_pos.y)) + (PHeight * yd_0);
		pts_1.x = (PWidth * ((int)xi_1 - z_pos.x)) + (PWidth * xd_1);
		pts_1.y = (PHeight * ((int)yi_1 - z_pos.y)) + (PHeight * yd_1);
		pts_2.x = (PWidth * ((int)xi_2 - z_pos.x)) + (PWidth * xd_2);
		pts_2.y = (PHeight * ((int)yi_2 - z_pos.y)) + (PHeight * yd_2);
		pts_3.x = (PWidth * ((int)xi_3 - z_pos.x)) + (PWidth * xd_3);
		pts_3.y = (PHeight * ((int)yi_3 - z_pos.y)) + (PHeight * yd_3);

		pDC->MoveTo(pts_0.x, pts_0.y);
		pDC->LineTo(pts_1.x, pts_1.y);
		pDC->LineTo(pts_2.x, pts_2.y);
		pDC->LineTo(pts_3.x, pts_3.y);
		pDC->LineTo(pts_0.x, pts_0.y);
	}
	else if (shapeType == DrawMode::DTriangle)
	{
		xd_0 = modf(shape.pts[0].x, &xi_0);
		yd_0 = modf(shape.pts[0].y, &yi_0);
		xd_1 = modf(shape.pts[1].x, &xi_1);
		yd_1 = modf(shape.pts[1].y, &yi_1);
		xd_2 = modf(shape.pts[2].x, &xi_2);
		yd_2 = modf(shape.pts[2].y, &yi_2);


		pts_0.x = (PWidth * ((int)xi_0 - z_pos.x)) + (PWidth * xd_0);
		pts_0.y = (PHeight * ((int)yi_0 - z_pos.y)) + (PHeight * yd_0);
		pts_1.x = (PWidth * ((int)xi_1 - z_pos.x)) + (PWidth * xd_1);
		pts_1.y = (PHeight * ((int)yi_1 - z_pos.y)) + (PHeight * yd_1);
		pts_2.x = (PWidth * ((int)xi_2 - z_pos.x)) + (PWidth * xd_2);
		pts_2.y = (PHeight * ((int)yi_2 - z_pos.y)) + (PHeight * yd_2);


		pDC->MoveTo(pts_0.x, pts_0.y);
		pDC->LineTo(pts_1.x, pts_1.y);
		pDC->LineTo(pts_2.x, pts_2.y);
		pDC->LineTo(pts_0.x, pts_0.y);
	}
	pDC->SelectObject(oldPen); // 이전 팬 선택		
	pen->DeleteObject();  // 생성한 펜 메모리에서 제거		
	delete pen;
}

// 선택 그리기
void CImgViewerView::SelectDrawShape(CDC* pDC, MyShape& shape)
{
	// 선택 실선
	CPen* pen = new CPen(PS_DOT, shape.penWidth, BLACK_PEN);
	CPen* oldPen;
	oldPen = pDC->SelectObject(pen);


	double dxi, dxd;
	double dyi, dyd;
	double uxi, uxd;
	double uyi, uyd;

	double xi_0, xd_0;
	double yi_0, yd_0;
	double xi_1, xd_1;
	double yi_1, yd_1;
	double xi_2, xd_2;
	double yi_2, yd_2;
	double xi_3, xd_3;
	double yi_3, yd_3;

	Point2d d_pts, d_pte;
	if (shape.shapeType == DrawMode::DPoint)
	{
		dxd = modf(shape.pts[0].x, &dxi);
		dyd = modf(shape.pts[0].y, &dyi);
		pts_0.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd) - PWidth * 5;
		pts_0.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd) - PHeight * 5;
		pts_1.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd) + PWidth * 5;
		pts_1.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd) - PWidth * 5;
		pts_2.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd) + PWidth * 5;
		pts_2.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd) + PWidth * 5;
		pts_3.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd) - PWidth * 5;
		pts_3.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd) + PWidth * 5;

		pDC->MoveTo(pts_0.x, pts_0.y);
		pDC->LineTo(pts_2.x, pts_2.y);
		pDC->MoveTo(pts_1.x, pts_1.y);
		pDC->LineTo(pts_3.x, pts_3.y);
	}
	else if (shape.shapeType == DrawMode::DLine)
	{
		dxd = modf(shape.pts[0].x, &dxi);
		dyd = modf(shape.pts[0].y, &dyi);
		uxd = modf(shape.pts[1].x, &uxi);
		uyd = modf(shape.pts[1].y, &uyi);

		pts_0.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd);
		pts_0.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd);
		pts_1.x = (PWidth * ((int)uxi - z_pos.x)) + (PWidth * uxd);
		pts_1.y = (PHeight * ((int)uyi - z_pos.y)) + (PHeight * uyd);

		pDC->MoveTo(pts_0.x, pts_0.y);
		pDC->LineTo(pts_1.x, pts_1.y);
	}
	else if (shape.shapeType == DrawMode::DEllipse) // 원 그리기
	{
		pDC->SelectStockObject(NULL_BRUSH);

		dxd = modf(shape.pts[0].x, &dxi);
		dyd = modf(shape.pts[0].y, &dyi);
		uxd = modf(shape.pts[2].x, &uxi);
		uyd = modf(shape.pts[2].y, &uyi);

		pts_0.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd);
		pts_0.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd);
		pts_2.x = (PWidth * ((int)uxi - z_pos.x)) + (PWidth * uxd);
		pts_2.y = (PHeight * ((int)uyi - z_pos.y)) + (PHeight * uyd);

		pDC->Ellipse(pts_0.x, pts_0.y, pts_2.x, pts_2.y);
	}
	else if (shape.shapeType == DrawMode::DRectangle)
	{
		//pDC->MoveTo(shape.pts[0].x, shape.pts[0].y);
		//pDC->LineTo(shape.pts[1].x, shape.pts[1].y);
		//pDC->LineTo(shape.pts[2].x, shape.pts[2].y);
		//pDC->LineTo(shape.pts[0].x, shape.pts[0].y);

		//dxd = modf(shape.pts[0].x, &dxi);
		//dyd = modf(shape.pts[0].y, &dyi);
		//uxd = modf(shape.pts[2].x, &uxi);
		//uyd = modf(shape.pts[2].y, &uyi);


		xd_0 = modf(shape.pts[0].x, &xi_0);
		yd_0 = modf(shape.pts[0].y, &yi_0);
		xd_1 = modf(shape.pts[1].x, &xi_1);
		yd_1 = modf(shape.pts[1].y, &yi_1);
		xd_2 = modf(shape.pts[2].x, &xi_2);
		yd_2 = modf(shape.pts[2].y, &yi_2);
		xd_3 = modf(shape.pts[3].x, &xi_3);
		yd_3 = modf(shape.pts[3].y, &yi_3);

		pts_0.x = (PWidth * ((int)xi_0 - z_pos.x)) + (PWidth * xd_0);
		pts_0.y = (PHeight * ((int)yi_0 - z_pos.y)) + (PHeight * yd_0);
		pts_1.x = (PWidth * ((int)xi_1 - z_pos.x)) + (PWidth * xd_1);
		pts_1.y = (PHeight * ((int)yi_1 - z_pos.y)) + (PHeight * yd_1);
		pts_2.x = (PWidth * ((int)xi_2 - z_pos.x)) + (PWidth * xd_2);
		pts_2.y = (PHeight * ((int)yi_2 - z_pos.y)) + (PHeight * yd_2);
		pts_3.x = (PWidth * ((int)xi_3 - z_pos.x)) + (PWidth * xd_3);
		pts_3.y = (PHeight * ((int)yi_3 - z_pos.y)) + (PHeight * yd_3);

		pDC->MoveTo(pts_0.x, pts_0.y);
		pDC->LineTo(pts_1.x, pts_1.y);
		pDC->LineTo(pts_2.x, pts_2.y);
		pDC->LineTo(pts_3.x, pts_3.y);
		pDC->LineTo(pts_0.x, pts_0.y);
	}
	else if (shape.shapeType == DrawMode::DTriangle)
	{
		xd_0 = modf(shape.pts[0].x, &xi_0);
		yd_0 = modf(shape.pts[0].y, &yi_0);
		xd_1 = modf(shape.pts[1].x, &xi_1);
		yd_1 = modf(shape.pts[1].y, &yi_1);
		xd_2 = modf(shape.pts[2].x, &xi_2);
		yd_2 = modf(shape.pts[2].y, &yi_2);


		pts_0.x = (PWidth * ((int)xi_0 - z_pos.x)) + (PWidth * xd_0);
		pts_0.y = (PHeight * ((int)yi_0 - z_pos.y)) + (PHeight * yd_0);
		pts_1.x = (PWidth * ((int)xi_1 - z_pos.x)) + (PWidth * xd_1);
		pts_1.y = (PHeight * ((int)yi_1 - z_pos.y)) + (PHeight * yd_1);
		pts_2.x = (PWidth * ((int)xi_2 - z_pos.x)) + (PWidth * xd_2);
		pts_2.y = (PHeight * ((int)yi_2 - z_pos.y)) + (PHeight * yd_2);


		pDC->MoveTo(pts_0.x, pts_0.y);
		pDC->LineTo(pts_1.x, pts_1.y);
		pDC->LineTo(pts_2.x, pts_2.y);
		pDC->LineTo(pts_0.x, pts_0.y);
	}

	pDC->SelectObject(oldPen);
	pen->DeleteObject();
	delete pen;

	// 확대 사각 박스 그리기
	pDC->SelectStockObject(WHITE_BRUSH);
	int rcCount = shape.pts.size();


	Point2d Center = shape.Center;
	Point2d Rotate = shape.Rotate;

	double theta = shape.R_theta;
	Point2d t_pts, c_pts;

	if (shape.shapeType != DrawMode::DPoint) {
		for (int i = 0; i < rcCount; i++) {
			if (shape.shapeType == DrawMode::DTriangle) {

				if (i == 2 || i == 3) {

					xd_0 = modf(shape.RotatePts[i - 1].x, &xi_0);
					yd_0 = modf(shape.RotatePts[i - 1].y, &yi_0);
				}
				else {
					double dX, dY;
					double a, b, c = 1;
					dX = shape.RotatePts[1].x - shape.RotatePts[2].x;
					dY = shape.RotatePts[1].y - shape.RotatePts[2].y;

					if (dX == 0)
					{
						a = 0;

					}
					else if (dY == 0)
					{
						c = 0;
						a = -1;
					}
					else {
						a = dY / dX;
					}
					b = -a * shape.RotatePts[0].x + c * shape.RotatePts[0].y;

					if (i == 0) {
						if (a != 0) {
							Point2d test = Perpendicular_intersection(shape.RotatePts[2], shape.RotatePts[0], Point2d((c - b) / a, 1));
							cout << test << endl;
							xd_0 = modf(test.x, &xi_0);
							yd_0 = modf(test.y, &yi_0);

						}
						else {
							xd_0 = modf(Intersection(shape.RotatePts[2], shape.RotatePts[0], Point2d(1, (a + b) / c)).x, &xi_0);
							yd_0 = modf(Intersection(shape.RotatePts[2], shape.RotatePts[0], Point2d(1, (a + b) / c)).y, &yi_0);
						}
					}
					else if (i == 1) {
						if (a != 0) {
							Point2d test = Perpendicular_intersection(shape.RotatePts[1], shape.RotatePts[0], Point2d((c - b) / a, 1));
							cout << test << endl;
							xd_0 = modf(test.x, &xi_0);
							yd_0 = modf(test.y, &yi_0);
						}
						else {
							xd_0 = modf(Intersection(shape.RotatePts[1], shape.RotatePts[0], Point2d(1, (a + b) / c)).x, &xi_0);
							yd_0 = modf(Intersection(shape.RotatePts[1], shape.RotatePts[0], Point2d(1, (a + b) / c)).y, &yi_0);

						}
					}
				}
			}
			else {
				xd_0 = modf(shape.RotatePts[i].x, &xi_0);
				yd_0 = modf(shape.RotatePts[i].y, &yi_0);
			}
			t_pts.x = (PWidth * ((int)xi_0 - z_pos.x)) + (PWidth * xd_0);
			t_pts.y = (PHeight * ((int)yi_0 - z_pos.y)) + (PHeight * yd_0);


			shape.edge[i][0].x = t_pts.x - 8;
			shape.edge[i][0].y = t_pts.y - 8;
			shape.edge[i][1].x = t_pts.x + 8;
			shape.edge[i][1].y = t_pts.y - 8;
			shape.edge[i][2].x = t_pts.x + 8;
			shape.edge[i][2].y = t_pts.y + 8;
			shape.edge[i][3].x = t_pts.x - 8;
			shape.edge[i][3].y = t_pts.y + 8;

			if (shape.shapeType == DrawMode::DLine) {
				if (i == 0) {
					shape.R_edge[i][0].x = t_pts.x - 15;
					shape.R_edge[i][0].y = t_pts.y - 15;
					shape.R_edge[i][1].x = t_pts.x + 8;
					shape.R_edge[i][1].y = t_pts.y - 15;
					shape.R_edge[i][2].x = t_pts.x + 8;
					shape.R_edge[i][2].y = t_pts.y + 8;
					shape.R_edge[i][3].x = t_pts.x - 15;
					shape.R_edge[i][3].y = t_pts.y + 8;
				}
				else if (i == 1) {
					shape.R_edge[i][0].x = t_pts.x - 8;
					shape.R_edge[i][0].y = t_pts.y - 8;
					shape.R_edge[i][1].x = t_pts.x + 15;
					shape.R_edge[i][1].y = t_pts.y - 8;
					shape.R_edge[i][2].x = t_pts.x + 15;
					shape.R_edge[i][2].y = t_pts.y + 15;
					shape.R_edge[i][3].x = t_pts.x - 8;
					shape.R_edge[i][3].y = t_pts.y + 15;
				}
			}
			else if (shape.shapeType != DrawMode::DEllipse) {
				if (i == 0) {
					shape.R_edge[i][0].x = t_pts.x - 15;
					shape.R_edge[i][0].y = t_pts.y - 15;
					shape.R_edge[i][1].x = t_pts.x + 8;
					shape.R_edge[i][1].y = t_pts.y - 15;
					shape.R_edge[i][2].x = t_pts.x + 8;
					shape.R_edge[i][2].y = t_pts.y + 8;
					shape.R_edge[i][3].x = t_pts.x - 15;
					shape.R_edge[i][3].y = t_pts.y + 8;
				}
				else if (i == 1) {
					shape.R_edge[i][0].x = t_pts.x - 8;
					shape.R_edge[i][0].y = t_pts.y - 15;
					shape.R_edge[i][1].x = t_pts.x + 15;
					shape.R_edge[i][1].y = t_pts.y - 15;
					shape.R_edge[i][2].x = t_pts.x + 15;
					shape.R_edge[i][2].y = t_pts.y + 8;
					shape.R_edge[i][3].x = t_pts.x - 8;
					shape.R_edge[i][3].y = t_pts.y + 8;
				}
				else if (i == 2) {
					shape.R_edge[i][0].x = t_pts.x - 8;
					shape.R_edge[i][0].y = t_pts.y - 8;
					shape.R_edge[i][1].x = t_pts.x + 15;
					shape.R_edge[i][1].y = t_pts.y - 8;
					shape.R_edge[i][2].x = t_pts.x + 15;
					shape.R_edge[i][2].y = t_pts.y + 15;
					shape.R_edge[i][3].x = t_pts.x - 8;
					shape.R_edge[i][3].y = t_pts.y + 15;
				}
				else if (i == 3) {
					shape.R_edge[i][0].x = t_pts.x - 15;
					shape.R_edge[i][0].y = t_pts.y - 8;
					shape.R_edge[i][1].x = t_pts.x + 8;
					shape.R_edge[i][1].y = t_pts.y - 8;
					shape.R_edge[i][2].x = t_pts.x + 8;
					shape.R_edge[i][2].y = t_pts.y + 15;
					shape.R_edge[i][3].x = t_pts.x - 15;
					shape.R_edge[i][3].y = t_pts.y + 15;
				}
			}

			Point2d pt, r_pt;
			for (int j = 0; j < 4; j++) {
				pt = shape.edge[i][j];
				pt.x = ((shape.edge[i][j].x - t_pts.x) * cos(theta) - (shape.edge[i][j].y - t_pts.y) * sin(theta) + t_pts.x);
				pt.y = ((shape.edge[i][j].x - t_pts.x) * sin(theta) + (shape.edge[i][j].y - t_pts.y) * cos(theta) + t_pts.y);
				shape.edge[i][j] = pt;

				if (shape.shapeType != DrawMode::DEllipse) {
					r_pt = shape.R_edge[i][j];
					r_pt.x = ((shape.R_edge[i][j].x - t_pts.x) * cos(theta) - (shape.R_edge[i][j].y - t_pts.y) * sin(theta) + t_pts.x);
					r_pt.y = ((shape.R_edge[i][j].x - t_pts.x) * sin(theta) + (shape.R_edge[i][j].y - t_pts.y) * cos(theta) + t_pts.y);
					shape.R_edge[i][j] = r_pt;
				}
			}
		}
		for (int i = 0; i < rcCount; i++) {

			pDC->MoveTo(shape.edge[i][0].x, shape.edge[i][0].y);
			for (int j = 1; j < 4; j++)
				pDC->LineTo(shape.edge[i][j].x, shape.edge[i][j].y);
			pDC->LineTo(shape.edge[i][0].x, shape.edge[i][0].y);
		}
		if (shape.shapeType != DrawMode::DEllipse) {
			for (int i = 0; i < rcCount; i++) {

				pDC->MoveTo(shape.R_edge[i][0].x, shape.R_edge[i][0].y);
				for (int j = 1; j < 4; j++)
					pDC->LineTo(shape.R_edge[i][j].x, shape.R_edge[i][j].y);
				pDC->LineTo(shape.R_edge[i][0].x, shape.R_edge[i][0].y);
			}
		}
		pDC->SelectStockObject(NULL_BRUSH);
	}


	// 회전 선택
	if (rotateID == TRUE) {
		pen = new CPen(PS_SOLID, shape.penWidth, RGB(0, 255, 0));
		oldPen = pDC->SelectObject(pen);
		RectCount = shape.pts.size();
		if (shape.shapeType == DrawMode::DEllipse)
		{
			
		}
		else if (shape.shapeType == DrawMode::DPoint) {

		}
		else if (shape.shapeType == DrawMode::DLine) {
			xd_0 = modf(shape.RotatePts[0].x, &xi_0);
			yd_0 = modf(shape.RotatePts[0].y, &yi_0);
			xd_1 = modf(shape.RotatePts[1].x, &xi_1);
			yd_1 = modf(shape.RotatePts[1].y, &yi_1);
			pts_0.x = (PWidth * ((int)xi_0 - z_pos.x)) + (PWidth * xd_0);
			pts_0.y = (PHeight * ((int)yi_0 - z_pos.y)) + (PHeight * yd_0);
			pts_1.x = (PWidth * ((int)xi_1 - z_pos.x)) + (PWidth * xd_1);
			pts_1.y = (PHeight * ((int)yi_1 - z_pos.y)) + (PHeight * yd_1);
			pDC->MoveTo(pts_0.x, pts_0.y);
			pDC->LineTo(pts_1.x, pts_1.y);
		}
		else if (shape.shapeType == DrawMode::DTriangle) {
			xd_0 = modf(shape.RotatePts[0].x, &xi_0);
			yd_0 = modf(shape.RotatePts[0].y, &yi_0);
			xd_1 = modf(shape.RotatePts[1].x, &xi_1);
			yd_1 = modf(shape.RotatePts[1].y, &yi_1);
			xd_2 = modf(shape.RotatePts[2].x, &xi_2);
			yd_2 = modf(shape.RotatePts[2].y, &yi_2);


			pts_0.x = (PWidth * ((int)xi_0 - z_pos.x)) + (PWidth * xd_0);
			pts_0.y = (PHeight * ((int)yi_0 - z_pos.y)) + (PHeight * yd_0);
			pts_1.x = (PWidth * ((int)xi_1 - z_pos.x)) + (PWidth * xd_1);
			pts_1.y = (PHeight * ((int)yi_1 - z_pos.y)) + (PHeight * yd_1);
			pts_2.x = (PWidth * ((int)xi_2 - z_pos.x)) + (PWidth * xd_2);
			pts_2.y = (PHeight * ((int)yi_2 - z_pos.y)) + (PHeight * yd_2);


			pDC->MoveTo(pts_0.x, pts_0.y);
			pDC->LineTo(pts_1.x, pts_1.y);
			pDC->LineTo(pts_2.x, pts_2.y);

			pDC->LineTo(pts_0.x, pts_0.y);
		}
		else
		{
			xd_0 = modf(shape.RotatePts[0].x, &xi_0);
			yd_0 = modf(shape.RotatePts[0].y, &yi_0);
			xd_1 = modf(shape.RotatePts[1].x, &xi_1);
			yd_1 = modf(shape.RotatePts[1].y, &yi_1);
			xd_2 = modf(shape.RotatePts[2].x, &xi_2);
			yd_2 = modf(shape.RotatePts[2].y, &yi_2);
			xd_3 = modf(shape.RotatePts[3].x, &xi_3);
			yd_3 = modf(shape.RotatePts[3].y, &yi_3);

			pts_0.x = (PWidth * ((int)xi_0 - z_pos.x)) + (PWidth * xd_0);
			pts_0.y = (PHeight * ((int)yi_0 - z_pos.y)) + (PHeight * yd_0);
			pts_1.x = (PWidth * ((int)xi_1 - z_pos.x)) + (PWidth * xd_1);
			pts_1.y = (PHeight * ((int)yi_1 - z_pos.y)) + (PHeight * yd_1);
			pts_2.x = (PWidth * ((int)xi_2 - z_pos.x)) + (PWidth * xd_2);
			pts_2.y = (PHeight * ((int)yi_2 - z_pos.y)) + (PHeight * yd_2);
			pts_3.x = (PWidth * ((int)xi_3 - z_pos.x)) + (PWidth * xd_3);
			pts_3.y = (PHeight * ((int)yi_3 - z_pos.y)) + (PHeight * yd_3);

			pDC->MoveTo(pts_0.x, pts_0.y);
			pDC->LineTo(pts_1.x, pts_1.y);
			pDC->LineTo(pts_2.x, pts_2.y);
			pDC->LineTo(pts_3.x, pts_3.y);
			pDC->LineTo(pts_0.x, pts_0.y);
			/*pDC->MoveTo(data[SelectIndex].RotatePts[0].x, data[SelectIndex].RotatePts[0].y);
			for (int i = 1; i < data[SelectIndex].RotatePts.size(); i++)
				pDC->LineTo(data[SelectIndex].RotatePts[i].x, data[SelectIndex].RotatePts[i].y);
			pDC->LineTo(data[SelectIndex].RotatePts[0].x, data[SelectIndex].RotatePts[0].y);*/
		}
		Rotate.x = d_pos.x;
		Rotate.y = d_pos.y;

		pDC->SelectObject(oldPen);
		pen->DeleteObject();
		delete pen;

	}
	if (penID == TRUE) {
		pen = new CPen(PS_SOLID, shape.penWidth, RGB(0, 255, 0));
		oldPen = pDC->SelectObject(pen);
		RectCount = shape.pts.size();
		if (shape.RotatePts.size() > 0)
		{
			if (shape.shapeType == DrawMode::DEllipse)
			{
				Point2d tmp1(shape.RotatePts[0]);
				Point2d tmp2(shape.RotatePts[2]);
				double width = 0, height = 0;

				if (abs(tmp2.x - tmp1.x) > abs(tmp2.y - tmp1.y))
				{
					width = ((tmp2.x - tmp1.x) / 2) - ((tmp2.y - tmp1.y) / 2);
					tmp1.x += width;
					tmp2.x -= width;
				}
				else
				{
					height = ((tmp2.y - tmp1.y) / 2) - ((tmp2.x - tmp1.x) / 2);
					tmp1.y += height;
					tmp2.y -= height;
				}

				dxd = modf(tmp1.x, &dxi);
				dyd = modf(tmp1.y, &dyi);
				uxd = modf(tmp2.x, &uxi);
				uyd = modf(tmp2.y, &uyi);

				pts_0.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd);
				pts_0.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd);
				pts_2.x = (PWidth * ((int)uxi - z_pos.x)) + (PWidth * uxd);
				pts_2.y = (PHeight * ((int)uyi - z_pos.y)) + (PHeight * uyd);

				pDC->Ellipse(pts_0.x, pts_0.y, pts_2.x, pts_2.y);


				
			}
			else if (shape.shapeType == DrawMode::DPoint) {

			}
			else if (shape.shapeType == DrawMode::DLine) {
				xd_0 = modf(shape.RotatePts[0].x, &xi_0);
				yd_0 = modf(shape.RotatePts[0].y, &yi_0);
				xd_1 = modf(shape.RotatePts[1].x, &xi_1);
				yd_1 = modf(shape.RotatePts[1].y, &yi_1);

				pts_0.x = (PWidth * ((int)xi_0 - z_pos.x)) + (PWidth * xd_0);
				pts_0.y = (PHeight * ((int)yi_0 - z_pos.y)) + (PHeight * yd_0);
				pts_1.x = (PWidth * ((int)xi_1 - z_pos.x)) + (PWidth * xd_1);
				pts_1.y = (PHeight * ((int)yi_1 - z_pos.y)) + (PHeight * yd_1);

				pDC->MoveTo(pts_0.x, pts_0.y);
				pDC->LineTo(pts_1.x, pts_1.y);
			}
			else if (shape.shapeType == DrawMode::DTriangle) {

				xd_0 = modf(shape.RotatePts[0].x, &xi_0);
				yd_0 = modf(shape.RotatePts[0].y, &yi_0);
				xd_1 = modf(shape.RotatePts[1].x, &xi_1);
				yd_1 = modf(shape.RotatePts[1].y, &yi_1);
				xd_2 = modf(shape.RotatePts[2].x, &xi_2);
				yd_2 = modf(shape.RotatePts[2].y, &yi_2);


				pts_0.x = (PWidth * ((int)xi_0 - z_pos.x)) + (PWidth * xd_0);
				pts_0.y = (PHeight * ((int)yi_0 - z_pos.y)) + (PHeight * yd_0);
				pts_1.x = (PWidth * ((int)xi_1 - z_pos.x)) + (PWidth * xd_1);
				pts_1.y = (PHeight * ((int)yi_1 - z_pos.y)) + (PHeight * yd_1);
				pts_2.x = (PWidth * ((int)xi_2 - z_pos.x)) + (PWidth * xd_2);
				pts_2.y = (PHeight * ((int)yi_2 - z_pos.y)) + (PHeight * yd_2);


				pDC->MoveTo(pts_0.x, pts_0.y);
				pDC->LineTo(pts_1.x, pts_1.y);
				pDC->LineTo(pts_2.x, pts_2.y);
				pDC->LineTo(pts_0.x, pts_0.y);
			}
			else
			{
				xd_0 = modf(shape.RotatePts[0].x, &xi_0);
				yd_0 = modf(shape.RotatePts[0].y, &yi_0);
				xd_1 = modf(shape.RotatePts[1].x, &xi_1);
				yd_1 = modf(shape.RotatePts[1].y, &yi_1);
				xd_2 = modf(shape.RotatePts[2].x, &xi_2);
				yd_2 = modf(shape.RotatePts[2].y, &yi_2);
				xd_3 = modf(shape.RotatePts[3].x, &xi_3);
				yd_3 = modf(shape.RotatePts[3].y, &yi_3);

				pts_0.x = (PWidth * ((int)xi_0 - z_pos.x)) + (PWidth * xd_0);
				pts_0.y = (PHeight * ((int)yi_0 - z_pos.y)) + (PHeight * yd_0);
				pts_1.x = (PWidth * ((int)xi_1 - z_pos.x)) + (PWidth * xd_1);
				pts_1.y = (PHeight * ((int)yi_1 - z_pos.y)) + (PHeight * yd_1);
				pts_2.x = (PWidth * ((int)xi_2 - z_pos.x)) + (PWidth * xd_2);
				pts_2.y = (PHeight * ((int)yi_2 - z_pos.y)) + (PHeight * yd_2);
				pts_3.x = (PWidth * ((int)xi_3 - z_pos.x)) + (PWidth * xd_3);
				pts_3.y = (PHeight * ((int)yi_3 - z_pos.y)) + (PHeight * yd_3);

				pDC->MoveTo(pts_0.x, pts_0.y);
				pDC->LineTo(pts_1.x, pts_1.y);
				pDC->LineTo(pts_2.x, pts_2.y);
				pDC->LineTo(pts_3.x, pts_3.y);
				pDC->LineTo(pts_0.x, pts_0.y);
			}

		}
		pDC->SelectObject(oldPen);
		pen->DeleteObject();
		delete pen;
	}
}

void CImgViewerView::DrawCircle(CDC* pDC, vector<Vec3f>& circleCenter)
{
	CPen* pen;
	pen = new CPen(PS_SOLID, 3, RGB(255, 0, 0));
	CPen* oldPen;
	oldPen = pDC->SelectObject(pen);
	pDC->SelectStockObject(NULL_BRUSH);

	double dxi, dxd;
	double dyi, dyd;
	double uxi, uxd;
	double uyi, uyd;

	double xi_0, xd_0;
	double yi_0, yd_0;
	double xi_1, xd_1;
	double yi_1, yd_1;
	double xi_2, xd_2;
	double yi_2, yd_2;
	double xi_3, xd_3;
	double yi_3, yd_3;

	for (int i = 0; i < circleCenter.size(); i++)
	{

		dxd = modf(circleCenter[i][0] - circleCenter[i][2], &dxi);
		dyd = modf(circleCenter[i][1] - circleCenter[i][2], &dyi);
		uxd = modf(circleCenter[i][0] + circleCenter[i][2], &uxi);
		uyd = modf(circleCenter[i][1] + circleCenter[i][2], &uyi);

		pts_0.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd);
		pts_0.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd);
		pts_1.x = (PWidth * ((int)uxi - z_pos.x)) + (PWidth * uxd);
		pts_1.y = (PHeight * ((int)uyi - z_pos.y)) + (PHeight * uyd);

		pDC->Ellipse(pts_0.x, pts_0.y, pts_1.x, pts_1.y);
	}

	pDC->SelectObject(oldPen); // 이전 팬 선택		
	pen->DeleteObject();  // 생성한 펜 메모리에서 제거		
	delete pen;
}

void CImgViewerView::DrawCirLines(CDC* pDC, vector<pair<Point2d, Point2d>>& cirLinePoints)
{
	CPen* pen;
	pen = new CPen(PS_SOLID, 3, RGB(255, 127, 0));
	CPen* oldPen;
	oldPen = pDC->SelectObject(pen);
	pDC->SelectStockObject(NULL_BRUSH);

	double dxi, dxd;
	double dyi, dyd;
	double uxi, uxd;
	double uyi, uyd;

	for (int i = 0; i < cirLinePoints.size(); i++)
	{
		dxd = modf(cirLinePoints[i].first.x, &dxi);
		dyd = modf(cirLinePoints[i].first.y, &dyi);
		uxd = modf(cirLinePoints[i].second.x, &uxi);
		uyd = modf(cirLinePoints[i].second.y, &uyi);

		pts_0.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd);
		pts_0.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd);
		pts_1.x = (PWidth * ((int)uxi - z_pos.x)) + (PWidth * uxd);
		pts_1.y = (PHeight * ((int)uyi - z_pos.y)) + (PHeight * uyd);

		pDC->MoveTo(pts_0.x, pts_0.y);
		pDC->LineTo(pts_1.x, pts_1.y);
	}

	pDC->SelectObject(oldPen); // 이전 팬 선택		
	pen->DeleteObject();  // 생성한 펜 메모리에서 제거		
	delete pen;
}

void CImgViewerView::DrawCirPoint(CDC* pDC, Point2d interPoint)
{
	CPen* pen;
	pen = new CPen(PS_SOLID, 3, RGB(191, 255, 0));
	CPen* oldPen;
	oldPen = pDC->SelectObject(pen);
	pDC->SelectStockObject(NULL_BRUSH);

	double dxi, dxd;
	double dyi, dyd;

	dxd = modf(interPoint.x, &dxi);
	dyd = modf(interPoint.y, &dyi);
	pts_0.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd) - PWidth * 5;
	pts_0.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd) - PHeight * 5;
	pts_1.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd) + PWidth * 5;
	pts_1.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd) - PWidth * 5;
	pts_2.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd) + PWidth * 5;
	pts_2.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd) + PWidth * 5;
	pts_3.x = (PWidth * ((int)dxi - z_pos.x)) + (PWidth * dxd) - PWidth * 5;
	pts_3.y = (PHeight * ((int)dyi - z_pos.y)) + (PHeight * dyd) + PWidth * 5;

	pDC->MoveTo(pts_0.x, pts_0.y);
	pDC->LineTo(pts_2.x, pts_2.y);
	pDC->MoveTo(pts_1.x, pts_1.y);
	pDC->LineTo(pts_3.x, pts_3.y);

	pDC->SelectObject(oldPen); // 이전 팬 선택		
	pen->DeleteObject();  // 생성한 펜 메모리에서 제거		
	delete pen;
}

void CImgViewerView::MyEllipseR(CDC* pDC, Point2d Center, int radinX, int radinY, double theta, COLORREF parm_color)
{
	// 직선 알고리즘 추가
	CPen* pen = new CPen(PS_SOLID, l_width, parm_color);
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
	delete pen;
}

void CImgViewerView::MyEllipsePS_DOT(CDC* pDC, Point2d Center, int radinX, int radinY, double theta)
{
	// 직선 알고리즘 추가
	int cnt = 0;
	CPoint pt;
	CPoint Rotatept;

	CPen* pen1 = new CPen(PS_SOLID, l_width, RGB(0, 0, 0));
	CPen* pen2 = new CPen(PS_SOLID, l_width, RGB(255, 255, 255));
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

BOOL CImgViewerView::overlap(vector<Point2d> rc, vector<Point2d> V) {

	// polygons overlap if either

	// 1. one of the points of one polygon is inside the other polygon polygon
	if (polygon_points_inside(rc, V)) {
		return TRUE;
	}

	// 2. one of the edges overlap
	if (polygon_edges_overlap(rc, V)) {
		return TRUE;
	}

	return FALSE;
}

void CImgViewerView::OnClose()
{
	data.RemoveAll();
	CView::OnClose();
}

void CImgViewerView::OnSelectLw()
{
	CSelectLineWidth lwDlg;
	if (lwDlg.DoModal() == IDOK)
	{
		l_width = lwDlg.l_width;
	}
}

void CImgViewerView::OnSelectColor()
{
	CColorDialog cDlg;
	if (cDlg.DoModal() == IDOK)
	{
		color = cDlg.GetColor();
	}
}