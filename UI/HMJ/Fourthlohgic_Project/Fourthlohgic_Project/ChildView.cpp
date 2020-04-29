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

	m_background.Attach(image.Detach());
	m_background.GetObject(sizeof(BITMAP), (LPVOID)&m_Bitmap);
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

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//---------------------------------------------------
	GetClientRect(&tmpRect);
	CDC* pDC = GetDC();

   // 이미 배경은 OnInitDialog() 혹은 OnInitialUpdate()에서 로드되어 있으므로 다시 할 필요는 없다.
	memDC.CreateCompatibleDC(pDC);
	mdcOffScreen.CreateCompatibleDC(pDC);

	// 화면 크기로 빈공간의 버퍼를 생성 한다.
	bmpOffScreen.CreateCompatibleBitmap(pDC, m_Bitmap.bmWidth, m_Bitmap.bmHeight);
	// 아직 dmemDC의 메모리에는 아무런 그림이 없다.
	// 만약 어떤 색깔로 채우고자 한다면 FillRect() 함수등으로 특정색으로 칠할 수 있다.
	// 그러나 다음에 배경 그림을 로드하므로 필요없는 일이다.

	oldbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);
	// 이제 메모리 준비는 끝났다. 지금 부터는 그림을 그린다.
	//우선 배경 그림이 맨 밑이므로 배경을 메모리에 복사 한다.

	memDC.SelectObject(&m_background);   // 배경 그림을 선택하고
	mdcOffScreen.BitBlt(tmpRect.left, tmpRect.top, tmpRect.right, tmpRect.bottom, &memDC, 0, 0, SRCCOPY);
	// ==> 배경을 메모리버퍼에 복사 한다. 아직 화면에는 나타나지 않는다.
	//따라서 그림은 화면에 나타나지 않고, 디버깅이 힘들다.
	//디버깅을 싶게 한다면
	//pDC->BitBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &memDC, 0, 0, SRCCOPY); 
	//    한줄 더 넣어 화면을 확인하고 디버깅이 끝나면 삭제 한다.
   // 이번에는 BitBlt 말고 다른 윈도우 함수를 역시 메모리에 복사 한다.
	//----------
	//mdcOffScreen.SetTextColor((COLORREF)0x00FFFFFF);
	//==> 이것은 pDC->SetTextColor( (COLORREF) 0x00FFFFFF );와는 별개로 상관없음
	//mdcOffScreen.SetBkMode(TRANSPARENT);   // 글자의 배경색을 없앤다.
	//-----------
	// 여기까지 모든 그림이 완성되어 지만, 아직 표시 버퍼에 출력된 상태가 아니다. 디버깅을 해보면 아직 그림이 표시되지 않는다.
	// 최종적으로 표시 화면 메모리에 복사 한다.
	//pDC->StretchBlt(0, 0, tmpRect.right, tmpRect.bottom, &mdcOffScreen, tmpRect.left, tmpRect.top, tmpRect.right, tmpRect.bottom, SRCCOPY);
	
	pDC->BitBlt(tmpRect.left, tmpRect.top, tmpRect.right, tmpRect.bottom, &mdcOffScreen, 0, 0, SRCCOPY);
	//pDC->BitBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &mdcOffScreen, 0, 0, SRCCOPY);

	// 이때서야 화면에 그림이 나타난다.
	memDC.DeleteDC();
	mdcOffScreen.SelectObject(oldbitmap);
	mdcOffScreen.DeleteDC();
	bmpOffScreen.DeleteObject();
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
		CDC* pDC = GetDC();
		

		// 메모리 DC 준비
		CClientDC DC(this);
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		CBitmap* pOldBM = (CBitmap*)memDC.SelectObject(&m_background); //pOldBM에 m_bmBack 넣기

		CPen pen;
		pen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
		CPen* p = (CPen*)pDC->SelectObject(&pen);

		// A영역의 사각형 그리기 //m_sPt는 기존 포인트, point는 이동
		if (m_sPt.x < point.x) // 오른쪽으로 끌었을때
		{
			//pDC->Rectangle(0, 0, (point.x - m_sPt.x), tmpRect.bottom);
			pDC->Rectangle(tmpRect.left + (point.x - m_sPt.x), tmpRect.top, tmpRect.right + (point.x - m_sPt.x), tmpRect.bottom);
		}
			//tmpRect = CRect((point.x + m_sPt.x), 0, (point.x - m_sPt.x), tmpRect.bottom);
		else // 왼쪽으로 끌었을때
			pDC->Rectangle(tmpRect.left - (m_sPt.x - point.x), tmpRect.top, tmpRect.right - (m_sPt.x - point.x), tmpRect.bottom);

		// B영역의 사각형 그리기
		if (m_sPt.y < point.y) // 위로 올렸을때
			pDC->Rectangle(tmpRect.left, tmpRect.top - (m_sPt.y - point.y), tmpRect.right, tmpRect.bottom - (m_sPt.x - point.x));
		else // 아래로 내렸을때
			pDC->Rectangle(tmpRect.left, tmpRect.top + (point.y - m_sPt.y), tmpRect.right, tmpRect.bottom + (point.x - m_sPt.x));

		// 이미지 붙이기
		/*pDC->BitBlt(offset.cx, offset.cy, tmpRect.right,
			tmpRect.bottom, &mdcOffScreen, 0, 0, SRCCOPY);
		pDC->SelectObject(p);
		memDC.SelectObject(pOldBM);*/

		Invalidate();
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

	Invalidate(0);
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
	//GetClientRect(tmpRect);

	//CClientDC dc(this);

	//// 기존에 생성되어 있던 메모리DC 객체를 떼어낸다.
	//m_memDC.DeleteDC();

	//// 새롭게 다시 화면DC와 호환되는 메모리 DC생성

	//m_memDC.CreateCompatibleDC(&dc);
	//m_bmBack.DeleteObject();

	//// 메모리 DC에 붙일 비트맵 다시 생성
	//m_bmBack.CreateCompatibleBitmap(&dc, tmpRect.Width(), tmpRect.Height());
}
