﻿// ChildView.cpp: CChildView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "Fourthlohgic_Project.h"
#include "ChildView.h"
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console");
#define IsCTRLPressed()  ( 0x8000 ==(GetKeyState(VK_CONTROL) & 0x8000 ))

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CFourthlohgicProjectApp theApp;
// CChildView

bool IsContain(CRect rc, Point pt) {
	int left, right, top, bottom;
	if (rc.Width() > 0) {
		left = rc.left;
		right = rc.right;
	}
	else {
		left = rc.right;
		right = rc.left;
	}
	if (rc.Height() > 0) {
		top = rc.top;
		bottom = rc.bottom;
	}
	else {
		top = rc.bottom;
		bottom = rc.top;
	}
	return left <= pt.x && pt.x <= right && top <= pt.y && pt.y <= bottom;
}
int isLeft(CPoint linePt1, CPoint linePt2, CPoint pos)
{
	return ((linePt2.x - linePt1.x) * (pos.y - linePt1.y) - (pos.x - linePt1.x) * (linePt2.y - linePt1.y));
}
int isContainPolygon(CPoint pos, CPoint* vertices, int size)
{
	int wideNum = 0;

	for (int i = 0; i < size; i++) {
		int nextpos = (i + 1 >= size) ? 0 : i + 1;
		if (vertices[i].y <= pos.y) {
			if (vertices[nextpos].y > pos.y)
				if (isLeft(vertices[i], vertices[nextpos], pos) > 0)
					++wideNum;
		}
		else {
			if (vertices[nextpos].y <= pos.y)
				if (isLeft(vertices[i], vertices[nextpos], pos) < 0)
					--wideNum;
		}
	}
	return wideNum;
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
	ON_COMMAND(ID_DRAWID, &CChildView::OnDrawid)
	ON_COMMAND(ID_PANID, &CChildView::OnPanid)
	ON_COMMAND(ID_LWIDTH1, &CChildView::OnLwidth1)
	ON_COMMAND(ID_LWIDTH2, &CChildView::OnLwidth2)
	ON_COMMAND(ID_32797, &CChildView::On32797)
	ON_COMMAND(ID_32798, &CChildView::On32798)
	ON_COMMAND(ID_32799, &CChildView::On32799)
	ON_COMMAND(ID_32800, &CChildView::On32800)
	ON_COMMAND(ID_COPY, &CChildView::OnCopy)
	ON_COMMAND(ID_PASTE, &CChildView::OnPaste)
	ON_COMMAND(ID_DELETE, &CChildView::OnDelete)
	ON_COMMAND(ID_LINECOLOR, &CChildView::OnLinecolor)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()



// CChildView 메시지 처리기

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

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
	//패닝
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

	if (choiceIdx != -1) {
		//도형 이동
		if (m_lbtn && panID && resize == 0)
		{
			if (data[choiceIdx].isClicked)
			{
				int xx, yy;
				//CPoint TLX;
				xx = d_sPt.x, yy = d_sPt.y;
				d_sPt.x = z_pos.x + (m_ePt.x + point.x) / PWidth;
				d_sPt.y = z_pos.y + (m_ePt.y + point.y) / PHeight;


				//if (data[choiceIdx].rect.left <= x && x <= data[choiceIdx].rect.right && data[choiceIdx].rect.top <= y && y <= data[choiceIdx].rect.bottom)
				{
					//if (data[choiceIdx].shapeType != DrawMode::DLine)
					{
						CPoint pt;
						pt.x = (d_sPt.x - xx);
						pt.y = (d_sPt.y - yy);

						data[choiceIdx].rect.right += pt.x;
						data[choiceIdx].rect.left += pt.x;
						data[choiceIdx].rect.top += pt.y;
						data[choiceIdx].rect.bottom += pt.y;

						//(pt);
					}
				}
				InvalidateRect(data[choiceIdx].rect);
			}
		}
		//도형 크기 변경
		else if ((m_lbtn && panID && resize != 0) || (m_lbtn && drawID && resize != 0))
		{
			CPoint pt;
			int xx, yy;
			
			xx = d_sPt.x, yy = d_sPt.y;
			d_sPt.x = z_pos.x + (m_ePt.x + point.x) / PWidth;
			d_sPt.y = z_pos.y + (m_ePt.y + point.y) / PHeight;
			if (data[choiceIdx].isClicked) {
				switch (resize)
				{
				case 1: {
					//if (data[choiceIdx].shapeType != DrawMode::DLine)
					{
						pt.x = (d_sPt.x - xx);
						pt.y = (d_sPt.y - yy);

						data[choiceIdx].rect.left += pt.x;

						data[choiceIdx].rect.top += pt.y;

						//draw(pt);
					}
					break;
				}
				case 2: {
					//if (data[choiceIdx].shapeType != DrawMode::DLine)
					{
						pt.x = (d_sPt.x - xx);
						pt.y = (d_sPt.y - yy);

						data[choiceIdx].rect.right += pt.x;
						data[choiceIdx].rect.top += pt.y;

						//draw(pt);
					}
					break;
				}
				case 3: {
					//if (data[choiceIdx].shapeType != DrawMode::DLine)
					{
						pt.x = (d_sPt.x - xx);
						pt.y = (d_sPt.y - yy);

						data[choiceIdx].rect.left += pt.x;
						data[choiceIdx].rect.bottom += pt.y;


						//draw(pt);
					}
					break;
				}
				case 4: {
					//if (data[choiceIdx].shapeType != DrawMode::DLine)
					{
						pt.x = (d_sPt.x - xx);
						pt.y = (d_sPt.y - yy);

						data[choiceIdx].rect.right += pt.x;
						data[choiceIdx].rect.bottom += pt.y;


						//draw(pt);
					}
					break;
				}
				default:
					break;
				}
				InvalidateRect(data[choiceIdx].rect);
			}
		}
	}
	//도형 그리기
	if (m_lbtn && drawID && resize == 0) // 마우스를 클릭하여 드래그일 동안만 
	{
		if (choiceIdx != -1)
			data[choiceIdx].isClicked = false;
		pts.x = (z_pos.x + (m_ePt.x + point.x) / PWidth);
		pts.y = (z_pos.y + (m_ePt.y + point.y) / PHeight);

		ePt.x = point.x;
		ePt.y = point.y;

		CRect rc(sPt.x, sPt.y, mov_Pt.x + 1, mov_Pt.y + 1);
		Invalidate();
		draw(); // 뷰에서 도형 그리기(draw) 함수 호출		
		// 마우스 드래그시의 좌표 값을 도형 끝 값에 다시 저장	

		mov_Pt.x = (z_pos.x + (m_ePt.x + pts.x) / PWidth);
		mov_Pt.y = (z_pos.y + (m_ePt.y + pts.y) / PHeight);
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
	drawID = true;
	panID = false;
	l_width = 1;
	idx = 0;
	rollbackIndex = -1;
	choiceIdx = -1;
	m_lbtn = false;
	resize = 0;
	ctrl = false;
	iscopy = false;



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
	
	return 0;
}


void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (IsCTRLPressed())
		cout << "컨트롤";
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
	case VK_DELETE: {
		if (choiceIdx != -1)
		{
			RollbackInfo info;
			info.idx = choiceIdx;
			info.undoShape = data[choiceIdx];
			info.rollbackmode = RollBackMode::Delete;
			if (rollbackIndex != -1)
				rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
			else if (rollback.size() != 0) 
				rollback.erase(rollback.begin(), rollback.end());
			rollback.push_back(info);
			rollbackIndex = rollback.size() - 1;
			data.RemoveAt(choiceIdx);
			InvalidateRect(rollback[rollbackIndex].undoShape.rect);
			choiceIdx = -1;
			break;
		}
		return;
	}
	case VK_CONTROL: {
		if (rollbackIndex + 1 < rollback.size()/* && rollbackIndex - 1 >= 0*/) {
			switch (rollback[++rollbackIndex].rollbackmode)
			{
			case RollBackMode::Create: {
				data.Add(rollback[rollbackIndex].undoShape);
				break;
			}
			case RollBackMode::Delete: {

				int idx = -1;
				for (int i = 0; i < data.GetSize(); i++) {
					if (data[i].shapeType == rollback[rollbackIndex].undoShape.shapeType
						&& data[i].rect == rollback[rollbackIndex].undoShape.rect)
					{
						idx = i;
						break;
					}
				}
				data.RemoveAt(idx);
				break;
			}
			case RollBackMode::Update: {
				if (choiceIdx != -1)
					data[choiceIdx].isClicked = false;
				data[rollback[rollbackIndex].idx] = rollback[rollbackIndex].redoShape;
				choiceIdx = rollback[rollbackIndex].idx;
				data[choiceIdx].isClicked = true;
				break;
			}
			default:
				break;
			}
			Invalidate();
		}
		return;
	}
	case 0x5A: {
		if (rollbackIndex != -1/* && rollbackIndex - 1 >= 0*/) {
			switch (rollback[rollbackIndex].rollbackmode)
			{
			case RollBackMode::Create: {
				data.RemoveAt(rollback[rollbackIndex].idx);
				for (int i = 0; i < zOrder.size(); i++) {
					if (zOrder[i] == rollback[rollbackIndex].idx)
						zOrder.erase(zOrder.begin() + i);
				}
				rollbackIndex--;
				choiceIdx = -1;
				Invalidate();
				break;
			}
			case RollBackMode::Delete: {
				data.Add(rollback[rollbackIndex--].undoShape);
				zOrder.insert(zOrder.begin(), data.GetSize() - 1);
				break;
			}
			case RollBackMode::Update: {
				if(choiceIdx!=-1)
				data[choiceIdx].isClicked = false;
				data[rollback[rollbackIndex].idx] = rollback[rollbackIndex].undoShape;
				choiceIdx = rollback[rollbackIndex].idx;
				data[choiceIdx].isClicked = true;
				rollbackIndex--;
				break;
			}
			default:
				return;
			}
			Invalidate();

		}
		return;
	}
	default:
		return;
	}
	cout << idx << endl;
	pFrame->imageList[idx].setImage();
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

	m_lbtn = true;

	d_sPt.x = z_pos.x + (m_ePt.x + point.x) / PWidth;
	d_sPt.y = z_pos.y + (m_ePt.y + point.y) / PHeight;

	sPt.x = point.x;
	sPt.y = point.y;

	//drawID = true; // 그리기 시작을 알리는 변수		
	CPoint cp;
	if (panID || (drawID && choiceIdx != -1))
	{
		if (mRect[0].left <= d_sPt.x && d_sPt.x <= mRect[0].right && mRect[0].top <= d_sPt.y && d_sPt.y <= mRect[0].bottom)
		{
			//왼쪽 위
			resize = 1;
		}
		else if (mRect[2].left <= d_sPt.x && d_sPt.x <= mRect[2].right && mRect[2].top <= d_sPt.y && d_sPt.y <= mRect[2].bottom)
		{
			//오른쪽 위
			resize = 2;
		}
		else if (mRect[3].left <= d_sPt.x && d_sPt.x <= mRect[3].right && mRect[3].top <= d_sPt.y && d_sPt.y <= mRect[3].bottom)
		{
			//왼쪽 아래
			resize = 3;
		}
		else if (mRect[1].left <= d_sPt.x && d_sPt.x <= mRect[1].right && mRect[1].top <= d_sPt.y && d_sPt.y <= mRect[1].bottom)
		{
			//오른쪽 아래
			resize = 4;
		}
		else if (panID)
		{
			if (choiceIdx != -1)
				data[choiceIdx].isClicked = false;

			//resize = 0;
			for (int i = 0; i < data.GetSize(); i++)
			{
				cp.x = z_pos.x + (m_ePt.x + point.x) / PWidth;
				cp.y = z_pos.y + (m_ePt.y + point.y) / PHeight;

				choiceIdx = -1;
				if (data[zOrder[i]].shapeType != DrawMode::DLine)
				{
					if (IsContain(data[zOrder[i]].rect, Point(cp.x, cp.y)))
					{
						drawID = false;
						panID = true;
						data[zOrder[i]].isClicked = true;
						temp = data[zOrder[i]];
						zOrder.insert(zOrder.begin(), zOrder[i]);
						zOrder.erase(zOrder.begin() + i + 1);
						choiceIdx = zOrder[0];
						data[choiceIdx].isClicked = true;
						break;
					}
				}
				else {
					CPoint po[4];
					double a = (data[zOrder[i]].rect.bottom - data[zOrder[i]].rect.top * 1.0) / (data[zOrder[i]].rect.right - data[zOrder[i]].rect.left * 1.0);
					double b = data[zOrder[i]].rect.top - a * data[zOrder[i]].rect.left;
					po[0] = CPoint(data[zOrder[i]].rect.left, data[zOrder[i]].rect.left * a + b - 10);
					po[1] = CPoint(data[zOrder[i]].rect.right, data[zOrder[i]].rect.right * a + b - 10);
					po[2] = CPoint(data[zOrder[i]].rect.right, data[zOrder[i]].rect.right * a + b + 10);
					po[3] = CPoint(data[zOrder[i]].rect.left, data[zOrder[i]].rect.left * a + b + 10);
					int flag = isContainPolygon(CPoint(cp.x, cp.y), po, 4);
					if (flag != 0) {
						drawID = false;
						panID = true;
						data[zOrder[i]].isClicked = true;
						temp = data[zOrder[i]];
						zOrder.insert(zOrder.begin(), zOrder[i]);
						zOrder.erase(zOrder.begin() + i + 1);
						choiceIdx = zOrder[0];
						break;
					}

				}
			}
		}
	}
	Invalidate();
}


void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CWnd::OnLButtonUp(nFlags, point);
	CMainFrame* frame = (CMainFrame*)AfxGetMainWnd();
	if (m_lbtn && drawID && resize == 0)  // 도형 그리기 상태일 동안만
	{

		mov_Pt.x = (z_pos.x + (m_ePt.x + point.x) / PWidth) + 1;
		mov_Pt.y = (z_pos.y + (m_ePt.y + point.y) / PHeight) + 1;

		shape.shapeType = drawStyle;  // 도형 콤보 상자에서 선택된 도형 스타일을 저장		
		shape.penWidth = l_width;
		shape.rect.left = d_sPt.x; // 도형 시작좌표 저장
		shape.rect.top = d_sPt.y;
		shape.rect.right = mov_Pt.x; // 도형 끝 좌표 저장
		shape.rect.bottom = mov_Pt.y;
		shape.fgColor = frame->color;
		shape.isClicked = true;

		//저장된 shape를 배열에 저장
		data.Add(shape);
		zOrder.insert(zOrder.begin(), data.GetSize() - 1);
		choiceIdx = zOrder[0];

		RollbackInfo info;
		info.idx = choiceIdx;
		info.redoShape = data[choiceIdx];
		info.undoShape = shape;
		info.rollbackmode = RollBackMode::Create;
		if (rollbackIndex != -1)
			rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
		else if (rollback.size() != 0) {
			rollback.erase(rollback.begin(), rollback.end());
		}
		rollback.push_back(info);
		rollbackIndex = rollback.size() - 1;

		InvalidateRect(shape.rect);
	}
	else if (panID)
	{
		if (choiceIdx != -1) {
			if (data[choiceIdx].isClicked)
			{
				RollbackInfo info;
				info.idx = choiceIdx;
				info.redoShape = data[choiceIdx];
				info.undoShape = temp;
				info.rollbackmode = RollBackMode::Update;
				if (rollbackIndex != -1)
					rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
				rollback.push_back(info);
				rollbackIndex = rollback.size() - 1;
				InvalidateRect(data[choiceIdx].rect);
			}
		}
	}
	m_lbtn = false;
	resize = 0;
	ReleaseCapture();
}

int CChildView::draw()
{
	//CClientDC dc(this);
	CMainFrame* frame = (CMainFrame*)AfxGetMainWnd();
	CDC* dc = GetDC();
	CPen myPen(PS_SOLID, PWidth * l_width, frame->color);
	CPen* oldPen;

	HBRUSH myBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldBrush = (HBRUSH)dc->SelectObject(myBrush);

	oldPen = dc->SelectObject(&myPen);

	if (drawStyle == DrawMode::DLine) // 콤보상자에서 직선 선택시
	{
		dc->MoveTo(sPt.x, sPt.y); dc->LineTo(ePt.x, ePt.y);
	}
	else if (drawStyle == DrawMode::DEllipse) // 콤보상자에서 원 선택시
	{
		dc->Ellipse(sPt.x, sPt.y, ePt.x, ePt.y);
	}
	else if (drawStyle == DrawMode::DRectangle) // 콤보상자에서 사각형 선택시
	{
		dc->Rectangle(sPt.x, sPt.y, ePt.x, ePt.y);
	}

	dc->SelectObject(oldPen); // 이전 팬 선택		
	myPen.DeleteObject();  // 생성한 펜 메모리에서 제거		
	dc->SelectObject(oldBrush);
	DeleteObject(myBrush);


	return d_sPt.x, d_sPt.y, mov_Pt.x, mov_Pt.y;
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
	CBitmap* oldbitmap, * oldbitmap2;	//원본 오브젝트 저장

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
	if (choiceIdx != -1)
	{
		if (data[choiceIdx].isClicked)
		{
			CPen pen0(PS_DOT, 1, RGB(0, 0, 0));
			memDC.SelectObject(pen0);
			memDC.SelectStockObject(NULL_BRUSH);

			if (data[choiceIdx].shapeType != DrawMode::DLine)
			{
				
				int left = data[choiceIdx].rect.left - 10;
				int top = data[choiceIdx].rect.top - 10;
				int right = data[choiceIdx].rect.right + 10;
				int bottom = data[choiceIdx].rect.bottom + 10;
				memDC.Rectangle(left, top, right, bottom);

				mRect[0].SetRect(left - 10, top - 10,
					left + 10, top + 10);
				mRect[1].SetRect(right - 10, bottom - 10,
					right + 10, bottom + 10);
				mRect[2].SetRect(right - 10, top - 10,
					right + 10, top + 10);
				mRect[3].SetRect(left - 10, bottom - 10,
					left + 10, bottom + 10);

				CPen* oldPen;
				CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
				oldPen = memDC.SelectObject(&pen);
				memDC.SelectStockObject(WHITE_BRUSH);
				memDC.Ellipse(mRect[0]);
				memDC.Ellipse(mRect[1]);
				memDC.Ellipse(mRect[2]);
				memDC.Ellipse(mRect[3]);

			}
			else {
				CPoint po[4];
				double a = (data[choiceIdx].rect.bottom - data[choiceIdx].rect.top * 1.0) / (data[choiceIdx].rect.right - data[choiceIdx].rect.left * 1.0);
				double b = data[choiceIdx].rect.top - a * data[choiceIdx].rect.left;
				po[0] = CPoint(data[choiceIdx].rect.left, data[choiceIdx].rect.left * a + b - 10);
				po[1] = CPoint(data[choiceIdx].rect.right, data[choiceIdx].rect.right * a + b - 10);
				po[2] = CPoint(data[choiceIdx].rect.right, data[choiceIdx].rect.right * a + b + 10);
				po[3] = CPoint(data[choiceIdx].rect.left, data[choiceIdx].rect.left * a + b + 10);

				mRect[0].SetRect(data[choiceIdx].rect.left - 10, data[choiceIdx].rect.top - 10,
					data[choiceIdx].rect.left + 10, data[choiceIdx].rect.top + 10);
				mRect[1].SetRect(data[choiceIdx].rect.right - 10, data[choiceIdx].rect.bottom - 10,
					data[choiceIdx].rect.right + 10, data[choiceIdx].rect.bottom + 10);

				memDC.Polygon(po, 4);
				CPen* oldPen;
				CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
				oldPen = memDC.SelectObject(&pen);
				memDC.SelectStockObject(WHITE_BRUSH);
				memDC.Ellipse(mRect[0]);
				memDC.Ellipse(mRect[1]);				
			}
		}
	}

	mdcOffScreen.SetStretchBltMode(HALFTONE);
	mdcOffScreen.StretchBlt(0, 0, PWidth * ((int)zoomWidth + 2), PHeight * ((int)zoomHeight + 2), &memDC,
		z_pos.x, z_pos.y, zoomWidth + 2, zoomHeight + 2, SRCCOPY);

	PrintText(&mdcOffScreen);

	pDC->SetStretchBltMode(HALFTONE);
	pDC->StretchBlt(0, 0, m_bgRect.right, m_bgRect.bottom, &mdcOffScreen,
		m_ePt.x, m_ePt.y, m_bgRect.right, m_bgRect.bottom, SRCCOPY);

	memDC.SelectObject(oldbitmap2);
	memDC.DeleteDC();
	mdcOffScreen.SelectObject(oldbitmap);
	mdcOffScreen.DeleteDC();
	bmpOffScreen.DeleteObject();
	oldbitmap->DeleteObject();
	oldbitmap2->DeleteObject();
}

void CChildView::OnDrawid()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	drawID = true;
	panID = false;
	if (choiceIdx != -1) {
		data[choiceIdx].isClicked = false;
		InvalidateRect(data[choiceIdx].rect);
		choiceIdx = -1;

	}
}


void CChildView::OnPanid()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	drawID = false;
	panID = true;
}


void CChildView::OnLwidth1()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	l_width = 1;
	if (choiceIdx != -1)
	{
		RollbackInfo info;
		info.idx = choiceIdx;
		info.undoShape = data[choiceIdx];
		data[choiceIdx].penWidth = l_width;
		//deepCopy(info.updateShape, data[choiceIdx]);
		info.redoShape = data[choiceIdx];
		info.rollbackmode = RollBackMode::Update;
		if (rollbackIndex != -1)
			rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
		else if (rollback.size() != 0) {
			rollback.erase(rollback.begin(), rollback.end());
		}
		rollback.push_back(info);
		rollbackIndex = rollback.size() - 1;
		Invalidate();
	}
	
}


void CChildView::OnLwidth2()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	l_width = 2;
	if (choiceIdx != -1)
	{
		RollbackInfo info;
		info.idx = choiceIdx;
		info.undoShape = data[choiceIdx];
		data[choiceIdx].penWidth = l_width;
		//deepCopy(info.updateShape, data[choiceIdx]);
		info.redoShape = data[choiceIdx];
		info.rollbackmode = RollBackMode::Update;
		if (rollbackIndex != -1)
			rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
		else if (rollback.size() != 0) {
			rollback.erase(rollback.begin(), rollback.end());
		}
		rollback.push_back(info);
		rollbackIndex = rollback.size() - 1;
		Invalidate();
	}
}


void CChildView::On32797()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	l_width = 3;
	if (choiceIdx != -1)
	{
		RollbackInfo info;
		info.idx = choiceIdx;
		info.undoShape = data[choiceIdx];
		data[choiceIdx].penWidth = l_width;
		//deepCopy(info.updateShape, data[choiceIdx]);
		info.redoShape = data[choiceIdx];
		info.rollbackmode = RollBackMode::Update;
		if (rollbackIndex != -1)
			rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
		else if (rollback.size() != 0) {
			rollback.erase(rollback.begin(), rollback.end());
		}
		rollback.push_back(info);
		rollbackIndex = rollback.size() - 1;
		Invalidate();
	}
}


void CChildView::On32798()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	l_width = 4;
	if (choiceIdx != -1)
	{
		RollbackInfo info;
		info.idx = choiceIdx;
		info.undoShape = data[choiceIdx];
		data[choiceIdx].penWidth = l_width;
		//deepCopy(info.updateShape, data[choiceIdx]);
		info.redoShape = data[choiceIdx];
		info.rollbackmode = RollBackMode::Update;
		if (rollbackIndex != -1)
			rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
		else if (rollback.size() != 0) {
			rollback.erase(rollback.begin(), rollback.end());
		}
		rollback.push_back(info);
		rollbackIndex = rollback.size() - 1;
		Invalidate();
	}
}


void CChildView::On32799()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	l_width = 5;
	if (choiceIdx != -1)
	{
		RollbackInfo info;
		info.idx = choiceIdx;
		info.undoShape = data[choiceIdx];
		data[choiceIdx].penWidth = l_width;
		//deepCopy(info.updateShape, data[choiceIdx]);
		info.redoShape = data[choiceIdx];
		info.rollbackmode = RollBackMode::Update;
		if (rollbackIndex != -1)
			rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
		else if (rollback.size() != 0) {
			rollback.erase(rollback.begin(), rollback.end());
		}
		rollback.push_back(info);
		rollbackIndex = rollback.size() - 1;
		Invalidate();
	}
}


void CChildView::On32800()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	l_width = 10;
	if (choiceIdx != -1)
	{
		RollbackInfo info;
		info.idx = choiceIdx;
		info.undoShape = data[choiceIdx];
		data[choiceIdx].penWidth = l_width;
		//deepCopy(info.updateShape, data[choiceIdx]);
		info.redoShape = data[choiceIdx];
		info.rollbackmode = RollBackMode::Update;
		if (rollbackIndex != -1)
			rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
		else if (rollback.size() != 0) {
			rollback.erase(rollback.begin(), rollback.end());
		}
		rollback.push_back(info);
		rollbackIndex = rollback.size() - 1;
		Invalidate();
	}
}


void CChildView::OnCopy()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//if (panID)  // 도형 그리기 상태일 동안만
	{
		if (choiceIdx != -1)
		{
			//info.updateShape = data[0];
			copyShape = data[choiceIdx];
			pasteW = data[choiceIdx].rect.right - data[choiceIdx].rect.left;
			pasteH = data[choiceIdx].rect.bottom - data[choiceIdx].rect.top;
			iscopy = true;
		}
	}
}


void CChildView::OnPaste()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (iscopy)
	{
		if (ctrl)
		{
			copyShape.rect.left += 5;
			copyShape.rect.top += 5;
			copyShape.rect.right += 5;
			copyShape.rect.bottom += 5;
		}
		else
		{
			copyShape.rect.left = p_pt.x;
			copyShape.rect.top = p_pt.y;
			copyShape.rect.right = p_pt.x + pasteW;
			copyShape.rect.bottom = p_pt.y + pasteH;
		}
		data.Add(copyShape);
		zOrder.insert(zOrder.begin(), data.GetSize() - 1);;
		choiceIdx = zOrder[0];
		RollbackInfo info;
		info.redoShape = data[choiceIdx];
		info.idx = choiceIdx;
		info.undoShape = shape;
		info.rollbackmode = RollBackMode::Create;
		if (rollbackIndex != -1)
			rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
		else if (rollback.size() != 0) {
			rollback.erase(rollback.begin(), rollback.end());
		}
		rollback.push_back(info);
		rollbackIndex = rollback.size() - 1;
		data[choiceIdx].isClicked = true;
		Invalidate();
		ctrl = false;
	}
}


void CChildView::OnDelete()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	if (choiceIdx != -1)
	{
		RollbackInfo info;
		info.idx = choiceIdx;
		//info.updateShape = data[0];
		info.undoShape = data[choiceIdx];
		info.rollbackmode = RollBackMode::Delete;
		if (rollbackIndex != -1)
			rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
		else if (rollback.size() != 0) {
			rollback.erase(rollback.begin(), rollback.end());
		}
		rollback.push_back(info);
		rollbackIndex = rollback.size() - 1;
		data.RemoveAt(choiceIdx);
		//InvalidateRect(data[choiceIdx].rect);
		Invalidate();
		choiceIdx = -1;
	}
	return;
}


void CChildView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (choiceIdx == -1)
		return;
	CMenu popup;
	CMenu* pMenu;

	popup.LoadMenuW(IDR_MENU1);
	pMenu = popup.GetSubMenu(0);

	pMenu->TrackPopupMenu(TPM_LEFTALIGN || TPM_RIGHTBUTTON, point.x, point.y, this);
}


void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnRButtonDown(nFlags, point);

	p_pt.x = z_pos.x + (m_ePt.x + point.x) / PWidth;
	p_pt.y = z_pos.y + (m_ePt.y + point.y) / PHeight;
}



void CChildView::OnLinecolor()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (choiceIdx !=-1)
	{
		CColorDialog colorDlg;

		if (colorDlg.DoModal() == IDOK)
		{
			RollbackInfo info;
			info.idx = choiceIdx;
			info.undoShape = data[choiceIdx];
			data[choiceIdx].fgColor = colorDlg.GetColor();
			//deepCopy(info.updateShape, data[choiceIdx]);
			info.redoShape = data[choiceIdx];
			info.rollbackmode = RollBackMode::Update;
			if (rollbackIndex != -1)
				rollback.erase(rollback.begin() + rollbackIndex + 1, rollback.end());
			else if (rollback.size() != 0) {
				rollback.erase(rollback.begin(), rollback.end());
			}
			rollback.push_back(info);
			rollbackIndex = rollback.size() - 1;
			Invalidate();
		}
	}
}