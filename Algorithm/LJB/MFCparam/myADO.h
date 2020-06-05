#pragma once

// myADO 명령 대상

class myADO : public CDatabase
{
public:
	myADO();
	virtual ~myADO();

	/* Catalog(Database), Table, Column의 Object에 대한 ADOX Object 포인터 정의*/
	ADOX::_CatalogPtr m_pCatalog = NULL;
	ADOX::_TablePtr m_pTable = NULL;
	ADOX::_ColumnPtr m_pColumn = NULL;
	ADOX::_IndexPtr m_pIndex = NULL;
	ADODB::_ConnectionPtr m_pConnection;

	void makeFile();
	void connectFile(CString pathName);
};


