﻿// ChildView.cpp: CChildView 클래스의 구현
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

bool IsContain(CRect rc, Point pt) {
		if (pt.x >= rc.left && pt.x <= rc.right) {
			if (pt.y >= rc.top && pt.y <= rc.bottom) {
				return true;
			}
		}
	
	return false;
}
int isLeft(Point P0, Point P1, Point P2)
{
	return ((P1.x - P0.x) * (P2.y - P0.y) - (P2.x - P0.x) * (P1.y - P0.y));
}
int isContainPolygon(Point P, Point2f* V, int n)
{
	int wn = 0;

	for (int i = 0; i < n; i++) { 
		int nextpos = (i + 1 >= n) ? 0 : i + 1;
		if (V[i].y <= P.y) {      
			if (V[nextpos].y > P.y)
				if (isLeft(V[i], V[nextpos], P) > 0)
					++wn;            
		}
		else {                        
			if (V[nextpos].y <= P.y)     
				if (isLeft(V[i], V[nextpos], P) < 0)  
					--wn;            
		}
	}
	return wn;
}

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



BOOL CChildView::OnEraseBkgnd(CDC* pDC) 
{
	
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
	
	//if (MK_LBUTTON && index !=-1) 
	if (MK_LBUTTON && choiceShape!=nullptr)
	{
		//if (data[index].isClicked)
		if (choiceShape->isClicked)
		{
			int o, p;
			int xx, yy;
			//CPoint TLX;
			xx = x, yy = y;
			x = z_pos.x + (m_ePt.x + point.x) / PWidth;
			y = z_pos.y + (m_ePt.y + point.y) / PHeight;

			o = (x - xx);
			p = (y - yy);

			/*data[index].rect.right += o;
			data[index].rect.left += o;
			data[index].rect.top += p;
			data[index].rect.bottom += p;*/
			choiceShape->rect.right += o;
			choiceShape->rect.left += o;
			choiceShape->rect.top += p;
			choiceShape->rect.bottom += p;

			InvalidateRect(choiceShape->rect);
		}

	}
	else if (MK_LBUTTON && drawID == true) // 마우스를 클릭하여 드래그일 동안만 
	{
		pts.x = (z_pos.x + (m_ePt.x + point.x) / PWidth);
		pts.y = (z_pos.y + (m_ePt.y + point.y) / PHeight);
		
		ePt.x = point.x;
		ePt.y = point.y;
		draw(pts); // 뷰에서 도형 그리기(draw) 함수 호출		
		// 마우스 드래그시의 좌표 값을 도형 끝 값에 다시 저장	

		
		mov_x = (z_pos.x + (m_ePt.x + pts.x) / PWidth) ;
		mov_y = (z_pos.y + (m_ePt.y + pts.y) / PHeight) ;
		//Invalidate();
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
					_T("맑은고딕")           // 글꼴
				);
				pDC->SelectObject(cFont);
				if (R == G == B)
				{
					strNum.Format(_T("%d\n%d\n%d"), R, G, B);
					DrawTextEx(pDC, strNum, rc, DT_VCENTER | DT_CENTER);
				}
				else
				{
					strNum.Format(_T("%d"), pixelvale);

					pDC->DrawText(strNum, rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				}
				
				pDC->SelectObject(oldBrush);
				DeleteObject(myBrush);
				DeleteObject(cFont);
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
	pFrame->imageList[idx].setImage();
	m_background.Attach(pFrame->imageList[idx].getImage());
	m_background.GetBitmap(&m_Bitmap);

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
	panID = false;

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

	CWnd::OnLButtonDown(nFlags, point);
	SetCapture();
	x = z_pos.x + (m_ePt.x + point.x) / PWidth;
	y = z_pos.y + (m_ePt.y + point.y) / PHeight; 

	sPt.x = point.x;
	sPt.y = point.y;

	drawID = true; // 그리기 시작을 알리는 변수		

	CPoint cp;
	
	for (int i = 0; i < data.GetSize(); i++)
	{
		cp.x = z_pos.x + (m_ePt.x + point.x) / PWidth;
		cp.y = z_pos.y + (m_ePt.y + point.y) / PHeight;

		if (data[i].shapeType != DrawMode::DLine)
		{
			if (IsContain(data[i].rect, Point(cp.x, cp.y)))
			{
				drawID = false;
				panID = true;
				choiceShape = &data[i];
				data[i].isClicked = true;
				MyShape test = data[i];
				data.InsertAt(0, test);
				data.RemoveAt(i + 1);
				choiceShape = &data[0];
				choiceShape->isClicked = true;
				index = i;
				break;
			}
		}
		else {
			vector < cv::Point> pt;
			pt.push_back(Point(data[i].rect.left + 10, data[i].rect.top - 10));
			pt.push_back(Point(data[i].rect.right - 10, data[i].rect.bottom - 10));
			pt.push_back(Point(data[i].rect.left - 10, data[i].rect.top + 10));
			pt.push_back(Point(data[i].rect.right + 10, data[i].rect.bottom + 10));
			POINT po[4] = { {data[i].rect.left - 10, data[i].rect.top + 10} ,{data[i].rect.left + 10, data[i].rect.top - 10},{data[i].rect.right + 10, data[i].rect.bottom - 10 },{data[i].rect.right - 10, data[i].rect.bottom + 10} };

			RotatedRect rt = minAreaRect(pt);
			Point2f test[4];
			rt.points(test);
			//vector<Point2f> ss;
			/*for (int i = 0; i < 4; i++) {
				ss.push_back(test[i]);
			}*/

			int flag = isContainPolygon(Point(cp.x, cp.y), test, 4);
			if (flag != 0) {
				drawID = false;
				panID = true;
				data[i].isClicked = true;
				data.InsertAt(0, data[i]);
				data.RemoveAt(i + 1);
				choiceShape = &data[0];
				index = i;
				CDC* dc = GetDC();
				HBRUSH myBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
				HBRUSH oldBrush = (HBRUSH)dc->SelectObject(myBrush);
				dc->Polygon(po, 4);
				dc->SelectObject(oldBrush);
				DeleteObject(myBrush);
				dc->DeleteDC();
				break;
			}
		}
	}
}


void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CWnd::OnLButtonUp(nFlags, point);
	if (drawID)  // 도형 그리기 상태일 동안만
	{
		mov_x = (z_pos.x + (m_ePt.x + point.x) / PWidth) + 1;
		mov_y = (z_pos.y + (m_ePt.y + point.y) / PHeight) + 1;

		shape.shapeType = drawStyle;  // 도형 콤보 상자에서 선택된 도형 스타일을 저장		
		shape.penWidth = 1;
		shape.rect.left = x; // 도형 시작좌표 저장
		shape.rect.top = y;
		shape.rect.right = mov_x; // 도형 끝 좌표 저장
		shape.rect.bottom = mov_y;
		shape.fgColor = RGB(0, 0, 0);
		
		//저장된 shape를 배열에 저장
		data.Add(shape);
		
		drawID = false;
		InvalidateRect(shape.rect);
	}

	if (panID)
	{
		//if (index != -1) {
		if (choiceShape!=nullptr) {
			//if (data[index].isClicked)
			if (choiceShape->isClicked)
			{
				/*data[index].isClicked = false;
				data[index].fgColor = RGB(255, 0, 0);*/
				choiceShape->isClicked = false;
				choiceShape = nullptr;
				panID == false;
				//InvalidateRect(data[index].rect);
				InvalidateRect(choiceShape->rect);
				index = -1;
			}

		}
	}
	ReleaseCapture();
}

int CChildView::draw(CPoint point)
{
	//CClientDC dc(this);
	
	//CMainFrame* cmr = (CMainFrame*)AfxGetMainWnd();
	//CPen myPen, * oldPen;

	CDC* dc = GetDC();
	CPen myPen(PS_SOLID, PWidth, RGB(255, 255, 255));
	CPen* oldPen;

	HBRUSH myBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldBrush = (HBRUSH)dc->SelectObject(myBrush);

	oldPen = dc->SelectObject(&myPen);
	mov_x = (z_pos.x + (m_ePt.x + point.x) / PWidth) + 1;
	mov_y = (z_pos.y + (m_ePt.y + point.y) / PHeight) + 1;
	if (drawStyle == DrawMode::DLine) // 콤보상자에서 직선 선택시
	{
		dc->SetROP2(R2_BLACK); // 픽셀인 펜 색 조합 및 반전 화면 색상 
		dc->MoveTo(sPt.x, sPt.y); dc->LineTo(ePt.x, ePt.y);
		printf("Draw x: %d y: %d\n", x, y);
	}
	else if (drawStyle == DrawMode::DEllipse) // 콤보상자에서 원 선택시
	{
		dc->SetROP2(R2_BLACK); // 픽셀인 펜 색 조합 및 반전 화면 색상 
		printf("원 movx: %d movy: %d\n", mov_x, mov_y);
		printf("원 x: %d y: %d\n", x, y);
		dc->Ellipse(sPt.x, sPt.y, ePt.x, ePt.y);
	}
	else if (drawStyle == DrawMode::DRectangle) // 콤보상자에서 사각형 선택시
	{
		dc->SetROP2(R2_BLACK); // 픽셀인 펜 색 조합 및 반전 화면 색상 
		printf("사각형 movx: %d movy: %d\n", mov_x, mov_y);
		printf("사각형 x: %d y: %d\n", x, y);
		dc->Rectangle(sPt.x, sPt.y, ePt.x, ePt.y);
	}

	dc->SelectObject(oldPen); // 이전 팬 선택		
	myPen.DeleteObject();  // 생성한 펜 메모리에서 제거		
	dc->SelectObject(oldBrush);
	DeleteObject(myBrush);
		
	CRect rc(sPt.x, sPt.y, mov_x+1, mov_y+1);
	InvalidateRect(rc);
	return x, y, mov_x, mov_y;
}

void CChildView::drawShape(int shapeNum, int penWd, int sx, int sy, int ex, int ey, COLORREF fgcolor)
{
	//CBrush myBrush, * oldBrush;
	CPen myPen(PS_SOLID, penWd, fgcolor);
	CPen* oldPen;
	oldPen = memDC.SelectObject(&myPen);

	HBRUSH myBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldBrush = (HBRUSH)memDC.SelectObject(myBrush);
	if (shapeNum == 1)	// 직선 선택시
	{
		memDC.MoveTo(sx, sy); memDC.LineTo(ex, ey);
		printf("Shape x: %d y: %d\n", sx, sy);
		
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
}


void CChildView::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//---------------------------------------------------
	CDC* pDC = GetDC();

	m_background.Detach();
	m_background.Attach(::CopyImage(pFrame->imageList[idx].getImage(), IMAGE_BITMAP, 0, 0, 0));
	memDC.CreateCompatibleDC(pDC);
	mdcOffScreen.CreateCompatibleDC(pDC);

	bmpOffScreen.CreateCompatibleBitmap(pDC, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CXFULLSCREEN));
	
	
	oldbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);
	oldbitmap2 = memDC.SelectObject(&m_background);


	
	for (int i = 0; i < data.GetSize(); i++)
	{
		drawShape(data[i].shapeType, data[i].penWidth,
			data[i].rect.left, data[i].rect.top,
			data[i].rect.right, data[i].rect.bottom, data[i].fgColor);
	}

	mdcOffScreen.SetStretchBltMode(COLORONCOLOR);
	mdcOffScreen.StretchBlt(0, 0, PWidth * ((int)zoomWidth + 2), PHeight * ((int)zoomHeight + 2), &memDC,
		z_pos.x, z_pos.y, zoomWidth + 2, zoomHeight + 2, SRCCOPY);
	
	PrintText(&mdcOffScreen);

	pDC->SetStretchBltMode(COLORONCOLOR);
	pDC->StretchBlt(0, 0, m_bgRect.right, m_bgRect.bottom, &mdcOffScreen,
		m_ePt.x, m_ePt.y, m_bgRect.right, m_bgRect.bottom, SRCCOPY);

	memDC.SelectObject(oldbitmap2);
	memDC.DeleteDC();
	mdcOffScreen.SelectObject(oldbitmap);
	mdcOffScreen.DeleteDC();
	bmpOffScreen.DeleteObject();
}