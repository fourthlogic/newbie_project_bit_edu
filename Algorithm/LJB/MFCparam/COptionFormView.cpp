// COptionFormView.cpp: 구현 파일
//

#include "pch.h"
#include "MFCparam.h"
#include "COptionFormView.h"

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
	ON_BN_CLICKED(IDC_BUTTON_OPTION_SAVE, &COptionFormView::OnBnClickedButtonOptionSave)
	ON_EN_CHANGE(IDC_EDIT_OPTION_DIST, &COptionFormView::OnEnChangeEditOptionDist)
	ON_EN_CHANGE(IDC_EDIT_OPTION_RMAX, &COptionFormView::OnEnChangeEditOptionRmax)
	ON_EN_CHANGE(IDC_EDIT_OPTION_RMIN, &COptionFormView::OnEnChangeEditOptionRmin)
	ON_EN_CHANGE(IDC_EDIT_OPTION_BGV, &COptionFormView::OnEnChangeEditOptionBgv)
	ON_EN_CHANGE(IDC_EDIT_OPTION_THMAX, &COptionFormView::OnEnChangeEditOptionThmax)
	ON_EN_CHANGE(IDC_EDIT_OPTION_THMIN, &COptionFormView::OnEnChangeEditOptionThmin)
	ON_WM_CREATE()
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



void COptionFormView::OnInitialUpdate() // Doc이 호출
{
	CFormView::OnInitialUpdate();
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	COptionDoc* pDoc = (COptionDoc*)GetDocument();

	strDist = pDoc->m_strDist;
	strRadMax = pDoc->m_strRadMax;
	strRadMin = pDoc->m_strRadMin;
	strBGV = pDoc->m_strBGV;
	strThMax = pDoc->m_strThMax;
	strThMin = pDoc->m_strThMin;
}

void COptionFormView::OnBnClickedButtonOptionSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	// 여기선 값들 유효한지 확인 후 .accdb에 저장하면 될 듯
	//CString strSQL = _T("UPDATE [Params] SET [Params].[File name] = 'change' WHERE((([Params].[File name]) = 'test1'));");
	//db.ExecuteSQL(strSQL);
	
	
}


void COptionFormView::OnEnChangeEditOptionDist()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CImgViewerView* pView = pMain->pImgViewerView;

	COptionDoc* pDoc = (COptionDoc*)GetDocument();
	UpdateData();
	int dist = _ttoi(strDist);
	if (dist > 0 && dist <= 30) // 직교거리 지정 가능한 범위
	{
		pDoc->m_strDist = strDist; // COptionDoc에 저장
		pView->m_Algorithm.SetDistance(dist);
		pView->paraChanged();
	}
	else
	{
		MessageBox(L"범위(1~30)를 벗어났습니다.");
	}
	Invalidate(FALSE);
}


void COptionFormView::OnEnChangeEditOptionRmax()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CImgViewerView* pView = pMain->pImgViewerView;

	COptionDoc* pDoc = (COptionDoc*)GetDocument();
	UpdateData();
	int radMax = _ttoi(strRadMax);
	if (radMax < _ttoi(pDoc->m_strRadMin))
	{
		MessageBox(L"볼 크기 최소값보다 작습니다.");
		return;
	}

	if (radMax > 0 && radMax <= 10) // 볼 최대크기 지정 가능한 범위
	{
		pDoc->m_strRadMax = strRadMax;
		//pView->m_Algorithm.SetCircleValue(_ttoi(pDoc->m_strRadMin), radMax, _ttoi(pDoc->m_strBGV));
		//pView->paraChanged();
	}
	else
	{
		MessageBox(L"범위(1~10)를 벗어났습니다.");
		return;
	}
}


void COptionFormView::OnEnChangeEditOptionRmin()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CImgViewerView* pView = pMain->pImgViewerView;

	COptionDoc* pDoc = (COptionDoc*)GetDocument();
	UpdateData();

	int radMin = _ttoi(strRadMin);
	if (radMin > _ttoi(pDoc->m_strRadMax))
	{
		MessageBox(L"볼 크기 최대값보다 큽니다.");
		return;
	}

	if (radMin > 0 && radMin <= 10) // 볼 최소크기 지정 가능한 범위
	{
		pDoc->m_strRadMin = strRadMin;
		//pView->m_Algorithm.SetCircleValue(radMin, _ttoi(pDoc->m_strRadMax), _ttoi(pDoc->m_strBGV));
		//pView->paraChanged();
	}
	else
	{
		MessageBox(L"범위(1~10)를 벗어났습니다.");
	}
}


void COptionFormView::OnEnChangeEditOptionBgv()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CImgViewerView* pView = pMain->pImgViewerView;

	COptionDoc* pDoc = (COptionDoc*)GetDocument();
	UpdateData();
	int bgv = _ttoi(strBGV);
	if (bgv >= 0 && bgv <= 255) // Ball Gray Value 지정 가능한 범위
	{
		pDoc->m_strBGV = strBGV;
		pView->m_Algorithm.SetCircleValue(_ttoi(pDoc->m_strRadMin), _ttoi(pDoc->m_strRadMax), bgv);
		pView->paraChanged();
	}
	else
	{
		MessageBox(L"범위를 벗어났습니다.");
	}
}


void COptionFormView::OnEnChangeEditOptionThmax()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CImgViewerView* pView = pMain->pImgViewerView;

	COptionDoc* pDoc = (COptionDoc*)GetDocument();
	UpdateData();
	int thmax = _ttoi(strThMax);
	if (thmax < _ttoi(pDoc->m_strThMin))
	{
		MessageBox(L"Threshold 최소값보다 작습니다.");
		return;
	}
	if (thmax >= 0 && thmax <= 255) // Ball Gray Value 지정 가능한 범위
	{
		pDoc->m_strThMax = strThMax;
		pView->m_Algorithm.SetThreshValue(_ttoi(pDoc->m_strThMin), thmax);
		pView->paraChanged();
	}
	else
	{
		MessageBox(L"범위를 벗어났습니다.");
	}
}


void COptionFormView::OnEnChangeEditOptionThmin()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CImgViewerView* pView = pMain->pImgViewerView;

	COptionDoc* pDoc = (COptionDoc*)GetDocument();
	UpdateData();
	int thmin = _ttoi(strThMin);
	if (thmin > _ttoi(pDoc->m_strThMax))
	{
		MessageBox(L"Threshold 최대값보다 큽니다.");
		return;
	}
	if (thmin >= 0 && thmin <= 255) // Ball Gray Value 지정 가능한 범위
	{
		pDoc->m_strThMin = strThMin;
		pView->m_Algorithm.SetThreshValue(thmin, _ttoi(pDoc->m_strThMax));
	}
	else
	{
		MessageBox(L"범위를 벗어났습니다.");
	}
}


int COptionFormView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	CDatabase db;
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	CString strCon = _T("Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=D://access test folder//Database2.accdb");
	if (!db.OpenEx(strCon))
	{
		MessageBox(L"설정값 저장 파일이 연결되지 않았습니다.");
	}
	//CRecordset record(&db);
	//record.Open(CRecordset::snapshot, _T("SELECT * FROM [Params]"));


	COptionDoc* pDoc = (COptionDoc*)GetDocument();
	strDist.Format(_T("%d"), 20);
	strRadMax.Format(_T("%d"), 4);
	strRadMin.Format(_T("%d"), 2);
	strBGV.Format(_T("%d"), 80);
	strThMax.Format(_T("%d"), 158);
	strThMin.Format(_T("%d"), 100);
	pDoc->m_strDist = strDist;
	pDoc->m_strRadMax = strRadMax;
	pDoc->m_strRadMin = strRadMin;
	pDoc->m_strBGV = strBGV;
	pDoc->m_strThMax = strThMax;
	pDoc->m_strThMin = strThMin;





	GetParent()->SetWindowText(L"파라미터 설정 뷰");

	return 0;
}
