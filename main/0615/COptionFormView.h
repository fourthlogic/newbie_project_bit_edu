#pragma once
#include "MainFrm.h"
#include "CircleDection.h"
#include <ctime>
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


	CString m_strDist;							// 직교거리
	CString m_strRadMax;						// 검출 원 최대 반지름
	CString m_strRadMin;						// 검출 원 최소 반지름
	CString m_strBGV;							// BGV 값

	int m_nDist;
	int m_nRadMax;
	int m_nRadMin;
	int m_nBGV;

	afx_msg void OnOptionOpen();				// 설정파일 열기
	afx_msg void OnOptionSave();				// 설정파일 저장
	afx_msg void OnAppExit();					// 끝내기
	afx_msg void OnBnClickedButtonDo();			// 실행버튼
	afx_msg void OnBnClickedButtonOptionSave();	// 설정파일 저장버튼
	int m_nRuntime;
	afx_msg void OnEnChangeEditOptionDist();
};


