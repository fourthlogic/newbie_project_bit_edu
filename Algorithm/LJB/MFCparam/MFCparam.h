
// MFCparam.h: MFCparam 애플리케이션의 기본 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.
#include "CImgViewerFrame.h"
#include "CImgViewerDoc.h"
#include "CImgViewerView.h"

#include "MainFrm.h"

#include "COptionFrame.h"
#include "COptionDoc.h"
#include "COptionFormView.h"

#include "COptionDlg.h"
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
	afx_msg void OnFileOpen();
	CMultiDocTemplate* pDocTemplate1;
	CMultiDocTemplate* pDocTemplate2;
	afx_msg void OnAppExit();
	afx_msg void OnOptionOpen();
};

extern CMFCparamApp theApp;
