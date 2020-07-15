
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
	ID_INDICATOR_USER_FILENAME,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	m_wndStatusBar.SetPaneInfo(0, ID_INDICATOR_USER_FILENAME, SBPS_NORMAL, 200);
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_USER_FILENAME, SBPS_STRETCH, 200);
	CString runtime, ballcount;
	runtime = _T("Runtime : 0");
	ballcount = _T("Ball Count : 0");
	m_wndStatusBar.SetPaneText(0, runtime);
	m_wndStatusBar.SetPaneText(1, ballcount);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

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

	start = TRUE;
}



void CMainFrame::OnClose()
{
	theApp.OnAppExit();

	CMDIFrameWnd::OnClose();
}


void CMainFrame::OnOptionSave()
{
	theApp.OnOptionSave();
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
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
