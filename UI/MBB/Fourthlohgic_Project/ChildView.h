
// ChildView.h: CChildView 클래스의 인터페이스
//
#include <opencv2/opencv.hpp>
//#include <iostream>
using namespace cv;
//using namespace std;

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
	CImage SrcImage;
	CImage DstImage;
	int a;
	BOOL MakeCellFrameImage(LPCTSTR InSourceImageFileName);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
public:
	CString pathname;
	String sFilename;
	CImage image;
	//Mat srcImage, dstImage;
	int height, width;
	//zoom
	int m_nZoomRate;
	int m_ImageWidth;
	int m_ImageHeight;
	void Paint(HDC dc);
};

