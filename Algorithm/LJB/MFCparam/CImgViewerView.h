
// MFCparamView.h: CMFCparamView 클래스의 인터페이스
//

#pragma once
//#include "CImgProcessing.h"

using namespace cv;
using namespace std;

class CImgViewerView : public CView
{
protected: // serialization에서만 만들어집니다.
	CImgViewerView() noexcept;
	DECLARE_DYNCREATE(CImgViewerView)

// 특성입니다.
public:
	CImgViewerDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CImgViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	
	CImage image;

	CPoint m_sPt, m_ePt;
	CPoint m_pos, z_pos;

	CDC memDC;  // 처리 CDC을 지정 한다.
	// 빈공간을 새롭게 만든다.
	CDC mdcOffScreen;      // 더블버퍼링을 위한 메모리 그림버퍼
	CBitmap bmpOffScreen; // 더블버퍼링을 위한 비트맵 객체를 만든다.
	CBitmap* oldbitmap;

	CBitmap m_background;
	BITMAP m_Bitmap;
	HBITMAP m_hbackground;

	CRect m_bgRect;   // 화면 전체의 크기 - client 윈도의 전체크기
	CBrush m_bkgBrush;
	float m_Zoom;

	double newWidth;
	double newHeight;
	int idx = 0;

	//CMainFrame* pFrame;

	double zoomWidth, zoomHeight;
	double PWidth, PHeight;


	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnFileOpen();
};

#ifndef _DEBUG  // MFCparamView.cpp의 디버그 버전
inline CMFCparamDoc* CMFCparamView::GetDocument() const
   { return reinterpret_cast<CMFCparamDoc*>(m_pDocument); }
#endif

