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
	/*if (!image.IsNull())
		image.Destroy();
	image.Load(theApp.sFilename);

	m_background.Attach(image.Detach());
	m_background.GetObject(sizeof(BITMAP), (LPVOID)&m_Bitmap);

	rectWidth = m_Bitmap.bmWidth;
	rectHeight = m_Bitmap.bmHeight;

	m_ePt.x = 0;
	m_ePt.y = 0;

	z_pos.x = 1;
	z_pos.y = 1;

	ViewScale = 1;
	ViewValue = 1;

	viewWidth = 0;
	viewHeight = 0;

	m_bkgBrush.CreateSolidBrush(0x00000000);*/
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_POINT, &CChildView::OnPoint)
	ON_COMMAND(ID_LINE, &CChildView::OnLine)
	ON_COMMAND(ID_ELLPSE, &CChildView::OnEllpse)
	ON_COMMAND(ID_RECTANGLE, &CChildView::OnRectangle)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
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

	memDC.SelectObject(&m_background);

	
	for (int i = 0; i < data.GetSize(); i++)
	{
		drawShape(data[i].shapeType, data[i].penWidth, 
			data[i].rect.left, data[i].rect.top, 
			data[i].rect.right, data[i].rect.bottom);
	}

	

	mdcOffScreen.SetStretchBltMode(COLORONCOLOR);
	mdcOffScreen.StretchBlt(0, 0, PWidth * ((int)zoomWidth+2), PHeight * ((int)zoomHeight+2), &memDC,
		z_pos.x, z_pos.y, zoomWidth+2, zoomHeight+2, SRCCOPY);

	PrintText(&mdcOffScreen);

	pDC->SetStretchBltMode(COLORONCOLOR);
	pDC->StretchBlt(0, 0, m_bgRect.right, m_bgRect.bottom, &mdcOffScreen,
		m_ePt.x, m_ePt.y, m_bgRect.right, m_bgRect.bottom, SRCCOPY);

	//draw(pDC, m_pos);
	memDC.DeleteDC();
	mdcOffScreen.SelectObject(oldbitmap);
	mdcOffScreen.DeleteDC();
	bmpOffScreen.DeleteObject();
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
	int i = 0;

	if (nFlags & MK_MBUTTON)
	{
		// A영역의 사각형 그리기 //m_sPt는 기존 포인트, point는 이동
		if (m_sPt.x < point.x) // 오른쪽으로 끌었을때
		{
			m_ePt.x -= point.x - m_sPt.x;

			if (m_ePt.x <= 0)
			{
				if (z_pos.x > 0)
				{
					i = 1 + ((0 - m_ePt.x) / (int)PWidth); //원본에서 움직여야되는 픽셀 수
					z_pos.x -= i;
					m_ePt.x += i * PWidth;
				}
			}
		}
		else // 왼쪽으로 끌었을때
		{
			m_ePt.x += m_sPt.x - point.x;

			if (m_ePt.x + m_bgRect.right - (m_Bitmap.bmWidth + PWidth) >= 0)
			{
				i = 1 + (m_ePt.x + m_bgRect.right - (m_Bitmap.bmWidth + PWidth)) / (int)PWidth;
				z_pos.x += i;
				m_ePt.x -= i * PWidth;
			}
		}
		// B영역의 사각형 그리기
		if (m_sPt.y > point.y) // 위로 올렸을때
		{
			m_ePt.y += m_sPt.y - point.y;

			if (m_ePt.y + m_bgRect.bottom - (m_Bitmap.bmHeight + PHeight) >= 0)
			{
				i = 1 + (m_ePt.y + m_bgRect.bottom - m_Bitmap.bmHeight + PHeight) / (int)PHeight;
				z_pos.y += i;
				m_ePt.y -= i * PHeight;
			}

		}
		else // 아래로 내렸을때
		{
			m_ePt.y -= point.y - m_sPt.y;
			if (m_ePt.y < 0)
			{
				i = 1 + ((0 - m_ePt.y) / (int)PHeight);
				z_pos.y -= i;
				m_ePt.y += i * PHeight;
			}
		}

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
		pts.x = (z_pos.x + (m_ePt.x + point.x) / PWidth)+1;
		pts.y = (z_pos.y + (m_ePt.y + point.y) / PHeight)+1;
		draw(pts); // 뷰에서 도형 그리기(draw) 함수 호출		
		// 마우스 드래그시의 좌표 값을 도형 끝 값에 다시 저장	
		//Invalidate();
		
		mov_x = (z_pos.x + (m_ePt.x + point.x) / PWidth) + 1;
		mov_y = (z_pos.y + (m_ePt.y + point.y) / PHeight) + 1;

		Invalidate();
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

	Invalidate();
}


BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (zDelta <= 0)// 휠 내릴때
	{
		m_Zoom = 1.2f;
	}
	else// 휠 올릴때
	{
		m_Zoom = 0.75f;
	}

	z_pos.x += round(((((float)m_ePt.x + (float)m_pos.x) / (float)(PWidth * ((int)zoomWidth + 2))) * zoomWidth) - ((((float)m_ePt.x + (float)m_pos.x) / (float)(PWidth * ((int)zoomWidth + 2))) * (zoomWidth * m_Zoom)));
	z_pos.y += round(((((float)m_ePt.y + (float)m_pos.y) / (float)(PHeight * ((int)zoomHeight + 2))) * zoomHeight) - ((((float)m_ePt.y + (float)m_pos.y) / (float)(PHeight * ((int)zoomHeight + 2))) * (zoomHeight * m_Zoom)));

	

	if (m_ePt.x + m_bgRect.right > m_Bitmap.bmWidth + PWidth)
	{
		z_pos.x += 1;
		m_ePt.x -= PWidth;
	}

	if (m_ePt.y + m_bgRect.bottom > m_Bitmap.bmHeight + PHeight)
	{
		z_pos.y += 1;
		m_ePt.y -= PHeight;
	}

	zoomWidth *= m_Zoom;
	zoomHeight *= m_Zoom;

	PWidth = m_Bitmap.bmWidth / zoomWidth;
	PHeight = m_Bitmap.bmHeight / zoomHeight;


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

//void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
//
//	CWnd::OnLButtonDown(nFlags, point);
//	CDC* dc = GetDC();
//
//	COLORREF rgb;
//	rgb = dc->GetPixel(point.x, point.y);
//
//	int R = 0, G = 0, B = 0;
//
//	R = GetRValue(rgb);
//	G = GetGValue(rgb);
//	B = GetBValue(rgb);
//
//	int pixelvale = (R + G + B) / 3.0;
//	CString strNum;
//	strNum.Format(_T("%d"), pixelvale);
//	dc->TextOutW(point.x, point.y, strNum);
//
//	printf("x: %d, y: %d\n", point.x, point.y);
//}

void CChildView::PrintText(CDC* pDC)
{
	COLORREF rgb;
	int a = 0, b = 0;
	
	if (PWidth >= 65)
	{
		for (int i = z_pos.x; i < z_pos.x + zoomWidth; i++)
		{
			for (int j = z_pos.y; j < z_pos.y + zoomHeight; j++)
			{
				rgb = memDC.GetPixel(i, j);
				int R = 0, G = 0, B = 0;

				R = GetRValue(rgb);
				G = GetGValue(rgb);
				B = GetBValue(rgb);

				int pixelvale = (R + G + B) / 3.0;



				pDC->SetBkMode(TRANSPARENT);
				//pDC->SetTextAlign(TA_CENTER);

				if (pixelvale >= 185)
					pDC->SetTextColor(0x00000000);
				else
					pDC->SetTextColor(RGB(255, 255, 255));

				HBRUSH myBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
				HBRUSH oldBrush = (HBRUSH)pDC->SelectObject(myBrush);
				CRect rc(a * PWidth, b * PHeight, a * PWidth + PWidth, b * PHeight + PHeight);

				pDC->Rectangle(rc);

				CString strNum;

				CFont cFont;
				cFont.CreateFontW(PHeight / 5,                     // 글자높이
					PWidth / 5,                     // 글자너비
					0,                      // 출력각도
					0,                      // 기준 선에서의각도
					FW_NORMAL,              // 글자굵기
					FALSE,                  // Italic 적용여부
					FALSE,                  // 밑줄적용여부
					FALSE,                  // 취소선적용여부
					DEFAULT_CHARSET,       // 문자셋종류
					OUT_DEFAULT_PRECIS,    // 출력정밀도
					CLIP_DEFAULT_PRECIS,   // 클리핑정밀도
					DEFAULT_QUALITY,       // 출력문자품질
					DEFAULT_PITCH,         // 글꼴Pitch
					_T("")           // 글꼴
				);

				pDC->SelectObject(cFont);

				if (R == G == B)
				{
					strNum.Format(_T("%d\n%d\n%d"), R, G, B);
					DrawTextEx(pDC, strNum, rc, DT_VCENTER | DT_CENTER);
					//pDC->DrawText(strNum, rc, DT_CENTER | DT_END_ELLIPSIS | DT_WORD_ELLIPSIS);
				}
				else
				{
					strNum.Format(_T("%d"), pixelvale);

					pDC->DrawText(strNum, rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				}
							

				
				
				pDC->SelectObject(oldBrush);
				DeleteObject(myBrush);
				DeleteObject(cFont);
				//pDC->TextOutW(i, j, strNum);
				b++;
			}
			a++;
			b = 0;
		}
	}
}

void CChildView::DrawTextEx(CDC* pDC, const CString& str, CRect rect, UINT nFormat)
{
	int    nLineCount = 0;

	if (str.GetLength())
	{
		nLineCount = 1;
		int nPos = -1;

		while ((nPos = str.Find('\n', nPos + 1)) > 0)
			nLineCount++;
	}

	CSize    szText = pDC->GetTextExtent(str);
	szText.cy = szText.cy * nLineCount;

	// 프린트시 위아래(음수값)가 바뀔수 있음..
	int    nHeight = rect.Height();
	if (nHeight < 0)
		nHeight = nHeight + szText.cy;
	else
		nHeight = nHeight - szText.cy;

	rect.top = rect.top + (nHeight / 2);

	pDC->DrawText(str, rect, nFormat);
}


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

	zoomWidth = m_Bitmap.bmWidth;
	zoomHeight = m_Bitmap.bmHeight;

	m_ePt.x = 0;
	m_ePt.y = 0;

	z_pos.x = 1;
	z_pos.y = 1;

	m_Zoom = 1;

	PWidth = m_Bitmap.bmWidth / zoomWidth;
	PHeight = m_Bitmap.bmHeight / zoomHeight;

	drawID = false;


	m_bkgBrush.CreateSolidBrush(0x00000000);

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


void CChildView::OnPoint()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	drawStyle = DrawMode::DPoint;
}


void CChildView::OnLine()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	drawStyle = DrawMode::DLine;
}


void CChildView::OnEllpse()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	drawStyle = DrawMode::DEllipse;
}


void CChildView::OnRectangle()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	drawStyle = DrawMode::DRectangle;
}


void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	SetCapture();
	x = z_pos.x + (m_ePt.x + point.x) / PWidth;
	y = z_pos.y + (m_ePt.y + point.y) / PHeight;
	// mov_x, mov_y는 도형 그리기 끝(이전)값
	printf("sibal == %d %d\n", x, y);
	mov_x = x; mov_y = y;
	drawID = TRUE; // 그리기 시작을 알리는 변수		


	CWnd::OnLButtonDown(nFlags, point);
}


void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (drawID)  // 도형 그리기 상태일 동안만
	{
		mov_x = (z_pos.x + (m_ePt.x + point.x) / PWidth) + 1;
		mov_y = (z_pos.y + (m_ePt.y + point.y) / PHeight) + 1;
		//printf("sex == %d %d\n", mov_x, mov_y);
		// 마우스 버트을 놓으면 각종 값을 shape 구조체 맴버변수에 저장
		shape.shapeType = drawStyle;  // 도형 콤보 상자에서 선택된 도형 스타일을 저장		
		shape.penWidth = PWidth;
		shape.rect.left = x; // 도형 시작좌표 저장
		shape.rect.top = y;
		shape.rect.right = mov_x; // 도형 끝 좌표 저장
		shape.rect.bottom = mov_y;

		//저장된 shape를 배열에 저장
		data.Add(shape);
	}
	drawID = FALSE;

	CWnd::OnLButtonUp(nFlags, point);
	ReleaseCapture();
	Invalidate();
}

int CChildView::draw(CPoint point)
{
	//CClientDC dc(this);
	CDC* dc = GetDC();
	//CMainFrame* cmr = (CMainFrame*)AfxGetMainWnd();
	//CPen myPen, * oldPen;


	CPen myPen(PS_SOLID, PWidth, RGB(255, 255, 255));
	CPen* oldPen;

	HBRUSH myBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldBrush = (HBRUSH)dc->SelectObject(myBrush);

	oldPen = dc->SelectObject(&myPen);

	if (drawStyle == DrawMode::DLine) // 콤보상자에서 직선 선택시
	{
		dc->SetROP2(R2_BLACK); // 픽셀인 펜 색 조합 및 반전 화면 색상 
		
		dc->MoveTo(x, y); dc->LineTo(mov_x, mov_y); //전단계를 다시 그려서 흰색으로 만듬
		mov_x = (z_pos.x + (m_ePt.x + point.x) / PWidth);
		mov_y = (z_pos.y + (m_ePt.y + point.y) / PHeight);
		printf("joj %d %d\n", mov_x, mov_y);
		printf("joj %d %d\n", x, y);
		dc->MoveTo(x, y); dc->LineTo(mov_x, mov_y);
	}
	else if (drawStyle == DrawMode::DEllipse) // 콤보상자에서 원 선택시
	{
		//dc->SetROP2(R2_XORPEN); // 픽셀인 펜 색 조합 및 반전 화면 색상 
		dc->Ellipse(x, y, mov_x, mov_y); //전단계를 다시 그려서 흰색으로 만듬
		mov_x = (z_pos.x + (m_ePt.x + point.x) / PWidth) + 1;
		mov_y = (z_pos.y + (m_ePt.y + point.y) / PHeight) + 1;
		printf("joj %d %d\n", mov_x, mov_y);
		printf("joj %d %d\n", x, y);
		dc->Ellipse(x, y, mov_x, mov_y);
	}
	else if (drawStyle == DrawMode::DRectangle) // 콤보상자에서 사각형 선택시
	{
		//dc->SetROP2(R2_XORPEN); // 픽셀인 펜 색 조합 및 반전 화면 색상 
		dc->Rectangle(x, y, mov_x, mov_y); //전단계를 다시 그려서 흰색으로 만듬, 지워지는 효과
		mov_x = (z_pos.x + (m_ePt.x + point.x) / PWidth) + 1;
		mov_y = (z_pos.y + (m_ePt.y + point.y) / PHeight) + 1;
		printf("joj %d %d\n", mov_x, mov_y);
		printf("joj %d %d\n", x, y);
		dc->Rectangle(x, y, mov_x, mov_y);
	}

	dc->SelectObject(oldPen); // 이전 팬 선택		
	myPen.DeleteObject();  // 생성한 펜 메모리에서 제거		
	dc->SelectObject(oldBrush);
	DeleteObject(myBrush);

	//memDC.DeleteDC();
	//memDC.SelectObject(oldPen); // 이전 팬 선택		
	//myPen.DeleteObject();  // 생성한 펜 메모리에서 제거		
	//myBrush.DeleteObject();  // 생성한 브러쉬 메모리에서 제거		

	Invalidate();
	return x, y, mov_x, mov_y;
}

void CChildView::drawShape(int shapeNum, int penWd, int sx, int sy, int ex, int ey)
{
	//CClientDC dc(this);
	//CBrush brush;
	//brush.CreateSolidBrush(RGB(255, 255, 255));
	//CMainFrame* cmr = (CMainFrame*)AfxGetMainWnd();
	//CPen myPen, * oldPen;
	//CBrush myBrush, * oldBrush;

	HBRUSH myBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldBrush = (HBRUSH)memDC.SelectObject(myBrush);

	//oldPen = dc.SelectObject(&myPen);

	if (shapeNum == 1)	// 직선 선택시
	{
		memDC.MoveTo(sx, sy); memDC.LineTo(ex, ey);
	}
	else if (shapeNum == 2) // 원 선택시
	{
		memDC.Ellipse(sx, sy, ex, ey);
	}
	else if (shapeNum == 3) // 사각형 선택시
	{
		// 화면에 사각형을 그린다.
		memDC.Rectangle(sx, sy, ex, ey);
	}

	memDC.SelectObject(oldBrush);
	DeleteObject(myBrush);
	
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