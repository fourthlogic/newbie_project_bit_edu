// COptionFormView.cpp: 구현 파일
//

#include "pch.h"
#include "MFCparam.h"
#include "COptionFormView.h"
#include "MainFrm.h"

// COptionFormView

IMPLEMENT_DYNCREATE(COptionFormView, CFormView)

COptionFormView::COptionFormView()
	: CFormView(IDD_COptionFormView)
	, strDist(_T(""))
	, strRadMax(_T(""))
	, strRadMin(_T(""))
	, strBGV(_T(""))
	, strThMax(_T(""))
	, strThMin(_T(""))
{

}

COptionFormView::~COptionFormView()
{
}

void COptionFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_OPTION_DIST, strDist);
	DDX_Text(pDX, IDC_EDIT_OPTION_RMAX, strRadMax);
	DDX_Text(pDX, IDC_EDIT_OPTION_RMIN, strRadMin);
	DDX_Text(pDX, IDC_EDIT_OPTION_BGV, strBGV);
	DDX_Text(pDX, IDC_EDIT_OPTION_THMAX, strThMax);
	DDX_Text(pDX, IDC_EDIT_OPTION_THMIN, strThMin);
}

BEGIN_MESSAGE_MAP(COptionFormView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &COptionFormView::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_OPTION_SAVE, &COptionFormView::OnBnClickedButtonOptionSave)
END_MESSAGE_MAP()


// COptionFormView 진단

#ifdef _DEBUG
void COptionFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void COptionFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// COptionFormView 메시지 처리기



void COptionFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	GetParent()->SetWindowText(L"파라미터 설정 뷰");
}


void COptionFormView::OnBnClickedButtonTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString test;
	CMainFrame* ptt;
	ptt = (CMainFrame*)AfxGetMainWnd();

	test.Format(_T("%d"), ptt->nn);
	MessageBox(test);
	ptt->nn++;
}


void COptionFormView::OnBnClickedButtonOptionSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
