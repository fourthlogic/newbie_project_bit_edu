
// MFCparam.h: MFCparam 애플리케이션의 기본 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.
#include "MainFrm.h"

#include "CImgViewerFrame.h"
#include "CImgViewerDoc.h"
#include "CImgViewerView.h"

#include "COptionFrame.h"
#include "COptionDoc.h"
#include "COptionFormView.h"

#include "CNavigatorFrame.h"
#include "CNavigatorDoc.h"
#include "CNavigatorView.h"

// CMFCparamApp:
// 이 클래스의 구현에 대해서는 MFCparam.cpp을(를) 참조하세요.
//

class CMFCparamApp : public CWinApp
{
public:
	CMFCparamApp() noexcept;


// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 구현입니다.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:
	CMultiDocTemplate* pDocTemplate1; // 파라미터 템플릿
	CMultiDocTemplate* pDocTemplate2; // 이미지뷰어 템플릿
	CMultiDocTemplate* pDocTemplate3; // 네비게이터 템플릿
	// 설정 창
	COptionDoc* pOptionDoc = NULL;
	COptionFormView* pOptionView = NULL;
	COptionFrame* pOptionFrame = NULL;
	// 이미지뷰어 창
	CImgViewerDoc* pImgViewerDoc = NULL;
	CImgViewerView* pImgViewerView = NULL;
	CImgViewerFrame* pImgViewerFrame = NULL;
	// 네비게이터 창
	CNavigatorDoc* pNavigatorDoc = NULL;
	CNavigatorView* pNavigatorView = NULL;
	CNavigatorFrame* pNavigatorFrame = NULL;

	afx_msg void OnFileOpen();
	afx_msg void OnFileSaveWithshape();
	afx_msg void OnFileSaveOnlyimg();
	afx_msg void OnAppExit();
	afx_msg void OnOptionOpen();
	afx_msg void OnOptionSave();
};

extern CMFCparamApp theApp;
