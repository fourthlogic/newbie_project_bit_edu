﻿
// ChildView.h: CChildView 클래스의 인터페이스
//

#include <opencv2/opencv.hpp>
#include "MainFrm.h"
using namespace cv;
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
	CDC memDC;  // 처리 CDC을 지정 한다.
	// 빈공간을 새롭게 만든다.
	CMainFrame* pFrame;
	CDC mdcOffScreen;      // 더블버퍼링을 위한 메모리 그림버퍼
	CDC mdcOffScreen2;
	CBitmap bmpOffScreen; // 더블버퍼링을 위한 비트맵 객체를 만든다.
	CBitmap* oldbitmap;
	int idx = 0;
	CRect m_bgRect;   // 화면 전체의 크기 - client 윈도의 전체크기
	float ViewScale;
	float ViewValue;

	//테스트
	CPoint m_pos;
	CPoint z_pos;
	double newWidth;
	double newHeight;

	double width;
	double height;

	double rectWidth;
	double rectHeight;
	CPoint m_sPt, m_ePt, m_aPt;
	
	CString sFilename;
	CString m_strOpenedFile;
	BITMAP m_Bitmap;
	CBitmap m_background;
	CRect m_bg_rect;
	CBrush m_bkgBrush;
	BITMAP bmpInfo;
	CRect zoomRect;
	double zoomFactor = 1;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	HBITMAP Convert2Bitmap(String fileName);
	void OnEraseBkgnd(HDC hdc);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};



