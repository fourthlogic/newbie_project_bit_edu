
// ChildView.h: CChildView 클래스의 인터페이스
//
#pragma once
// CChildView 창

enum DrawMode
{
	DPoint, DLine, DEllipse, DRectangle
};

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
	CPoint m_pos, z_pos;

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

	double newWidth;
	double newHeight;
	int idx = 0;

	CMainFrame* pFrame;

	double zoomWidth, zoomHeight;
	double PWidth, PHeight;

public:
	//그리기
	int drawStyle;
	bool drawID;
	int x, y;	//그리기 시작값
	int mov_x, mov_y;	//그리기 끝값
		// 그린 도형을 배열에 저장하기 위한 구조체 정의
	struct MyShape
	{
		int shapeType; // 도형 모양		
		int penWidth; // 펜의 두깨
		CString shapeText; // 도형 문자열
		int textSize; // 도형 문자열 크기		
		CRect rect; // 도형 그리기 좌표
		COLORREF fgColor, bgColor; // 전경색과 배경색 저장		
	};
	MyShape shape; // 도형 값을 저장하기 위한 구조체 변수 선언	
	// 그린 도형을 저장할 동적 배열 선언
	CArray<MyShape, MyShape&> data;
	int draw(CPoint point);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

	void PrintText(CDC* pDC);
	void DrawTextEx(CDC* pDC, const CString& str, CRect rect, UINT nFormat);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPoint();
	afx_msg void OnLine();
	afx_msg void OnEllpse();
	afx_msg void OnRectangle();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

