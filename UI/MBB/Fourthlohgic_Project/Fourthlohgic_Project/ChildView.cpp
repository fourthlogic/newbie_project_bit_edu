
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

CChildView::CChildView()
{
	a = 0;

	if (!SrcImage.IsNull())
		SrcImage.Destroy();
	SrcImage.Load(theApp.sFilename);

	/*static TCHAR BASED_CODE szFilter[] = _T("이미지 파일(*.BMP, *.GIF, *.JPG, *PNG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.png;*.gif; |모든파일(*.*)|*.*||");

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (IDOK != dlg.DoModal()) return;

	pathname = dlg.GetPathName();
	sFilename = CT2CA(pathname);

	srcImage = imread(sFilename, IMREAD_COLOR);

	height = srcImage.rows;
	width = srcImage.cols;*/



	m_nZoomRate = 0.f;	

	
	CDC* pSourceDC = CDC::FromHandle(SrcImage.GetDC());

	m_ImageWidth = SrcImage.GetWidth();
	m_ImageHeight = SrcImage.GetHeight();


	int BitPerPixel = pSourceDC->GetDeviceCaps(BITSPIXEL);
	DstImage.Create(m_ImageWidth, m_ImageHeight, BitPerPixel);
	CDC* pDestDC = CDC::FromHandle(DstImage.GetDC());

	if (!pSourceDC || !pDestDC)
	{
		return;
	}

	pDestDC->BitBlt(0, 0, m_ImageWidth, m_ImageHeight,
		pSourceDC, 0, 0, SRCCOPY);
	SrcImage.ReleaseDC();

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

void SetPointColor(Mat* image, int x, int y, COLORREF c)
{
	//m_image.SetPixel() call::SetPixel() which is too slow
	//since it has to work with all DCs.

	//BYTE* p = (BYTE*)image->GetPixelAddress(x, y);
	auto* p = &image->at<uchar>(x, y);
	//if (m_nBitdepth == 16) {
	//	*(WORD*)p = (WORD)(((c & 0xf80000) >> 19) | ((c & 0xf800) >> 6) | ((c & 0xf8) << 7));
	//}
	//else {
	*p++ = GetBValue(c);
	*p++ = GetGValue(c);
	*p = GetRValue(c);
	//}
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.
	HDC hdc = dc;
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	/*DstImage.AlphaBlend(dc, 0, 0, m_ImageWidth << m_nZoomRate, m_ImageHeight << m_nZoomRate,
		0, 0, m_ImageWidth, m_ImageHeight, 128);*/

	if (a == 0)
	{
		DstImage.Draw(dc, 0, 0);
		//Paint(hdc);
		a++;
	}
	else
	{
		CPoint pt;
		GetCursorPos(&pt);
		if (m_nZoomRate < 0)
		{
			m_nZoomRate = 0;
			DstImage.Draw(dc, 0, 0, m_ImageWidth << m_nZoomRate, m_ImageHeight << m_nZoomRate,
				pt.x, pt.y, m_ImageWidth, m_ImageHeight);
		}
		else if(m_nZoomRate>4)
		{
			DstImage.Draw(dc, 0, 0, m_ImageWidth << m_nZoomRate, m_ImageHeight << m_nZoomRate,
				pt.x, pt.y, m_ImageWidth*0.3, m_ImageHeight*0.3);
		}
		else
		{
			DstImage.Draw(dc, 0, 0, m_ImageWidth << m_nZoomRate, m_ImageHeight << m_nZoomRate,
				pt.x, pt.y, m_ImageWidth, m_ImageHeight);
		}
	}

	

	//DstImage.ReleaseDC();

	/*COLORREF abb = SrcImage.GetPixel(800, 500);
	CString strTmp;

	strTmp.Format(_T("RGB 출력: (%u, %u, %u) "), GetRValue(abb), GetGValue(abb), GetBValue(abb));
	MessageBox(strTmp);*/

	// 그리기 메시지에 대해서는 CWnd::OnPaint()를 호출하지 마십시오.
}


BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (zDelta <= 0)	//마우스 휠 다운
	{
		m_nZoomRate += 1;
		/*if (m_nZoomRate > 400)
			m_nZoomRate = 400;*/
		//CString strTmp;

		//strTmp.Format(_T("좌표: (%d, %d) "), pt.x, pt.y);

		//MessageBox(strTmp);
	}
	else	//마우스 휠 업
	{
		m_nZoomRate -= 1;
		/*if (m_nZoomRate < -1000000)
			m_nZoomRate = -1000000;*/
		//MessageBox(_T("마우스 업"));
	}
	Invalidate();
	//RedrawWindow();
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


//void CChildView::Paint(HDC dc) {
//
//	Mat dstImage1(height, width, CV_8UC1);
//
//	for (int y = 0; y < height; y++)
//	{
//		uchar* pointer_input = srcImage.ptr<uchar>(y);
//		uchar* pointer_output = dstImage1.ptr<uchar>(y);
//		for (int x = 0; x < width; x++)
//		{
//			uchar b = pointer_input[x * 3 + 0];
//			uchar g = pointer_input[x * 3 + 1];
//			uchar r = pointer_input[x * 3 + 2];
//
//			pointer_output[x] = (r + g + b) / 3.0;
//			//COLORREF PixColor = RGB(r, g, b);
//			//SetPointColor(&dstImage1, x, y, RGB(r, g, b));
//			SetPixel(dc, x, y, RGB(r, g, b));
//
//			//SetBitmapBits(bm, pointer_output[x]);
//			//SetPixelV(dc, x, y, PixColor);
//			/*temp.Format(_T("%u"), *(char*)pointer_output);
//			TextOut(dc, x, y, temp, 0);*/
//			//SetPixelV(dc, x, y, PixColor);
//		}
//	}
//}


//GetPixel과 SetPixel은 cpu 혹사 
//SetPixel 대용
//void SetPointColor(CImage* image, int x, int y, COLORREF c)
//{
//	//m_image.SetPixel() call::SetPixel() which is too slow
//	//since it has to work with all DCs.
//
//	BYTE* p = (BYTE*)image->GetPixelAddress(x, y);
//	if (m_nBitdepth == 16) {
//		*(WORD*)p = (WORD)(((c & 0xf80000) >> 19) | ((c & 0xf800) >> 6) | ((c & 0xf8) << 7));
//	}
//	else {
//		*p++ = GetBValue(c);
//		*p++ = GetGValue(c);
//		*p = GetRValue(c);
//	}
//}

//GetPixel 대용
//COLORREF GetPointColor(CImage* image, int x, int y)
//{
//	COLORREF result = *((COLORREF*)image->GetPixelAddress(x, y));
//
//	//메모리에서 가져올때, blue와 red위치가 바뀌어서 가져와진다.
//	BYTE r = GetBValue(result);
//	BYTE g = GetGValue(result);
//	BYTE b = GetRValue(result);
//
//	return RGB(r, g, b);
//}