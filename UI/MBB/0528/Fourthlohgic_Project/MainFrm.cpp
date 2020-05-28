
// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "Fourthlohgic_Project.h"

#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_DROPFILES()
    ON_COMMAND(ID_SELECTCOLOR, &CMainFrame::OnClickedToolBarEX)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

    //if (!m_viewToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
    //    !m_viewToolBar.LoadToolBar(IDR_MAINFRAME))
    //{
    //    TRACE0("도구 모음을 만들지 못했습니다.\n");
    //    return -1;      // 만들지 못했습니다.
    //}

	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    //m_viewToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
    //DockControlBar(&m_viewToolBar);
	DragAcceptFiles(TRUE);


	return 0;
}
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnClickedToolBarEX()
{
	CColorDialog colorDlg;

	if (colorDlg.DoModal() == IDOK)
	{
		color = colorDlg.GetColor();
	}
}


void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int ucount = DragQueryFile(hDropInfo, 0xffffffff, nullptr, 0);
	for (int i = 0; i < ucount; i++) {

#ifdef _UNICODE

		LPWSTR buffer;

#else

		char* buffer;

#endif

		buffer = nullptr;

		// 여러개 들어올수도 있으나 여기선 하나만 처리

		//if (ucount > 1) AfxMessageBox(_T("File은 하나만 처리가능, 첫번째 파일만 처리합니다."));


		int nLength = DragQueryFile(hDropInfo, i, nullptr, 0);

#ifdef _UNICODE

		buffer = (LPWSTR)malloc(2 * (nLength + 1)); // 여기서 Uncode는 2 Byte이므로 2배로 해줘야 한다.

#else

		buffer = (char*)malloc(nLength + 1);

#endif

		DragQueryFile(hDropInfo, i, buffer, nLength + 1);

		if (GetFileAttributes(buffer) == FILE_ATTRIBUTE_DIRECTORY) {

			AfxMessageBox(_T("Directory는 처리하지 않습니다."));

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
			imageData temp;
			temp.setFileName(fileName);
			imageList.push_back(temp);
		
			Invalidate();
		}
		if (buffer != nullptr) free(buffer);
	}
	CFourthlohgicProjectApp* pApp = (CFourthlohgicProjectApp*)AfxGetApp();
	pApp->OnFileNew();
	CWnd::OnDropFiles(hDropInfo);
	//CMDIFrameWnd::OnDropFiles(hDropInfo);
}

HBITMAP imageData::getHbitmap()
{

    Mat mat_temp;
    this->src = imread(this->fileName, IMREAD_GRAYSCALE);

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