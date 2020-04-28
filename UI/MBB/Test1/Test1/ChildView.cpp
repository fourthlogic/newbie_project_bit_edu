
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
CChildView::CChildView()
{
	static TCHAR BASED_CODE szFilter[] = _T("이미지 파일(*.BMP, *.GIF, *.JPG, *PNG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.png;*.gif; |모든파일(*.*)|*.*||");

	//CFileDialog dlg(TRUE, _T("*.jpg"), _T("image"), OFN_HIDEREADONLY, szFilter);
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (IDOK != dlg.DoModal()) return;

	filePath = dlg.GetPathName();

	
	
	//CImage -> HBITMAP
	//hbm = (HBITMAP)srcimage;
	
	//CImage -> CBitmap
	//cbm = CBitmap::FromHandle(srcimage);

	//Mat imread
	/*filePathName = CT2CA(filePath);
	image = imread(filePathName, IMREAD_COLOR);*/

	

	/*CString tmp;
	tmp.Format(_T("%d, %d"), _iViewportWidth, _iViewportHeight);
	MessageBox(tmp);*/

	//m_nZoom = 100;


	_fViewportScale = 1.f;
	_bMouseLClick = true;

}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
ON_WM_PAINT()
ON_WM_ERASEBKGND()
ON_WM_MOUSEMOVE()
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

	// DC 포인터
	
	CDC* pDC = &dc;
	srcimage.Load(filePath);
	cbm = CBitmap::FromHandle(srcimage);

	pDC->SetMapMode(MM_ANISOTROPIC);

	pDC->SetViewportExt(m_nZoom, m_nZoom);

	pDC->SetWindowExt(100, 100);
	// 메모리 DC 생성
	CDC MemDC;

	MemDC.CreateCompatibleDC(pDC);
	pOldBmp = NULL;

	cbm->GetBitmap(&bmInfo);

	pOldBmp = (CBitmap*)MemDC.SelectObject(cbm);

	pDC->BitBlt(0, 0, bmInfo.bmWidth, bmInfo.bmHeight,
		&MemDC, 0, 0, SRCCOPY);



	// 클라이언트 영역 저장
	CRect rect;
	GetClientRect(&rect);

	//// 이미지 영역을 메모리 DC에 복사
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, width, height);
	CBitmap* pOldBmp;
	pOldBmp = (CBitmap*)MemDC.SelectObject(&bmp);
	MemDC.PatBlt(0, 0, width, height, WHITENESS);

	// 마우스 좌표 출력
	/*CString mousePoint;
	mousePoint.Format(L"ClientMousePoint : %d, %d", 
		_clientMousePoint.x, _clientMousePoint.y);
	MemDC.TextOut(0, 0, mousePoint);*/

	//MessageBox(mousePoint);

	// 새로 그린 영역을 뷰포트 크기만큼 DC에 복사
	pDC->SetStretchBltMode(HALFTONE);
	
	int bx = bmInfo.bmWidth;
	int by = bmInfo.bmHeight;
	pDC->StretchBlt(0, 0, bx, by, &MemDC, 
		_clientMousePoint.x/2, _clientMousePoint.y/2,
		_iViewportWidth, _iViewportHeight, SRCCOPY);

	// DC 메모리 해제
	srcimage.Destroy();
	MemDC.SelectObject(pOldBmp);
	MemDC.DeleteDC();
	bmp.DeleteObject();
	

	// 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.
}



//void CChildView::OnPaint() 
//{
//	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.
//	//PrintImg(dc);
//	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
//	//srcimage.Draw(dc, height, width);
//
//
//
//	CDC MemDC;
//	MemDC.CreateCompatibleDC(&dc);
//
//	CBitmap* pOldBmp = NULL;
//
//	cbm->GetBitmap(&bmInfo);
//	
//	pOldBmp = MemDC.SelectObject(cbm);
//
//	dc.BitBlt(0, 0, bmInfo.bmWidth, bmInfo.bmHeight, &MemDC, 0, 0, SRCCOPY);
//
//	MemDC.SelectObject(pOldBmp);
//	
//	
//	//SelectObject(dc, cbm);
//	//BitBlt(dc, 0, 0, width, height, dc, 0, 0, SRCCOPY); //화면 출력부분
//
//	//BITMAP obj;
//	//GetObject(cbm, sizeof(BITMAP), &obj);
//	//BYTE* data = new BYTE[obj.bmWidth * obj.bmHeight * (obj.bmBitsPixel / 8)];
//	//GetBitmapBits(cbm, obj.bmWidth * obj.bmHeight * (obj.bmBitsPixel / 8), data);
//
//	//for (int t = 0; t < obj.bmHeight; t++)
//	//	for (int i = 0; i < obj.bmWidth; i++)
// //  {
//
// //  //여기서 하고싶은대로 변환
// //  int pixeldata = *((int*)(data + i * (obj.bmBitsPixel / 8) + t * obj.bmWidthBytes));  // (i,t)에 해당하는 픽셀 데이타 값.
// //  }
//
//
//
//	//SetBitmapBits(cbm, obj.bmWidth * obj.bmHeight * (obj.bmBitsPixel / 8), data);//변환된값 다시 적용.
//	//BitBlt(dc, 0, 0, obj.bmWidth, obj.bmHeight, dc, 0, 0, SRCCOPY); //변환된것 다시 출력
//	// 그리기 메시지에 대해서는 CWnd::OnPaint()를 호출하지 마십시오.
//}

void CChildView::PrintImg(HDC dc)
{
	Mat dstImg(height, width, CV_8UC3);

	for (int y = 0; y < height; y++)
	{
		// y번째 row에 대한 주소를 포인터에 저장한 후
		uchar* pointer_input = image.ptr<uchar>(y);
		uchar* pointer_ouput = dstImg.ptr<uchar>(y);

		for (int x = 0; x < width; x++) {

			// row 포인터로부터 (x * 3 )번째 떨어져 있는 픽셀을 가져옵니다.
			//0, 1, 2 순서대로 blue, green, red 채널값을 가져올 수있는 이유는 하나의 픽셀이 메모리상에 b g r 순서대로 저장되기 때문입니다. 

			uchar b = pointer_input[x * 3 + 0];
			uchar g = pointer_input[x * 3 + 1];
			uchar r = pointer_input[x * 3 + 2];

			pointer_ouput[x] = (r + g + b) / 3.0;
			SetPixel(dc, x, y, RGB(r, g, b));
		}
	}
}



BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.


	srcimage.Load(filePath);

	height = srcimage.GetHeight();
	width = srcimage.GetWidth();

	_iViewportWidth = width;
	_iViewportHeight = height;

	cbm = CBitmap::FromHandle(srcimage);


	CDC MemDC;

	MemDC.CreateCompatibleDC(pDC);
	pOldBmp = NULL;

	cbm->GetBitmap(&bmInfo);

	pOldBmp = (CBitmap*)MemDC.SelectObject(cbm);

	pDC->BitBlt(0, 0, bmInfo.bmWidth, bmInfo.bmHeight, 
		&MemDC, 0, 0, SRCCOPY);

	srcimage.Destroy();
	MemDC.SelectObject(pOldBmp);
	MemDC.DeleteDC();



	return TRUE;

	//return CWnd::OnEraseBkgnd(pDC);
}




void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// 마우스를 클릭하고 있는 경우 뷰포트 Org 위치 수정
	/*
	if (_bMouseLClick)
		_viewportPoint = _mouseLClickPoint - point;

	// 마우스 위치 클라이언트상 좌표
	CPoint temp = CPoint(point.x * _fViewportScale, point.y * _fViewportScale);
	_clientMousePoint = _viewportPoint + temp;
	*/

	if (nFlags & MK_LBUTTON)
		OnLButtonDown(nFlags, point);
	else if (nFlags == 0)
		OnLButtonUp(nFlags, point);

	//CDialogEx::OnMouseMove(nFlags, point);
	CWnd::OnMouseMove(nFlags, point);
}


BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//마우스 휠 다운
	if (zDelta <= 0)
	{
		_fViewportScale += 1.25f;
		//m_nZoom += 150;

	}
	//마우스 휠 업
	else
	{
		_fViewportScale -= 0.8f;
		//m_nZoom -= 50;
	}
	_iViewportWidth = _fViewportScale * width;
	_iViewportHeight = _fViewportScale * height;
	/*CString tmp;
	tmp.Format(_T("%d, %d"), _iViewportWidth, _iViewportHeight);
	MessageBox(tmp);*/

	//RedrawWindow();
	/*Invalidate(false);
	UpdateWindow();*/
	Invalidate(false);
	
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}