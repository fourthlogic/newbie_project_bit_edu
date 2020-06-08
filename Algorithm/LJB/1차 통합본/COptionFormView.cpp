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
	, m_strRadMax(_T(""))
	, m_strRadMin(_T(""))
	, m_strBGV(_T(""))
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
}

BEGIN_MESSAGE_MAP(COptionFormView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_OPTION_SAVE, &COptionFormView::OnBnClickedButtonOptionSave)
//	ON_WM_CREATE()
	ON_COMMAND(ID_APP_EXIT, &COptionFormView::OnAppExit)
	ON_COMMAND(ID_OPTION_OPEN, &COptionFormView::OnOptionOpen)
	ON_BN_CLICKED(IDC_BUTTON_DO, &COptionFormView::OnBnClickedButtonDo)
	ON_COMMAND(ID_OPTION_SAVE, &COptionFormView::OnOptionSave)
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
	GetParent()->SetWindowText(_T("설정 창"));
	// 읽기모드 파일 열기
	CFile saveFile;
	if (saveFile.Open(_T("saveFile.dat"), CFile::modeRead))
	{
		CArchive ar(&saveFile, CArchive::load);
		ar >> m_strDist >> m_strRadMax >> m_strRadMin >> m_strBGV;
		ar.Close();
		saveFile.Close();
	}
	UpdateData(FALSE);
}

void COptionFormView::OnBnClickedButtonOptionSave() // 설정 창 -> 저장버튼
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	this->OnOptionSave(); // 메뉴의 저장
}

void COptionFormView::OnAppExit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	UpdateData();

	// 쓰기모드 파일 열기
	CFile saveFile;
	if (saveFile.Open(_T("saveFile.dat"), CFile::modeCreate | CFile::modeWrite))
	{
		CArchive ar(&saveFile, CArchive::store);
		ar << m_strDist << m_strRadMax << m_strRadMin << m_strBGV;
		ar.Close();
		saveFile.Close();
	}

	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	pMain->PostMessage(WM_CLOSE, NULL, NULL);
}

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

void COptionFormView::OnOptionOpen() // 설정파일 열기
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	static TCHAR BASED_CODE szFilter[] = _T("데이터 파일(*.accdb) | *.ACCDB;*.accdb |모든파일(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.accdb"), _T(""), OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal())
	{
		CString pathName = dlg.GetPathName();
		//myADO ado;
		::CoInitialize(NULL);
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
				::CoUninitialize();
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




void COptionFormView::OnOptionSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	static TCHAR BASED_CODE szFilter[] = _T("데이터 파일(*.accdb) | *.accdb;*.ACCDB |모든파일(*.*)|*.*||");
	CFileDialog dlg(FALSE, _T("*.accdb"), _T(""), OFN_OVERWRITEPROMPT, szFilter);
	// 경로, 이름이 중복일 때에도 파일을 Create하려고 해서 에러남. 중복일 때는 Create 건너뛰고 값 수정만 하도록 코드 수정해야 함.
	if (IDOK == dlg.DoModal())
	{
		CString pathName = dlg.GetPathName();
		//myADO ado;
		::CoInitialize(NULL);
		ADOX::_CatalogPtr m_pCatalog = NULL;
		ADOX::_TablePtr m_pTable = NULL;
		ADOX::_ColumnPtr m_pColumn = NULL;
		ADOX::_IndexPtr m_pIndex = NULL;
		ADODB::_ConnectionPtr m_pConnection = NULL;

		/* 각 Object에 대한 Instance 생성 */
		m_pCatalog.CreateInstance(__uuidof (Catalog));
		m_pTable.CreateInstance(__uuidof (Table));
		//m_pIndex.CreateInstance(__uuidof (Index));

		CString temp;
		temp.Format(_T("Provider=Microsoft.ACE.OLEDB.16.0;Data Source=%s;"), pathName);
		m_pCatalog->Create((_bstr_t)temp);

		m_pTable->PutName("Params");
		//m_pTable->Columns->Append("ID", ADOX::adVarWChar, 10);
		m_pTable->Columns->Append("Distance", ADOX::adVarWChar, 10);
		m_pTable->Columns->Append("RadMax", ADOX::adVarWChar, 10);
		m_pTable->Columns->Append("RadMin", ADOX::adVarWChar, 10);
		m_pTable->Columns->Append("BGV", ADOX::adVarWChar, 10);

		//m_pIndex->Name = "ParamsIndex";
		//m_pIndex->Columns->Append("ID", ADOX::adVarWChar, 0);
		//m_pIndex->PutPrimaryKey(-1);
		//m_pIndex->PutUnique(-1);

		//m_pTable->Indexes->Append(_variant_t((IDispatch*)m_pIndex));
		m_pCatalog->Tables->Append(_variant_t((IDispatch*)m_pTable));

		m_pCatalog = NULL;
		m_pTable = NULL;
		//m_pIndex = NULL;
		try
		{
			HRESULT hr = S_OK;
			/* 연결 인스턴스 생성*/
			hr = m_pConnection.CreateInstance(__uuidof(Connection));
			if (SUCCEEDED(hr))
			{
				/* DB 연결*/
				m_pConnection->ConnectionString = _bstr_t(temp);
				m_pConnection->Open("", "", "", adModeUnknown);
			}
			// 직접 변경한 설정값들 적용
			UpdateData();
			m_nDist = _ttoi(m_strDist);
			m_nRadMax = _ttoi(m_strRadMax);
			m_nRadMin = _ttoi(m_strRadMin);
			m_nBGV = _ttoi(m_strBGV);

			CString query; // 쿼리문이 저장될 변수
			query.Format(_T("INSERT INTO Params(Distance, RadMax, RadMin, BGV) values('%d', '%d', '%d', '%d')"), m_nDist, m_nRadMax, m_nRadMin, m_nBGV);
			_bstr_t executeQuery = query;
			m_pConnection->BeginTrans();
			m_pConnection->Execute(executeQuery, NULL, adCmdUnspecified);
			m_pConnection->CommitTrans();
			::CoUninitialize();
		}
		catch (_com_error& e)
		{
			AfxMessageBox(e.Description());
		}
		m_pConnection->Close();
		m_pConnection = NULL;
	}
}
