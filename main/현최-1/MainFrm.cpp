
// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "MFCparam.h"
#include "MainFrm.h"


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_OPTION_SAVE, &CMainFrame::OnOptionSave)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	//ID_INDICATOR_USER_FILENAME,
	//ID_INDICATOR_CAPS,
	//ID_INDICATOR_NUM,
	//ID_INDICATOR_SCRL,
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

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	m_wndStatusBar.SetPaneInfo(0, ID_INDICATOR_USER_FILENAME, SBPS_STRETCH, 200);
	CString runtime;
	runtime = _T("Runtime : 0");
	m_wndStatusBar.SetPaneText(0, runtime);

	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);

	//this->GetClientRect(m_rect);
	//c_Option.sx = 0;
	//c_Option.sy = 0;
	//c_Option.cx = m_rect.right * 0.15;
	//c_Option.cy = m_rect.bottom * 0.4;

	//c_Navi.sx = 0;
	//c_Navi.sy = m_rect.bottom * 0.4;
	//c_Navi.cx = m_rect.right * 0.15;
	//c_Navi.cy = m_rect.bottom - c_Option.cy;

	//c_ImgView.sx = m_rect.right * 0.15;
	//c_ImgView.sy = 0;
	//c_ImgView.cx = m_rect.right - c_Option.cx;
	//c_ImgView.cy = m_rect.bottom;


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	//cs.style = WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME;
	cs.style &= ~(LONG)FWS_ADDTOTITLE;
	cs.lpszName = _T("포스로직 프로젝트");

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
//	c_Option.cy = cy*0.4;
//	c_Option.cx = cx*0.15;
	GetClientRect(m_rect);

	c_Option.sx = 0;
	c_Option.sy = 0;
	c_Option.cx = m_rect.right * 0.172;
	c_Option.cy = m_rect.bottom * 0.48;

	c_Navi.sx = 0;
	c_Navi.sy = c_Option.cy;
	c_Navi.cx = c_Option.cx;
	c_Navi.cy = m_rect.bottom - c_Option.cy;

	c_ImgView.sx = c_Option.cx;
	c_ImgView.sy = 0;
	c_ImgView.cx = m_rect.right - c_Option.cx;
	c_ImgView.cy = m_rect.bottom;

	theApp.pOptionFrame->MoveWindow(c_Option.sx, c_Option.sy, c_Option.cx, c_Option.cy, 1);
	theApp.pNavigatorFrame->MoveWindow(c_Navi.sx, c_Navi.sy, c_Navi.cx, c_Navi.cy, 1);
	theApp.pImgViewerFrame->MoveWindow(c_ImgView.sx, c_ImgView.sy, c_ImgView.cx, c_ImgView.cy, 1);

	//s_Option = TRUE;
	//s_Navi = TRUE;
	//s_Viewer = TRUE;
	start = TRUE;
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

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	// ALT + F4 방지 
		if (pMsg->message == WM_SYSKEYDOWN) { 
			if (pMsg->wParam == VK_F4) 
				return TRUE; 
		} 
		if (pMsg->message == WM_KEYDOWN) {
			if (pMsg->wParam == VK_ESCAPE || 
				pMsg->wParam == VK_RETURN || 
				pMsg->wParam == VK_SPACE || 
				pMsg->wParam == VK_CANCEL) //CTRL + PAUSE 방지 
				return TRUE; 
		}

			
	return CMDIFrameWnd::PreTranslateMessage(pMsg);
}
