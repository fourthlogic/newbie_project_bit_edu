// ChildView.cpp: CChildView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "Fourthlohgic_Project.h"
#include "ChildView.h"
#include "main.h"
#include "MainFrm.h"
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console");


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CFourthlohgicProjectApp theApp;
// CChildView

CChildView::CChildView()
{/*
    static TCHAR BASED_CODE szFilter[] = _T("이미지 파일(*.BMP, *.GIF, *.JPG, *PNG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.png;*.gif; |모든파일(*.*)|*.*||");

    CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);

    if (IDOK != dlg.DoModal()) return;
    sFilename = dlg.GetPathName();*/
   
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
    ON_WM_CREATE()
    ON_WM_DROPFILES()
    ON_WM_KEYDOWN()
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

    if (!m_background.m_hObject) {
        return;
    }

    // 이미 배경은 OnInitDialog() 혹은 OnInitialUpdate()에서 로드되어 있으므로 다시 할 필요는 없다.
    memDC.CreateCompatibleDC(pDC);
    mdcOffScreen.CreateCompatibleDC(pDC);

    // 화면 크기로 빈공간의 버퍼를 생성 한다.
    bmpOffScreen.CreateCompatibleBitmap(pDC, m_Bitmap.bmWidth, m_Bitmap.bmHeight);
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
    mdcOffScreen.StretchBlt(0, 0, m_bgRect.right, m_bgRect.bottom, &memDC, z_pos.x, z_pos.y,
        rectWidth, rectHeight, SRCCOPY);
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
    pDC->StretchBlt(0, 0, m_bgRect.right, m_bgRect.bottom, &mdcOffScreen, m_ePt.x, m_ePt.y, rectWidth, rectHeight, SRCCOPY);
    //pDC->BitBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &mdcOffScreen, 0, 0, SRCCOPY);
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
    if (!m_background.m_hObject)
        return false;
   
    //클라 크기가 이미지보다 클때 검은화면으로 채워주기
    if (m_Bitmap.bmWidth < m_bg_rect.right) { //오른쪽
        RECT rect2;
        rect2.left = m_Bitmap.bmWidth;
        rect2.right = m_bg_rect.right;
        rect2.top = 0;
        rect2.bottom = m_Bitmap.bmHeight;
        //m_ePt.x = 0;
        pDC->FillRect(&rect2, &m_bkgBrush);  // B영역 칠하기
    }
    if (m_Bitmap.bmHeight < m_bg_rect.bottom) { //아래
        RECT rect2;
        rect2.left = 0;
        rect2.right = m_bg_rect.right;
        rect2.top = m_Bitmap.bmHeight;
        rect2.bottom = m_bg_rect.bottom;
        //m_ePt.y = 0;
        pDC->FillRect(&rect2, &m_bkgBrush); // C영역 칠하기
    }

    //return CWnd::OnEraseBkgnd(pDC);
    return false;
}


void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

    //CWnd::OnMouseMove(nFlags, point);
    m_pos = point;
    double posRateX = m_Bitmap.bmWidth*1.0 / (m_bgRect.Width()*3.0);
    double posRateY = m_Bitmap.bmHeight / m_bgRect.Height();
    if (nFlags & MK_LBUTTON)
    {
        // A영역의 사각형 그리기 //m_sPt는 기존 포인트, point는 이동
        if (m_sPt.x < m_pos.x) // 오른쪽으로 끌었을때
        {
            //pDC->Rectangle(0, 0, (point.x - m_sPt.x), tmpRect.bottom);
            m_ePt.x -= (m_pos.x*1.0 - m_sPt.x)*posRateX;
            if (m_ePt.x < 0)
            {
                m_ePt.x = 0;
                if (z_pos.x > 0)
                    z_pos.x -= 1;
            }
        }
        else // 왼쪽으로 끌었을때
        {

            if (z_pos.x + rectWidth < m_Bitmap.bmWidth)
                m_ePt.x += (m_sPt.x * 1.0 - m_pos.x)*posRateX;
            /*if (m_Bitmap.bmWidth > m_bgRect.right)
            {
                if (m_ePt.x > m_Bitmap.bmWidth - rectWidth)
                    m_ePt.x = m_Bitmap.bmWidth - rectWidth;
            }*/
        }

        // B영역의 사각형 그리기
        if (m_sPt.y < m_pos.y) // 위로 올렸을때
        {
            m_ePt.y -= (m_pos.y * 1.0 - m_sPt.y)*posRateX;
            if (m_ePt.y < 0)
            {
                m_ePt.y = 0;
                if (z_pos.y > 0)
                {
                    z_pos.y -= 1;
                }
            }
        }
        else // 아래로 내렸을때
        {
            m_ePt.y += (m_sPt.y * 1.0 - m_pos.y)*posRateX;
            if (m_Bitmap.bmHeight > m_bgRect.bottom)
            {
                if (m_ePt.y > m_Bitmap.bmHeight - rectHeight)
                    m_ePt.y = m_Bitmap.bmHeight - rectHeight;
            }
        }

        m_sPt = m_pos;
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
    //double pos_RateX = m_bg_rect.Width() / m_Bitmap.bmWidth;
    //double pos_RateY = m_bg_rect.Height() / m_Bitmap.bmHeight;
    //CPoint image_pos;
    //image_pos.x = (pt.x * pos_RateX);
    //image_pos.y = (pt.y * pos_RateY);
    //if (zDelta > 0) {
    //    zoomFactor *= 1.25;
    //}
    //else {
    //    //down
    //    zoomFactor *= 0.8;
    //}
    //double dx = m_bg_rect.Width() * (1 - 1 / zoomFactor);
    //double dy = m_bg_rect.Height() * (1 - 1 / zoomFactor);
    //double d_left = dx * image_pos.x;
    //double d_up = dy * image_pos.y;
    //zoomRect.left += d_left;
    //zoomRect.top += d_up;
    //zoomRect.right = zoomRect.left+(zoomRect.Width()-dx);
    //zoomRect.bottom = zoomRect.top + (zoomRect.Height() - dy);

    //Invalidate();
    //return CWnd::OnMouseWheel(nFlags, zDelta, pt);
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

    z_pos.x += round((((float)m_pos.x / (float)m_bgRect.right) * ((float)rectWidth)) - (((float)m_pos.x / (float)m_bgRect.right) * ((float)rectWidth * ViewScale)));
    z_pos.y += round((((float)m_pos.y / (float)m_bgRect.bottom) * (float)rectHeight) - (((float)m_pos.y / (float)m_bgRect.bottom) * ((float)rectHeight * ViewScale)));

    // 사각형 크기
    rectWidth *= ViewScale;
    rectHeight *= ViewScale;

    //m_ePt = z_pos;

    Invalidate();
    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    // TODO:  여기에 특수화된 작성 코드를 추가합니다.
    
    pFrame = (CMainFrame*)AfxGetMainWnd();
    pFrame->imageList[idx].setImage();
    m_background.Attach(pFrame->imageList[idx].getImage());
    m_background.GetBitmap(&m_Bitmap);
    for (int i = 0; i < pFrame->imageList.size();i++) {
        pFrame->imageList[i].setImage();
    }
    m_bkgBrush.CreateSolidBrush(0x00000000);
    m_ePt.x = 0;
    m_ePt.y = 0;
    z_pos = m_ePt;
    ViewScale = 1;
    ViewValue = 1;
    
    return 0;
}


void CChildView::OnDropFiles(HDROP hDropInfo)
{
    
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    int ucount = DragQueryFile(hDropInfo, 0xffffffff, nullptr, 0);
#ifdef _UNICODE

    LPWSTR buffer;

#else

    char* buffer;

#endif

    buffer = nullptr;

    // 여러개 들어올수도 있으나 여기선 하나만 처리

    //if (ucount > 1) AfxMessageBox(_T("File은 하나만 처리가능, 첫번째 파일만 처리합니다."));

    int nLength = DragQueryFile(hDropInfo, 0, nullptr, 0);

#ifdef _UNICODE

    buffer = (LPWSTR)malloc(2 * (nLength + 1)); // 여기서 Uncode는 2 Byte이므로 2배로 해줘야 한다.

#else

    buffer = (char*)malloc(nLength + 1);

#endif

    DragQueryFile(hDropInfo, 0, buffer, nLength + 1);

    /*if (GetFileAttributes(buffer) == FILE_ATTRIBUTE_DIRECTORY) {

        AfxMessageBox(_T("Directory는 처리하지 않습니다."));

    }*/
    if (false) {

    }
    else {
        std::string fileName;
#ifdef _UNICODE
        CT2CA pszConvertedAnsiString(buffer);
        std::string s(pszConvertedAnsiString);
        fileName = s;

#else
        std::string s((LPCTSTR)buffer);
        fileName = s;

       
#endif
        HBITMAP hBit = Convert2Bitmap(fileName);
        m_background.Detach();
        m_background.Attach(hBit);
        m_background.GetBitmap(&m_Bitmap);
        GetClientRect(m_bg_rect);
        zoomFactor = 1.0;
        
        //m_background.GetObject(sizeof(BITMAP), (LPVOID)&m_Bitmap);
        Invalidate();
    }

    if (buffer != nullptr) free(buffer);

    CWnd::OnDropFiles(hDropInfo);
}

HBITMAP CChildView::Convert2Bitmap(std::string fileName)
{

    Mat mat_temp;
    Mat src = imread(fileName, IMREAD_GRAYSCALE);

    Mat dst;

    vector<Point> cornerpts; // 외곽 3점 좌표 값

    vector<Point> vCirCenters; // Vertical의 중심 좌표들의 값 
    vector<Point> hCirCenters; // Horizontal의 중심 좌표들의 값 

    vector<Point> vertexPts;   //


    GetCornerPoints(src, cornerpts); // 3점 좌표 추출
    GetVertexPoints(src, cornerpts, vertexPts, 20);
    ContourDetection(src, vCirCenters, hCirCenters, vertexPts, 2, 4, 100);

    Drawing(src, dst, vCirCenters, hCirCenters);



    HDC hDC = ::CreateCompatibleDC(0);
    HBITMAP hBmp;

    //화면에 보여주기 위한 처리
    //CString tmp;
    //tmp.Format(_T("%d"), src.elemSize());
    //MessageBox(tmp);
    int bpp = 8 * dst.elemSize(); // elemSize() 는 한화소에 데이터 개수 Color = 3개 Gray = 1개, 한 화소에 비트수 구하는거
    assert((bpp == 8 || bpp == 24 || bpp == 32)); // 이 3개가 아니면 종료

    int padding = 0;
    //32 bit image is always DWORD aligned because each pixel requires 4 bytes
    if (bpp < 32)
        padding = 4 - (dst.cols % 4);

    if (padding == 4)
        padding = 0;

    int border = 0;
    // 32 bit image is always DWORD aligned because each pixel requires 4 bytes
    if (bpp < 32)
    {
        border = 4 - (dst.cols % 4);
    }

    if (border > 0 || dst.isContinuous() == false)
    {
        // Adding needed columns on the right(max 3 px)
        cv::copyMakeBorder(dst, mat_temp, 0, 0, 0, border, cv::BORDER_CONSTANT, 0);
    }
    else
    {
        mat_temp = dst;
    }

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

    hBmp = ::CreateDIBSection(hDC, pBitmapInfo, DIB_RGB_COLORS, NULL, 0, 0);
    ::SetBitmapBits(hBmp, mat_temp.total() * mat_temp.channels(), mat_temp.data);
    ::DeleteDC(hDC);

    return hBmp;

    /*::ReleaseDC(this->m_hWnd, mPicDC);
    free(pBitmapInfo);*/
}

void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    switch (nChar) {
        case VK_LEFT:
            if (idx != 0)
                idx--;
            break;
        case VK_RIGHT:
            if (idx != pFrame->imageList.size() - 1)
                idx++;
            break;
        default:
            return;
    }
    cout << idx << endl;
    m_background.Detach();
    m_background.Attach(pFrame->imageList[idx].getImage());
    m_background.GetBitmap(&m_Bitmap);
    Invalidate();
    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
