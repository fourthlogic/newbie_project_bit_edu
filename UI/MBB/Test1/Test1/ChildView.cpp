
// ChildView.cpp: CChildView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "Test1.h"
#include "ChildView.h"
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;



// CChildView
CImage srcimage;
CBitmap* pOldBmp;
CString filePath;
int a;
CDC* pDC;
CChildView::CChildView()
{
	static TCHAR BASED_CODE szFilter[] = _T("이미지 파일(*.BMP, *.GIF, *.JPG, *PNG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.png;*.gif; |모든파일(*.*)|*.*||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);

	if (IDOK != dlg.DoModal()) return;

	filePath = dlg.GetPathName();

	a = 0;

	pOldBmp = NULL;

	_fViewportScale = 1.f;
	_bMouseLClick = true;

	srcimage.Load(filePath);
	cbm = CBitmap::FromHandle(srcimage);
	
	cbm->GetBitmap(&bmInfo);
	height = srcimage.GetHeight();
	width = srcimage.GetWidth();

	_iViewportWidth = width;
	_iViewportHeight = height;

}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
ON_WM_PAINT()
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
	CPaintDC dc(this); // device context for painting

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	pDC = &dc;
	CDC MemDC;

	CPoint pt;
	GetCursorPos(&pt);
	MemDC.CreateCompatibleDC(pDC);
	

	pOldBmp = (CBitmap*)MemDC.SelectObject(cbm);
	if (a == 0)
	{
		pDC->BitBlt(0, 0, bmInfo.bmWidth, bmInfo.bmHeight,
			&MemDC, 0, 0, SRCCOPY);

		//MemDC.DeleteDC();
		a++;
	}
	else
	{
		//// 이미지 영역을 메모리 DC에 복사
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(pDC, width, height);

		pOldBmp = (CBitmap*)MemDC.SelectObject(&bmp);
		MemDC.PatBlt(0, 0, width, height, PATCOPY);

		// 새로 그린 영역을 뷰포트 크기만큼 DC에 복사
		pDC->SetStretchBltMode(HALFTONE);

		int bx = bmInfo.bmWidth;
		int by = bmInfo.bmHeight;
		pDC->StretchBlt(0, 0, _iViewportWidth, _iViewportHeight, &MemDC,
			pt.x/2, pt.y/2, bx/2, by/2, SRCCOPY);
	}

	// DC 포인터
	srcimage.Destroy();
	MemDC.SelectObject(pOldBmp);
	//ReleaseDC(&MemDC);
	ReleaseDC(&dc);
	
	// 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.
}

BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//마우스 휠 다운
	if (zDelta <= 0)
	{
		_fViewportScale += 1.25f;
	}
	//마우스 휠 업
	else
	{
		_fViewportScale -= 0.8f;
	}
	_iViewportWidth = _fViewportScale * width;
	_iViewportHeight = _fViewportScale * height;

	Invalidate(false);
	
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}