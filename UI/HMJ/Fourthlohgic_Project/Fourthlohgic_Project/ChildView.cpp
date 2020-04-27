// ChildView.cpp: CChildView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "Fourthlohgic_Project.h"
#include "ChildView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CFourthlohgicProjectApp theApp;
// CChildView

CChildView::CChildView()
{
	if (!image.IsNull())
		image.Destroy();
	image.Load(theApp.sFilename);
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CChildView 메시지 처리기

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);

	return TRUE;
}

void CChildView::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.
	CDC* pDC = GetDC();

	// 비트맵과 메모리 DC를 연결
	//CBitmap* m_bmBack = CBitmap::FromHandle(image);
	//CBitmap* bt = (CBitmap*)m_memDC.SelectObject(&m_bmBack);

	CBitmap* pOldBitmap;
	CBitmap bmp;

	GetClientRect(tmpRect);
	m_memDC.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, tmpRect.Width(), tmpRect.Height());
	pOldBitmap = (CBitmap*)m_memDC.SelectObject(&bmp);
	m_memDC.PatBlt(0, 0, tmpRect.Width(), tmpRect.Height(), WHITENESS);

	DrawImage(&m_memDC); // 그리기 함수 구현 해줄곳

	pDC->BitBlt(0, 0, tmpRect.Width(), tmpRect.Height(), &m_memDC, 0, 0, SRCCOPY);

	m_memDC.SelectObject(pOldBitmap);
	m_memDC.DeleteDC();
}

void CChildView::DrawImage(CDC* pDC)
{
	//image.Draw(pDC, tmpRect);
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//return CWnd::OnEraseBkgnd(pDC);
	return false;
}


void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnMouseMove(nFlags, point);

	if (nFlags & MK_LBUTTON)

	{
		// 이동 크기 구하기
		CSize offset = point - m_sPt;

		// 화면의 크기를 구하기
		GetClientRect(&tmpRect);

		// 메모리 DC 준비
		CClientDC DC(this);
		CDC memDC;
		memDC.CreateCompatibleDC(&DC);
		CBitmap* pOldBM = (CBitmap*)memDC.SelectObject(&m_bmBack); //pOldBM에 m_bmBack 넣기

		CPen pen;
		pen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
		CPen* p = (CPen*)DC.SelectObject(&pen);

		// A영역의 사각형 그리기
		if (m_sPt.x < point.x) // 오른쪽으로 끌었을때
			DC.Rectangle(0, 0, (point.x - m_sPt.x), tmpRect.bottom);
		else // 왼쪽으로 끌었을때
			DC.Rectangle(tmpRect.right - (m_sPt.x - point.x),
				0, tmpRect.right, tmpRect.bottom);

		// B영역의 사각형 그리기

		if (m_sPt.y < point.y) // 위로 올렸을때
			DC.Rectangle(0, 0, tmpRect.right, (point.y - m_sPt.y));
		else // 아래로 내렸을때
			DC.Rectangle(0, tmpRect.bottom - (m_sPt.y - point.y),
				tmpRect.right, tmpRect.bottom);

		// 이미지 붙이기
		DC.BitBlt(offset.cx, offset.cy, tmpRect.right,
			tmpRect.bottom, &memDC, 0, 0, SRCCOPY);
		DC.SelectObject(p);
		memDC.SelectObject(pOldBM);
	}
}


void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnLButtonDown(nFlags, point);
	m_sPt = m_ePt = point;
}


void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_ePt = point;
	double mx1, mx2, my1, my2;
	DeviceToWorld(m_sPt.x, m_sPt.y, mx1, my1);
	DeviceToWorld(m_ePt.x, m_ePt.y, mx2, my2);

	mox += (mx1 - mx2);
	moy += (my1 - my2);

	double mWidth = mxMax - mxMin;
	double mHeight = myMax - myMin;

	mxMin = mox - (mWidth / 2);
	mxMax = mox + (mWidth / 2);
	myMin = moy - (mHeight / 2);
	myMax = moy + (mHeight / 2);

	Invalidate();
	CWnd::OnLButtonUp(nFlags, point);
}

void CChildView::DeviceToWorld(long m_sPtx, long m_sPty, double &mx, double &my)
{
	mx = (long)(m_sPtx) + fOffsetX;
	my = (long)(m_sPtx) + fOffsetY;
}


void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	// 화면 크기 얻기
	GetClientRect(tmpRect);

	CClientDC dc(this);

	// 기존에 생성되어 있던 메모리DC 객체를 떼어낸다.
	m_memDC.DeleteDC();

	// 새롭게 다시 화면DC와 호환되는 메모리 DC생성

	m_memDC.CreateCompatibleDC(&dc);
	m_bmBack.DeleteObject();

	// 메모리 DC에 붙일 비트맵 다시 생성
	m_bmBack.CreateCompatibleBitmap(&dc, tmpRect.Width(), tmpRect.Height());
}
