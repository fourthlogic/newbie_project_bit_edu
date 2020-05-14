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
	char szFilter[] = "Image(*.BMP, *.GOF, *.JPG, *.PNG)|*.BMP; *.GIF; *.JPG; *.bmp; *.gif; *.jpg; *.png; | All Files(*.*) | *.* ||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CA2CT(szFilter), AfxGetMainWnd());
	if (dlg.DoModal() == IDOK)
	{
		CString cstrImgPath = dlg.GetPathName();
		CT2CA pszConvertedAnsiString(cstrImgPath);
		std::string strStd(pszConvertedAnsiString);
		src = imread(strStd);
		//
		
	}
	
	/*if (!image.IsNull())
		image.Destroy();
	image.Load(theApp.sFilename);*/

	/*m_background.Attach(image.Detach());
	m_background.GetObject(sizeof(BITMAP), (LPVOID)&m_Bitmap);*/

	m_ePt.x = 0;
	m_ePt.y = 0;
	ViewScale = 1;
	ViewValue = 1;

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

	HDC hdc = ::GetDC(this->m_hWnd);
	//OnEraseBkgnd(hdc);
	DisplayImage(src, hdc);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//---------------------------------------------------
	//CDC* pDC = GetDC();
	//CString str;
	//str.Format(_T("X = %d, Y = %d"), m_pos.x, m_pos.y);
	//dc.TextOut(m_pos.x, m_pos.y, str);

 //  // 이미 배경은 OnInitDialog() 혹은 OnInitialUpdate()에서 로드되어 있으므로 다시 할 필요는 없다.
	//memDC.CreateCompatibleDC(pDC);
	//mdcOffScreen.CreateCompatibleDC(pDC);

	//// 화면 크기로 빈공간의 버퍼를 생성 한다.
	//bmpOffScreen.CreateCompatibleBitmap(pDC, m_Bitmap.bmWidth, m_Bitmap.bmHeight);
	//// 아직 dmemDC의 메모리에는 아무런 그림이 없다.
	//// 만약 어떤 색깔로 채우고자 한다면 FillRect() 함수등으로 특정색으로 칠할 수 있다.
	//// 그러나 다음에 배경 그림을 로드하므로 필요없는 일이다.

	//oldbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);
	//// 이제 메모리 준비는 끝났다. 지금 부터는 그림을 그린다.
	////우선 배경 그림이 맨 밑이므로 배경을 메모리에 복사 한다.

	//memDC.SelectObject(&m_background);   // 배경 그림을 선택하고
	//mdcOffScreen.SetStretchBltMode(COLORONCOLOR);
	////mdcOffScreen.BitBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &memDC, 0, 0, SRCCOPY);
	////mdcOffScreen.StretchBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &memDC, m_ePt.x, m_ePt.y, newWidth, newHeight, SRCCOPY);
	//mdcOffScreen.StretchBlt(0, 0, m_bgRect.right, m_bgRect.bottom, &memDC, m_ePt.x, m_ePt.y, rectWidth, rectHeight, SRCCOPY);
	//// ==> 배경을 메모리버퍼에 복사 한다. 아직 화면에는 나타나지 않는다.
	////따라서 그림은 화면에 나타나지 않고, 디버깅이 힘들다.
	////디버깅을 싶게 한다면
	////pDC->BitBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &memDC, 0, 0, SRCCOPY); 
	////    한줄 더 넣어 화면을 확인하고 디버깅이 끝나면 삭제 한다.
 //  // 이번에는 BitBlt 말고 다른 윈도우 함수를 역시 메모리에 복사 한다.
	////----------
	////mdcOffScreen.SetTextColor((COLORREF)0x00FFFFFF);
	////==> 이것은 pDC->SetTextColor( (COLORREF) 0x00FFFFFF );와는 별개로 상관없음
	////mdcOffScreen.SetBkMode(TRANSPARENT);   // 글자의 배경색을 없앤다.
	////-----------
	//// 여기까지 모든 그림이 완성되어 지만, 아직 표시 버퍼에 출력된 상태가 아니다. 디버깅을 해보면 아직 그림이 표시되지 않는다.
	//// 최종적으로 표시 화면 메모리에 복사 한다.
	////pDC->StretchBlt(0, 0, tmpRect.right, tmpRect.bottom, &mdcOffScreen, tmpRect.left, tmpRect.top, tmpRect.right, tmpRect.bottom, SRCCOPY);
	//
	//pDC->BitBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &mdcOffScreen, 0, 0, SRCCOPY);
	////pDC->StretchBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &mdcOffScreen, m_ePt.x, m_ePt.y, m_Bitmap.bmWidth, m_Bitmap.bmHeight, SRCCOPY);

	//// 이때서야 화면에 그림이 나타난다.
	//memDC.DeleteDC();
	//mdcOffScreen.SelectObject(oldbitmap);
	//mdcOffScreen.DeleteDC();
	//bmpOffScreen.DeleteObject();
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// 클라 크기가 이미지보다 클때 검은화면으로 채워주기
	//if (m_Bitmap.bmWidth < m_bgRect.right) { //오른쪽
	//	RECT rect2;
	//	rect2.left = m_Bitmap.bmWidth;
	//	rect2.right = m_bgRect.right;
	//	rect2.top = 0;
	//	rect2.bottom = m_Bitmap.bmHeight;
	//	//m_ePt.x = 0;
	//	pDC->FillRect(&rect2, &m_bkgBrush);  // B영역 칠하기
	//}
	//if (m_Bitmap.bmHeight < m_bgRect.bottom) { //아래
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
				if (m_ePt.x > m_Bitmap.bmWidth - rectWidth)
					m_ePt.x = m_Bitmap.bmWidth - rectWidth;
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
				if (m_ePt.y > m_Bitmap.bmHeight - rectHeight)
					m_ePt.y = m_Bitmap.bmHeight - rectHeight;
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
}


void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_bgRect.left = 0;
	m_bgRect.top = 0;
	m_bgRect.right = cx;
	m_bgRect.bottom = cy;

	

	rectWidth = cx;
	rectHeight = cy;

	Invalidate();
}


BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (zDelta <= 0)// 휠 내릴때
	{
		ViewScale = 1.2f;
		ViewValue *= 1.2f;
	}
	else// 휠 올릴때
	{
		ViewScale = 0.75f;
		ViewValue *= 0.75f;
	}
	
	//왼쪽위 꼭짓점

	m_ePt.x += round((((float)m_pos.x / (float)m_bgRect.right) * ((float)rectWidth)) - (((float)m_pos.x / (float)m_bgRect.right) * ((float)rectWidth * ViewScale)));
	m_ePt.y += round((((float)m_pos.y/ (float)m_bgRect.bottom)* (float)rectHeight) - (((float)m_pos.y / (float)m_bgRect.bottom) * ((float)rectHeight * ViewScale)));

	// 사각형 크기
	rectWidth *= ViewScale;
	rectHeight *= ViewScale;


	Invalidate();
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CChildView::DisplayImage(Mat& src, HDC hdc)
{
	Mat mat_temp;
	//CStatic* pPic = img;
	HDC mPicDC = hdc;
	//CPaintDC dc(this);
	

	//화면에 보여주기 위한 처리
	//CString tmp;
	//tmp.Format(_T("%d"), src.elemSize());
	//MessageBox(tmp);
	int bpp = 8 * src.elemSize(); // elemSize() 는 한화소에 데이터 개수 Color = 3개 Gray = 1개, 한 화소에 비트수 구하는거
	assert((bpp == 8 || bpp == 24 || bpp == 32)); // 이 3개가 아니면 종료

	int padding = 0;
	//32 bit image is always DWORD aligned because each pixel requires 4 bytes
	if (bpp < 32)
		padding = 4 - (src.cols % 4);

	if (padding == 4)
		padding = 0;

	int border = 0;
	// 32 bit image is always DWORD aligned because each pixel requires 4 bytes
	if (bpp < 32)
	{
		border = 4 - (src.cols % 4);
	}

	if (border > 0 || src.isContinuous() == false)
	{
		// Adding needed columns on the right(max 3 px)
		cv::copyMakeBorder(src, mat_temp, 0, 0, 0, border, cv::BORDER_CONSTANT, 0);
	}
	else
	{
		mat_temp = src;
	}

	//Rect rect;
	//this->GetClientRect((LPRECT)&rect);

	cv::Size winSize(m_bgRect.right, m_bgRect.bottom);
	BITMAPINFO* pBitmapInfo;
	if (bpp == 8) //그레이스케일인경우 팔레트가 필요
	{
		pBitmapInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 4 * 256);
	}
	else
	{
		pBitmapInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO));
	}

	pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	pBitmapInfo->bmiHeader.biBitCount = bpp;
	pBitmapInfo->bmiHeader.biWidth = mat_temp.cols;
	pBitmapInfo->bmiHeader.biHeight = -mat_temp.rows;
	pBitmapInfo->bmiHeader.biPlanes = 1;
	pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	pBitmapInfo->bmiHeader.biClrImportant = 0;
	if (bpp == 8) { //그레이스케일인경우 팔레트가 필요
		pBitmapInfo->bmiHeader.biClrUsed = 4 * 256;
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
	pBitmapInfo->bmiHeader.biSizeImage = 0;
	pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;

	//Image is bigger or smaller than into desination rectangle
	// we use stretch in full rect

	

	CDC* pDC = GetDC();

	memDC.CreateCompatibleDC(pDC);
	mdcOffScreen.CreateCompatibleDC(pDC);
	oldbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);
	bmpOffScreen.CreateCompatibleBitmap(pDC, src.rows, src.cols);
	oldbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);

	memDC.SelectObject(&src);

	SetStretchBltMode(mPicDC, COLORONCOLOR);

	StretchDIBits(mPicDC,
		//destination rectangle
		0, 0, m_bgRect.right, m_bgRect.bottom,
		m_ePt.x, m_ePt.y, rectWidth, rectHeight,
		mat_temp.data, pBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

	//SetDIBitsToDevice(mPicDC,
	//	//destination rectangle
	//	0, 0, mat_temp.rows, mat_temp.cols,
	//	0, 0, mat_temp.rows, mat_temp.cols,
	//	mat_temp.data, pBitmapInfo, DIB_RGB_COLORS);

	memDC.DeleteDC();
	mdcOffScreen.SelectObject(oldbitmap);
	mdcOffScreen.DeleteDC();
	bmpOffScreen.DeleteObject();

	//if (mat_temp.cols == winSize.width && mat_temp.rows == winSize.height)
	//{
	//	//source and destination have same size
	//	//transfer memory block
	//	//NOTE : the padding border will be shown here. Anyway it will be max 3px width

	//	SetDIBitsToDevice(mPicDC,
	//		//destination rectangle
	//		0, 0, winSize.width, winSize.height,
	//		0, 0, 0, mat_temp.rows,
	//		mat_temp.data, pBitmapInfo, DIB_RGB_COLORS);
	//}
	//else
	//{
	//	//destination rectangle

	//	double ratioX = (double)winSize.width / mat_temp.cols;
	//	double ratioY = (double)winSize.height / mat_temp.rows;
	//	double ratio;

	//	if (ratioX < ratioY)
	//		ratio = ratioX;
	//	else
	//		ratio = ratioY;

	//	int destx = (winSize.width / 2 - cvRound(mat_temp.cols * ratio) / 2);
	//	int	desty = (winSize.height / 2 - cvRound(mat_temp.rows * ratio) / 2);
	//	int destw = cvRound(mat_temp.cols * ratio);
	//	int desth = cvRound(mat_temp.rows * ratio);

	//	// rectangle defined on source bitmap
	//	// using imgWidth instead of mat_temp.cols will ignore the padding border
	//	int imgx = 0, imgy = 0;
	//	int imgWidth = mat_temp.cols - border;
	//	int imgHeight = mat_temp.rows;

	//	StretchDIBits(mPicDC,
	//		//destination rectangle
	//		destx, desty, destw, desth,
	//		imgx, imgy, imgWidth, imgHeight,
	//		mat_temp.data, pBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
	//}

	::ReleaseDC(this->m_hWnd, mPicDC);
	free(pBitmapInfo);
}

void CChildView::OnEraseBkgnd(HDC hdc)
{
	CRect rt;
	//CStatic* pPic = img;
	this->GetClientRect(&rt);
	this->GetDC()->FillSolidRect(&rt, RGB(240, 240, 240));
	//GetDlgItem(IDC_SLIDER_EDGE)->ShowWindow(SW_HIDE);
}