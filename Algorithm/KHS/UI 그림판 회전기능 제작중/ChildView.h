
// ChildView.h: CChildView 클래스의 인터페이스
//
#pragma once
// CChildView 창

enum DrawMode
{
	DPoint, DLine, DEllipse, DRectangle, DTriangle
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

	CPoint m_sPt, m_ePt, m_aPt, z_pos;

	CDC memDC; // 빈공간을 새롭게 만든다.
	CDC mdcOffScreen;      // 더블버퍼링을 위한 메모리 그림버퍼
	CBitmap bmpOffScreen; // 더블버퍼링을 위한 비트맵 객체를 만든다.
	CBitmap* oldbitmap;

	CBitmap m_background;
	BITMAP m_Bitmap;
	
	CRect m_bgRect;   // 화면 전체의 크기 - client 윈도의 전체크기
	CBrush m_bkgBrush;
	float ViewScale;
	float ViewValue;

	//newZoom Test
	float zoomScale;
	float zoomView;
	float printWidth;
	float printHeight;

	//테스트
	CPoint m_pos;
	double newWidth;
	double newHeight;
	int idx = 0;

	CMainFrame* pFrame;
	double width;
	double height;

	double rectWidth;
	double rectHeight;

	double viewWidth;
	double viewHeight;

	void CChildView::PrintText(CDC* pDC);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	

	// 그리기 ----------------------------------
	CRect lRect[2], rRect[4];
	int RectCount = 0;
	CPoint cPoint[2];
	CPoint R_pos;

	int drawStyle;
	bool drawID, panID, chooseID, EdgeChosen;
	bool rotateClick = FALSE;
	int choRotate;

	int chosenShape;
	int chosenEdge;
	int x, y;	//그리기 시작값
	int mov_x, mov_y;	//그리기 끝값
		
	// 그린 도형을 배열에 저장하기 위한 구조체 정의
	struct MyShape
	{
		int shapeType; // 도형 모양	
		COLORREF shapeColor;
		vector<CPoint> pts;
		vector<CPoint> RotatePts;
		CPoint Center;
		CPoint Rotate;
		double theta;
		double R_theta;
		int radin[2];
	};

	CPoint Center;
	CPoint Rotate;

	CArray<MyShape, MyShape&> data;


	int draw(CDC* pDC);
	bool isInside(CPoint& pt, vector<CPoint>& pts);
	void drawShape(MyShape& shape);
	void SelectDrawShape(MyShape& shape);
	void CChildView::Circle(CDC* pDC, int cx, int cy, int rad);
	void CChildView::plot_circle_points(CDC* pDC, int cx, int cy, int x, int y);
	void CChildView::MyEllipse(CDC* parm_dc, int parm_x, int parm_y, int parm_rx, int parm_ry, COLORREF parm_color);
	void CChildView::MyEllipseR(CDC* parm_dc, CPoint Center, int radinX, int radinY, double theta, COLORREF parm_color);
	void CChildView::MyEllipsePS_DOT(CDC* parm_dc, CPoint Center, int radinX, int radinY, double theta);


	afx_msg void OnLine();
	afx_msg void OnRectangle();
	afx_msg void OnEllipse();
};

