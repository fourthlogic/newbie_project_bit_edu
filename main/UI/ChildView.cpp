﻿// ChildView.cpp: CChildView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "Fourthlohgic_Project.h"
#include "ChildView.h"
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console");
#define GetRValue( rgb )    ( ( BYTE )( rgb ) )

#define GetGValue( rgb )    ( ( BYTE )( ( ( WORD )( rgb ) ) >> 8 ))

#define GetBValue( rgb )    ( ( BYTE )( ( rgb ) >> 16 ) )

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CFourthlohgicProjectApp theApp;
// CChildView

CChildView::CChildView()
{
    if (!image.IsNull())
        image.Destroy();
    image.Load(theApp.sFilename);

    m_background.Attach(image.Detach());
    m_background.GetObject(sizeof(BITMAP), (LPVOID)&m_Bitmap);

    zoomWidth = m_Bitmap.bmWidth;
    zoomHeight = m_Bitmap.bmHeight;

    m_ePt.x = 0;
    m_ePt.y = 0;

    z_pos.x = 1;
    z_pos.y = 1;

    m_Zoom = 1;

    PWidth = 0;
    PHeight = 0;

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
    ON_WM_MOUSEWHEEL()
    ON_WM_MBUTTONDOWN()
    ON_WM_MBUTTONUP()
    ON_WM_LBUTTONDOWN()
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
    mdcOffScreen.StretchBlt(0, 0, m_Bitmap.bmWidth + PWidth, m_Bitmap.bmHeight + PHeight, &memDC, 
        z_pos.x, z_pos.y, zoomWidth + 1, zoomHeight + 1, SRCCOPY);

    PrintText(&mdcOffScreen);

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

    pDC->SetStretchBltMode(COLORONCOLOR);
    pDC->StretchBlt(0, 0, m_bgRect.right, m_bgRect.bottom, &mdcOffScreen, 
        m_ePt.x, m_ePt.y, m_bgRect.right, m_bgRect.bottom, SRCCOPY);


    


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
    //   RECT rect2;
    //   rect2.left = m_Bitmap.bmWidth;
    //   rect2.right = m_bgRect.right;
    //   rect2.top = 0;
    //   rect2.bottom = m_Bitmap.bmHeight;
    //   //m_ePt.x = 0;
    //   pDC->FillRect(&rect2, &m_bkgBrush);  // B영역 칠하기
    //}
    //if (m_Bitmap.bmHeight < m_bgRect.bottom) { //아래
    //   RECT rect2;
    //   rect2.left = 0;
    //   rect2.right = m_bgRect.right;
    //   rect2.top = m_Bitmap.bmHeight;
    //   rect2.bottom = m_bgRect.bottom;
    //   //m_ePt.y = 0;
    //   pDC->FillRect(&rect2, &m_bkgBrush); // C영역 칠하기
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

    z_pos.x += round(((((float)m_ePt.x + (float)m_pos.x) / (float)m_Bitmap.bmWidth) * zoomWidth) - ((((float)m_ePt.x + (float)m_pos.x) / (float)m_Bitmap.bmWidth) * (zoomWidth * m_Zoom)));
    z_pos.y += round(((((float)m_ePt.y + (float)m_pos.y) / (float)m_Bitmap.bmHeight) * zoomHeight) - ((((float)m_ePt.y + (float)m_pos.y) / (float)m_Bitmap.bmHeight) * (zoomHeight * m_Zoom)));

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

    zoomWidth = zoomWidth * m_Zoom;
    zoomHeight = zoomHeight * m_Zoom;

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

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

    CWnd::OnLButtonDown(nFlags, point);
    CDC* dc = GetDC();

    COLORREF rgb;
    rgb = dc->GetPixel(point.x, point.y);

    int R = 0, G = 0, B = 0;

    R = GetRValue(rgb);
    G = GetGValue(rgb);
    B = GetBValue(rgb);

    double pixelvale = (R + G + B) / 3.0;
    CString strNum;
    strNum.Format(_T("%f"), pixelvale);
    dc->TextOutW(point.x, point.y, strNum);
}

void CChildView::PrintText(CDC* pDC)
{
    COLORREF rgb;

    if (PWidth >= 80)
    {
        printf("zoomWidth : %f ", zoomWidth);
        printf("zoomHeight : %f ", zoomHeight);
        printf("\n");
        printf("PWidth : %f ", PWidth);
        printf("PHeight : %f ", PHeight);
        printf("\n");
        for (int i = 0; i < m_bgRect.right; i += PWidth)
        {
            for (int j = 0; j < m_bgRect.bottom; j += PHeight)
            {
                rgb = pDC->GetPixel(i, j);
                int R = 0, G = 0, B = 0;

                R = GetRValue(rgb);
                G = GetGValue(rgb);
                B = GetBValue(rgb);

                int pixelvale = (R + G + B) / 3.0;
                CString strNum;
                strNum.Format(_T("%d"), pixelvale);

                pDC->SetBkMode(TRANSPARENT);

                if (pixelvale >= 125)
                    pDC->SetTextColor(0x00000000);
                else
                    pDC->SetTextColor(RGB(255, 255, 255));


                pDC->DrawText(strNum, CRect(i, j, i + PWidth, j + PHeight), DT_SINGLELINE | DT_VCENTER);


                //pDC->TextOutW(i, j, strNum);

            }
        }
    }
}