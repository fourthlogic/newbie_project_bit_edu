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

	//m_ePt.x = 0;
	//m_ePt.y = 0;

	m_bkgBrush.CreateSolidBrush(0x00000000);
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
	ON_WM_MOUSEWHEEL()
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
	//mdcOffScreen.BitBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &memDC, 0, 0, SRCCOPY);
	mdcOffScreen.StretchBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &memDC, m_ePt.x, m_ePt.y, m_Bitmap.bmWidth, m_Bitmap.bmHeight, SRCCOPY);
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
	
	pDC->BitBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &mdcOffScreen, 0, 0, SRCCOPY);
	//pDC->StretchBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &mdcOffScreen, m_ePt.x, m_ePt.y, m_Bitmap.bmWidth, m_Bitmap.bmHeight, SRCCOPY);

	// 이때서야 화면에 그림이 나타난다.
	memDC.DeleteDC();
	mdcOffScreen.SelectObject(oldbitmap);
	mdcOffScreen.DeleteDC();
	bmpOffScreen.DeleteObject();
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// 클라 크기가 이미지보다 클때 검은화면으로 채워주기
	if (m_Bitmap.bmWidth < m_bgRect.right) { //오른쪽
		RECT rect2;
		rect2.left = m_Bitmap.bmWidth;
		rect2.right = m_bgRect.right;
		rect2.top = 0;
		rect2.bottom = m_Bitmap.bmHeight;
		m_ePt.x = 0;
		pDC->FillRect(&rect2, &m_bkgBrush);  // B영역 칠하기
	}
	if (m_Bitmap.bmHeight < m_bgRect.bottom) { //아래
		RECT rect2;
		rect2.left = 0;
		rect2.right = m_bgRect.right;
		rect2.top = m_Bitmap.bmHeight;
		rect2.bottom = m_bgRect.bottom;
		m_ePt.y = 0;
		pDC->FillRect(&rect2, &m_bkgBrush); // C영역 칠하기
	}

	//return CWnd::OnEraseBkgnd(pDC);
	return false;
}


void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnMouseMove(nFlags, point);

	if (nFlags & MK_LBUTTON)
	{
		int width = m_bgRect.right - m_bgRect.left;
		int height = m_bgRect.bottom - m_bgRect.top;

		// A영역의 사각형 그리기 //m_sPt는 기존 포인트, point는 이동
		if (m_sPt.x < point.x) // 오른쪽으로 끌었을때
		{
			//pDC->Rectangle(0, 0, (point.x - m_sPt.x), tmpRect.bottom);
			m_ePt.x -= point.x - m_sPt.x;
			if (m_ePt.x < 0)
				m_ePt.x = 0;
		}
		else // 왼쪽으로 끌었을때
		{
			m_ePt.x += m_sPt.x - point.x;
			if (m_Bitmap.bmWidth > m_bgRect.right)
			{
				if (m_ePt.x > m_Bitmap.bmWidth - width)
					m_ePt.x = m_Bitmap.bmWidth - width;
			}
		}
			
		// B영역의 사각형 그리기
		if (m_sPt.y < point.y) // 위로 올렸을때
		{
			m_ePt.y -= point.y - m_sPt.y;
			if (m_ePt.y < 0)
				m_ePt.y = 0;
		}
		else // 아래로 내렸을때
		{
			m_ePt.y += m_sPt.y - point.y;
			if (m_Bitmap.bmHeight > m_bgRect.bottom)
			{
				if (m_ePt.y > m_Bitmap.bmHeight - height)
					m_ePt.y = m_Bitmap.bmHeight - height;
			}
		}

		m_sPt = point;
		Invalidate();
	}
}


void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnLButtonDown(nFlags, point);
	m_sPt = point;
}


void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_bgRect.left = 0;
	m_bgRect.top = 0;
	m_bgRect.right = cx;
	m_bgRect.bottom = cy;
}


BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	int width = m_bgRect.right - m_bgRect.left;
	int height = m_bgRect.bottom - m_bgRect.top;

	if (zDelta <= 0)// 휠 내릴때
	{
		ViewScale += 0.1f;
	}
	else// 휠 올릴때
	{
		ViewScale -= 0.1f;
	}

	Invalidate();
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
