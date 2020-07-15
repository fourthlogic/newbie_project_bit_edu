#pragma once

#include "MainFrm.h"

// COptionFrame 프레임

class COptionFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(COptionFrame)
protected:
	COptionFrame();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~COptionFrame();

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


