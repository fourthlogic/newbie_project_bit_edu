﻿
// ChildView.h: CChildView 클래스의 인터페이스
//


#pragma once


// CChildView 창

class CChildView : public CWnd
{
// 생성입니다.
public:
	CChildView();

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CChildView();

	// 생성된 메시지 맵 함수
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	CImage image;

	CBitmap m_bmBack;
	CDC m_memDC;
	CRect  tmpRect;

	float fOffsetX = 0.0f;
	float fOffsetY = 0.0f;

	double mox, moy;
	double mxMax, mxMin, myMax, myMin;

	CPoint m_sPt, m_ePt;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void CChildView::DeviceToWorld(long m_sPtx, long m_sPty, double& mx, double& my);
	void CChildView::DrawImage(CDC* pDC);
};
