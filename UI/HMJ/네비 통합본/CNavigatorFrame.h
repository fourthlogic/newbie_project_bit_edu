#pragma once


// CNavigatorFrame 프레임

class CNavigatorFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CNavigatorFrame)
protected:
	CNavigatorFrame();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CNavigatorFrame();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


