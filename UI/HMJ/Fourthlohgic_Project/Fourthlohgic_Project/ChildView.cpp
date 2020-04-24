﻿// ChildView.cpp: CChildView 클래스의 구현
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

	//MakeCellFrameImage(theApp.sFilename);
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
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

	CImage    TempSourceImage;
	CImage    TempDestImage;

	TempSourceImage.Load(theApp.sFilename);
	CDC* pSourceDC = CDC::FromHandle(TempSourceImage.GetDC());

	int BitPerPixel = pSourceDC->GetDeviceCaps(BITSPIXEL);
	TempDestImage.Create(image.GetWidth(), image.GetHeight(), BitPerPixel);
	CDC* pDestDC = CDC::FromHandle(TempDestImage.GetDC());

	if (!pSourceDC || !pDestDC)
	{
		return;
	}

	pDestDC->BitBlt(0, 0, image.GetWidth(), image.GetHeight(),
		pSourceDC, 0, 0, SRCCOPY);

	TempDestImage.Draw(dc, 0, 0);

	TempDestImage.ReleaseDC();
	TempSourceImage.ReleaseDC();
}
