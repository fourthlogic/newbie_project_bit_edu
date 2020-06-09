﻿#pragma once
#include "MainFrm.h"
#include "CImgViewerDoc.h"
#include "CircleDection.h"

// COptionFormView 폼 보기

class COptionFormView : public CFormView
{
	DECLARE_DYNCREATE(COptionFormView)

protected:
	COptionFormView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~COptionFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COptionFormView };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();


	afx_msg void OnBnClickedButtonOptionSave();
	CString strFilename;
	CString m_strDist;
	CString m_strRadMax;
	CString m_strRadMin;
	CString m_strBGV;
	int m_nDist;
	int m_nRadMax;
	int m_nRadMin;
	int m_nBGV;
	//CString strThMax;
	//CString strThMin;

//	afx_msg void OnEnChangeEditOptionDist();
//	afx_msg void OnEnChangeEditOptionRmax();
//	afx_msg void OnEnChangeEditOptionRmin();
//	afx_msg void OnEnChangeEditOptionBgv();
	//afx_msg void OnEnChangeEditOptionThmax();
	//afx_msg void OnEnChangeEditOptionThmin();

//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg void OnAppExit();
	afx_msg void OnOptionOpen();
	//void paraChanged2();
	afx_msg void OnBnClickedButtonDo();
	afx_msg void OnOptionSave();
};


