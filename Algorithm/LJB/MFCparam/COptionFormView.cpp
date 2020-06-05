﻿// COptionFormView.cpp: 구현 파일
//
#include "pch.h"
#include "MFCparam.h"
#include "COptionFormView.h"


// 테스트용
#include "myADO.h"
//////////

//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console");
// COptionFormView

IMPLEMENT_DYNCREATE(COptionFormView, CFormView)

COptionFormView::COptionFormView()
	: CFormView(IDD_COptionFormView)
	, m_strDist(_T(""))
	, m_strRadMax(_T(""))
	, m_strRadMin(_T(""))
	, m_strBGV(_T(""))
	//, strThMax(_T(""))
	//, strThMin(_T(""))
{

}

COptionFormView::~COptionFormView()
{
}

void COptionFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_OPTION_DIST, m_strDist);
	DDX_Text(pDX, IDC_EDIT_OPTION_RMAX, m_strRadMax);
	DDX_Text(pDX, IDC_EDIT_OPTION_RMIN, m_strRadMin);
	DDX_Text(pDX, IDC_EDIT_OPTION_BGV, m_strBGV);
	//DDX_Text(pDX, IDC_EDIT_OPTION_THMAX, strThMax);
	//DDX_Text(pDX, IDC_EDIT_OPTION_THMIN, strThMin);
}

BEGIN_MESSAGE_MAP(COptionFormView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_OPTION_SAVE, &COptionFormView::OnBnClickedButtonOptionSave)
//	ON_EN_CHANGE(IDC_EDIT_OPTION_DIST, &COptionFormView::OnEnChangeEditOptionDist)
//	ON_EN_CHANGE(IDC_EDIT_OPTION_RMAX, &COptionFormView::OnEnChangeEditOptionRmax)
//	ON_EN_CHANGE(IDC_EDIT_OPTION_RMIN, &COptionFormView::OnEnChangeEditOptionRmin)
//	ON_EN_CHANGE(IDC_EDIT_OPTION_BGV, &COptionFormView::OnEnChangeEditOptionBgv)
	//ON_EN_CHANGE(IDC_EDIT_OPTION_THMAX, &COptionFormView::OnEnChangeEditOptionThmax)
	//ON_EN_CHANGE(IDC_EDIT_OPTION_THMIN, &COptionFormView::OnEnChangeEditOptionThmin)
	ON_WM_CREATE()
	ON_COMMAND(ID_APP_EXIT, &COptionFormView::OnAppExit)
	ON_COMMAND(ID_OPTION_OPEN, &COptionFormView::OnOptionOpen)
	ON_BN_CLICKED(IDC_BUTTON_DO, &COptionFormView::OnBnClickedButtonDo)
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
}


//void COptionFormView::OnEnChangeEditOptionDist()
//{
//	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
//	// CFormView::OnInitDialog() 함수를 재지정 
//	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
//	// 이 알림 메시지를 보내지 않습니다.
//
//	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
//	CImgViewerView* pView = pMain->pImgViewerView;
//
//	//COptionDoc* pDoc = (COptionDoc*)GetDocument();
//	UpdateData();
//	int dist = _ttoi(m_strDist);
//	if (dist > 0 && dist <= 30) // 직교거리 지정 가능한 범위
//		paraChanged2();
//	else
//	{
//		MessageBox(_T("범위(1~30)를 벗어났습니다."));
//	}
//	Invalidate(FALSE);
//}


//void COptionFormView::OnEnChangeEditOptionRmax()
//{
//	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
//	// CFormView::OnInitDialog() 함수를 재지정 
//	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
//	// 이 알림 메시지를 보내지 않습니다.
//
//	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
//	CImgViewerView* pView = pMain->pImgViewerView;
//
//	//COptionDoc* pDoc = (COptionDoc*)GetDocument();
//	UpdateData();
//	int radMax = _ttoi(m_strRadMax);
//	if (radMax < _ttoi(m_strRadMin))
//	{
//		MessageBox(_T("볼 크기 최소값보다 작습니다."));
//		return;
//	}
//
//	if (radMax > 0 && radMax <= 10) // 볼 최대크기 지정 가능한 범위
//		paraChanged2();
//	else
//	{
//		MessageBox(_T("범위(1~10)를 벗어났습니다."));
//		return;
//	}
//}


//void COptionFormView::OnEnChangeEditOptionRmin()
//{
//	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
//	// CFormView::OnInitDialog() 함수를 재지정 
//	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
//	// 이 알림 메시지를 보내지 않습니다.
//
//	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
//	CImgViewerView* pView = pMain->pImgViewerView;
//
//	//COptionDoc* pDoc = (COptionDoc*)GetDocument();
//	UpdateData();
//
//	int radMin = _ttoi(m_strRadMin);
//	if (radMin > _ttoi(m_strRadMax))
//	{
//		MessageBox(_T("볼 크기 최대값보다 큽니다."));
//		return;
//	}
//
//	if (radMin > 0 && radMin <= 10) // 볼 최소크기 지정 가능한 범위
//		paraChanged2();
//	else
//	{
//		MessageBox(_T("범위(1~10)를 벗어났습니다."));
//	}
//}


//void COptionFormView::OnEnChangeEditOptionBgv()
//{
//	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
//	// CFormView::OnInitDialog() 함수를 재지정 
//	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
//	// 이 알림 메시지를 보내지 않습니다.
//
//	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
//	CImgViewerView* pView = pMain->pImgViewerView;
//
//	//COptionDoc* pDoc = (COptionDoc*)GetDocument();
//	UpdateData();
//	int bgv = _ttoi(m_strBGV);
//	if (bgv > 0 && bgv <= 255) // Ball Gray Value 지정 가능한 범위
//		paraChanged2();
//	else
//	{
//		MessageBox(_T("범위를 벗어났습니다."));
//	}
//}


//void COptionFormView::OnEnChangeEditOptionThmax()
//{
//	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
//	// CFormView::OnInitDialog() 함수를 재지정 
//	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
//	// 이 알림 메시지를 보내지 않습니다.
//
//	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
//	CImgViewerView* pView = pMain->pImgViewerView;
//
//	//COptionDoc* pDoc = (COptionDoc*)GetDocument();
//	UpdateData();
//	int thmax = _ttoi(strThMax);
//	if (thmax < _ttoi(strThMin))
//	{
//		MessageBox(_T("Threshold 최소값보다 작습니다."));
//		return;
//	}
//	if (thmax > 0 && thmax <= 255)
//	{
//		//pDoc->m_strThMax = strThMax;
//		pView->m_Algorithm.SetThreshValue(_ttoi(strThMin), thmax);
//		pView->paraChanged();
//		paraUpdate(_T("ThMax"), strThMax);
//	}
//	else
//	{
//		MessageBox(_T("범위를 벗어났습니다."));
//		return;
//	}
//}
//
//
//void COptionFormView::OnEnChangeEditOptionThmin()
//{
//	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
//	// CFormView::OnInitDialog() 함수를 재지정 
//	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
//	// 이 알림 메시지를 보내지 않습니다.
//
//	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
//	CImgViewerView* pView = pMain->pImgViewerView;
//
//	//COptionDoc* pDoc = (COptionDoc*)GetDocument();
//	UpdateData();
//	int thmin = _ttoi(strThMin);
//	if (thmin > _ttoi(strThMax))
//	{
//		MessageBox(_T("Threshold 최대값보다 큽니다."));
//		return;
//	}
//	if (thmin > 0 && thmin <= 255)
//	{
//		//pDoc->m_strThMin = strThMin;
//		pView->m_Algorithm.SetThreshValue(thmin, _ttoi(strThMax));
//		pView->paraChanged();
//		paraUpdate(_T("ThMin"), strThMin);
//	}
//	else
//	{
//		MessageBox(_T("범위를 벗어났습니다."));
//		return;
//	}
//}


int COptionFormView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	CDatabase db;
	CString strCon = _T("Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=D://access test folder//Database2.accdb");
	if (!db.OpenEx(strCon))
	{
		MessageBox(_T("설정값 저장 파일이 연결되지 않았습니다."));
	}
	CRecordset rs(&db);
	try
	{
		rs.Open(CRecordset::forwardOnly, _T("SELECT * FROM [Params]"));

		rs.GetFieldValue(_T("Distance"), m_strDist);
		rs.GetFieldValue(_T("RadMax"), m_strRadMax);
		rs.GetFieldValue(_T("RadMin"), m_strRadMin);
		rs.GetFieldValue(_T("BGV"), m_strBGV);
		//rs.GetFieldValue(_T("ThMax"), strThMax);
		//rs.GetFieldValue(_T("ThMin"), strThMin);

		m_nDist = _ttoi(m_strDist);
		m_nRadMax = _ttoi(m_strRadMax);
		m_nRadMin = _ttoi(m_strRadMin);
		m_nBGV = _ttoi(m_strBGV);

		db.Close();
	}
	catch(CDBException *e)
	{
		e->ReportError();
	}
	
	GetParent()->SetWindowText(_T("설정 창"));

	return 0;
}

void COptionFormView::OnBnClickedButtonOptionSave() // Edit Control의 값을 새로운 accdb파일에 저장
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	myADO myado;
	myado.makeFile();

}

void COptionFormView::OnAppExit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDatabase db;
	CString strCon = _T("Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=D://access test folder//Database2.accdb");
	if (!db.OpenEx(strCon))
	{
		MessageBox(_T("설정값 저장 파일이 연결되지 않았습니다."));
	}
	CString strUpdate;
	strUpdate.Format(_T("UPDATE [Params] SET [%s] = '%d' WHERE([file name] = 'default');"), _T("Distance"), _ttoi(m_strDist));
	db.ExecuteSQL(strUpdate);
	strUpdate.Format(_T("UPDATE [Params] SET [%s] = '%d' WHERE([file name] = 'default');"), _T("RadMax"), _ttoi(m_strRadMax));
	db.ExecuteSQL(strUpdate);
	strUpdate.Format(_T("UPDATE [Params] SET [%s] = '%d' WHERE([file name] = 'default');"), _T("RadMin"), _ttoi(m_strRadMin));
	db.ExecuteSQL(strUpdate);
	strUpdate.Format(_T("UPDATE [Params] SET [%s] = '%d' WHERE([file name] = 'default');"), _T("BGV"), _ttoi(m_strBGV));
	db.ExecuteSQL(strUpdate);
	//strUpdate.Format(_T("UPDATE [Params] SET [%s] = '%d' WHERE([file name] = 'default');"), _T("ThMax"), _ttoi(strThMax));
	//db.ExecuteSQL(strUpdate);
	//strUpdate.Format(_T("UPDATE [Params] SET [%s] = '%d' WHERE([file name] = 'default');"), _T("ThMin"), _ttoi(strThMin));
	//db.ExecuteSQL(strUpdate);

	db.Close();
	//MessageBox(_T("Option EXIT"));

	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	pMain->PostMessage(WM_CLOSE, NULL, NULL);
}

void COptionFormView::OnOptionOpen() // 설정파일 열기
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	static TCHAR BASED_CODE szFilter[] = _T("데이터 파일(*.accdb) | *.ACCDB;*.accdb |모든파일(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.accdb"), _T(""), OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal())
	{
		CString pathName = dlg.GetPathName();
		myADO ado;
		ADODB::_ConnectionPtr m_pConnection;
		try
		{
			HRESULT hr = S_OK;
			//연결 인스턴스 생성
			hr = m_pConnection.CreateInstance(__uuidof(Connection));
			if (SUCCEEDED(hr))
			{
				// DB연결
				CString temp;
				temp.Format(_T("Provider=Microsoft.ACE.OLEDB.16.0;Data Source=%s;"), pathName);
				m_pConnection->ConnectionString = _bstr_t(temp);
				m_pConnection->Open("", "", "", adModeUnknown);

				ADODB::_RecordsetPtr record; // DB의 레코드셋

				CString query;// 쿼리문이 저장될 변수
				query = "SELECT * FROM (Params)";
				_bstr_t executeQuery = query;

				record = m_pConnection->Execute(executeQuery, NULL, adCmdUnspecified);
				m_strDist = record->Fields->GetItem("Distance")->Value;
				m_strRadMax = record->Fields->GetItem("RadMax")->Value;
				m_strRadMin = record->Fields->GetItem("RadMin")->Value;
				m_strBGV = record->Fields->GetItem("BGV")->Value;

				record->Close();
				record = NULL;
			}
		}
		catch (_com_error& e)
		{
			AfxMessageBox(e.Description());
		}
		m_pConnection->Close();
		m_pConnection = NULL;
	}
	UpdateData(FALSE);
}


//void COptionFormView::paraChanged2()
//{
//	// TODO: 여기에 구현 코드 추가.
//
//	// m_Algorithm에 값 Setting
//	UpdateData(FALSE);
//	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
//	CImgViewerView* pView = pMain->pImgViewerView;
//
//	m_nDist = _ttoi(m_strDist);
//	m_nRadMax = _ttoi(m_strRadMax);
//	m_nRadMin = _ttoi(m_strRadMin);
//	m_nBGV = _ttoi(m_strBGV);
//
//	pView->m_Algorithm.SetDistance(m_nDist);
//	pView->m_Algorithm.SetCircleValue(m_nRadMin, m_nRadMax, m_nBGV);
//
//	// 이미지에 적용
//	//pView->paraChanged();
//
//	// DB에 저장
//	CDatabase db;
//	CString strCon = _T("Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=D://access test folder//Database2.accdb");
//	if (!db.OpenEx(strCon))
//	{
//		MessageBox(_T("설정값 저장 파일이 연결되지 않았습니다."));
//	}
//	CString strUpdate;
//	strUpdate.Format(_T("UPDATE [Params] SET [%s] = '%d' WHERE([file name] = 'default');"), _T("Distance"), _ttoi(m_strDist));
//	db.ExecuteSQL(strUpdate);
//	strUpdate.Format(_T("UPDATE [Params] SET [%s] = '%d' WHERE([file name] = 'default');"), _T("RadMax"), _ttoi(m_strRadMax));
//	db.ExecuteSQL(strUpdate);
//	strUpdate.Format(_T("UPDATE [Params] SET [%s] = '%d' WHERE([file name] = 'default');"), _T("RadMin"), _ttoi(m_strRadMin));
//	db.ExecuteSQL(strUpdate);
//	strUpdate.Format(_T("UPDATE [Params] SET [%s] = '%d' WHERE([file name] = 'default');"), _T("BGV"), _ttoi(m_strBGV));
//	db.ExecuteSQL(strUpdate);
//	db.Close();
//}


void COptionFormView::OnBnClickedButtonDo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CImgViewerView* pView = pMain->pImgViewerView;
	m_nDist = _ttoi(m_strDist);
	m_nRadMax = _ttoi(m_strRadMax);
	m_nRadMin = _ttoi(m_strRadMin);
	m_nBGV = _ttoi(m_strBGV);

	pView->m_Algorithm.SetDistance(m_nDist);
	pView->m_Algorithm.SetCircleValue(m_nRadMin, m_nRadMax, m_nBGV);
	pView->paraChanged();
}

