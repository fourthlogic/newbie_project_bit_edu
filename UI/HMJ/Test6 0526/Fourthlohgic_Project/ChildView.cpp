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
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
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

	// 이미 배경은 OnInitDialog() 혹은 OnInitialUpdate()에서 로드되어 있으므로 다시 할 필요는 없다.
	memDC.CreateCompatibleDC(pDC);
	mdcOffScreen.CreateCompatibleDC(pDC);

	// 화면 크기로 빈공간의 버퍼를 생성 한다.
	bmpOffScreen.CreateCompatibleBitmap(pDC, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CXFULLSCREEN));
	// 아직 dmemDC의 메모리에는 아무런 그림이 없다.
	// 만약 어떤 색깔로 채우고자 한다면 FillRect() 함수등으로 특정색으로 칠할 수 있다.
	// 그러나 다음에 배경 그림을 로드하므로 필요없는 일이다.

	oldbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);
	// 이제 메모리 준비는 끝났다. 지금 부터는 그림을 그린다.
	//우선 배경 그림이 맨 밑이므로 배경을 메모리에 복사 한다.

	memDC.SelectObject(&m_background);   // 배경 그림을 선택하고
	mdcOffScreen.SetStretchBltMode(COLORONCOLOR);
	//mdcOffScreen.BitBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &memDC, 0, 0, SRCCOPY);
	//mdcOffScreen.StretchBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &memDC, m_ePt.x, m_ePt.y, newWidth, newHeight, SRCCOPY);
	mdcOffScreen.StretchBlt(0, 0, viewWidth * (int)(rectWidth + 2), viewHeight * (int)(rectHeight +2), &memDC, z_pos.x, z_pos.y, rectWidth + 2, rectHeight + 2, SRCCOPY);

	PrintText(&mdcOffScreen);

	printf("printWidth : %f", printWidth);
	printf("printHeight : %f", printHeight);
	printf("viewWidth * rectWidth : %f", viewWidth * (int)(rectWidth + 2));
	printf("viewHeight * rectHeight : %f", viewHeight * (int)(rectHeight + 2));
	printf("\n");
	printf("z_pos.x : %d ", z_pos.x);
	printf("rectWidth : %f ", rectWidth);
	printf("m_Bitmap.bmWidth + view Width : %f ", m_Bitmap.bmWidth + viewWidth);
	printf("\n");
	printf("m_ePt.x : %d ", m_ePt.x);
	printf("viewWidth : %f ", viewWidth);
	printf("viewHeight : %f ", viewHeight);
	printf("m_bgRect.right : %d ", m_bgRect.right);
	printf("\n");
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
	pDC->SetStretchBltMode(COLORONCOLOR);
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
		/*printf("z_pos.x : %d ", z_pos.x);
		printf("rectWidth : %f ", rectWidth);

		printf("\n");
		printf("m_ePt.x : %d ", m_ePt.x);
		printf("i : %d ", i);
		printf("m_bgRect.right : %d ", m_bgRect.right);
		printf("m_Bitmap.bmWidth : %d ", m_Bitmap.bmWidth);
		printf("viewWidth : %f ", viewWidth);
		printf("\n");*/

		if (m_ePt.x < 0)
			m_ePt.x = 0;
		if (m_ePt.y < 0)
			m_ePt.y = 0;

		m_sPt = point;
		Invalidate();
		UpdateWindow();
	}
}


void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd::OnLButtonDown(nFlags, point);

	SetCapture();
	m_sPt = point;
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.d

	CWnd::OnLButtonUp(nFlags, point);

	ReleaseCapture();
}


void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_bgRect.left = 0;
	m_bgRect.top = 0;
	m_bgRect.right = cx;
	m_bgRect.bottom = cy;



	//rectWidth = cx;
	//rectHeight = cy;

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

	//z_pos.x += (((m_ePt.x + m_pos.x) / (viewWidth * (int)(rectWidth + 2))) * (int)rectWidth) - (((m_ePt.x + m_pos.x) / (viewWidth * (int)(rectWidth + 2))) * (int)(rectWidth * zoomScale));
	//z_pos.y += (((m_ePt.y + m_pos.y) / (viewHeight * (int)(rectHeight + 2))) * (int)rectHeight) - (((m_ePt.y + m_pos.y) / (viewHeight * (int)(rectHeight + 2))) * (int)(rectHeight * zoomScale));

	//m_ePt.x += round((((float)m_pos.x / (float)m_bgRect.right) * ((float)rectWidth)) - (((float)m_pos.x / (float)m_bgRect.right) * ((float)rectWidth * ViewScale)));
	//m_ePt.y += round((((float)m_pos.y / (float)m_bgRect.bottom) * (float)rectHeight) - (((float)m_pos.y / (float)m_bgRect.bottom) * ((float)rectHeight * ViewScale)));


	// 오른쪽 아래 꼭짓점
	//rectWidth = rectWidth * ViewScale;
	//rectHeight = rectHeight * ViewScale;

	rectWidth = rectWidth * zoomScale;
	rectHeight = rectHeight * zoomScale;

	printWidth = rectWidth * zoomView;
	printHeight = rectHeight * zoomView;


	viewWidth = (printWidth ) / (rectWidth );
	viewHeight = (printHeight ) / (rectHeight);

	for (int i = 0; i < rectWidth; i++)
	{

	}

	/*printf("z_pos.x : %d ", z_pos.x);
	printf("rectWidth : %f ", rectWidth);
	printf("\n");
	printf("m_ePt.x : %d ", m_ePt.x);
	printf("m_ePt.y : %d ", m_ePt.y);
	printf("rectHeight : %f ", rectHeight);
	printf("viewWidth : %f ", viewWidth);
	printf("viewHeight : %f ", viewHeight);
	printf("\n");*/


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
			printf("cntHeight : %d ", cntHeight);
			cntHeight = 0;
		}
	}

	printf("viewWidth : %f", viewWidth);
	printf("viewHeight : %f", viewHeight);

	printf("cntWidth : %d ", cntWidth);
	printf("\n");

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
