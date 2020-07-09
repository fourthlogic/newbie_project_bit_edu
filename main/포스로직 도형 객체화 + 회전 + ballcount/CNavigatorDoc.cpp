// CNavigatorDoc.cpp: 구현 파일
//

#include "pch.h"
#include "MFCparam.h"
#include "CNavigatorDoc.h"


// CNavigatorDoc

IMPLEMENT_DYNCREATE(CNavigatorDoc, CDocument)

CNavigatorDoc::CNavigatorDoc()
{
}

BOOL CNavigatorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CNavigatorDoc::~CNavigatorDoc()
{
}


BEGIN_MESSAGE_MAP(CNavigatorDoc, CDocument)
END_MESSAGE_MAP()


// CNavigatorDoc 진단

#ifdef _DEBUG
void CNavigatorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CNavigatorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CNavigatorDoc serialization

void CNavigatorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}
#endif


// CNavigatorDoc 명령
