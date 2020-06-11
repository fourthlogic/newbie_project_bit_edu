// COptionDoc.cpp: 구현 파일
//

#include "pch.h"
#include "MFCparam.h"
#include "COptionDoc.h"


// COptionDoc

IMPLEMENT_DYNCREATE(COptionDoc, CDocument)

COptionDoc::COptionDoc()
{
}

BOOL COptionDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

COptionDoc::~COptionDoc()
{
}


BEGIN_MESSAGE_MAP(COptionDoc, CDocument)
	//ON_COMMAND(ID_OPTION_OPEN, &COptionDoc::OnOptionOpen)
END_MESSAGE_MAP()


// COptionDoc 진단

#ifdef _DEBUG
void COptionDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void COptionDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// COptionDoc serialization

void COptionDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
		//ar << m_strDist;
		//AfxMessageBox(m_strDist);
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
		//ar >> m_strDist;
		//AfxMessageBox(m_strDist);
	}
}
#endif


// COptionDoc 명령


void COptionDoc::OnOptionOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

}
