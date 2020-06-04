// ChildView.cpp: CChildView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "Fourthlohgic_Project.h"
#include "ChildView.h"
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console");


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CFourthlohgicProjectApp theApp;
// CChildView

CChildView::CChildView()
{

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
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_COMMAND(DrawLine, &CChildView::OnDrawline)
	ON_COMMAND(DrawRect, &CChildView::OnDrawrect)
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
	bmpOffScreen.CreateCompatibleBitmap(pDC, viewWidth * (int)(rectWidth + 2), viewHeight * (int)(rectHeight + 2));
	// 아직 dmemDC의 메모리에는 아무런 그림이 없다.
	// 만약 어떤 색깔로 채우고자 한다면 FillRect() 함수등으로 특정색으로 칠할 수 있다.
	// 그러나 다음에 배경 그림을 로드하므로 필요없는 일이다.
	oldbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);
	// 이제 메모리 준비는 끝났다. 지금 부터는 그림을 그린다.
	//우선 배경 그림이 맨 밑이므로 배경을 메모리에 복사 한다.

	m_background.Detach();
	m_background.Attach(::CopyImage(pFrame->imageList[idx].getImage(),IMAGE_BITMAP, 0, 0, 0));

	memDC.SelectObject(&m_background);   // 배경 그림을 선택하고

	for (int i = 0; i < data.GetSize(); i++)
	{
		drawShape(data[i].shapeType, data[i].penWidth,
			data[i].rect.left, data[i].rect.top,
			data[i].rect.right, data[i].rect.bottom);
	}

	mdcOffScreen.SetStretchBltMode(HALFTONE);
	//mdcOffScreen.BitBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &memDC, 0, 0, SRCCOPY);
	//mdcOffScreen.StretchBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &memDC, m_ePt.x, m_ePt.y, newWidth, newHeight, SRCCOPY);
	mdcOffScreen.StretchBlt(startX, startY, viewWidth * (int)(rectWidth + 2), viewHeight * (int)(rectHeight +2), &memDC, z_pos.x, z_pos.y, rectWidth + 2, rectHeight + 2, SRCCOPY);
	//if(viewWidth * (m_bgRect.right + 2) >= m_)
	//mdcOffScreen.StretchBlt(0, 0, viewWidth * (m_bgRect.right), viewHeight * (m_bgRect.bottom), &memDC, z_pos.x, z_pos.y, m_bgRect.right, m_bgRect.bottom, SRCCOPY);

	PrintText(&mdcOffScreen);

	/*printf("\n");
	printf("z_pos.x : %d ", z_pos.x);
	printf("z_pos.y : %d ", z_pos.y);
	printf("rectWidth : %f ", rectWidth);
	printf("viewWidth * (int)(rectWidth + 2) : %f", viewWidth * (int)(rectWidth + 2));
	printf("\n");
	printf("m_ePt.x : %d ", m_ePt.x);
	printf("m_ePt.y : %d ", m_ePt.y);
	printf("viewWidth : %f ", viewWidth);
	printf("viewHeight : %f ", viewHeight);
	printf("m_bgRect.right : %d ", m_bgRect.right); 
	printf("\n");
	printf("startX : %f ", startX);
	printf("startY : %f ", startY);*/

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
	pDC->SetStretchBltMode(HALFTONE);
	pDC->StretchBlt(0, 0, m_bgRect.right, m_bgRect.bottom, &mdcOffScreen, m_ePt.x, m_ePt.y, m_bgRect.right, m_bgRect.bottom, SRCCOPY);
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
	return false;
}

// Zoom + Panning ------------------------------------------------------------------------------
void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnMouseMove(nFlags, point);
	m_pos = point;
	int i = 0;

	if (nFlags & MK_MBUTTON)
	{
		//int width = m_bgRect.right - m_bgRect.left;
		//int height = m_bgRect.bottom - m_bgRect.top;

		// A영역의 사각형 그리기 //m_sPt는 기존 포인트, point는 이동
		if (startX == 0)
		{
			if (m_sPt.x < point.x) // 오른쪽으로 끌었을때
			{
				m_ePt.x -= point.x - m_sPt.x;

				if (m_ePt.x <= 0)
				{
					if (z_pos.x > 0)
					{
						i = 1 + ((0 - m_ePt.x) / viewWidth);
						z_pos.x -= i;
						m_ePt.x += i * viewWidth;
					}
				}
			}
			else // 왼쪽으로 끌었을때
			{
				m_ePt.x += m_sPt.x - point.x;

				if (m_ePt.x + m_bgRect.right - (viewWidth * (int)(rectWidth + 2)) >= 0)
				{
						i = 1 + (m_ePt.x + m_bgRect.right - (viewWidth * (int)(rectWidth + 2))) / viewWidth;
						z_pos.x += i;
						m_ePt.x -= i * viewWidth;
					
				}
			}
		}


		if (startY == 0)
		{
			if (m_sPt.y > point.y) // 위로 올렸을때
			{
				m_ePt.y += m_sPt.y - point.y;

				if (m_ePt.y + m_bgRect.bottom - (viewHeight * (int)(rectHeight + 2)) >= 0)
				{
						i = 1 + (m_ePt.y + m_bgRect.bottom - (viewHeight * (int)(rectHeight + 2))) / viewHeight;
						z_pos.y += i;
						m_ePt.y -= i * viewHeight;
				}

			}
			else // 아래로 내렸을때
			{
				m_ePt.y -= point.y - m_sPt.y;
				if (m_ePt.y < 0)
				{
					if (z_pos.y > 0)
					{
						i = 1 + ((0 - m_ePt.y) / viewHeight);
						z_pos.y -= i;
						m_ePt.y += i * viewHeight;
					}
				}
			}
		}
		// B영역의 사각형 그리기
		

		//---------------------------
		if (m_ePt.x + m_bgRect.right > viewWidth * (int)(rectWidth + 2))
		{
			z_pos.x += 1;
			m_ePt.x -= viewWidth;
		}

		if (m_ePt.y + m_bgRect.bottom > viewHeight * (int)(rectHeight + 2))
		{
			z_pos.y += 1;
			m_ePt.y -= viewHeight;
		}
		//----------------------------

		if (m_ePt.x < 0)
			m_ePt.x = 0;
		if (m_ePt.y < 0)
			m_ePt.y = 0;

		m_sPt = point;
		Invalidate();
		UpdateWindow();
	}

	if (MK_LBUTTON && drawID == TRUE) // 마우스를 클릭하여 드래그일 동안만 
	{
		draw(&memDC, point); // 뷰에서 도형 그리기(draw) 함수 호출		
		// 마우스 드래그시의 좌표 값을 도형 끝 값에 다시 저장	
		//Invalidate();

		mov_x = z_pos.x + (m_ePt.x + point.x) / viewWidth;
		mov_y = z_pos.y + (m_ePt.y + point.y) / viewHeight;

		Invalidate();
	}

	if (MK_LBUTTON && panID == TRUE) 
	{
		int o, p;
		int xx, yy;
		//CPoint TLX;
		xx = x, yy = y;
		x = z_pos.x + (m_ePt.x + point.x) / viewWidth;
		y = z_pos.y + (m_ePt.y + point.y) / viewHeight;

		o = (x - xx);
		p = (y - yy);

		data[chosenShape].rect.right += o;
		data[chosenShape].rect.left += o;
		data[chosenShape].rect.top += p;
		data[chosenShape].rect.bottom += p;

		sPoint.x = data[chosenShape].rect.left;
		sPoint.y = data[chosenShape].rect.top;
		ePoint.x = data[chosenShape].rect.right;
		ePoint.y = data[chosenShape].rect.bottom;

		rRect[0].SetRect(sPoint.x - 15, sPoint.y - 15, sPoint.x + 15, sPoint.y + 15);//1사분면
		rRect[1].SetRect(ePoint.x - 15, ePoint.y - 15, ePoint.x + 15, ePoint.y + 15);//4사분면
		rRect[2].SetRect(sPoint.x - 15, ePoint.y - 15, sPoint.x + 15, ePoint.y + 15);//3사분면
		rRect[3].SetRect(ePoint.x - 15, sPoint.y - 15, ePoint.x + 15, sPoint.y + 15);//2사분면

		Invalidate();
		UpdateWindow();
		//InvalidateRect(&r);
	}

	if (MK_LBUTTON && EdgeChosen == TRUE)
	{
		int o, p;
		int xx, yy;
		//CPoint TLX;
		xx = x, yy = y;
		x = z_pos.x + (m_ePt.x + point.x) / viewWidth;
		y = z_pos.y + (m_ePt.y + point.y) / viewHeight;

		o = (x - xx);
		p = (y - yy);

		if (chosenEdge == 0)
		{
			data[chosenShape].rect.left += o;
			data[chosenShape].rect.top += p;
		}
		else if (chosenEdge == 1)
		{
			data[chosenShape].rect.right += o;
			data[chosenShape].rect.bottom += p;
		}
		else if (chosenEdge == 2)
		{
			data[chosenShape].rect.left += o;
			data[chosenShape].rect.bottom += p;
		}
		else if (chosenEdge == 3)
		{
			data[chosenShape].rect.right += o;
			data[chosenShape].rect.top += p;
		}

	}
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_bgRect.left = 0;
	m_bgRect.top = 0;
	m_bgRect.right = cx;
	m_bgRect.bottom = cy;

	rectWidth = cx * rectScale;
	rectHeight = cy * rectScale;

	Invalidate();
}


BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (zDelta <= 0)// 휠 내릴때
	{
		zoomScale = 1.25f;
		zoomView *= 0.8f;
		rectScale *= 1.25f;
	}
	else// 휠 올릴때
	{
		zoomScale = 0.8f;
		zoomView *= 1.25f;
		rectScale *= 0.8f;
	}
	float xxx = z_pos.x + (m_ePt.x + m_pos.x) / viewWidth;
	float yyy = z_pos.y + (m_ePt.y + m_pos.y) / viewHeight;

	float xxx2 = (((m_ePt.x + m_pos.x) / viewWidth - (int)((m_ePt.x + m_pos.x) / viewWidth)));
	float yyy2 = (((m_ePt.y + m_pos.y) / viewHeight - (int)((m_ePt.y + m_pos.y) / viewHeight)));

	/*printf("변경 전 : \n");
	printf("\n");
	printf("xxx : %f ", xxx);
	printf("yyy : %f ", yyy);
	printf("xxx2 : %f ", xxx2);
	printf("yyy2 : %f ", yyy2);
	printf("\n");
	printf("m_ePt.x : %d ", m_ePt.x);
	printf("m_ePt.y : %d ", m_ePt.y);
	printf("m_pos.x : %d ", m_pos.x);
	printf("\n");
	printf("z_pos.x : %d ", z_pos.x);
	printf("viewWidth : %f ", viewWidth);
	printf("m_pos.x % viewWidth : %f ", (m_pos.x % (int)viewWidth / viewWidth));
	printf("\n\n");*/

	//왼쪽위 꼭짓점
	z_pos.x += round(((((float)m_ePt.x + (float)m_pos.x) / (viewWidth * (int)(rectWidth + 2))) * rectWidth) - ((((float)m_ePt.x + (float)m_pos.x) / (viewWidth * (int)(rectWidth + 2))) * (rectWidth * zoomScale)));
	z_pos.y += round(((((float)m_ePt.y + (float)m_pos.y) / (viewHeight * (int)(rectHeight + 2))) * rectHeight) - ((((float)m_ePt.y + (float)m_pos.y) / (viewHeight * (int)(rectHeight + 2))) * (rectHeight * zoomScale)));


	//원본에서 출력될 길이
	rectWidth = rectWidth * zoomScale;
	rectHeight = rectHeight * zoomScale;

	printWidth = rectWidth * zoomView;
	printHeight = rectHeight * zoomView;

	//픽셀 크기
	viewWidth = (printWidth ) / (rectWidth );
	viewHeight = (printHeight ) / (rectHeight);

	/*printf("변경 후 : \n");
	printf("\n");
	printf("xxx : %f ", z_pos.x + (m_ePt.x + m_pos.x) / viewWidth);
	printf("yyy : %f ", z_pos.y + (m_ePt.y + m_pos.y) / viewHeight);
	printf("xxx2 : %f ", ((m_ePt.x + m_pos.x) % (int)viewWidth / viewWidth));
	printf("yyy2 : %f ", ((m_ePt.y + m_pos.y) % (int)viewHeight / viewHeight));
	printf("\n");
	printf("m_ePt.x : %d ", m_ePt.x);
	printf("m_ePt.y : %d ", m_ePt.y);
	printf("m_pos.x : %d ", m_pos.x);
	printf("\n");
	printf("z_pos.x : %d ", z_pos.x);
	printf("viewWidth : %f ", viewWidth);
	printf("m_pos.x % viewWidth : %f ", ((m_ePt.x + m_pos.x) % (int)viewWidth / viewWidth));
	printf("\n");
	printf("m_ePt.x+? : %f ", (xxx2 * viewWidth) - ((m_ePt.x + m_pos.x) % (int)viewWidth / viewWidth) * viewWidth);
	printf("m_ePt.y+? : %f ", (yyy2 * viewHeight) - ((m_ePt.y + m_pos.y) % (int)viewHeight / viewHeight) * viewHeight);
	printf("\n");
	printf("----------------------------------");*/

	if (xxx - (int)(z_pos.x + (m_ePt.x + m_pos.x) / viewWidth) != 0)
	{
		z_pos.x += xxx - (int)(z_pos.x + (m_ePt.x + m_pos.x) / viewWidth);
		//m_ePt.x += (xxx2 * viewWidth) - ((m_ePt.x + m_pos.x) % (int)viewWidth / viewWidth) * viewWidth;
		m_ePt.x += (xxx2 * viewWidth) - (((m_ePt.x + m_pos.x) / viewWidth - (int)((m_ePt.x + m_pos.x) / viewWidth))) * viewWidth;
	}

	if (yyy - (z_pos.y + (int)(m_ePt.y + m_pos.y) / viewHeight) != 0)
	{
		z_pos.y += yyy - (int)(z_pos.y + (m_ePt.y + m_pos.y) / viewHeight);
		//m_ePt.y += (yyy2 * viewHeight) - ((m_ePt.y + m_pos.y) % (int)viewHeight / viewHeight) * viewHeight;
		m_ePt.y += (yyy2* viewHeight) - (((m_ePt.y + m_pos.y) / viewHeight - (int)((m_ePt.y + m_pos.y) / viewHeight))) * viewHeight;
	}


	//-----------------------------------------------
	if (m_Bitmap.bmWidth * zoomView <= m_bgRect.right)
	{
		startX = m_bgRect.right / 2 - (m_Bitmap.bmWidth * zoomView) / 2;
		z_pos.x = 0;
	}
	else
	{
		startX = 0;
	}

	if (m_Bitmap.bmHeight * zoomView <= m_bgRect.bottom)
	{
		startY = m_bgRect.bottom / 2 - (m_Bitmap.bmHeight * zoomView) / 2;
		z_pos.y = 0;
	}
	else
	{
		startY = 0;
	}
	//-----------------------------------
	if (m_ePt.x >= viewWidth)
	{
		z_pos.x += 1;
		m_ePt.x -= viewWidth;
	}
	if (m_ePt.y >= viewHeight)
	{
		z_pos.y += 1;
		m_ePt.y -= viewHeight;
	}
	//-----------------------------------
	if (m_ePt.x < 0 && z_pos.x > 0)
	{
		z_pos.x -= 1;
		m_ePt.x += viewWidth;
	}
	if (m_ePt.y < 0 && z_pos.y > 0)
	{
		z_pos.y -= 1;
		m_ePt.y += viewHeight;
	}

	//-----------------------------------
	if (m_ePt.x >= viewWidth)
	{
		z_pos.x -= m_ePt.x / viewWidth;
		//m_ePt.x = m_ePt.x % (int)viewWidth;
		m_ePt.x = m_ePt.x - viewWidth * (int)(m_ePt.x / viewWidth);
	}

	if (m_ePt.y >= viewHeight)
	{
		z_pos.y -= m_ePt.y / viewHeight;
		//m_ePt.y = m_ePt.y % (int)viewHeight;
		m_ePt.y = m_ePt.y - viewHeight * (int)(m_ePt.y / viewHeight);
	}

	//----------------------------------
	if (z_pos.x <= 0)
		z_pos.x = 0;
	if (z_pos.y <= 0)
		z_pos.y = 0;

	//z_pos.x + (m_ePt.x + m_pos.x) / viewWidth;

	Invalidate();
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}




void CChildView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnMButtonDown(nFlags, point);

	SetCapture();
	m_sPt = point;
}


void CChildView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnMButtonUp(nFlags, point);

	ReleaseCapture();
}

// RGB Value -----------------------------------------------------------------------
void CChildView::PrintText(CDC* pDC)
{
	COLORREF rgb;
	int cntWidth = 0, cntHeight = 0;


	if (viewWidth >= 80)
	{
		for (int i = z_pos.x; i < z_pos.x + rectWidth + 2; i++)
		{
			for (int j = z_pos.y; j < z_pos.y + rectHeight + 2; j++)
			{
				rgb = memDC.GetPixel(i, j);

				int R = 0, G = 0, B = 0;

				R = GetRValue(rgb);
				G = GetGValue(rgb);
				B = GetBValue(rgb);

				int pixelvale = (R + G + B) / 3.0;
				CString strNum;
				strNum.Format(_T("%d"), pixelvale);

				//pDC->SetBkMode(TRANSPARENT);
				mdcOffScreen.SetBkMode(TRANSPARENT);

				if (pixelvale >= 125)
				{
					//pDC->SetTextColor(0x00000000);
					mdcOffScreen.SetTextColor(0x00000000);
				}
				else
				{
					//pDC->SetTextColor(RGB(255, 255, 255));
					mdcOffScreen.SetTextColor(RGB(255, 255, 255));
				}


				//pDC->DrawText(strNum, CRect(i, j, i + viewWidth, j + viewHeight), DT_SINGLELINE | DT_VCENTER);


				CBrush brush;
				brush.CreateStockObject(NULL_BRUSH);
				CBrush* pOldBrush = mdcOffScreen.SelectObject(&brush);

				CRect m_rect = CRect(cntWidth * viewWidth, cntHeight * viewHeight, cntWidth * viewWidth + viewWidth, cntHeight * viewHeight + viewHeight);
				mdcOffScreen.Rectangle(m_rect);
				mdcOffScreen.SelectObject(pOldBrush);

				mdcOffScreen.DrawText(strNum, m_rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

				cntHeight++;
			}
			cntWidth++;
			//printf("cntHeight : %d ", cntHeight);
			cntHeight = 0;
		}
	}

	//printf("viewWidth : %f", viewWidth);
	//printf("viewHeight : %f", viewHeight);

	//printf("cntWidth : %d ", cntWidth);
	printf("\n");

}

// += 알고리즘 파트---------------------------------------------------------------------------------------------------------
int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	pFrame = (CMainFrame*)AfxGetMainWnd();
	if (pFrame->imageList.empty()) {
		static TCHAR BASED_CODE szFilter[] = _T("이미지 파일(*.BMP, *.GIF, *.JPG, *PNG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.png;*.gif; |모든파일(*.*)|*.*||");
		CString strFileList;
		CString File;
		//CFileDialog dlg(TRUE, _T("*.jpg"), _T("image"), OFN_HIDEREADONLY, szFilter);
		CFileDialog dlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, szFilter);

		const int c_cMaxFiles = 400 /*선택할 파일 숫자*/;	// 메모리 부족현상으로 확장 안해주면 몇개 못씀

		const int c_cbBuffSize = (c_cMaxFiles * (MAX_PATH + 1)) + 1;

		dlg.GetOFN().lpstrFile = strFileList.GetBuffer(c_cbBuffSize);

		dlg.GetOFN().nMaxFile = c_cbBuffSize;



		if (dlg.DoModal() == IDOK)

		{

			for (POSITION pos = dlg.GetStartPosition(); pos != NULL;)

			{

				// 전체삭제는 ResetContent

				CString sFilename = dlg.GetNextPathName(pos);
#ifdef _UNICODE
				CT2CA pszConvertedAnsiString(sFilename);
				std::string fileName(pszConvertedAnsiString);
				imageData temp;
				temp.setFileName(fileName);
				pFrame->imageList.push_back(temp);

#else
				std::string fileName((LPCTSTR)sFilename);
				imageData temp;
				temp.setFileName(fileName);
				pFrame->imageList.push_back(temp);
#endif

			}

		}
		else  return -1;
	
	}

	m_background.Detach();
	pFrame->imageList[idx].setImage();
	m_background.Attach(pFrame->imageList[idx].getImage());
	m_background.GetBitmap(&m_Bitmap);
	//for (int i = 0; i < pFrame->imageList.size(); i++) {
	//	pFrame->imageList[i].setImage();
	//}

	///*if (!image.IsNull())
	//	image.Destroy();
	//image.Load(theApp.sFilename);*/

	//m_background.Detach();
	//m_background.Attach(pFrame->imageList[idx].getImage());
	m_background.GetObject(sizeof(BITMAP), (LPVOID)&m_Bitmap);

	rectWidth = m_Bitmap.bmWidth;
	rectHeight = m_Bitmap.bmHeight;

	m_ePt.x = 0;
	m_ePt.y = 0;

	z_pos.x = 1;
	z_pos.y = 1;


	viewWidth = 1;
	viewHeight = 1;

	zoomView = 1;
	zoomScale = 1;
	rectScale = 1;
	printWidth = rectWidth;
	printHeight = rectHeight;

	startX = 0;
	startY = 0;

	m_bkgBrush.CreateSolidBrush(0x00000000);
	m_ePt.x = 0;
	m_ePt.y = 0;
	z_pos = m_ePt;

	drawID = false;

	return 0;
}


void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nChar) {
	case VK_LEFT:
		if (idx != 0)
			idx--;
		else
			return;
		break;
	case VK_RIGHT:
		if (idx != pFrame->imageList.size() - 1)
			idx++;
		else return;
		break;
	default:
		return;
	}
	cout << idx << endl;
	pFrame->imageList[idx].setImage();
	m_background.Detach();
	m_background.Attach(pFrame->imageList[idx].getImage());
	m_background.GetBitmap(&m_Bitmap);
	Invalidate();

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


// 그리기-------------------------------------------------------------------------------

void CChildView::OnDrawline()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	drawStyle = DrawMode::DLine;
}


void CChildView::OnDrawrect()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	drawStyle = DrawMode::DRectangle;
}


void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	SetCapture();
	x = z_pos.x + (m_ePt.x + point.x) / viewWidth;
	y = z_pos.y + (m_ePt.y + point.y) / viewHeight;
	// mov_x, mov_y는 도형 그리기 끝(이전)값
	mov_x = x;
	mov_y = y;
	int rLeft, rRight, rTop, rBottom;
	

	printf("도형 개수 : %d\n", data.GetSize());
	for (int i = 0; i < 4; i++)
	{
		if (x >= rRect[i].left && x <= rRect[i].right && y >= rRect[i].top && y <= rRect[i].bottom)
		{
			EdgeChosen = true;
			chosenEdge = i;
			break;
		}
	}

	for (int i = data.GetSize() - 1; i >= 0; i--)
	{
		rLeft = data[i].rect.left;
		rRight = data[i].rect.right;
		rTop = data[i].rect.top;
		rBottom = data[i].rect.bottom;

		if (x >= rLeft && x <= rRight && y >= rTop && y <= rBottom)
		{
			
			sPoint.x = rLeft;
			sPoint.y = rTop;
			ePoint.x = rRight;
			ePoint.y = rBottom;

			rRect[0].SetRect(sPoint.x - 15, sPoint.y - 15, sPoint.x + 15, sPoint.y + 15);//1사분면
			rRect[1].SetRect(ePoint.x - 15, ePoint.y - 15, ePoint.x + 15, ePoint.y + 15);//4사분면
			rRect[2].SetRect(sPoint.x - 15, ePoint.y - 15, sPoint.x + 15, ePoint.y + 15);//3사분면
			rRect[3].SetRect(ePoint.x - 15, sPoint.y - 15, ePoint.x + 15, sPoint.y + 15);//2사분면

			panID = true;
			chooseID = true;
			chosenShape = i;
			printf("n번째 도형 눌림 : %d\n", i);
			break;
		}
		panID = false;
		chooseID = false;
	}

	if(panID != true)
		drawID = TRUE; // 그리기 시작을 알리는 변수	

	CWnd::OnLButtonDown(nFlags, point);
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.d

	if (drawID)  // 도형 그리기 상태일 동안만
	{
		mov_x = z_pos.x + (m_ePt.x + point.x) / viewWidth;
		mov_y = z_pos.y + (m_ePt.y + point.y) / viewHeight;
		// 마우스 버트을 놓으면 각종 값을 shape 구조체 맴버변수에 저장
		shape.shapeType = drawStyle;  // 도형 콤보 상자에서 선택된 도형 스타일을 저장		
		shape.penWidth = viewWidth;
		shape.rect.left = x; // 도형 시작좌표 저장
		shape.rect.top = y;
		shape.rect.right = mov_x; // 도형 끝 좌표 저장
		shape.rect.bottom = mov_y;

		//저장된 shape를 배열에 저장
		data.Add(shape);
	}
	drawID = FALSE;
	panID = FALSE;
	EdgeChosen = FALSE;

	printf("data.size : %d", data.GetSize());

	CWnd::OnLButtonUp(nFlags, point);
	ReleaseCapture();
	Invalidate();
}


int CChildView::draw(CDC* pDC, CPoint point)
{
	//CClientDC dc(this);
	CDC* dc = GetDC();
	CMainFrame* cmr = (CMainFrame*)AfxGetMainWnd();
	//CPen myPen, * oldPen;
	//CBrush myBrush, * oldBrush;

	CPen myPen(PS_SOLID, viewWidth, RGB(255, 255, 255));
	CPen* oldPen;
	//oldPen = memDC.SelectObject(&myPen);
	//CDC* pDd = GetDC();

	//memDC.CreateCompatibleDC(pDd);


	//memDC.SelectObject(&m_background);
	oldPen = dc->SelectObject(&myPen);

	if (drawStyle == DrawMode::DLine) // 콤보상자에서 직선 선택시
	{
		printf("%d, %d\n", x, y);
		dc->SetROP2(R2_XORPEN); // 픽셀인 펜 색 조합 및 반전 화면 색상 

		dc->MoveTo(x, y); dc->LineTo(mov_x, mov_y); //전단계를 다시 그려서 흰색으로 만듬
		mov_x = m_pos.x;
		mov_y = m_pos.y;
		dc->MoveTo(x, y); dc->LineTo(mov_x, mov_y);
	}
	//if (drawStyle == DrawMode::DLine) // 콤보상자에서 직선 선택시
	//{
	//	memDC.SetROP2(R2_NOTXORPEN); // 픽셀인 펜 색 조합 및 반전 화면 색상 
	//	memDC.MoveTo(x, y); memDC.LineTo(mov_x, mov_y); //전단계를 다시 그려서 흰색으로 만듬
	//	mov_x = point.x;
	//	mov_y = point.y;
	//	memDC.MoveTo(x, y); memDC.LineTo(mov_x, mov_y);
	//}
	else if (drawStyle == DrawMode::DRectangle) // 콤보상자에서 사각형 선택시
	{
		dc->SetROP2(R2_NOTXORPEN); // 픽셀인 펜 색 조합 및 반전 화면 색상 
		dc->Rectangle(x, y, mov_x, mov_y); //전단계를 다시 그려서 흰색으로 만듬, 지워지는 효과
		mov_x = m_pos.x;
		mov_y = m_pos.y;
		dc->Rectangle(x, y, mov_x, mov_y);
	}
	else if (drawStyle == DrawMode::DEllipse) // 콤보상자에서 원 선택시
	{
		dc->SetROP2(R2_NOTXORPEN); // 픽셀인 펜 색 조합 및 반전 화면 색상 
		dc->Ellipse(x, y, mov_x, mov_y); //전단계를 다시 그려서 흰색으로 만듬
		mov_x = m_pos.x;
		mov_y = m_pos.y;
		dc->Ellipse(x, y, mov_x, mov_y);
	}

	dc->SelectObject(oldPen); // 이전 팬 선택		
	myPen.DeleteObject();  // 생성한 펜 메모리에서 제거		

	//memDC.DeleteDC();
	//memDC.SelectObject(oldPen); // 이전 팬 선택		
	//myPen.DeleteObject();  // 생성한 펜 메모리에서 제거		
	//myBrush.DeleteObject();  // 생성한 브러쉬 메모리에서 제거			
	return x, y, mov_x, mov_y;
}

void CChildView::drawShape(int shapeNum, int penWd, int sx, int sy, int ex, int ey)
{
	//CClientDC dc(this);
	CDC* dc= GetDC();
	//CBrush brush;
	//brush.CreateSolidBrush();
	//CMainFrame* cmr = (CMainFrame*)AfxGetMainWnd();
	//CPen myPen, * oldPen;
	//CBrush myBrush, * oldBrush;

	//oldPen = dc.SelectObject(&myPen);
	//memDC.MoveTo(sx, sy); memDC.LineTo(ex, ey);
	
	CBrush brush;
	brush.CreateStockObject(NULL_BRUSH);
	CBrush* pOldBrush = memDC.SelectObject(&brush);
	
	if (shapeNum == DrawMode::DLine) // 콤보상자에서 직선 선택시
	{
		memDC.MoveTo(sx, sy); 
		memDC.LineTo(ex, ey);
	}

	else if (shapeNum == 3) // 콤보상자에서 사각형 선택시
	{
		// 화면에 사각형을 그린다.
		memDC.Rectangle(sx, sy, ex, ey);
	}

	else if (shapeNum == 2) // 콤보상자에서 원 선택시
	{
		memDC.Ellipse(sx, sy, ex, ey);
	}

	if (chooseID == TRUE)
	{
		CRect rect;
		CPen* oldPen;
		rect.SetRect(sPoint, ePoint);
		memDC.SelectObject(&oldPen);
		CPen pen1(PS_DOT, 1, BLACK_PEN);
		oldPen = memDC.SelectObject(&pen1);
		memDC.SelectStockObject(NULL_BRUSH);
		memDC.Rectangle(rect);  //rect 그리기
		memDC.SelectObject(&oldPen);

		CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
		oldPen = memDC.SelectObject(&pen);
		memDC.SelectStockObject(WHITE_BRUSH);
		memDC.Ellipse(rRect[0]);
		memDC.Ellipse(rRect[1]);
		memDC.Ellipse(rRect[2]);
		memDC.Ellipse(rRect[3]);
	}

	memDC.SelectObject(pOldBrush);
	DeleteObject(brush);
	
	//else if (shapeNum == 3) // 콤보상자에서 부채꼴 선택시
	//{
	//	oldBrush = dc.SelectObject(&myBrush);
	//	CRect PieRect(sx, sy, ex, ey);
	//	dc.Pie(PieRect, CPoint(PieRect.CenterPoint().x, PieRect.top), CPoint(PieRect.right, PieRect.CenterPoint().y));
	//	dc.SelectObject(oldBrush); // 이전 브러시 선택
	//}
	//else if (shapeNum == 4) // 그리기텍스트 아이콘 선택시
	//{
	//	dc.SetTextColor(fgcolor); // 텍스트 색상 설정
	//	dc.SetBkColor(bgcolor); // 텍스트 배경색 설정
	//							// 폰트 생성
	//			
	//	dc.DrawText(str, CRect(sx, sy, ex, ey), DT_SINGLELINE | DT_CENTER | DT_VCENTER);  // 화면에 문자열 출력

	//}
	//memDC.SelectObject(oldPen); // 이전 팬 선택	
	//myPen.DeleteObject();  // 생성한 펜 메모리에서 제거		
	//myBrush.DeleteObject(); // 생성한 브러쉬 메모리에서 제거		

}