﻿// ChildView.cpp: CChildView 클래스의 구현
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

using namespace cv;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//extern CFourthlohgicProjectApp theApp;
// CChildView

//Find the position "factor"

CChildView::CChildView()
{
	//static TCHAR BASED_CODE szFilter[] = _T("이미지 파일(*.BMP, *.GIF, *.JPG, *PNG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.png;*.gif; |모든파일(*.*)|*.*||");

	////CFileDialog dlg(TRUE, _T("*.jpg"), _T("image"), OFN_HIDEREADONLY, szFilter);
	//CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);

	//if (IDOK != dlg.DoModal()) return;
	//sFilename = dlg.GetPathName();
	//sPathName = CT2CA(sFilename);
	//if (!image.IsNull())
	//	image.Destroy();

	char szFilter[] = "이미지 파일(*.BMP, *.GIF, *.JPG, *PNG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.gif;*.png; |모든파일(*.*)|*.*||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CA2CT(szFilter), AfxGetMainWnd());
	if (dlg.DoModal() == IDOK)
	{
		CString cstrImgPath = dlg.GetPathName();
		CT2CA pszConvertedAnsiString(cstrImgPath);
		std::string strStd(pszConvertedAnsiString);
		srcImage = cv::imread(strStd, IMREAD_COLOR);
		//저쪽 알고리즘 호출
		//Mat -> Bitmap 변환 호출

		//HWND hWnd = ::GetActiveWindow();
		//HDC hdc = ::GetDC(hWnd);
		//DisplayImage(srcImage, hdc);

		//image.Load(cstrImgPath);
	}

	//char szFilter[] = "Image(*.BMP, *.GOF, *.JPG, *.PNG)|*.BMP; *.GIF; *.JPG; *.bmp; *.gif; *.jpg; *.png | All Files(*.*) | *.* ||";
	//CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CA2CT(szFilter), AfxGetMainWnd());
	//if (dlg.DoModal() == IDOK)
	//{
	//	CString cstrImgPath = dlg.GetPathName();
	//	CT2CA pszConvertedAnsiString(cstrImgPath);
	//	std::string strStd(pszConvertedAnsiString);
	//	src = imread(strStd);
	//	HWND hWnd = ::GetActiveWindow();
	//	HDC hdc = ::GetDC(hWnd);
	//	DisplayImage(src, hdc);
	//}


	m_ePt = CPoint(0, 0);
	zoomWidth = srcImage.rows;
	zoomHeight = srcImage.cols;

	

	//m_background.Attach(image.Detach());
	//m_background.GetObject(sizeof(BITMAP), (LPVOID)&m_Bitmap);

	//m_bkgBrush.CreateSolidBrush(0x00000000);

	m_Zoom = 1.0f;
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
	HDC hdc = ::GetDC(this->m_hWnd);
	
	//OnEraseBkgnd();
	DisplayImage(srcImage, hdc);
	//HDC hDC = CreateCompatibleDC(dc.GetSafeHdc());

   // 이미 배경은 OnInitDialog() 혹은 OnInitialUpdate()에서 로드되어 있으므로 다시 할 필요는 없다.
//	memDC.CreateCompatibleDC(pDC);
//	mdcOffScreen.CreateCompatibleDC(pDC);

	// 화면 크기로 빈공간의 버퍼를 생성 한다.

//	bmpOffScreen.CreateCompatibleBitmap(pDC, m_Bitmap.bmWidth, m_Bitmap.bmHeight);
	// 아직 dmemDC의 메모리에는 아무런 그림이 없다.
	// 만약 어떤 색깔로 채우고자 한다면 FillRect() 함수등으로 특정색으로 칠할 수 있다.
	// 그러나 다음에 배경 그림을 로드하므로 필요없는 일이다.

	//oldbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);
	// 이제 메모리 준비는 끝났다. 지금 부터는 그림을 그린다.
	//우선 배경 그림이 맨 밑이므로 배경을 메모리에 복사 한다.

	//memDC.SelectObject(&m_background);   // 배경 그림을 선택하고

	//

	//mdcOffScreen.SetStretchBltMode(COLORONCOLOR);	

	//mdcOffScreen.StretchBlt(0, 0, m_bgRect.right, m_bgRect.bottom,
	//	&memDC, m_ePt.x, m_ePt.y, round(zoomWidth), round(zoomHeight), SRCCOPY);

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

//	pDC->BitBlt(0, 0, m_Bitmap.bmWidth , m_Bitmap.bmHeight, &mdcOffScreen, 0, 0, SRCCOPY);

	// 이때서야 화면에 그림이 나타난다.
//	memDC.DeleteDC();
//	mdcOffScreen.SelectObject(oldbitmap);
//	mdcOffScreen.DeleteDC();
//	bmpOffScreen.DeleteObject();
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
			m_ePt.x -= point.x  - m_sPt.x;
			if (m_ePt.x < 0)
				m_ePt.x = 0;
		}
		else // 왼쪽으로 끌었을때
		{
			m_ePt.x += m_sPt.x - point.x;
			if (m_Bitmap.bmWidth > m_bgRect.right)
			{
				if (m_ePt.x > m_bgRect.right - zoomWidth)
					m_ePt.x = m_bgRect.right - zoomWidth;
			}
		}

		// B영역의 사각형 그리기
		if (m_sPt.y < point.y) // 위로 올렸을때
		{
			m_ePt.y += point.y - m_sPt.y;
			if (m_ePt.y < 0)
				m_ePt.y = 0;
		}
		else // 아래로 내렸을때
		{
			m_ePt.y -= m_sPt.y - point.y;
			if (m_Bitmap.bmHeight > m_bgRect.bottom)
			{
				if (m_ePt.y > m_bgRect.bottom - zoomHeight)
					m_ePt.y = m_bgRect.bottom - zoomHeight;
			}
		}

		m_sPt = point;
		Invalidate();
		//::GetCursorPos(&point);
	}
}


void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnLButtonDown(nFlags, point);
	m_sPt = point;

	//좌표의 픽셀값
	CDC* dc = GetDC();
	
	COLORREF rgb;
	rgb = dc->GetPixel(point.x, point.y);

	int R = 0, G = 0, B = 0;

	R = GetRValue(rgb);
	G = GetGValue(rgb);
	B = GetBValue(rgb);

	cout << R << " " << G << " " << B << endl << endl;
	double pixelvale = (R + G + B) / 3.0;
	
	cout << pixelvale << endl;;

	
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

	zoomWidth = cx;
	zoomHeight = cy;
}


BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//휠 다운
	if (zDelta <= 0)
	{
		m_Zoom = 1.25f;
	}
	//휠 업
	else
	{
		m_Zoom = 0.75f;
	}

	//시작 좌표
	m_ePt.x += round((((float)m_pos.x / (float)m_bgRect.right) * zoomWidth)
		- (((float)m_pos.x / (float)m_bgRect.right) * (zoomWidth * m_Zoom)));
	m_ePt.y += round((((float)m_pos.y / (float)m_bgRect.bottom) * zoomHeight)
		- (((float)m_pos.y / (float)m_bgRect.bottom) * (zoomHeight * m_Zoom)));

	//사각형 넓이
	zoomWidth *= m_Zoom;
	zoomHeight *= m_Zoom;


	Invalidate();
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


void CChildView::DisplayImage(Mat src, HDC dc)
{
	Mat mat_temp;
	//HDC mPicDC = dc;
	CDC* pDC = GetDC();


	//화면에 보여주기 위한 처리
	//CString tmp;
	//tmp.Format(_T("%d"), 8 * src.elemSize());
	//MessageBox(tmp);

	int bpp = 8 * src.elemSize();	//컬러3, 그레이1 * 8 하면 비트수
	assert((bpp == 8 || bpp == 24 || bpp == 32));

	int padding = 0;
	//32 bit image is always DWORD aligned because each pixel requires 4 bytes
	if (bpp < 32)
		padding = 4 - (src.cols % 4);

	if (padding == 4)
		padding = 0;

	int border = 0;
	//32 bit image is always DWORD aligned because each pixel requires 4 bytes
	if (bpp < 32)
	{
		border = 4 - (src.cols % 4);
	}

	//isContinuous() : 행렬 요소가 각 행의 끝에 공백없이 연속적으로 저장된 경우 이 메소드는 true를 리턴합니다. 그렇지 않으면 false를 반환합니다
	if (border > 0 || src.isContinuous() == false)
	{
		//Adding needed columns on the right (max 3 px)
		cv::copyMakeBorder(src, mat_temp, 0, 0, 0, border, cv::BORDER_CONSTANT, 0);
	}
	else
	{
		mat_temp = src;
	}

	cv::Size winSize(m_bgRect.right, m_bgRect.bottom);
	BITMAPINFO* pBitmapInfo;
	if (bpp == 8)	//그레이스케일 인경우 팔레트가 필요
	{
		pBitmapInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 4 * 256);
	}
	else
	{
		pBitmapInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO));
	}

	pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;	//	그림의 세로 해상도(미터당 픽셀)
	pBitmapInfo->bmiHeader.biBitCount = bpp;	//	픽셀 하나를 표현하는 비트 수
	pBitmapInfo->bmiHeader.biWidth = mat_temp.cols;	//	비트맵 이미지의 가로 크기(픽셀)
	pBitmapInfo->bmiHeader.biHeight = -mat_temp.rows;	//	비트맵 이미지의 세로 크기(픽셀).
														//  양수: 이미지의 상하가 뒤집혀서 저장된 상태
														//  음수 : 이미지가 그대로 저장된 상태
														//  보통 세로 크기는 양수로 저장되어 있습니다.
	pBitmapInfo->bmiHeader.biPlanes = 1;	//사용하는 색상판의 수. 항상 1입니다.
	pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);	//현재 비트맵 정보 헤더(BITMAPINFOHEADER)의 크기
	pBitmapInfo->bmiHeader.biCompression = BI_RGB;	//압축 방식. 보통 비트맵은 압축을 하지 않으므로 0입니다.
	pBitmapInfo->bmiHeader.biClrImportant = 0;	//비트맵을 표현하기 위해 필요한 색상 인덱스 수
	if (bpp == 8) {	//그레이스케일 인경우 팔레트가 필요
		pBitmapInfo->bmiHeader.biClrUsed = 4 * 256;	//색상 테이블에서 실제 사용되는 색상 수
		for (int i = 0; i < 256; i++)
		{
			pBitmapInfo->bmiColors[i].rgbBlue = (BYTE)i;
			pBitmapInfo->bmiColors[i].rgbGreen = (BYTE)i;
			pBitmapInfo->bmiColors[i].rgbRed = (BYTE)i;
			pBitmapInfo->bmiColors[i].rgbReserved = (BYTE)0;
		}
	}
	else {
		pBitmapInfo->bmiHeader.biClrUsed = 0;
	}
	pBitmapInfo->bmiHeader.biSizeImage = 0;	//비트맵 이미지의 픽셀 데이터 크기(압축 되지 않은 크기)
	pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;	//그림의 가로 해상도(미터당 픽셀)

	//Image is bigger or smaller than into destination rectangle
	//we use stretch in full rect
	//CDC* mDC, *mos;
	//mDC = CDC::FromHandle(CreateCompatibleDC(dc));
	//mos = CDC::FromHandle(CreateCompatibleDC(dc));

	//Mat matOffScreen(srcImage.rows, srcImage.cols, 0);
	//mos->SelectObject(&matOffScreen);
	//mDC->SelectObject(&srcImage);

	////memDC.SelectObject(src);

	//mos->SetStretchBltMode(COLORONCOLOR);

	SetStretchBltMode(dc, COLORONCOLOR);

	StretchDIBits(dc,
		0, 0, winSize.width, winSize.height,
		m_ePt.x, m_ePt.y, zoomWidth, zoomHeight,
		mat_temp.data, pBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

	//mDC->DeleteDC();
	//mos->SelectObject(oldmat);
	//mos->DeleteDC();
	free(pBitmapInfo);

	//if (mat_temp.cols == winSize.width && mat_temp.rows == winSize.height)
	//{
	//	//source and destination have same size
	//	//transfer memory block
	//	//NOTE: the padding border will be shown here. Anyway it will be max 3px width

	//	SetDIBitsToDevice(dc,
	//		//destination rectangle
	//		0, 0, winSize.width, winSize.height,
	//		0, 0, 0, mat_temp.rows,
	//		mat_temp.data, pBitmapInfo, DIB_RGB_COLORS);
	//}
	//else
	//{
	//	//destination rectangle
	//	int destx = 0, desty = 0;

	//	double ratioX = (double)winSize.width / mat_temp.cols;
	//	double ratioY = (double)winSize.height / mat_temp.rows;
	//	double ratio;
	//	if (ratioX < ratioY)
	//		ratio = ratioX;
	//	else
	//		ratio = ratioY;

	//	int destw = cvRound(mat_temp.cols * ratio);
	//	int desth = cvRound(mat_temp.rows * ratio);

	//	//rectangle defined on source bitmap
	//	//using imgWidth instead of mat_temp.cols will ignore the padding border
	//	int imgx = 0, imgy = 0;
	//	int imgWidth = mat_temp.cols - border;
	//	int imgHeight = mat_temp.rows;

	//	StretchDIBits(dc,
	//		m_ePt.x, m_ePt.y, zoomWidth, zoomHeight,
	//		0, 0, mat_temp.rows, mat_temp.cols,
	//		mat_temp.data, pBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
	//}
	//DeleteDC(dc);

}

void CChildView::OnEraseBkgnd()
{
	CRect rt;
	//CStatic* pPic = (CStatic*)GetDlgItem(nID);
	this->GetClientRect(&rt);
	this->GetDC()->FillSolidRect(&rt, RGB(0, 0, 0));
}