// myADO.cpp: 구현 파일
//

#include "pch.h"
#include "MFCparam.h"
#include "myADO.h"


// myADO

myADO::myADO()
{
	::CoInitialize(NULL);
}

myADO::~myADO()
{
	::CoUninitialize();
}


// myADO 멤버 함수


void myADO::makeFile()
{
	// TODO: 여기에 구현 코드 추가.
	/* 각 Object에 대한 Instance 생성 */
	//m_pCatalog.CreateInstance(__uuidof (Catalog));
	//m_pTable.CreateInstance(__uuidof (Table));
	//m_pIndex.CreateInstance(__uuidof (Index));

	////Data Source 이름을 저장대화상자에서 받아온다음 testADO.accdb 자리에 바꿔서 넣어야 함
	//m_pCatalog->Create("Provider=Microsoft.ACE.OLEDB.16.0;Data Source=testADO.accdb");

	//m_pTable->PutName("Params");
	//m_pTable->Columns->Append("ID", ADOX::adVarWChar, 10);
	//m_pTable->Columns->Append("Distance", ADOX::adVarWChar, 10);
	//m_pTable->Columns->Append("RadMax", ADOX::adVarWChar, 10);
	//m_pTable->Columns->Append("RadMin", ADOX::adVarWChar, 10);
	//m_pTable->Columns->Append("BGV", ADOX::adVarWChar, 10);

	//m_pIndex->Name = "ParamsIndex";
	//m_pIndex->Columns->Append("ID", ADOX::adVarWChar, 0);
	//m_pIndex->PutPrimaryKey(-1);
	//m_pIndex->PutUnique(-1);

	//m_pTable->Indexes->Append(_variant_t((IDispatch*)m_pIndex));

	//m_pCatalog->Tables->Append(_variant_t((IDispatch*)m_pTable));

	///////////////////////////////////////////
	try
	{
		HRESULT hr = S_OK;
		/* 연결 인스턴스 생성*/
		hr = m_pConnection.CreateInstance(__uuidof(Connection));
		if (SUCCEEDED(hr))
		{
			/* DB 연결*/
			m_pConnection->ConnectionString = _bstr_t("Provider=Microsoft.ACE.OLEDB.16.0;Data Source=testADO.accdb;");
			m_pConnection->Open("", "", "", adModeUnknown);
		}

		CString query;// 쿼리문이 저장될 변수
		/* 쿼리*/
		query = "INSERT INTO Params (ID, Distance, RadMax, RadMin, BGV) values ('1', '20', '4', '2', '80')";
		_bstr_t executeQuery = query;
		/* 트랜잭션 시작*/
		m_pConnection->BeginTrans();
		/* "추가" 쿼리 실행*/
		m_pConnection->Execute(executeQuery, NULL, adCmdUnspecified);
		/* 트랜잭션 종료*/
		m_pConnection->CommitTrans();
	}
	catch(_com_error& e)
	{
		AfxMessageBox(e.Description());
	}
	m_pConnection->Close();
	m_pConnection = NULL;
}


void myADO::connectFile(CString pathName) // accdb파일에 연결하고 데이터 가져옴
{
	// TODO: 여기에 구현 코드 추가.
	try
	{
		HRESULT hr = S_OK;
		/* 연결 인스턴스 생성*/
		hr = m_pConnection.CreateInstance(__uuidof(Connection));
		if (SUCCEEDED(hr))
		{
			/* DB 연결*/
			CString temp;
			temp.Format(_T("Provider=Microsoft.ACE.OLEDB.16.0;Data Source=%s;"), pathName);
			m_pConnection->ConnectionString = _bstr_t(temp);
			m_pConnection->Open("", "", "", adModeUnknown);

			CString query;// 쿼리문이 저장될 변수
			query = "SELECT * FROM (Params)";
			_bstr_t executeQuery = query;
			/* 트랜잭션 시작*/
			m_pConnection->BeginTrans();
			/* "읽기" 쿼리 실행*/
			m_pConnection->Execute(executeQuery, NULL, adCmdUnspecified);
			/* 트랜잭션 종료*/
			m_pConnection->CommitTrans();
		}

	}
	catch(_com_error& e)
	{
		AfxMessageBox(e.Description());
	}
	m_pConnection->Close();
	m_pConnection = NULL;
}
