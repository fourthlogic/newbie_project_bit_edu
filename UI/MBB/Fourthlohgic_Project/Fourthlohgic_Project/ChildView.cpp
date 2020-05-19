// ChildView.cpp: CChildView 클래스의 구현
//


#include "pch.h"
#include "framework.h"
#include "Fourthlohgic_Project.h"
#include "ChildView.h"

#include <iostream>

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#define GetRValue( rgb )    ( ( BYTE )( rgb ) )

#define GetGValue( rgb )    ( ( BYTE )( ( ( WORD )( rgb ) ) >> 8 ))

#define GetBValue( rgb )    ( ( BYTE )( ( rgb ) >> 16 ) )

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//extern CFourthlohgicProjectApp theApp;
// CChildView

//Find the position "factor"


CChildView::CChildView()
{
	m_Zoom = 1.f;
	m_ePt = CPoint(0, 0);
	//width = GetSystemMetrics(SM_CXSCREEN);
	//height = GetSystemMetrics(SM_CYSCREEN);

	m_bkgBrush.CreateSolidBrush(0x00000000);



	char szFilter[] = "이미지 파일(*.BMP, *.GIF, *.JPG, *PNG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.gif;*.png; |모든파일(*.*)|*.*||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CA2CT(szFilter), AfxGetMainWnd());
	if (dlg.DoModal() == IDOK)
	{
		CString cstrImgPath = dlg.GetPathName();
		CT2CA pszConvertedAnsiString(cstrImgPath);
		std::string strStd(pszConvertedAnsiString);

		srcImage = cv::imread(strStd, IMREAD_COLOR);	//Mat으로 로드

		HBITMAP hbitmap = mat2bmp(&srcImage);	//Mat을 Bitmap으로 변환

		m_background.Attach(hbitmap);
		m_background.GetObject(sizeof(BITMAP), (LPVOID)&m_Bitmap);
	}
	scalex = m_Bitmap.bmWidth;
	scaley = m_Bitmap.bmHeight;

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
	bmpOffScreen.CreateCompatibleBitmap(pDC, scalex, scaley);

	// 아직 dmemDC의 메모리에는 아무런 그림이 없다.
	// 만약 어떤 색깔로 채우고자 한다면 FillRect() 함수등으로 특정색으로 칠할 수 있다.
	// 그러나 다음에 배경 그림을 로드하므로 필요없는 일이다.

	oldbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);
	// 이제 메모리 준비는 끝났다. 지금 부터는 그림을 그린다.
	//우선 배경 그림이 맨 밑이므로 배경을 메모리에 복사 한다.

	memDC.SelectObject(&m_background);   // 배경 그림을 선택하고
	
	mdcOffScreen.SetStretchBltMode(COLORONCOLOR);	

	mdcOffScreen.StretchBlt(0, 0, scalex, scaley,
		&memDC, 0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, SRCCOPY);

	cout << "scalex: " << scalex << " scaley: " << scaley << endl;
	cout <<"m_Bitmap.bmWidth: "<< m_Bitmap.bmWidth << " m_Bitmap.bmHeight: " << m_Bitmap.bmHeight << endl;

	pDC->SetStretchBltMode(COLORONCOLOR);
	pDC->StretchBlt(0, 0, m_bgRect.right, m_bgRect.bottom, 
		&mdcOffScreen, m_ePt.x, m_ePt.y, m_bgRect.right, m_bgRect.bottom, SRCCOPY);

	cout << "m_ePt.x: " << m_ePt.x << " m_ePt.y: " << m_ePt.y << endl;

	//cout << "[" << m_ePt.x << ", " << m_ePt.y << "]" << endl;

	//oldcbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);


	//cdcOffScreen.SetStretchBltMode(COLORONCOLOR);

	//cdcOffScreen.StretchBlt(0, 0, m_bgRect.right, m_bgRect.bottom,
	//	&mdcOffScreen, m_aPt.x, m_aPt.y, m_bgRect.right, m_bgRect.bottom, SRCCOPY);

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



	//pDC->BitBlt(0, 0, m_Bitmap.bmWidth , m_Bitmap.bmHeight, &mdcOffScreen, 0, 0, SRCCOPY);

	// 이때서야 화면에 그림이 나타난다.
	memDC.DeleteDC();
	mdcOffScreen.SelectObject(oldbitmap);
	mdcOffScreen.DeleteDC();
	bmpOffScreen.DeleteObject();
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//if (m_Bitmap.bmWidth < m_bgRect.right) {
	//	RECT rect2;
	//	rect2.left = m_Bitmap.bmWidth;
	//	rect2.right = m_bgRect.right;
	//	rect2.top = 0;
	//	rect2.bottom = m_Bitmap.bmHeight;
	//	//m_ePt.x = 0;
	//	pDC->FillRect(&rect2, &m_bkgBrush);  // B영역 칠하기
	//}
	//if (m_Bitmap.bmHeight < m_bgRect.bottom) {
	//	RECT rect2;
	//	rect2.left = 0;
	//	rect2.right = m_bgRect.right;
	//	rect2.top = m_Bitmap.bmHeight;
	//	rect2.bottom = m_bgRect.bottom;
	//	//m_ePt.y = 0;
	//	pDC->FillRect(&rect2, &m_bkgBrush); // C영역 칠하기
	//}

	//return CWnd::OnEraseBkgnd(pDC);
	return false;
}


void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnMouseMove(nFlags, point);
	m_pos = point;

	
	if (nFlags & MK_LBUTTON)
	{
		// A영역의 사각형 그리기 //m_sPt는 기존 포인트, point는 이동
		if (m_sPt.x < point.x) // 오른쪽으로 끌었을때
		{
			m_ePt.x -= point.x - m_sPt.x;
			if (m_ePt.x < 0)
			{
				m_ePt.x = 0;

				//if (m_aPt.x > 0)
				//	m_aPt.x -= point.x - m_sPt.x;
			}
		}
		else // 왼쪽으로 끌었을때
		{

			if (m_ePt.x + zoomWidth < m_Bitmap.bmWidth)
				m_ePt.x += m_sPt.x - point.x;
			if (m_Bitmap.bmWidth > m_bgRect.right)
			{
				if (m_ePt.x > m_Bitmap.bmWidth - zoomWidth)
					m_ePt.x = m_Bitmap.bmWidth - zoomWidth;
			}
		}

		// B영역의 사각형 그리기
		if (m_sPt.y < point.y) // 위로 올렸을때
		{
			m_ePt.y -= point.y - m_sPt.y;
			if (m_ePt.y < 0)
			{
				m_ePt.y = 0;
				/*if (m_aPt.y > 0)
				{
					m_aPt.y -= 1;
				}*/
			}
		}
		else // 아래로 내렸을때
		{
			m_ePt.y += m_sPt.y - point.y;
			if (m_Bitmap.bmHeight > m_bgRect.bottom)
			{
				if (m_ePt.y > m_Bitmap.bmHeight - zoomHeight)
					m_ePt.y = m_Bitmap.bmHeight - zoomHeight;
			}
		}

		m_sPt = point;
		Invalidate(FALSE);
	}
}



void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnLButtonDown(nFlags, point);
	m_sPt = point;

	//좌표의 픽셀값
	//CDC* dc = GetDC();
	//
	//COLORREF rgb;
	//rgb = dc->GetPixel(point.x, point.y);

	//int R = 0, G = 0, B = 0;

	//R = GetRValue(rgb);
	//G = GetGValue(rgb);
	//B = GetBValue(rgb);

	//cout << R << " " << G << " " << B << endl << endl;
	//double pixelvale = (R + G + B) / 3.0;
	//
	//cout << pixelvale << endl;
}

void RGBtoGray(COLORREF& ref)
{
	BYTE byGray =
		(GetRValue(ref) * 30
			+ GetGValue(ref) * 59
			+ GetBValue(ref) * 11) / 100;

	ref = RGB(byGray, byGray, byGray);
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	m_bgRect.left = 0;
	m_bgRect.top = 0;
	m_bgRect.right = cx;
	m_bgRect.bottom = cy;

	//scalex = cx;
	//scaley = cy;

	zoomWidth = cx;
	zoomHeight = cy;

	Invalidate(FALSE);
}


BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//휠 다운
	if (zDelta <= 0)
	{
		m_Zoom = 0.8f;
		zoom = 1;
		//scalex = scalex / zoom;
		//scaley = scaley / zoom;
	}
	//휠 업
	else
	{
		m_Zoom = 1.25f;
		zoom = 2;
		//scalex = scalex * zoom;
		//scaley = scaley * zoom;
	}

	//시작 좌표
	//m_ePt.x += round((((float)m_pos.x / (float)m_bgRect.right) * scalex)
	//	- (((float)m_pos.x / (float)m_bgRect.right) * (scalex * m_Zoom)));
	//m_ePt.y += round((((float)m_pos.y / (float)m_bgRect.bottom) * scaley )
	//	- (((float)m_pos.y / (float)m_bgRect.bottom) * (scaley * m_Zoom)));

	//사각형 넓이
	zoomWidth *= m_Zoom;
	zoomHeight *= m_Zoom;

	scalex *= m_Zoom;
	scaley *= m_Zoom;
	//cout << zoomWidth << "  " << zoomHeight << endl;
	//int pixel;
	//CString pixels;
	//String pixelvalue;
	//if (zoomWidth < 40)
	//{
	//	for (int y = m_ePt.y; y < zoomHeight; y++)
	//	{
	//		uchar* pointer_input = srcImage.ptr<uchar>(y);
	//		//uchar* pointer_output = dstImage1.ptr<uchar>(y);
	//		for (int x = m_ePt.x; x < zoomWidth; x++)
	//		{
	//			uchar b = pointer_input[x * 3 + 0];
	//			uchar g = pointer_input[x * 3 + 1];
	//			uchar r = pointer_input[x * 3 + 2];

	//			pixel = round((r + g + b) / 3.0);
	//			pixels.Format(_T("%d"), pixel);
	//			pixelvalue = CT2CA(pixels);

	//			cv::putText(srcImage, pixelvalue, Point(x, y), 1, 4, (255, 255, 255), 2, 0);
	//		}
	//	}
	//}
	
	Invalidate();
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


HBITMAP CChildView::mat2bmp(Mat* image)
{
	// 현재 응용프로그램의 스크린과 호환되는 memory dc를 생성한다.
	HDC         hDC = ::CreateCompatibleDC(0);
	BYTE        tmp[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
	BITMAPINFO* bmi = (BITMAPINFO*)tmp;
	HBITMAP     hBmp;
	int i;
	int w = image->cols, h = image->rows;
	int bpp = image->channels() * 8;

	memset(bmi, 0, sizeof(BITMAPINFO));
	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi->bmiHeader.biWidth = w;
	bmi->bmiHeader.biHeight = h;
	bmi->bmiHeader.biPlanes = 1;
	bmi->bmiHeader.biBitCount = bpp;
	bmi->bmiHeader.biCompression = BI_RGB;
	bmi->bmiHeader.biSizeImage = w * h * 1;
	bmi->bmiHeader.biClrImportant = 0;

	switch (bpp)
	{
	case 8:
		for (i = 0; i < 256; i++)
		{
			bmi->bmiColors[i].rgbBlue = i;
			bmi->bmiColors[i].rgbGreen = i;
			bmi->bmiColors[i].rgbRed = i;
		}
		break;
	case 32:
	case 24:
		((DWORD*)bmi->bmiColors)[0] = 0x00FF0000; /* red mask  */
		((DWORD*)bmi->bmiColors)[1] = 0x0000FF00; /* green mask */
		((DWORD*)bmi->bmiColors)[2] = 0x000000FF; /* blue mask  */
		break;
	}

	hBmp = ::CreateDIBSection(hDC, bmi, DIB_RGB_COLORS, NULL, 0, 0);
	::SetBitmapBits(hBmp, image->total() * image->channels(), image->data);
	::DeleteDC(hDC);

	return hBmp;
}