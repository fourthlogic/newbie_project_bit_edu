
// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "MFCparam.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_CLOSE()
	ON_COMMAND(ID_OPTION_SAVE, &CMainFrame::OnOptionSave)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	//ID_INDICATOR_USER_FILENAME,
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

	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("도구 모음을 만들지 못했습니다.\n");
	//	return -1;      // 만들지 못했습니다.
	//}

	//if (!m_wndStatusBar.Create(this))
	//{
	//	TRACE0("상태 표시줄을 만들지 못했습니다.\n");
	//	return -1;      // 만들지 못했습니다.
	//}
	//m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	//m_wndStatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_NORMAL, 200);
	//m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_USER_FILENAME, SBPS_STRETCH, 200);
	//
	//m_wndStatusBar.SetPaneText(1, _T("테스트"));

	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	//cs.style = WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME;
	

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

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIFrameWnd::OnSize(nType, cx, cy);
	
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int width = 205;
	int height = 300;
	GetClientRect(m_rect);

	//posTemplate = theApp.GetFirstDocTemplatePosition();

	//pDocTemplate = (CMultiDocTemplate*)theApp.GetNextDocTemplate(posTemplate); // 첫번째 템플릿
	//posDocument = pDocTemplate->GetFirstDocPosition(); // 도큐먼트 포지션 얻기
	//pOptionDoc = (COptionDoc*)pDocTemplate->GetNextDoc(posDocument); // 첫번째 도큐먼트
	//posView = pOptionDoc->GetFirstViewPosition(); // 뷰 포지션 얻기
	//pOptionView = (COptionFormView*)pOptionDoc->GetNextView(posView); // 도큐먼트의 뷰
	//pOptionFrame = (COptionFrame*)pOptionView->GetParentFrame(); // 프레임 포지션 얻기

	//pDocTemplate = (CMultiDocTemplate*)theApp.GetNextDocTemplate(posTemplate); // 두번째 템플릿
	//posDocument = pDocTemplate->GetFirstDocPosition();
	//pImgViewerDoc = (CImgViewerDoc*)pDocTemplate->GetNextDoc(posDocument);
	//posView = pImgViewerDoc->GetFirstViewPosition();
	//pImgViewerView = (CImgViewerView*)pImgViewerDoc->GetNextView(posView);
	//pImgViewerFrame = (CImgViewerFrame*)pImgViewerView->GetParentFrame();

	//pDocTemplate = (CMultiDocTemplate*)theApp.GetNextDocTemplate(posTemplate); // 세번째 템플릿
	//posDocument = pDocTemplate->GetFirstDocPosition();
	//pNavigatorDoc = (CNavigatorDoc*)pDocTemplate->GetNextDoc(posDocument);
	//posView = pNavigatorDoc->GetFirstViewPosition();
	//pNavigatorView = (CNavigatorView*)pNavigatorDoc->GetNextView(posView);
	//pNavigatorFrame = (CNavigatorFrame*)pNavigatorView->GetParentFrame();
	
	//pOptionFrame->MoveWindow(0, 0, width, m_rect.bottom / 2, 1);
	//pImgViewerFrame->MoveWindow(width, 0, m_rect.right - width, m_rect.bottom, 1);
	//pNavigatorFrame->MoveWindow(0, m_rect.bottom / 2, width, m_rect.bottom / 2, 1);
	
	theApp.pOptionFrame->MoveWindow(0, 0, width, height, 1);
	theApp.pImgViewerFrame->MoveWindow(width, 0, m_rect.right - width, m_rect.bottom, 1);
	theApp.pNavigatorFrame->MoveWindow(0, height, width, m_rect.bottom-height, 1);
	
	Invalidate();
}


void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// Min size
	lpMMI->ptMinTrackSize.x = 800;
	lpMMI->ptMinTrackSize.y = 540;

	CMDIFrameWnd::OnGetMinMaxInfo(lpMMI);
}

void CMainFrame::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//pOptionView->OnAppExit();
	theApp.OnAppExit();

	CMDIFrameWnd::OnClose();
}


void CMainFrame::OnOptionSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//pOptionView->OnOptionSave();
	theApp.OnOptionSave();
}
