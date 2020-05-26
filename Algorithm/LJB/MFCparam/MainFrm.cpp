﻿
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

	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	//// set the visual manager used to draw all user interface elements
	//CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	//// enable smart docking style window behavior
	//CDockingManager::SetDockingMode(DT_SMART);

	//// Enable docking and redocking pane to frame any sides (you can pass a combination of CBRS_ALIGN_ flags)
	//m_wndPane.EnableDocking(CBRS_ALIGN_ANY);

	//// Dock pane to the default (left) side of the frame.
	//DockPane(&m_wndPane);



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



void CMainFrame::RecalcLayout(BOOL bNotify)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//COptionFrame* pOptFrame = (COptionFrame*)GetActi

	CMDIFrameWnd::RecalcLayout(bNotify);
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIFrameWnd::OnSize(nType, cx, cy);
	
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int width = 300;
	GetClientRect(m_rect);

	posTemplate = theApp.GetFirstDocTemplatePosition();
	pDocTemplate = (CMultiDocTemplate*)theApp.GetNextDocTemplate(posTemplate); // 첫번째 템플릿
	posDocument = pDocTemplate->GetFirstDocPosition(); // 도큐먼트 포지션 얻기
	pDoc = (COptionDoc*)pDocTemplate->GetNextDoc(posDocument); // 첫번째 도큐먼트
	posView = pDoc->GetFirstViewPosition(); // 뷰 포지션 얻기
	pView = (COptionFormView*)pDoc->GetNextView(posView); // 도큐먼트의 뷰
	pFrame = (COptionFrame*)pView->GetParentFrame(); // 프레임 포지션 얻기

	pDocTemplate = (CMultiDocTemplate*)theApp.GetNextDocTemplate(posTemplate); // 두번째 템플릿
	posDocument = pDocTemplate->GetFirstDocPosition();
	pImgViewerDoc = (CMFCparamDoc*)pDocTemplate->GetNextDoc(posDocument);
	posView = pImgViewerDoc->GetFirstViewPosition();
	pImgViewerView = (CMFCparamView*)pImgViewerDoc->GetNextView(posView);
	pImgViewerFrame = (CChildFrame*)pImgViewerView->GetParentFrame();
	
	pFrame->MoveWindow(m_rect.right - (width + 5), 0, width, m_rect.bottom - 50, 1);
	pImgViewerFrame->MoveWindow(0, 0, m_rect.right - width, m_rect.bottom - 50, 1);
}


void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// Min size
	lpMMI->ptMinTrackSize.x = 800;
	lpMMI->ptMinTrackSize.y = 540;

	CMDIFrameWnd::OnGetMinMaxInfo(lpMMI);
}
