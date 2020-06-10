// ChildView.cpp: CChildView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "Fourthlohgic_Project.h"
#include "ChildView.h"
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")


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
	//ON_WM_KEYDOWN()
	ON_COMMAND(ID_LINE, &CChildView::OnLine)
	ON_COMMAND(ID_RECTANGLE, &CChildView::OnRectangle)
	ON_COMMAND(ID_PointRect, &CChildView::OnTriangle)
	ON_COMMAND(ID_ELLPSE, &CChildView::OnEllipse)
	ON_COMMAND(ID_CROSSHAIR, &CChildView::OnCrosshair)
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
	CString str;
	str.Format(_T("X = %d, Y = %d"), m_pos.x, m_pos.y);
	dc.TextOut(m_pos.x, m_pos.y, str);

	memDC.CreateCompatibleDC(pDC);
	mdcOffScreen.CreateCompatibleDC(pDC);

	bmpOffScreen.CreateCompatibleBitmap(pDC, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CXFULLSCREEN));

	oldbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);

	m_background.Detach();
	m_background.Attach(::CopyImage(pFrame->imageList[idx].getImage(),IMAGE_BITMAP, 0, 0, 0));

	memDC.SelectObject(&m_background);   // 배경 그림을 선택하고

	//if (drawID == TRUE) {
	//	//if (PrevPt.x != mov_x || PrevPt.y != mov_y)
	//		draw();
	//}
	for (int i = 0; i < data.GetSize(); i++)
		drawShape(data[i]);
	if(chooseID==TRUE)
		SelectDrawShape(data[SelectIndex]);

	mdcOffScreen.SetStretchBltMode(COLORONCOLOR);
	mdcOffScreen.StretchBlt(0, 0, viewWidth * (int)(rectWidth + 2), viewHeight * (int)(rectHeight +2), &memDC, z_pos.x, z_pos.y, rectWidth + 2, rectHeight + 2, SRCCOPY);

	PrintText(&mdcOffScreen);

	pDC->SetStretchBltMode(COLORONCOLOR);
	pDC->StretchBlt(0, 0, m_bgRect.right, m_bgRect.bottom, &mdcOffScreen, m_ePt.x, m_ePt.y, m_bgRect.right, m_bgRect.bottom, SRCCOPY);

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

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_bgRect.left = 0;
	m_bgRect.top = 0;
	m_bgRect.right = cx;
	m_bgRect.bottom = cy;

	Invalidate(FALSE);
}

BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (zDelta <= 0)// 휠 내릴때
	{
		ViewScale = 1.2f;
		ViewValue *= 1.2f;

		zoomScale = 1.25f;
		zoomView *= 0.8f;
	}
	else// 휠 올릴때
	{
		ViewScale = 0.75f;
		ViewValue *= 0.75f;

		zoomScale = 0.8f;
		zoomView *= 1.25f;
	}

	//왼쪽위 꼭짓점
	z_pos.x += round(((((float)m_ePt.x + (float)m_pos.x) / (viewWidth * (int)(rectWidth + 2))) * rectWidth) - ((((float)m_ePt.x + (float)m_pos.x) / (viewWidth * (int)(rectWidth + 2))) * (rectWidth * zoomScale)));
	z_pos.y += round(((((float)m_ePt.y + (float)m_pos.y) / (viewHeight * (int)(rectHeight + 2))) * rectHeight) - ((((float)m_ePt.y + (float)m_pos.y) / (viewHeight * (int)(rectHeight + 2))) * (rectHeight * zoomScale)));


	// 오른쪽 아래 꼭짓점

	rectWidth = rectWidth * zoomScale;
	rectHeight = rectHeight * zoomScale;

	printWidth = rectWidth * zoomView;
	printHeight = rectHeight * zoomView;


	viewWidth = (printWidth ) / (rectWidth );
	viewHeight = (printHeight ) / (rectHeight);


	Invalidate(FALSE);
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
	//
	//printf("cntWidth : %d ", cntWidth);
	//printf("\n");

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

	ViewScale = 1;
	ViewValue = 1;

	viewWidth = 1;
	viewHeight = 1;

	zoomView = 1;
	zoomScale = 1;
	printWidth = rectWidth;
	printHeight = rectHeight;

	m_bkgBrush.CreateSolidBrush(0x00000000);
	m_ePt.x = 0;
	m_ePt.y = 0;
	z_pos = m_ePt;
	ViewScale = 1;
	ViewValue = 1;

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
	Invalidate(FALSE);

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

// Zoom + Panning ------------------------------------------------------------------------------
void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnMouseMove(nFlags, point);
	int i = 0;
	m_pos = point;
	
	if (nFlags == MK_MBUTTON)
	{
		//int width = m_bgRect.right - m_bgRect.left;
		//int height = m_bgRect.bottom - m_bgRect.top;

		// A영역의 사각형 그리기 //m_sPt는 기존 포인트, point는 이동
		if (m_sPt.x < point.x) // 오른쪽으로 끌었을때
		{
			m_ePt.x -= point.x - m_sPt.x;

			if (m_ePt.x <= 0)
			{
				if (z_pos.x > 0)
				{
					i = 1 + ((0 - m_ePt.x) / (int)viewWidth);
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
				i = 1 + (m_ePt.x + m_bgRect.right - (viewWidth * (int)(rectWidth + 2))) / (int)viewWidth;
				z_pos.x += i;
				m_ePt.x -= i * viewWidth;
			}
		}



		// B영역의 사각형 그리기
		if (m_sPt.y > point.y) // 위로 올렸을때
		{
			m_ePt.y += m_sPt.y - point.y;

			if (m_ePt.y + m_bgRect.bottom - (viewHeight * (int)(rectHeight + 2)) >= 0)
			{
				//m_ePt.y = 0;
				//z_pos.y += 1;
				//m_ePt.y -= viewHeight;

				i = 1 + (m_ePt.y + m_bgRect.bottom - (viewHeight * (int)(rectHeight + 2))) / (int)viewHeight;
				z_pos.y += i;
				m_ePt.y -= i * viewHeight;
			}

		}
		else // 아래로 내렸을때
		{
			m_ePt.y -= point.y - m_sPt.y;
			if (m_ePt.y < 0)
			{
				//z_pos.y -= 1;
				//m_ePt.y += viewHeight;

				i = 1 + ((0 - m_ePt.y) / (int)viewHeight);
				z_pos.y -= i;
				m_ePt.y += i * viewHeight;
			}
		}

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
		Invalidate(FALSE);
		UpdateWindow();
	}
	// 그림
	if (MK_LBUTTON == nFlags && drawID == TRUE) // 마우스를 클릭하여 드래그일 동안만 
		draw();
	// 이동
	else if (MK_LBUTTON == nFlags && (panID == TRUE) && (rotateClick == FALSE))
	{
		int o, p;
		int xx, yy;

		xx = x, yy = y;
		x = z_pos.x + (m_ePt.x + point.x) / viewWidth;
		y = z_pos.y + (m_ePt.y + point.y) / viewHeight;

		o = (x - xx);
		p = (y - yy);

		CPoint pt;
		RectCount = data[SelectIndex].pts.size();
		if (data[SelectIndex].shapeType != DrawMode::DTriangle)
		{
			// 이동
			for (int i = 0; i < RectCount; i++)
			{
				pt.x = data[SelectIndex].RotatePts[i].x += o;
				pt.y = data[SelectIndex].RotatePts[i].y += p;
			}
		}
		else
		{
			// 이동
			for (int i = 0; i < RectCount; i++)
			{
				pt.x = data[SelectIndex].RotatePts[i].x += o;
				pt.y = data[SelectIndex].RotatePts[i].y += p;
			}
		}

		//SelectShapeUpdate();
		UpdateWindow();
		Invalidate(FALSE);
	}
	// 크기 조정
	else if (MK_LBUTTON == nFlags && (EdgeSelect == TRUE) && rotateClick == FALSE)
	{
		int o, p;
		int xx, yy;
		//CPoint TLX;
		xx = x, yy = y;
		x = z_pos.x + (m_ePt.x + point.x) / viewWidth;
		y = z_pos.y + (m_ePt.y + point.y) / viewHeight;
		o = (x - xx);
		p = (y - yy);
		double theta = data[SelectIndex].theta;

		RectCount = data[SelectIndex].pts.size();

		// 길이
		if (data[SelectIndex].shapeType == DrawMode::DLine)
		{
			data[SelectIndex].pts[EdgeIndex].x += o;
			data[SelectIndex].pts[EdgeIndex].y += p;
		}
		// 삼각형
		else if(data[SelectIndex].shapeType == DrawMode::DTriangle)
		{
			data[SelectIndex].pts[EdgeIndex].x += o;
			data[SelectIndex].pts[EdgeIndex].y += p;
		}
		// 크로스 헤어
		else if (data[SelectIndex].shapeType == DrawMode::DCrossHair)
		{
			if (EdgeIndex / 2 == 0)
			{
				data[SelectIndex].pts[EdgeIndex].x += o;
				data[SelectIndex].pts[EdgeIndex].y += p;
				data[SelectIndex].pts[EdgeIndex + 1].x -= o;
				data[SelectIndex].pts[EdgeIndex + 1].y -= p;
			}
			else
			{
				data[SelectIndex].pts[EdgeIndex].x += o;
				data[SelectIndex].pts[EdgeIndex].y += p;
				data[SelectIndex].pts[EdgeIndex - 1].x -= o;
				data[SelectIndex].pts[EdgeIndex - 1].y -= p;
			}

		}
		//사각형 원
		else
		{
			data[SelectIndex].pts[EdgeIndex].x += o;
			data[SelectIndex].pts[EdgeIndex].y += p;

			if (data[SelectIndex].theta == 0)
			{
				if (EdgeIndex == 0)
				{
					data[SelectIndex].pts[3].x += o;
					data[SelectIndex].pts[1].y += p;
				}
				else if (EdgeIndex == 1)
				{

					data[SelectIndex].pts[2].x += o;
					data[SelectIndex].pts[0].y += p;
				}
				else if (EdgeIndex == 2)
				{
					data[SelectIndex].pts[1].x += o;
					data[SelectIndex].pts[3].y += p;
				}
				else if (EdgeIndex == 3)
				{
					data[SelectIndex].pts[0].x += o;
					data[SelectIndex].pts[2].y += p;
				}
			}
			else
			{
				if (EdgeIndex == 0)
				{
					data[SelectIndex].pts[1] = Intersection(data[SelectIndex].pts[0], data[SelectIndex].pts[1], data[SelectIndex].pts[2]);
					data[SelectIndex].pts[3] = Intersection(data[SelectIndex].pts[0], data[SelectIndex].pts[3], data[SelectIndex].pts[2]);
				}
				else if (EdgeIndex == 1)
				{
					data[SelectIndex].pts[0] = Intersection(data[SelectIndex].pts[1], data[SelectIndex].pts[0], data[SelectIndex].pts[3]);
					data[SelectIndex].pts[2] = Intersection(data[SelectIndex].pts[1], data[SelectIndex].pts[2], data[SelectIndex].pts[3]);
				}
				else if (EdgeIndex == 2)
				{
					data[SelectIndex].pts[1] = Intersection(data[SelectIndex].pts[2], data[SelectIndex].pts[1], data[SelectIndex].pts[0]);
					data[SelectIndex].pts[3] = Intersection(data[SelectIndex].pts[2], data[SelectIndex].pts[3], data[SelectIndex].pts[0]);
				}
				else if (EdgeIndex == 3)
				{
					data[SelectIndex].pts[0] = Intersection(data[SelectIndex].pts[3], data[SelectIndex].pts[0], data[SelectIndex].pts[1]);
					data[SelectIndex].pts[2] = Intersection(data[SelectIndex].pts[3], data[SelectIndex].pts[2], data[SelectIndex].pts[1]);
				}
			}
		}

		// 꼭지점 업데이트
		SelectShapeUpdate();
		Invalidate(FALSE);
	}
	// 회전 클릭
	else if (MK_LBUTTON == nFlags && (rotateClick == TRUE))
	{
		Point2d center = data[SelectIndex].Center;
		Point2d Rotate = data[SelectIndex].Rotate;
		vector<Point2d> pts = data[SelectIndex].pts; // 복사 -> 이동 -> R복사
		Point2d pt;
		x = z_pos.x + (m_ePt.x + point.x) / viewWidth;
		y = z_pos.y + (m_ePt.y + point.y) / viewHeight;

		double theta = atan2((Rotate.x - center.x), (Rotate.y - center.y)) - atan2((x - center.x), (y - center.y));
		for (int i = 0; i < pts.size(); i++)
		{

			pt.x = cvRound((pts[i].x - center.x) * cos(theta) - (pts[i].y - center.y) * sin(theta) + center.x);
			pt.y = cvRound((pts[i].x - center.x) * sin(theta) + (pts[i].y - center.y) * cos(theta) + center.y);
			pts[i] = pt;
		}
		data[SelectIndex].RotatePts = pts;
		data[SelectIndex].R_theta = theta + data[SelectIndex].theta;

		//if (data[SelectIndex].shapeType == DrawMode::DLine)
		//{
		//	for (int i = 0; i < 2; i++)
		//	{
		//
		//		pt.x = cvRound((pts[i].x - center.x) * cos(theta) - (pts[i].y - center.y) * sin(theta) + center.x);
		//		pt.y = cvRound((pts[i].x - center.x) * sin(theta) + (pts[i].y - center.y) * cos(theta) + center.y);
		//		pts[i] = pt;
		//	}
		//	data[SelectIndex].RotatePts = pts;
		//	data[SelectIndex].R_theta = theta + data[SelectIndex].theta;
		//}
		//else if (data[SelectIndex].shapeType == DrawMode::DEllipse)
		//{
		//	for (int i = 0; i < 4; i++)
		//	{
		//		pt.x = cvRound((pts[i].x - center.x) * cos(theta) - (pts[i].y - center.y) * sin(theta) + center.x);
		//		pt.y = cvRound((pts[i].x - center.x) * sin(theta) + (pts[i].y - center.y) * cos(theta) + center.y);
		//		pts[i] = pt;
		//	}
		//	data[SelectIndex].RotatePts = pts;
		//	data[SelectIndex].R_theta = theta + data[SelectIndex].theta;
		//}
		//else if (data[SelectIndex].shapeType == DrawMode::DRectangle)
		//{
		//	for (int i = 0; i < 4; i++)
		//	{
		//		pt.x = cvRound((pts[i].x - center.x) * cos(theta) - (pts[i].y - center.y) * sin(theta) + center.x);
		//		pt.y = cvRound((pts[i].x - center.x) * sin(theta) + (pts[i].y - center.y) * cos(theta) + center.y);
		//		pts[i] = pt;
		//	}
		//	data[SelectIndex].RotatePts = pts;
		//	data[SelectIndex].R_theta = theta + data[SelectIndex].theta;
		//}
		//else if (data[SelectIndex].shapeType == DrawMode::DTriangle)
		//{
		//	for (int i = 0; i < 4; i++)
		//	{
		//		pt.x = cvRound((pts[i].x - center.x) * cos(theta) - (pts[i].y - center.y) * sin(theta) + center.x);
		//		pt.y = cvRound((pts[i].x - center.x) * sin(theta) + (pts[i].y - center.y) * cos(theta) + center.y);
		//		pts[i] = pt;
		//	}
		//	data[SelectIndex].RotatePts = pts;
		//	data[SelectIndex].R_theta = theta + data[SelectIndex].theta;
		//}
		Invalidate(FALSE);
	}
}

// 그리기-------------------------------------------------------------------------------
void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//chooseID = FALSE; // 채크해제후 검사
	SetCapture();
	x = z_pos.x + (m_ePt.x + point.x) / viewWidth;
	y = z_pos.y + (m_ePt.y + point.y) / viewHeight;
	// mov_x, mov_y는 도형 그리기 끝(이전)값
	mov_x = x;
	mov_y = y;
	//printf("도형 개수 : %d\n", data.GetSize());
	
	// 엣지 클릭 검사 // 회전 선 클릭 검사
	if (chooseID == TRUE) {
		
		//엣지 클릭 검사
		for (int i = 0; i < RectCount; i++)
		{
			if (x >= rRect[i].left && x <= rRect[i].right && y >= rRect[i].top && y <= rRect[i].bottom)
			{
				EdgeSelect = TRUE;
				EdgeIndex = i;
				Invalidate(FALSE);
				return;
			}
		}

		// 회전 선 클릭 검사
		CRect centRC;
		centRC.left = min(Rotate.x, Center.x) - 10;
		centRC.right = max(Rotate.x, Center.x) + 10;
		centRC.top = min(Rotate.y, Center.y) - 10;
		centRC.bottom = max(Rotate.y, Center.y) + 10;
		if (x >= centRC.left && x <= centRC.right && y >= centRC.top && y <= centRC.bottom) {
			rotateClick = TRUE;
			Invalidate(FALSE);
			return;
		}
	}

	// 이미지 객체 클릭 검사
	for (int i = data.GetSize() - 1; i >= 0; i--)
	{
		if (data[i].shapeType != DrawMode::DLine) {
			if (isInside(CPoint(x, y), data[i].pts))
			{
				Point pt;
				RectCount = data[i].pts.size();
				for (int j = 0; j < RectCount; j++)
				{
					pt.x = data[i].pts[j].x;
					pt.y = data[i].pts[j].y;
					rRect[j].SetRect(pt.x - 5, pt.y - 5, pt.x + 5, pt.y + 5);
				}

				// !추가 객체 선택 정보 전달
				Center.x = data[i].Center.x;
				Center.y = data[i].Center.y;
				Rotate = data[i].Rotate;

				panID = TRUE;
				chooseID = TRUE;
				SelectIndex = i;
				Invalidate(FALSE);
				return;
			}
		}
		else if(data[i].shapeType == DrawMode::DLine)
		{
			vector<Point2d> linePts;
			int Pt1, Pt2;
			Pt1 = pow(data[i].pts[0].x, 2) + pow(data[i].pts[0].y, 2);
			Pt2 = pow(data[i].pts[1].x, 2) + pow(data[i].pts[1].y, 2);
			if (Pt1 > Pt2)
			{
				Point2d temp = data[i].pts[0];
				data[i].pts[0] = data[i].pts[1];
				data[i].pts[1] = temp;
			}
			linePts.clear();
			linePts.push_back(Point2d(data[i].pts[0].x-5, data[i].pts[0].y-5));
			linePts.push_back(Point2d(data[i].pts[0].x+5, data[i].pts[0].y-5));
			linePts.push_back(Point2d(data[i].pts[1].x+5, data[i].pts[1].y+5));
			linePts.push_back(Point2d(data[i].pts[1].x-5, data[i].pts[1].y+5));
			if (isInside(CPoint(x, y), linePts))
			{
				CPoint pt;
				RectCount = data[i].pts.size();
				for (int j = 0; j < RectCount; j++)
				{
					pt.x = data[i].pts[j].x;
					pt.y = data[i].pts[j].y;
					rRect[j].SetRect(pt.x - 5, pt.y - 5, pt.x + 5, pt.y + 5);
				}
				// !추가 객체 선택 정보 전달
				Center.x = data[i].Center.x;
				Center.y = data[i].Center.y;
				Rotate = data[i].Rotate;

				panID = TRUE;
				chooseID = TRUE;
				SelectIndex = i;
				Invalidate(FALSE);
				return;
			}
		}
	}

	chooseID = FALSE;
	CWnd::OnLButtonDown(nFlags, point);
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.d

	// 도형 그리기 일경우 도형 생성후 추가
	if (drawID)  // 도형 그리기 상태일 동안만
	{
		MyShape shape;

		mov_x = z_pos.x + (m_ePt.x + point.x) / viewWidth;
		mov_y = z_pos.y + (m_ePt.y + point.y) / viewHeight;
		// 마우스 버트을 놓으면 각종 값을 shape 구조체 맴버변수에 저장
		shape.shapeType = drawStyle;  // 도형 콤보 상자에서 선택된 도형 스타일을 저장		

		if (shape.shapeType == DrawMode::DLine)
		{
			shape.pts.push_back(Point2d(x, y));
			shape.pts.push_back(Point2d(mov_x, mov_y));
			shape.radin[0] = 0;
			shape.radin[1] = 0;
		}
		else if (shape.shapeType == DrawMode::DEllipse)
		{
			shape.pts.push_back(Point2d(x, y));
			shape.pts.push_back(Point2d(mov_x, y));
			shape.pts.push_back(Point2d(mov_x, mov_y));
			shape.pts.push_back(Point2d(x, mov_y));
			shape.radin[0] = abs(x - mov_x) / 2;
			shape.radin[1] = abs(y - mov_y) / 2;
		}
		else if (shape.shapeType == DrawMode::DRectangle)
		{
			shape.pts.push_back(Point2d(x, y));
			shape.pts.push_back(Point2d(mov_x, y));
			shape.pts.push_back(Point2d(mov_x, mov_y));
			shape.pts.push_back(Point2d(x, mov_y));
			shape.radin[0] = abs(x - mov_x) / 2;
			shape.radin[1] = abs(y - mov_y) / 2;
		}
		else if (shape.shapeType == DrawMode::DTriangle)
		{
			shape.pts.push_back(Point2d((x + mov_x) / 2, y));
			shape.pts.push_back(Point2d(mov_x, mov_y));
			shape.pts.push_back(Point2d(x, mov_y));
			//shape.pts.push_back(Point2d(x, mov_y));
			shape.radin[0] = abs(x - mov_x) / 2;
			shape.radin[1] = abs(y - mov_y) / 2;
			//shape.RotatePts=shape.T_Pts = shape.pts;
		}
		else if (shape.shapeType == DrawMode::DCrossHair)
		{
			shape.pts.push_back(Point2d((x + mov_x) / 2, y));
			shape.pts.push_back(Point2d((x + mov_x) / 2, mov_y));
			shape.pts.push_back(Point2d(x, (y + mov_y) / 2));
			shape.pts.push_back(Point2d(mov_x, (y + mov_y) / 2));
			shape.radin[0] = abs(x - mov_x) / 2;
			shape.radin[1] = abs(y - mov_y) / 2;
		}
		Point2d pt(0, 0);
		for (int i = 0; i < shape.pts.size(); i++)
			pt += shape.pts[i];
		int Count = shape.pts.size();
		shape.RotatePts = shape.pts;
		shape.Center = pt / Count;
		shape.Rotate = shape.Center;
		shape.Rotate.y -= 15;
		shape.theta = 0;
		shape.R_theta = 0;
		shape.shapeColor = RGB(0, 255, 255); // 색상값
		shape.thickness = 1; // 굵기 
		data.Add(shape);
	}

	if (rotateClick == TRUE)
	{
		if (data[SelectIndex].R_theta >= (CV_PI * 2))
			data[SelectIndex].R_theta = data[SelectIndex].R_theta - (CV_PI * 2);

		if (drawStyle == DrawMode::DLine) {// 직선 선택시
			data[SelectIndex].pts = data[SelectIndex].RotatePts;
			data[SelectIndex].theta = data[SelectIndex].R_theta;
		}
		else if (drawStyle == DrawMode::DEllipse) { // 원 선택시
			data[SelectIndex].theta = data[SelectIndex].R_theta;
			data[SelectIndex].pts = data[SelectIndex].RotatePts;
		}
		else if (drawStyle == DrawMode::DRectangle) { // 사각형 선택시
			data[SelectIndex].pts = data[SelectIndex].RotatePts;
			data[SelectIndex].theta = data[SelectIndex].R_theta;
		}
		else if (drawStyle == DrawMode::DTriangle) { // 사각형 선택시
			data[SelectIndex].pts = data[SelectIndex].RotatePts;
			data[SelectIndex].theta = data[SelectIndex].R_theta;
		}
		else if (drawStyle == DrawMode::DCrossHair) { // 사각형 선택시
			data[SelectIndex].pts = data[SelectIndex].RotatePts;
			data[SelectIndex].theta = data[SelectIndex].R_theta;
		}
	}

	if (panID == TRUE)
		data[SelectIndex].pts = data[SelectIndex].RotatePts;


	drawID = FALSE;
	panID = FALSE;
	EdgeSelect = FALSE;
	rotateClick = FALSE; 
	ReleaseCapture();
	SelectShapeRectUpdate();
	SelectShpaeCenterUpdate();
	Invalidate(FALSE);

	CWnd::OnLButtonUp(nFlags, point);
}

// 처음 그리기
void CChildView::draw()
{
	CDC* dc = GetDC();
	CPen myPen(PS_DOT, viewWidth, drawPenColor);
	CPen* oldPen;
	oldPen = dc->SelectObject(&myPen);

	if (drawStyle == DrawMode::DLine) // 콤보상자에서 직선 선택시
	{
		dc->SetROP2(R2_NOTXORPEN); // 픽셀인 펜 색 조합 및 반전 화면 색상 
		dc->MoveTo(x, y); dc->LineTo(mov_x, mov_y); //전단계를 다시 그려서 흰색으로 만듬
		mov_x = m_pos.x;
		mov_y = m_pos.y;
		//mov_x = z_pos.x + (m_ePt.x + m_pos.x) / viewWidth;
		//mov_y = z_pos.y + (m_ePt.y + m_pos.y) / viewHeight;
		dc->MoveTo(x, y); dc->LineTo(mov_x, mov_y);
	}
	else if (drawStyle == DrawMode::DRectangle) // 콤보상자에서 사각형 선택시
	{
		dc->SetROP2(R2_NOTXORPEN); // 픽셀인 펜 색 조합 및 반전 화면 색상 
		dc->Rectangle(x, y, mov_x, mov_y);
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
	else if (drawStyle == DrawMode::DTriangle) // 콤보상자에서 사각형 선택시
	{
		dc->SetROP2(R2_NOTXORPEN); // 픽셀인 펜 색 조합 및 반전 화면 색상 

		dc->MoveTo((x + mov_x)/2, y);
		dc->LineTo(mov_x, mov_y);
		dc->LineTo(x, mov_y);
		dc->LineTo((x + mov_x) / 2, y);

		mov_x = m_pos.x;
		mov_y = m_pos.y;

		dc->MoveTo((x + mov_x) / 2, y);
		dc->LineTo(mov_x, mov_y);
		dc->LineTo(x, mov_y);
		dc->LineTo((x + mov_x) / 2, y);
	}
	else if (drawStyle == DrawMode::DCrossHair) // 콤보상자에서 사각형 선택시
	{
		dc->SetROP2(R2_NOTXORPEN); // 픽셀인 펜 색 조합 및 반전 화면 색상 

		dc->MoveTo((x + mov_x) / 2, y);
		dc->LineTo((x + mov_x) / 2, mov_y);
		dc->MoveTo(x , (y + mov_y) / 2);
		dc->LineTo(mov_x, (y + mov_y) / 2);


		mov_x = m_pos.x;
		mov_y = m_pos.y;

		dc->MoveTo((x + mov_x) / 2, y);
		dc->LineTo((x + mov_x) / 2, mov_y);
		dc->MoveTo(x, (y + mov_y) / 2);
		dc->LineTo(mov_x, (y + mov_y) / 2);
	}
	dc->SelectObject(oldPen); // 이전 팬 선택		
	myPen.DeleteObject();  // 생성한 펜 메모리에서 제거			
}

// 그림 복구
void CChildView::drawShape(MyShape& shape)
{
	CDC* dc= GetDC();
	int shapeNum = shape.shapeType;
	CPen myPen(PS_SOLID, shape.thickness, shape.shapeColor);
	CPen* oldPen;
	oldPen = memDC.SelectObject(&myPen);

	if (shapeNum == DrawMode::DLine) // 콤보상자에서 직선 선택시
	{
		memDC.MoveTo(shape.pts[0].x, shape.pts[0].y);
		memDC.LineTo(shape.pts[1].x, shape.pts[1].y);
	}
	else if (shapeNum == DrawMode::DEllipse) // 원 그리기
	{
		MyEllipseR(&memDC, shape.Center, shape.radin[0], shape.radin[1], shape.theta, shape.shapeColor);
	}
	else if (shapeNum == DrawMode::DRectangle) // 콤보상자에서 사각형 선택시
	{
		// 화면에 사각형을 그린다.
		memDC.MoveTo(shape.pts[0].x, shape.pts[0].y);
		memDC.LineTo(shape.pts[1].x, shape.pts[1].y);
		memDC.LineTo(shape.pts[2].x, shape.pts[2].y);
		memDC.LineTo(shape.pts[3].x, shape.pts[3].y);
		memDC.LineTo(shape.pts[0].x, shape.pts[0].y);
	}
	else if (shapeNum == DrawMode::DTriangle) // 콤보상자에서 사각형 선택시
	{
		// 화면에 사각형을 그린다.
		memDC.MoveTo(shape.pts[0].x, shape.pts[0].y);
		memDC.LineTo(shape.pts[1].x, shape.pts[1].y);
		memDC.LineTo(shape.pts[2].x, shape.pts[2].y);
		//memDC.LineTo(shape.pts[3].x, shape.pts[3].y);
		memDC.LineTo(shape.pts[0].x, shape.pts[0].y);
	}
	else if (shapeNum == DrawMode::DCrossHair) // 콤보상자에서 사각형 선택시
	{
		// 화면에 사각형을 그린다.
		memDC.MoveTo(shape.pts[0].x, shape.pts[0].y);
		memDC.LineTo(shape.pts[1].x, shape.pts[1].y);
		memDC.MoveTo(shape.pts[2].x, shape.pts[2].y);
		memDC.LineTo(shape.pts[3].x, shape.pts[3].y);
	}
	memDC.SelectObject(oldPen); // 이전 팬 선택		
	myPen.DeleteObject();  // 생성한 펜 메모리에서 제거		
	DeleteObject(oldPen);
}

// 선택 그리기
void CChildView::SelectDrawShape(MyShape& shape)
{
	CDC* dc = GetDC();
	int shapeNum = shape.shapeType;
	double thickness = shape.thickness;
	// 선택 실선
	CPen* pen = new CPen(PS_DOT, thickness, BLACK_PEN);
	CPen* oldPen;
	oldPen = memDC.SelectObject(pen);
	memDC.SelectStockObject(NULL_BRUSH);
	if (shape.shapeType == DrawMode::DLine) // 콤보상자에서 직선 선택시
	{
		memDC.MoveTo(shape.pts[0].x, shape.pts[0].y);
		memDC.LineTo(shape.pts[1].x, shape.pts[1].y);
	}
	else if (shape.shapeType == DrawMode::DRectangle) // 콤보상자에서 사각형 선택시
	{
		memDC.MoveTo(shape.pts[0].x, shape.pts[0].y);
		memDC.LineTo(shape.pts[1].x, shape.pts[1].y);
		memDC.LineTo(shape.pts[2].x, shape.pts[2].y);
		memDC.LineTo(shape.pts[3].x, shape.pts[3].y);
		memDC.LineTo(shape.pts[0].x, shape.pts[0].y);
	}
	else if (shape.shapeType == DrawMode::DEllipse)
	{
		MyEllipsePS_DOT(&memDC, shape.Center, shape.radin[0], shape.radin[1], shape.R_theta);
	}
	else if (shape.shapeType == DrawMode::DTriangle) // 콤보상자에서 사각형 선택시
	{
		memDC.MoveTo(shape.pts[0].x, shape.pts[0].y);
		memDC.LineTo(shape.pts[1].x, shape.pts[1].y);
		memDC.LineTo(shape.pts[2].x, shape.pts[2].y);
		//memDC.LineTo(shape.pts[3].x, shape.pts[3].y);
		memDC.LineTo(shape.pts[0].x, shape.pts[0].y);
	}
	else if (shape.shapeType == DrawMode::DCrossHair) // 콤보상자에서 사각형 선택시
	{
		memDC.MoveTo(shape.pts[0].x, shape.pts[0].y);
		memDC.LineTo(shape.pts[1].x, shape.pts[1].y);
		memDC.MoveTo(shape.pts[2].x, shape.pts[2].y);
		memDC.LineTo(shape.pts[3].x, shape.pts[3].y);
	}
	memDC.SelectObject(&oldPen);

	// 확대 사각 박스 그리기
	pen = new CPen(PS_SOLID, 1, RGB(0, 0, 0));
	memDC.SelectObject(pen);
	memDC.SelectStockObject(WHITE_BRUSH);
	for (int i = 0; i < RectCount; i++)
		memDC.Rectangle(rRect[i]);
	memDC.SelectObject(&oldPen);

	// !추가  회전 직선 그리기
	pen = new CPen(PS_SOLID, 1, RGB(0, 0, 255));
	memDC.SelectObject(pen);
	memDC.SelectStockObject(NULL_BRUSH);
	memDC.MoveTo(Center.x, Center.y);
	memDC.LineTo(Rotate.x, Rotate.y);
	memDC.SelectObject(&oldPen);

	// 회전 선택
	if (rotateClick == TRUE) {
		pen = new CPen(PS_SOLID, thickness, RGB(0, 255, 0));
		memDC.SelectObject(pen);
		// 시작
		RectCount = shape.pts.size();
		if (shape.RotatePts.size() > 0)
		{

			// 사각형 OR 직선
			if (shape.shapeType == DrawMode::DCrossHair)
			{
				memDC.MoveTo(shape.RotatePts[0].x, shape.RotatePts[0].y);
				memDC.LineTo(shape.RotatePts[1].x, shape.RotatePts[1].y);
				memDC.MoveTo(shape.RotatePts[2].x, shape.RotatePts[2].y);
				memDC.LineTo(shape.RotatePts[3].x, shape.RotatePts[3].y);
			}
			else if (shape.shapeType == DrawMode::DEllipse)
			{
				MyEllipseR(&memDC, shape.Center, shape.radin[0], shape.radin[1], shape.R_theta, shape.shapeColor);
			}
			//else if (shape.shapeType != DrawMode::DEllipse) 
			else
			{
				memDC.MoveTo(data[SelectIndex].RotatePts[0].x, data[SelectIndex].RotatePts[0].y);
				for (int i = 1; i < data[SelectIndex].RotatePts.size(); i++)
					memDC.LineTo(data[SelectIndex].RotatePts[i].x, data[SelectIndex].RotatePts[i].y);
				memDC.LineTo(data[SelectIndex].RotatePts[0].x, data[SelectIndex].RotatePts[0].y);
			}
			Rotate.x = x;
			Rotate.y = y;
		}
		memDC.SelectObject(&oldPen);
	}
	if (panID == TRUE) {
		pen = new CPen(PS_SOLID, thickness, RGB(0, 255, 0));
		memDC.SelectObject(pen);
		// 시작
		RectCount = shape.pts.size();
		if (shape.RotatePts.size() > 0)
		{
			// 사각형 OR 직선
			if (shape.shapeType == DrawMode::DCrossHair)
			{
				memDC.MoveTo(shape.RotatePts[0].x, shape.RotatePts[0].y);
				memDC.LineTo(shape.RotatePts[1].x, shape.RotatePts[1].y);
				memDC.MoveTo(shape.RotatePts[2].x, shape.RotatePts[2].y);
				memDC.LineTo(shape.RotatePts[3].x, shape.RotatePts[3].y);
			}
			else if (shape.shapeType == DrawMode::DEllipse)
			{
				MyEllipseR(&memDC, shape.Center, shape.radin[0], shape.radin[1], shape.R_theta, RGB(0, 255, 0));
			}
			//else if (shape.shapeType != DrawMode::DEllipse) 
			else
			{
				memDC.MoveTo(data[SelectIndex].RotatePts[0].x, data[SelectIndex].RotatePts[0].y);
				for (int i = 1; i < data[SelectIndex].RotatePts.size(); i++)
					memDC.LineTo(data[SelectIndex].RotatePts[i].x, data[SelectIndex].RotatePts[i].y);
				memDC.LineTo(data[SelectIndex].RotatePts[0].x, data[SelectIndex].RotatePts[0].y);
			}

		}
		memDC.SelectObject(&oldPen);
	}
	DeleteObject(pen);
	DeleteObject(oldPen);
}

void CChildView::OnLine()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	drawStyle = DrawMode::DLine;
	drawID = TRUE;
}

void CChildView::OnRectangle()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	drawStyle = DrawMode::DRectangle;
	drawID = TRUE;
}

void CChildView::OnTriangle()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	drawStyle = DrawMode::DTriangle;
	drawID = TRUE;
}

void CChildView::OnEllipse()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	drawStyle = DrawMode::DEllipse;
	drawID = TRUE;
}

void CChildView::OnCrosshair()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	drawStyle = DrawMode::DCrossHair;
	drawID = TRUE;
}

bool CChildView::isInside(CPoint& pt, vector<Point2d>& pts) 
{
	//crosses는 점q와 오른쪽 반직선과 다각형과의 교점의 개수
	Point temp;
	int crosses = 0;
	for (int i = 0; i < pts.size(); i++) {
		int j = (i + 1) % pts.size();
		//점 B가 선분 (p[i], p[j])의 y좌표 사이에 있음
		if ((pts[i].y > pt.y) != (pts[j].y > pt.y)) {
			//atX는 점 B를 지나는 수평선과 선분 (p[i], p[j])의 교점
			double atX = (pts[j].x - pts[i].x) * (pt.y - pts[i].y) / (pts[j].y - pts[i].y) + pts[i].x;
			//atX가 오른쪽 반직선과의 교점이 맞으면 교점의 개수를 증가시킨다.
			if (pt.x < atX)
				crosses++;
		}
	}
	return crosses % 2 > 0;
}

void CChildView::MyEllipseR(CDC* parm_dc, Point2d Center, int radinX, int radinY,double theta ,COLORREF parm_color)
{
	// 직선 알고리즘 추가
	CPoint pt;
	CPoint Rotatept;
	pt.x = Center.x + int(sin(0 * CV_PI / 180) * radinX);
	pt.y = Center.y + int(cos(0 * CV_PI / 180) * radinY);
	Rotatept.x = cvRound((pt.x - Center.x) * cos(theta) - (pt.y - Center.y) * sin(theta) + Center.x);
	Rotatept.y = cvRound((pt.x - Center.x) * sin(theta) + (pt.y - Center.y) * cos(theta) + Center.y);
	parm_dc->SetPixel(Rotatept, parm_color);
	for (int i = 1; i < 360; i++) {
		parm_dc->MoveTo(Rotatept);
		pt.x = Center.x + int(sin(i * CV_PI / 180) * radinX);
		pt.y = Center.y + int(cos(i * CV_PI / 180) * radinY);

		Rotatept.x = cvRound((pt.x - Center.x) * cos(theta) - (pt.y - Center.y) * sin(theta) + Center.x);
		Rotatept.y = cvRound((pt.x - Center.x) * sin(theta) + (pt.y - Center.y) * cos(theta) + Center.y);

		parm_dc->LineTo(Rotatept);
		parm_dc->SetPixel(Rotatept, parm_color);
	}
}

void CChildView::MyEllipsePS_DOT(CDC* parm_dc, Point2d Center, int radinX, int radinY, double theta)
{
	// 직선 알고리즘 추가
	int cnt = 0;
	CPoint pt;
	CPoint Rotatept;
	CPen* pen1 = new CPen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* pen2 = new CPen(PS_SOLID, 1, RGB(255, 255, 255));
	CPen* oldPen;
	oldPen = memDC.SelectObject(pen1);
	memDC.SelectStockObject(NULL_BRUSH);
	memDC.SelectObject(&oldPen);


	pt.x = Center.x + int(sin(0 * CV_PI / 180) * radinX);
	pt.y = Center.y + int(cos(0 * CV_PI / 180) * radinY);
	Rotatept.x = cvRound((pt.x - Center.x) * cos(theta) - (pt.y - Center.y) * sin(theta) + Center.x);
	Rotatept.y = cvRound((pt.x - Center.x) * sin(theta) + (pt.y - Center.y) * cos(theta) + Center.y);
	parm_dc->SetPixel(Rotatept, RGB(0,0,0));
	for (int i = 1; i < 360; i++) {
		if (i % 5 == 0)
			cnt++;
		if (cnt % 2 == 0) {
			parm_dc->MoveTo(Rotatept);
			pt.x = Center.x + int(sin(i * CV_PI / 180) * radinX);
			pt.y = Center.y + int(cos(i * CV_PI / 180) * radinY);

			Rotatept.x = cvRound((pt.x - Center.x) * cos(theta) - (pt.y - Center.y) * sin(theta) + Center.x);
			Rotatept.y = cvRound((pt.x - Center.x) * sin(theta) + (pt.y - Center.y) * cos(theta) + Center.y);

			oldPen = memDC.SelectObject(pen1);
			memDC.SelectStockObject(NULL_BRUSH);
			parm_dc->LineTo(Rotatept);
			memDC.SelectObject(&oldPen);
			parm_dc->SetPixel(Rotatept, RGB(0, 0, 0));
		}
		else
		{
			parm_dc->MoveTo(Rotatept);
			pt.x = Center.x + int(sin(i * CV_PI / 180) * radinX);
			pt.y = Center.y + int(cos(i * CV_PI / 180) * radinY);

			Rotatept.x = cvRound((pt.x - Center.x) * cos(theta) - (pt.y - Center.y) * sin(theta) + Center.x);
			Rotatept.y = cvRound((pt.x - Center.x) * sin(theta) + (pt.y - Center.y) * cos(theta) + Center.y);

			oldPen = memDC.SelectObject(pen2);
			memDC.SelectStockObject(NULL_BRUSH);
			parm_dc->LineTo(Rotatept);
			memDC.SelectObject(&oldPen);
			parm_dc->SetPixel(Rotatept, RGB(255, 255, 255));
		}
	}
	DeleteObject(pen1);
	DeleteObject(pen2);
	DeleteObject(oldPen);
}

Point2d CChildView::Intersection(Point2d& pt, Point2d& LinePt1, Point2d& LinePt2)
{
	Point2d result_2d;
	Point2d result;
	double A, B, t_A, t_B;
	if ((LinePt1.x - LinePt2.x) != 0 && LinePt1.y - LinePt2.y != 0) {
		A = (LinePt1.y - LinePt2.y) / (LinePt1.x - LinePt2.x);
		B = LinePt1.y - A * LinePt1.x;
		t_A = -1 / A;
		t_B = pt.y - t_A * pt.x;

		result_2d.x = (t_B - B) / (A - t_A);
		result_2d.y = result_2d.x * A + B;
		result.x = result_2d.x;
		result.y = result_2d.y;
		return result;
	}
	else if(LinePt1.x - LinePt2.x == 0){
		result.x = LinePt1.x;
		result.y = pt.y;
		return result;
	}
	else if (LinePt1.y - LinePt2.y == 0) {
		result.x = pt.x;
		result.y = LinePt1.y;
		return result;
	}
}

void CChildView:: SelectShapeRectUpdate()
{
	if (chooseID == FALSE)
		return;
	Point2d pt;
	RectCount = data[SelectIndex].pts.size();
	for (int i = 0; i < RectCount; i++)
	{
		pt = data[SelectIndex].pts[i];
		rRect[i].SetRect(pt.x - 5, pt.y - 5, pt.x + 5, pt.y + 5);
	}
}

void CChildView::SelectShapeUpdate()
{
	if (chooseID == FALSE)
		return;

	//  중심 업데이트
	Point2d pt(0,0);
	RectCount = data[SelectIndex].pts.size();
	for (int i = 0; i < RectCount; i++)
		pt += data[SelectIndex].pts[i];

	data[SelectIndex].Center = pt / RectCount;
	data[SelectIndex].Rotate = data[SelectIndex].Center;
	data[SelectIndex].Rotate.y -= 15;
	this->Center = data[SelectIndex].Center;
	this->Rotate = data[SelectIndex].Rotate;

	// 사각 업데이트
	for (int i = 0; i < RectCount; i++)
	{
		pt = data[SelectIndex].pts[i];
		rRect[i].SetRect(pt.x - 5, pt.y - 5, pt.x + 5, pt.y + 5);
	}

	// 각도 업데이트
	data[SelectIndex].RotatePts = data[SelectIndex].pts;
	data[SelectIndex].R_theta = data[SelectIndex].theta;

	if (data[SelectIndex].shapeType == DrawMode::DEllipse)
	{
		pt = data[SelectIndex].pts[0] - data[SelectIndex].pts[1];
		data[SelectIndex].radin[0] = sqrt(pow(pt.x, 2) + pow(pt.y, 2))/2;
		pt = data[SelectIndex].pts[1] - data[SelectIndex].pts[2];
		data[SelectIndex].radin[1] = sqrt(pow(pt.x, 2) + pow(pt.y, 2))/2;
	}

}

void CChildView::SelectShpaeCenterUpdate()
{
	if (chooseID == FALSE)
		return;

	Point2d pt(0, 0);
	RectCount = data[SelectIndex].pts.size();
	for (int i = 0; i < data[SelectIndex].pts.size(); i++)
		pt += data[SelectIndex].pts[i];

	data[SelectIndex].Center = pt / RectCount;
	data[SelectIndex].Rotate = data[SelectIndex].Center;
	data[SelectIndex].Rotate.y -= 15;
	this->Center = data[SelectIndex].Center;
	this->Rotate = data[SelectIndex].Rotate;

}
