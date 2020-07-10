// COptionFormView.cpp: 구현 파일
//

#include "pch.h"
#include "MFCparam.h"
#include "COptionFormView.h"

// COptionFormView

IMPLEMENT_DYNCREATE(COptionFormView, CFormView)

COptionFormView::COptionFormView()
	: CFormView(IDD_COptionFormView)
	, m_strDist(_T(""))
	, m_strAdjDist(_T(""))
	, m_strRadMax(_T(""))
	, m_strRadMin(_T(""))
	, m_strBGV(_T(""))
	, m_nRuntime(0)
{

}

COptionFormView::~COptionFormView()
{
}

void COptionFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_OPTION_DIST, m_strDist);
	DDX_Text(pDX, IDC_EDIT_OPTION_ADJDIST, m_strAdjDist);
	DDX_Text(pDX, IDC_EDIT_OPTION_RMAX, m_strRadMax);
	DDX_Text(pDX, IDC_EDIT_OPTION_RMIN, m_strRadMin);
	DDX_Text(pDX, IDC_EDIT_OPTION_BGV, m_strBGV);
	DDX_Control(pDX, IDC_CHECK_Rotate, m_RotateCheck);
}

BEGIN_MESSAGE_MAP(COptionFormView, CFormView)
	ON_COMMAND(ID_OPTION_OPEN, &COptionFormView::OnOptionOpen)
	ON_COMMAND(ID_OPTION_SAVE, &COptionFormView::OnOptionSave)
	ON_COMMAND(ID_APP_EXIT, &COptionFormView::OnAppExit)
	ON_BN_CLICKED(IDC_BUTTON_DO, &COptionFormView::OnBnClickedButtonDo)
	ON_BN_CLICKED(IDC_BUTTON_OPTION_SAVE, &COptionFormView::OnBnClickedButtonOptionSave)
	ON_EN_UPDATE(IDC_EDIT_OPTION_DIST, &COptionFormView::OnEnUpdateEditOptionDist)
	ON_EN_UPDATE(IDC_EDIT_OPTION_RMAX, &COptionFormView::OnEnUpdateEditOptionRmax)
	ON_EN_UPDATE(IDC_EDIT_OPTION_RMIN, &COptionFormView::OnEnUpdateEditOptionRmin)
	ON_EN_UPDATE(IDC_EDIT_OPTION_BGV, &COptionFormView::OnEnUpdateEditOptionBgv)
	ON_WM_CTLCOLOR()
	ON_WM_DRAWITEM()
	ON_WM_CONTEXTMENU()
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

	GetParent()->SetWindowText(_T("설정 창"));
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)); //icon 변경
	GetParent()->SetIcon(hIcon, FALSE); //icon 셋팅
	// 읽기모드 파일 열기
	CString pathName = _T("saveParams.accdb");
	CFileFind pFind;
	BOOL bRet = pFind.FindFile(pathName);
	if (!bRet)
	{
		m_strDist = _T("0");
		m_strAdjDist = _T("0");
		m_strRadMax = _T("0");
		m_strRadMin = _T("0");
		m_strBGV = _T("0");
		UpdateData(FALSE);
		return;
	}
	::CoInitialize(NULL);
	ADODB::_ConnectionPtr m_pConnection;

	HRESULT hr = S_OK;
	// 연결 인스턴스 생성
	hr = m_pConnection.CreateInstance(__uuidof(Connection));
	if (SUCCEEDED(hr))
	{
		// DB연결
		CString temp;
		temp.Format(_T("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;"), pathName);
		m_pConnection->ConnectionString = _bstr_t(temp);
		m_pConnection->Open("", "", "", adModeUnknown);

		ADODB::_RecordsetPtr record; // DB의 레코드셋

		CString query;// 쿼리문이 저장될 변수
		query = "SELECT * FROM (Params)";
		_bstr_t executeQuery = query;

		record = m_pConnection->Execute(executeQuery, NULL, adCmdUnspecified);
		m_strDist = record->Fields->GetItem("Distance")->Value;
		m_strAdjDist = record->Fields->GetItem("AdjDist")->Value;
		m_strRadMax = record->Fields->GetItem("RadMax")->Value;
		m_strRadMin = record->Fields->GetItem("RadMin")->Value;
		m_strBGV = record->Fields->GetItem("BGV")->Value;

		record->Close();
		record = NULL;
		m_pConnection->Close();
		m_pConnection = NULL;
	}
	::CoUninitialize();
	double tempadj = _ttof(m_strAdjDist);
	m_strAdjDist.Format(_T("%.1lf"), tempadj);
	UpdateData(FALSE);
}

void COptionFormView::OnBnClickedButtonOptionSave() // 설정 창 -> 저장버튼
{
	this->OnOptionSave(); // 메뉴의 저장
}

void COptionFormView::OnAppExit()
{
	UpdateData();

	m_nDist = _ttoi(m_strDist);
	m_adjDist = _ttof(m_strAdjDist);
	m_nRadMax = _ttoi(m_strRadMax);
	m_nRadMin = _ttoi(m_strRadMin);
	m_nBGV = _ttoi(m_strBGV);

	// 쓰기모드 파일 열기
	CString pathName = _T("saveParams.accdb");
	::CoInitialize(NULL);
	ADOX::_CatalogPtr m_pCatalog = NULL;
	ADOX::_TablePtr m_pTable = NULL;
	ADOX::_ColumnPtr m_pColumn = NULL;
	ADODB::_ConnectionPtr m_pConnection = NULL;

	// 각 Object에 대한 Instance 생성
	m_pCatalog.CreateInstance(__uuidof (Catalog));
	m_pTable.CreateInstance(__uuidof (Table));

	CString strPath;
	strPath.Format(_T("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;"), pathName);

	CString queryInsert, queryUpdate; // 쿼리문이 저장될 변수
	_bstr_t executeQuery;

	// 파일이 존재하지 않을 경우
	if (!PathFileExists(pathName))
	{
		// 파일 생성
		m_pCatalog->Create((_bstr_t)strPath);

		// 테이블 항목 생성
		m_pTable->PutName("Params");
		m_pTable->Columns->Append("Distance", ADOX::adVarWChar, 10);
		m_pTable->Columns->Append("AdjDist", ADOX::adVarWChar, 10);
		m_pTable->Columns->Append("RadMax", ADOX::adVarWChar, 10);
		m_pTable->Columns->Append("RadMin", ADOX::adVarWChar, 10);
		m_pTable->Columns->Append("BGV", ADOX::adVarWChar, 10);

		m_pCatalog->Tables->Append(_variant_t((IDispatch*)m_pTable));

		m_pCatalog = NULL;
		m_pTable = NULL;

		queryInsert.Format(_T("INSERT INTO Params(Distance, AdjDist, RadMax, RadMin, BGV) values('%d', '%lf', '%d', '%d', '%d')"), m_nDist, m_adjDist, m_nRadMax, m_nRadMin, m_nBGV);
		executeQuery = queryInsert;
	}
	// 파일이 이미 존재할 경우
	else
	{
		queryUpdate.Format(_T("UPDATE [Params] SET [Distance] = '%d', [AdjDist] = '%lf', [Radmax] = '%d', [RadMin] = '%d', [BGV] = '%d'"), m_nDist, m_adjDist, m_nRadMax, m_nRadMin, m_nBGV);
		executeQuery = queryUpdate;
	}

	// 파일에 연결 후 값 저장
	try
	{
		HRESULT hr = S_OK;
		// 연결 인스턴스 생성
		hr = m_pConnection.CreateInstance(__uuidof(Connection));
		if (SUCCEEDED(hr))
		{
			// DB 연결
			m_pConnection->ConnectionString = _bstr_t(strPath);
			m_pConnection->Open("", "", "", adModeUnknown);
		}

		m_pConnection->BeginTrans();
		m_pConnection->Execute(executeQuery, NULL, adCmdUnspecified);
		m_pConnection->CommitTrans();
		m_pConnection->Close();
		m_pConnection = NULL;
	}
	catch (_com_error& e)
	{
		AfxMessageBox(e.Description());
	}
	::CoUninitialize();

	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	pMain->PostMessage(WM_CLOSE, NULL, NULL);
}

void COptionFormView::OnBnClickedButtonDo()
{
	if (m_RotateCheck.GetCheck() == 0) {
		std::chrono::system_clock::time_point StartTime = std::chrono::system_clock::now();
		UpdateData();

		CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
		CImgViewerView* pView = theApp.pImgViewerView;
		m_nDist = _ttoi(m_strDist);
		m_adjDist = _ttof(m_strAdjDist);
		m_nRadMax = _ttoi(m_strRadMax);
		m_nRadMin = _ttoi(m_strRadMin);
		m_nBGV = _ttoi(m_strBGV);

		pView->m_Algorithm.SetDistance(m_nDist);
		pView->m_Algorithm.SetAdjDist(m_adjDist);
		pView->m_Algorithm.SetCircleValue(m_nRadMin, m_nRadMax, m_nBGV);
		pView->paraChanged(false);
		std::chrono::system_clock::time_point EndTime = std::chrono::system_clock::now();
		std::chrono::milliseconds mill = std::chrono::duration_cast<std::chrono::milliseconds>(EndTime - StartTime);
		m_nRuntime = mill.count();
		CString runtime;
		runtime.Format(_T("Runtime : %d ms"), m_nRuntime);
		pMain->m_wndStatusBar.SetPaneText(0, runtime);
	}
	else
	{
		std::chrono::system_clock::time_point StartTime = std::chrono::system_clock::now();
		UpdateData();

		CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
		CImgViewerView* pView = theApp.pImgViewerView;
		m_nDist = _ttoi(m_strDist);
		m_adjDist = _ttof(m_strAdjDist);
		m_nRadMax = _ttoi(m_strRadMax);
		m_nRadMin = _ttoi(m_strRadMin);
		m_nBGV = _ttoi(m_strBGV);

		pView->m_Algorithm.SetDistance(m_nDist);
		pView->m_Algorithm.SetAdjDist(m_adjDist);
		pView->m_Algorithm.SetCircleValue(m_nRadMin, m_nRadMax, m_nBGV);
		pView->paraChanged(true);
		std::chrono::system_clock::time_point EndTime = std::chrono::system_clock::now();
		std::chrono::milliseconds mill = std::chrono::duration_cast<std::chrono::milliseconds>(EndTime - StartTime);
		m_nRuntime = mill.count();
		CString runtime;
		runtime.Format(_T("Runtime : %d ms"), m_nRuntime);
		pMain->m_wndStatusBar.SetPaneText(0, runtime);
	}
	UpdateData(FALSE);
}

void COptionFormView::OnOptionOpen() // 설정파일 열기
{
	static TCHAR BASED_CODE szFilter[] = _T("데이터 파일(*.accdb) | *.ACCDB;*.accdb |모든파일(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.accdb"), _T(""), OFN_HIDEREADONLY, szFilter);
	if (dlg.DoModal() == IDOK)
	{
		CString pathName = dlg.GetPathName();
		::CoInitialize(NULL);
		ADODB::_ConnectionPtr m_pConnection;
		try
		{
			HRESULT hr = S_OK;
			// 연결 인스턴스 생성
			hr = m_pConnection.CreateInstance(__uuidof(Connection));
			if (SUCCEEDED(hr))
			{
				// DB연결
				CString temp;
				temp.Format(_T("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;"), pathName);
				m_pConnection->ConnectionString = _bstr_t(temp);
				m_pConnection->Open("", "", "", adModeUnknown);

				ADODB::_RecordsetPtr record; // DB의 레코드셋

				CString query;// 쿼리문이 저장될 변수
				query = "SELECT * FROM (Params)";
				_bstr_t executeQuery = query;

				record = m_pConnection->Execute(executeQuery, NULL, adCmdUnspecified);
				m_strDist = record->Fields->GetItem("Distance")->Value;
				m_strAdjDist = record->Fields->GetItem("AdjDist")->Value;
				m_strRadMax = record->Fields->GetItem("RadMax")->Value;
				m_strRadMin = record->Fields->GetItem("RadMin")->Value;
				m_strBGV = record->Fields->GetItem("BGV")->Value;

				record->Close();
				record = NULL;
				m_pConnection->Close();
				m_pConnection = NULL;
				::CoUninitialize();
			}
		}
		catch (_com_error& e)
		{
			AfxMessageBox(e.Description());
		}
	}
	double tempadj = _ttof(m_strAdjDist);
	m_strAdjDist.Format(_T("%.1lf"), tempadj);
	this->CheckParams();
}

void COptionFormView::OnOptionSave()
{
	// Edit control에 직접 변경한 설정값들 적용
	UpdateData();
	m_nDist = _ttoi(m_strDist);
	m_adjDist = _ttof(m_strAdjDist);
	m_nRadMax = _ttoi(m_strRadMax);
	m_nRadMin = _ttoi(m_strRadMin);
	m_nBGV = _ttoi(m_strBGV);

	static TCHAR BASED_CODE szFilter[] = _T("데이터 파일(*.accdb) | *.accdb;*.ACCDB |모든파일(*.*)|*.*||");
	CFileDialog dlg(FALSE, _T("*.accdb"), _T(""), OFN_OVERWRITEPROMPT, szFilter);
	if (dlg.DoModal() == IDOK)
	{
		CString pathName = dlg.GetPathName();
		::CoInitialize(NULL);
		ADOX::_CatalogPtr m_pCatalog = NULL;
		ADOX::_TablePtr m_pTable = NULL;
		ADOX::_ColumnPtr m_pColumn = NULL;
		ADODB::_ConnectionPtr m_pConnection = NULL;

		// 각 Object에 대한 Instance 생성
		m_pCatalog.CreateInstance(__uuidof (Catalog));
		m_pTable.CreateInstance(__uuidof (Table));

		CString strPath;
		strPath.Format(_T("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;"), pathName);

		CString queryInsert, queryUpdate; // 쿼리문이 저장될 변수
		_bstr_t executeQuery;

		// 파일이 존재하지 않을 경우
		if (!PathFileExists(pathName))
		{
			// 파일 생성
			m_pCatalog->Create((_bstr_t)strPath);

			// 테이블 항목 생성
			m_pTable->PutName("Params");
			m_pTable->Columns->Append("Distance", ADOX::adVarWChar, 10);
			m_pTable->Columns->Append("AdjDist", ADOX::adVarWChar, 10);
			m_pTable->Columns->Append("RadMax", ADOX::adVarWChar, 10);
			m_pTable->Columns->Append("RadMin", ADOX::adVarWChar, 10);
			m_pTable->Columns->Append("BGV", ADOX::adVarWChar, 10);

			m_pCatalog->Tables->Append(_variant_t((IDispatch*)m_pTable));

			m_pCatalog = NULL;
			m_pTable = NULL;

			queryInsert.Format(_T("INSERT INTO Params(Distance, AdjDist, RadMax, RadMin, BGV) values('%d', '%lf', '%d', '%d', '%d')"), m_nDist, m_adjDist, m_nRadMax, m_nRadMin, m_nBGV);
			executeQuery = queryInsert;
		}
		// 파일이 이미 존재할 경우
		else
		{
			queryUpdate.Format(_T("UPDATE [Params] SET [Distance] = '%d', [AdjDist] = '%lf', [Radmax] = '%d', [RadMin] = '%d', [BGV] = '%d'"), m_nDist, m_adjDist, m_nRadMax, m_nRadMin, m_nBGV);
			executeQuery = queryUpdate;
		}
		
		// 파일에 연결 후 값 저장
		try
		{
			HRESULT hr = S_OK;
			// 연결 인스턴스 생성
			hr = m_pConnection.CreateInstance(__uuidof(Connection));
			if (SUCCEEDED(hr))
			{
				// DB 연결
				m_pConnection->ConnectionString = _bstr_t(strPath);
				m_pConnection->Open("", "", "", adModeUnknown);
			}

			m_pConnection->BeginTrans();
			m_pConnection->Execute(executeQuery, NULL, adCmdUnspecified);
			m_pConnection->CommitTrans();
			m_pConnection->Close();
			m_pConnection = NULL;
		}
		catch (_com_error& e)
		{
			AfxMessageBox(e.Description());
		}
		::CoUninitialize();
	}
}

void COptionFormView::OnEnUpdateEditOptionDist()
{
	UpdateData();
	int dist;
	dist = _ttoi(m_strDist);
	if (dist > 50)
	{
		MessageBox(_T("Distance 범위(0~50)를 초과하였습니다."));
		dist = 50;
	}
	else if (dist < 0)
	{
		MessageBox(_T("Distance 범위(0~50)를 초과하였습니다."));
		dist = 0;
	}
	m_strDist.Format(_T("%d"), dist);
	UpdateData(FALSE);
}

void COptionFormView::OnEnUpdateEditOptionRmax()
{
	UpdateData();
	int rmax, rmin;
	rmax = _ttoi(m_strRadMax);
	rmin = _ttoi(m_strRadMin);
	if (rmax > 20)
	{
		MessageBox(_T("최대 반지름 범위(1~20)를 초과하였습니다."));
		rmax = 20;
	}
	else if (rmax < 1)
	{
		MessageBox(_T("최대 반지름 범위(1~20)를 초과하였습니다."));
		rmax = 1;
	}

	if (rmax < rmin)
	{
		MessageBox(_T("최대 반지름 값이 최소 반지름보다 작습니다."));
		rmax = rmin;
	}
	m_strRadMax.Format(_T("%d"), rmax);
	UpdateData(FALSE);
}

void COptionFormView::OnEnUpdateEditOptionRmin()
{
	UpdateData();
	int rmax, rmin;
	rmax = _ttoi(m_strRadMax);
	rmin = _ttoi(m_strRadMin);
	if (rmin > 20)
	{
		MessageBox(_T("최소 반지름 범위(1~20)를 초과하였습니다."));
		rmin = 20;
	}
	else if (rmin < 1)
	{
		MessageBox(_T("최소 반지름 범위(1~20)를 초과하였습니다."));
		rmin = 1;
	}

	if (rmin > rmax)
	{
		MessageBox(_T("최소 반지름 값이 최대 반지름보다 큽니다."));
		rmin = rmax;
	}
	m_strRadMin.Format(_T("%d"), rmin);
	UpdateData(FALSE);
}

void COptionFormView::OnEnUpdateEditOptionBgv()
{
	UpdateData();
	int bgv;
	bgv = _ttoi(m_strBGV);
	if (bgv > 255)
	{
		MessageBox(_T("볼 GV 임계값 범위(0~255)를 초과하였습니다."));
		bgv = 255;
	}
	else if (bgv < 0)
	{
		MessageBox(_T("볼 GV 임계값 범위(0~255)를 초과하였습니다."));
		bgv = 0;
	}
	m_strBGV.Format(_T("%d"), bgv);
	UpdateData(FALSE);
}


HBRUSH COptionFormView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
	switch (nCtlColor) {
	case CTLCOLOR_DLG:   /// 다이얼로그 배경색을 white로.
	{
		return (HBRUSH)GetStockObject(WHITE_BRUSH);
		break;
	}
	case CTLCOLOR_STATIC:
	{
		pDC->SetBkMode(TRANSPARENT);   // static text 배경색 투명
		hbr = (HBRUSH)RGB(255, 255, 255);
		break;
	default:
		hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
		break;
	}
	}
	return hbr;
}

void COptionFormView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == IDC_BUTTON_DO || nIDCtl == IDC_BUTTON_OPTION_SAVE)
	{
		UINT state;
		CDC dc;
		RECT rctBtn;
		TCHAR buffer[MAX_PATH];
		ZeroMemory(buffer, MAX_PATH);     

		dc.Attach(lpDrawItemStruct->hDC);  
		rctBtn = lpDrawItemStruct->rcItem;     
		dc.Draw3dRect(&rctBtn, RGB(255, 255, 255), RGB(68, 114, 196));
		dc.FillSolidRect(&rctBtn, RGB(255, 255, 255)); 
		state = lpDrawItemStruct->itemState;   
		if ((state & ODS_SELECTED))
		{
			dc.DrawEdge(&rctBtn, EDGE_SUNKEN, BF_RECT);
		}
		else
		{
			dc.DrawEdge(&rctBtn, EDGE_RAISED, BF_RECT);
		}

		dc.SetBkColor(RGB(255, 255, 255));   // 텍스트 배경 색
		dc.SetTextColor(RGB(0, 0, 0));     // 텍스트 색

		::GetWindowText(lpDrawItemStruct->hwndItem, buffer, MAX_PATH); 
		dc.DrawText(buffer, &rctBtn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		dc.Detach();  
	}
	CFormView::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void COptionFormView::CheckParams()
{
	int dist;
	int rmax, rmin;
	int bgv;
	dist = _ttoi(m_strDist);
	rmax = _ttoi(m_strRadMax);
	rmin = _ttoi(m_strRadMin);
	bgv = _ttoi(m_strBGV);
	if (dist > 50)
	{
		dist = 50;
	}
	else if (dist < 0)
	{
		dist = 0;
	}
	m_strDist.Format(_T("%d"), dist);

	if (rmax > 20)
	{
		rmax = 20;
	}
	else if (rmax < 1)
	{
		rmax = 1;
	}

	if (rmax < rmin)
	{
		rmax = rmin;
	}
	m_strRadMax.Format(_T("%d"), rmax);
	if (rmin > 20)
	{
		rmin = 20;
	}
	else if (rmin < 1)
	{
		rmin = 1;
	}

	if (rmin > rmax)
	{
		rmin = rmax;
	}
	m_strRadMin.Format(_T("%d"), rmin);
	if (bgv > 255)
	{
		bgv = 255;
	}
	else if (bgv < 0)
	{
		bgv = 0;
	}
	m_strBGV.Format(_T("%d"), bgv);
	UpdateData(FALSE);
}

void COptionFormView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu popup;
	CMenu* pMenu;
	popup.LoadMenuW(IDR_MENU_OPTION);
	pMenu = popup.GetSubMenu(1);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN || TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
}

