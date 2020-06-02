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
	//ON_WM_KEYDOWN()
	ON_COMMAND(ID_LINE, &CChildView::OnLine)
	ON_COMMAND(ID_RECTANGLE, &CChildView::OnRectangle)
	ON_COMMAND(ID_PointRect, &CChildView::OnEllipse)
	ON_COMMAND(ID_ELLPSE, &CChildView::OnEllipse)
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

	for (int i = 0; i < data.GetSize(); i++)
		drawShape(data[i]);
	if(chooseID==TRUE)
		SelectDrawShape(data[chosenShape]);

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

// Zoom + Panning ------------------------------------------------------------------------------
void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnMouseMove(nFlags, point);
	m_pos = point;
	R_pos = point;
	int i = 0;
	if (nFlags & MK_MBUTTON)
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
		Invalidate();
		UpdateWindow();
	}

	// 그림
	if (MK_LBUTTON && drawID == TRUE) // 마우스를 클릭하여 드래그일 동안만 
	{
		draw(&memDC, point); // 뷰에서 도형 그리기(draw) 함수 호출		
		// 마우스 드래그시의 좌표 값을 도형 끝 값에 다시 저장	
		mov_x = z_pos.x + (m_ePt.x + point.x) / viewWidth;
		mov_y = z_pos.y + (m_ePt.y + point.y) / viewHeight;

		//Invalidate(FALSE);
	}

	// 크기 조정
	if (MK_LBUTTON && (EdgeChosen == TRUE) && rotateClick == FALSE)
	{
		int o, p;
		int xx, yy;
		//CPoint TLX;
		xx = x, yy = y;
		x = z_pos.x + (m_ePt.x + point.x) / viewWidth;
		y = z_pos.y + (m_ePt.y + point.y) / viewHeight;

		o = (x - xx);
		p = (y - yy);

		RectCount = data[chosenShape].pts.size();
		if (RectCount == 4) {
			if (chosenEdge == 0)
			{
				data[chosenShape].pts[0].x += o;
				data[chosenShape].pts[0].y += p;
				data[chosenShape].pts[1].y += p;
				data[chosenShape].pts[3].x += o;
			}
			else if (chosenEdge == 1)
			{
				data[chosenShape].pts[1].x += o;
				data[chosenShape].pts[1].y += p;
				data[chosenShape].pts[0].y += p;
				data[chosenShape].pts[2].x += o;
			}
			else if (chosenEdge == 2)
			{
				data[chosenShape].pts[2].x += o;
				data[chosenShape].pts[2].y += p;
				data[chosenShape].pts[3].y += p;
				data[chosenShape].pts[1].x += o;
			}
			else if (chosenEdge == 3)
			{
				data[chosenShape].pts[3].x += o;
				data[chosenShape].pts[3].y += p;
				data[chosenShape].pts[2].y += p;
				data[chosenShape].pts[0].x += o;
			}
		}
		else
		{
			if (chosenEdge == 0)
			{
				data[chosenShape].pts[0].x += o;
				data[chosenShape].pts[0].y += p;
			}
			else if (chosenEdge == 1)
			{
				data[chosenShape].pts[1].x += o;
				data[chosenShape].pts[1].y += p;
			}
		}

		Point pt(0, 0);
		for (int i = 0; i < RectCount; i++)
		{
			pt.x += data[chosenShape].pts[i].x;
			pt.y += data[chosenShape].pts[i].y;
		}

		Center.x = data[chosenShape].Center.x = pt.x / RectCount;
		Center.y = data[chosenShape].Center.y = pt.y / RectCount;
		Rotate.x = data[chosenShape].Rotate.x = pt.x / RectCount;
		Rotate.y = data[chosenShape].Rotate.y = pt.y / RectCount - 15;

		for (int i = 0; i < RectCount; i++)
		{
			pt.x = data[chosenShape].pts[i].x;
			pt.y = data[chosenShape].pts[i].y;
			rRect[i].SetRect(pt.x - 5, pt.y - 5, pt.x + 5, pt.y + 5);
		}
		//Invalidate(FALSE);
	}
	
	// 이동
	if (MK_LBUTTON && (panID == TRUE) && (rotateClick==FALSE))
	{
		int o, p;
		int xx, yy;
		//CPoint TLX;
		xx = x, yy = y;
		x = z_pos.x + (m_ePt.x + point.x) / viewWidth;
		y = z_pos.y + (m_ePt.y + point.y) / viewHeight;

		o = (x - xx);
		p = (y - yy);

		Point pt;
		RectCount = data[chosenShape].pts.size();
		for (int i = 0; i < RectCount; i++)
		{
			pt.x = data[chosenShape].pts[i].x += o;
			pt.y = data[chosenShape].pts[i].y += p;
			rRect[i].SetRect(pt.x - 5, pt.y - 5, pt.x + 5, pt.y + 5);
		}
		pt.x = 0; pt.y = 0;
		for (int i = 0; i < RectCount; i++)
		{
			pt.x += data[chosenShape].pts[i].x;
			pt.y += data[chosenShape].pts[i].y;
		}

		data[chosenShape].Center = Point(pt.x / RectCount, pt.y / RectCount);
		data[chosenShape].Rotate = Point(pt.x / RectCount, pt.y / RectCount - 15);

		Center = data[chosenShape].Center;
		Rotate = data[chosenShape].Rotate;

		//Invalidate(FALSE);
		//UpdateWindow();
	}

	// 회전 클릭
	if (MK_LBUTTON && (rotateClick == TRUE))
	{
		x = z_pos.x + (m_ePt.x + point.x) / viewWidth;
		y = z_pos.y + (m_ePt.y + point.y) / viewHeight;
		Point cneter = data[chosenShape].Center;
		Point Rotate = data[chosenShape].Rotate;
		Point pt;
		vector<Point> pts = data[chosenShape].pts;
		double theta;

		if ((y - cneter.y) == 0)
			theta = CV_PI/2 - atan((((float)Rotate.x - cneter.x) / ((float)Rotate.y - cneter.y)));
		else if ((Rotate.y - cneter.y) == 0)
			theta = atan(((float)x - cneter.x) / ((float)y - cneter.y)) - CV_PI / 2;
		else
			theta = atan((((float)x - cneter.x) / ((float)y - cneter.y))) - atan((((float)Rotate.x - cneter.x) / (float)(Rotate.y - cneter.y)));
		printf("\n theta = %d\n", theta);

		if (data[chosenShape].shapeType == DrawMode::DLine)
		{
			for (int i = 0; i < 2; i++)
			{
				pt.x = cvRound((pts[i].x - cneter.x) * cos(theta) + (pts[i].y - cneter.y) * sin(theta) + cneter.x);
				pt.y = cvRound(-(pts[i].x - cneter.x) * sin(theta) + (pts[i].y - cneter.y) * cos(theta) + cneter.y);
				pts[i] = pt;
			}
			data[chosenShape].RotatePts = pts;
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				pt.x = cvRound((pts[i].x - cneter.x) * cos(theta) + (pts[i].y - cneter.y) * sin(theta) + cneter.x);
				pt.y = cvRound(-(pts[i].x - cneter.x) * sin(theta) + (pts[i].y - cneter.y) * cos(theta) + cneter.y);
				pts[i] = pt;
			}
			data[chosenShape].RotatePts = pts;
		}
		//Invalidate(FALSE);
	}
	Invalidate(FALSE);
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_bgRect.left = 0;
	m_bgRect.top = 0;
	m_bgRect.right = cx;
	m_bgRect.bottom = cy;

	Invalidate();
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
	Invalidate();

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

// 그리기-------------------------------------------------------------------------------
void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	chooseID = FALSE; // 채크해제후 검사
	SetCapture();
	x = z_pos.x + (m_ePt.x + point.x) / viewWidth;
	y = z_pos.y + (m_ePt.y + point.y) / viewHeight;
	// mov_x, mov_y는 도형 그리기 끝(이전)값
	mov_x = x;
	mov_y = y;
	//printf("도형 개수 : %d\n", data.GetSize());
	
	// 엣지 클릭 검사
	for (int i = 0; i < RectCount; i++)
	{
		if (x >= rRect[i].left && x <= rRect[i].right && y >= rRect[i].top && y <= rRect[i].bottom)
		{
			EdgeChosen = TRUE;
			chosenEdge = i;
			break;
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
	}
	// 이미지 객체 클릭 검사
	for (int i = data.GetSize() - 1; i >= 0; i--)
	{
		if (data[i].shapeType != DrawMode::DLine) {
			if (isInside(Point(x, y), data[i].pts))
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
				Center = data[i].Center;
				Rotate = data[i].Rotate;

				panID = TRUE;
				chooseID = TRUE;
				chosenShape = i;
				Invalidate(FALSE);
				break;
			}
		}
		else if(data[i].shapeType == DrawMode::DLine)
		{
			vector<Point> linePts;
			int Pt1, Pt2;
			Pt1 = pow(data[i].pts[0].x, 2) + pow(data[i].pts[0].y, 2);
			Pt2 = pow(data[i].pts[1].x, 2) + pow(data[i].pts[1].y, 2);
			if (Pt1 > Pt2)
			{
				Point temp = data[i].pts[0];
				data[i].pts[0] = data[i].pts[1];
				data[i].pts[1] = temp;
			}
			linePts.clear();
			linePts.push_back(Point(data[i].pts[0].x-5, data[i].pts[0].y-5));
			linePts.push_back(Point(data[i].pts[0].x+5, data[i].pts[0].y-5));
			linePts.push_back(Point(data[i].pts[1].x+5, data[i].pts[1].y+5));
			linePts.push_back(Point(data[i].pts[1].x-5, data[i].pts[1].y+5));
			if (isInside(Point(x, y), linePts))
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
				Center = data[i].Center;
				Rotate = data[i].Rotate;

				panID = TRUE;
				chooseID = TRUE;
				chosenShape = i;
				Invalidate(FALSE);
				break;
			}
		}

	}

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
			shape.pts.push_back(Point(x, y));
			shape.pts.push_back(Point(mov_x, mov_y));

			// !추가 - 객체 생성 정보추가
			shape.Center = Point((x + mov_x) / 2, (y + mov_y) / 2);
			shape.Rotate = Point((x + mov_x) / 2, (y + mov_y) / 2 - 15);
		}
		else if (shape.shapeType == DrawMode::DRectangle)
		{
			shape.pts.push_back(Point(x, y));
			shape.pts.push_back(Point(mov_x, y));
			shape.pts.push_back(Point(mov_x, mov_y));
			shape.pts.push_back(Point(x, mov_y));

			// !추가 - 객체 생성 정보추가
			shape.Center = Point((x + mov_x) / 2, (y + mov_y) / 2);
			shape.Rotate = Point((x + mov_x) / 2, (y + mov_y) / 2 - 15);
		}
		else if (shape.shapeType == DrawMode::DEllipse)
		{
			shape.pts.push_back(Point(x, y));
			shape.pts.push_back(Point(mov_x, y));
			shape.pts.push_back(Point(mov_x, mov_y));
			shape.pts.push_back(Point(x, mov_y));

			// !추가 - 객체 생성 정보추가
			shape.Center = Point((x + mov_x) / 2, (y + mov_y) / 2);
			shape.Rotate = Point((x + mov_x) / 2, (y + mov_y) / 2 - 15);
		}

		//저장된 shape를 배열에 저장
		data.Add(shape);
	}
	if (rotateClick == TRUE)
	{
		data[chosenShape].pts;
		data[chosenShape].RotatePts;
	}

	ReleaseCapture();
	drawID = FALSE;
	panID = FALSE;
	EdgeChosen = FALSE;
	rotateClick = FALSE; 
	Invalidate(FALSE);

	CWnd::OnLButtonUp(nFlags, point);
}

int CChildView::draw(CDC* pDC, CPoint point)
{
	CDC* dc = GetDC();
	CMainFrame* cmr = (CMainFrame*)AfxGetMainWnd();

	CPen myPen(PS_SOLID, viewWidth, RGB(255, 255, 255));
	CPen* oldPen;

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
	else if (drawStyle == DrawMode::DRectangle) // 콤보상자에서 사각형 선택시
	{
		dc->SetROP2(R2_NOTXORPEN); // 픽셀인 펜 색 조합 및 반전 화면 색상 
		dc->MoveTo(x, y);
		dc->LineTo(mov_x, y);
		dc->LineTo(mov_x, mov_y);
		dc->LineTo(x, mov_y);
		dc->LineTo(x, y);

		mov_x = m_pos.x;
		mov_y = m_pos.y;

		dc->MoveTo(x, y);
		dc->LineTo(mov_x, y);
		dc->LineTo(mov_x, mov_y);
		dc->LineTo(x, mov_y);
		dc->LineTo(x, y);
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
	return 0;
}

void CChildView::drawShape(MyShape& shape)
{
	CDC* dc= GetDC();
	int shapeNum = shape.shapeType;
	CBrush brush;
	brush.CreateStockObject(NULL_BRUSH);
	CBrush* pOldBrush = memDC.SelectObject(&brush);
	
	if (shapeNum == DrawMode::DLine) // 콤보상자에서 직선 선택시
	{
		memDC.MoveTo(shape.pts[0].x, shape.pts[0].y);
		memDC.LineTo(shape.pts[1].x, shape.pts[1].y);
	}
	else if (shapeNum == DrawMode::DEllipse) // 콤보상자에서 원 선택시
	{
		long aa,bb,cc;
		long a = sqrt(pow(shape.pts[0].x - shape.pts[1].x, 2) + pow(shape.pts[0].y - shape.pts[1].y, 2)) / 2;
		long b = sqrt(pow(shape.pts[0].x - shape.pts[3].x, 2) + pow(shape.pts[0].y - shape.pts[3].y, 2)) / 2;
		long c = abs(sqrt(pow(a, 2) - pow(b, 2)));

		// 검색 범위 설정
		Point MaxPt(shape.pts[0].x, shape.pts[0].y);
		Point MinPt(shape.pts[0].x, shape.pts[0].y);
		for (int i = 1; i < shape.pts.size(); i++)
		{
			if (MaxPt.x < shape.pts[i].x)
				MaxPt.x = shape.pts[i].x;

			if (MaxPt.y < shape.pts[i].y)
				MaxPt.y = shape.pts[i].y;

			if (MinPt.x > shape.pts[i].x)
				MinPt.x = shape.pts[i].x;

			if (MinPt.y > shape.pts[i].y)
				MinPt.y = shape.pts[i].y;
		}

		if (a > b)
		{
			Point pt1 = shape.Center, pt2 = shape.Center;
			pt1.x -= c;
			pt2.x += c;

			for (int i = MinPt.y; i <= MaxPt.y; i++)
			{
				for (int j = MinPt.x; j <= MaxPt.x; j++)
				{
					aa = sqrt((pow(pt1.x - j, 2) + pow(pt1.y - i, 2)));
					bb = sqrt((pow(pt2.x - j, 2) + pow(pt2.y - i, 2)));
					cc = sqrt((pow(pt1.x - pt2.x, 2) + pow(pt1.y - pt2.y, 2)));
					if (a*2 == aa + bb)
						memDC.SetPixel(CPoint(j, i), RGB(255, 0, 0));
				}
			}
		}
		else
		{
			Point pt1 = shape.Center, pt2 = shape.Center;
			pt1.y -= c;
			pt2.y += c;

			for (int i = MinPt.y; i <= MaxPt.y; i++)
			{
				for (int j = MinPt.x; j <= MaxPt.x; j++)
				{
					aa = sqrt((pow(pt1.x - j, 2) + pow(pt1.y - i, 2)));
					bb = sqrt((pow(pt2.x - j, 2) + pow(pt2.y - i, 2)));
					cc = sqrt((pow(pt1.x - pt2.x, 2) + pow(pt1.y - pt2.y, 2)));
					if (b*2 == (aa + bb))
						memDC.SetPixel(CPoint(j, i), RGB(255, 0, 0));
				}
			}
		}
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

	memDC.SelectObject(pOldBrush);
	DeleteObject(brush);
}

void CChildView::SelectDrawShape(MyShape& shape)
{
	CDC* dc = GetDC();
	int shapeNum = shape.shapeType;

	CPen* oldPen;
	memDC.SelectObject(&oldPen);
	CPen pen1(PS_DOT, 1, BLACK_PEN);
	oldPen = memDC.SelectObject(&pen1);
	memDC.SelectStockObject(NULL_BRUSH);
	if (data[chosenShape].shapeType == DrawMode::DLine) // 콤보상자에서 직선 선택시
	{
		memDC.MoveTo(shape.pts[0].x, shape.pts[0].y);
		memDC.LineTo(shape.pts[1].x, shape.pts[1].y);
	}
	else if (data[chosenShape].shapeType == DrawMode::DRectangle) // 콤보상자에서 사각형 선택시
	{
		// 화면에 사각형을 그린다.
		memDC.MoveTo(shape.pts[0].x, shape.pts[0].y);
		memDC.LineTo(shape.pts[1].x, shape.pts[1].y);
		memDC.LineTo(shape.pts[2].x, shape.pts[2].y);
		memDC.LineTo(shape.pts[3].x, shape.pts[3].y);
		memDC.LineTo(shape.pts[0].x, shape.pts[0].y);
	}
	else if (data[chosenShape].shapeType == DrawMode::DEllipse)
	{
		long xx, yy;
		long a = sqrt(pow(shape.pts[0].x - shape.pts[1].x, 2) + pow(shape.pts[0].y - shape.pts[1].y, 2)) / 2;
		long b = sqrt(pow(shape.pts[0].x - shape.pts[3].x, 2) + pow(shape.pts[0].y - shape.pts[3].y, 2)) / 2;
		long temp = min(a, b);
		printf("\n a = %d b = %d\n", a, b);
		a = max(a, b);
		b = temp;
		a = pow(a, 2);
		b = pow(b, 2);
		printf("\n aa = %d bb = %d\n", a, b);
		// 검색 범위 설정
		Point MaxPt(shape.pts[0].x, shape.pts[0].y);
		Point MinPt(shape.pts[0].x, shape.pts[0].y);
		for (int i = 1; i < shape.pts.size(); i++)
		{
			if (MaxPt.x < shape.pts[i].x)
				MaxPt.x = shape.pts[i].x;
			if (MaxPt.y < shape.pts[i].y)
				MaxPt.y = shape.pts[i].y;

			if (MinPt.x > shape.pts[i].x)
				MinPt.x = shape.pts[i].x;
			if (MinPt.y > shape.pts[i].y)
				MinPt.y = shape.pts[i].y;
		}
		printf("\n min y = %d  x = %d\n", MinPt.y, MinPt.x);
		printf("\n min y = %d  x = %d\n", MaxPt.y, MaxPt.x);
		printf("\n center y = %d  x = %d\n", shape.Center.y, shape.Center.x);
		for (int i = 0; i < 4; i++)
		{
			printf("\n %d y = %d  x = %d\n", i, shape.pts[i].y, shape.pts[i].x);
		}
		Point center = shape.Center;
		for (int i = MinPt.y; i < MaxPt.y; i++)
		{
			yy = pow(i - center.y, 2);
			for (int j = MinPt.x; j < MaxPt.x; j++)
			{
				xx = pow(j - center.x, 2);
				if (((xx / a) + (yy / b)) == 1)
					memDC.SetPixel(CPoint(j, i), RGB(0, 0, 0));
			}
		}
	}

	memDC.SelectObject(&oldPen);

	// 확대 원그리기
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
	oldPen = memDC.SelectObject(&pen);
	memDC.SelectStockObject(WHITE_BRUSH);
	for (int i = 0; i < RectCount; i++)
		memDC.Rectangle(rRect[i]);

	// !추가  회전 직선 그리기
	CPen pen3(PS_SOLID, 1, RGB(0, 0, 255));
	oldPen = memDC.SelectObject(&pen3);
	memDC.SelectStockObject(PS_SOLID);
	memDC.MoveTo(Center.x, Center.y);
	memDC.LineTo(Rotate.x, Rotate.y);

	// 회전 선택
	if (rotateClick == TRUE) {
		CPen pen4(PS_SOLID, 1, RGB(0, 255, 0));
		oldPen = memDC.SelectObject(&pen4);
		memDC.SelectStockObject(PS_SOLID);
		// 시작
		RectCount = data[chosenShape].pts.size();
		if (data[chosenShape].RotatePts.size() > 0)
		{
			memDC.MoveTo(data[chosenShape].RotatePts[0].x, data[chosenShape].RotatePts[0].y);
			for (int i = 1; i < data[chosenShape].RotatePts.size(); i++)
				memDC.LineTo(data[chosenShape].RotatePts[i].x, data[chosenShape].RotatePts[i].y);
			memDC.LineTo(data[chosenShape].RotatePts[0].x, data[chosenShape].RotatePts[0].y);
			Rotate.x = x;
			Rotate.y = y;
		}
	}
	memDC.SelectObject(&oldPen);
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

void CChildView::OnEllipse()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	drawStyle = DrawMode::DEllipse;
	drawID = TRUE;
}

bool CChildView::isInside(Point& pt, vector<Point>& pts) 
{
	//crosses는 점q와 오른쪽 반직선과 다각형과의 교점의 개수
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

