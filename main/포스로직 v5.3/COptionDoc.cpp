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


// COptionDoc 명령


void COptionDoc::OnOptionOpen()
{

}
