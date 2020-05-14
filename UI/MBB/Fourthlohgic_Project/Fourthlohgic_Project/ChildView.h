
// ChildView.h: CChildView 클래스의 인터페이스
//


#pragma once
#include <opencv2/opencv.hpp>
using namespace cv;

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
	CPoint m_sPt, m_ePt;
	CPoint m_pos;

	CDC memDC;  // 처리 CDC을 지정 한다.
	// 빈공간을 새롭게 만든다.
	CDC mdcOffScreen;      // 더블버퍼링을 위한 메모리 그림버퍼
	CBitmap bmpOffScreen; // 더블버퍼링을 위한 비트맵 객체를 만든다.
	CBitmap* oldbitmap;

	CBitmap m_background;
	BITMAP m_Bitmap;

	CRect m_bgRect;   // 화면 전체의 크기 - client 윈도의 전체크기
	CBrush m_bkgBrush;
	float m_Zoom;
	float zoomWidth;
	float zoomHeight;
public:
	String sPathName;
	Mat srcImage, matOffScreen, *oldmat;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void DisplayImage(Mat src, HDC dc);
	void OnEraseBkgnd();
};

