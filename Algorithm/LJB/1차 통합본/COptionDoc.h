#pragma once
#include "COptionFormView.h"
#include <afxdb.h>
// COptionDoc 문서

class COptionDoc : public CDocument
{
	DECLARE_DYNCREATE(COptionDoc)

public:
	COptionDoc();
	virtual ~COptionDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // 문서 입/출력을 위해 재정의되었습니다.
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
	CString m_strDist;
	CString m_strRadMax;
	CString m_strRadMin;
	CString m_strBGV;



	afx_msg void OnOptionOpen();
};
