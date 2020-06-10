
// ChildView.h: CChildView 클래스의 인터페이스
//
#pragma once
// CChildView 창

enum DrawMode
{
	DPoint, DLine, DEllipse, DRectangle, DTriangle , DCrossHair
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
	afx_msg void OnEllipse();
	afx_msg void OnLine();
	afx_msg void OnRectangle();
	afx_msg void OnTriangle();

	// 그리기 ----------------------------------
	CRect lRect[2], rRect[4];
	//CPoint cPoint[2];
	//CPoint pt;

	int drawStyle;
	int RectCount = 0;
	bool drawID = FALSE, panID = FALSE, chooseID = FALSE, EdgeSelect = FALSE;
	bool rotateClick = FALSE;
	int choRotate;

	int SelectIndex;
	int EdgeIndex;
	int x, y;	//그리기 시작값
	int mov_x, mov_y;	//그리기 끝값
	COLORREF drawPenColor=RGB(255, 0, 255);

	
	// 그린 도형을 배열에 저장하기 위한 구조체 정의
	struct MyShape
	{
		int shapeType; // 도형 모양	
		double thickness;
		COLORREF shapeColor; // 색상
		vector<Point2d> pts; // 좌표
		vector<Point2d> RotatePts; // 회전 좌표
		Point2d Center; // 센터
		Point2d Rotate; // 회전
		double theta;  // 각
		double R_theta; // 회전각
		double radin[2]; // 크기
	};

	Point2d Center;
	Point2d Rotate;

	CArray<MyShape, MyShape&> data;


	void draw();
	bool isInside(CPoint& pt, vector<Point2d>& pts);
	void PrintText(CDC* pDC);
	void drawShape(MyShape& shape);
	void SelectDrawShape(MyShape& shape);
	void MyEllipseR(CDC* parm_dc, Point2d Center, int radinX, int radinY, double theta, COLORREF parm_color);
	void MyEllipsePS_DOT(CDC* parm_dc, Point2d Center, int radinX, int radinY, double theta);
	Point2d Intersection(Point2d& pt, Point2d& LinePt1, Point2d& LinePt2);
	void SelectShapeUpdate();
	void SelectShapeRectUpdate();
	void SelectShpaeCenterUpdate();
	afx_msg void OnCrosshair();
};



//struct m_Point
//{
//	double x;
//	double y;
//
//	m_Point()
//	{
//		this->x = 0;
//		this->y = 0;
//	}
//
//	m_Point(double X = 0, double Y = 0) : x(X), y(Y) {}
//
//	 CPoint 대입시
//	m_Point& operator=(const CPoint& ref)
//	{
//		x = ref.x;
//		y = ref.y;
//		return *this;
//	}
//
//	 자기자신 대입
//	m_Point& operator=(const m_Point& ref)
//	{
//		x = ref.x;
//		y = ref.y;
//		return *this;
//	}
//
//	 CPoint에 대입
//
//	m_Point& m_Point::operator>>(CPoint& pt)
//	{
//		x = this->x;
//		y = this->y;
//		return *this;
//	}
//	ostream& operator<<(m_Point& os, const CPoint& pos)
//	{
//		os << '[' << pos.xpos << ',' << pos.ypos << ']' << endl;
//		return os;
//	}
//};
//
//m_Point& operator<<(CPoint& ref, m_Point& pt)
//{
//	ref.x = pt.x;
//	ref.y = pt.y;
//
//	return pt;
//}
//
//m_Point& operator>>(CPoint& ref, m_Point& pt)
//{
//	ref.x = pt.x;
//	ref.y = pt.y;
//
//	return pt;
//}
//
//CPoint& operator>>(m_Point& pt, CPoint& ref)
//{
//	ref.x = pt.x;
//	ref.y = pt.y;
//
//	return ref;
//}
//



//static CPoint& operator<<(CPoint& pt, Point2d& ref)
//{
//	pt.x = ref.x;
//	pt.y = ref.y;
//	return pt;
//}

//CPoint& operator>>(CPoint& pt, Point2d& ref)
//{
//	pt.x = ref.x;
//	pt.y = ref.y;
//	return pt;
//}
//
//Point2d& operator<<(Point2d& pt, CPoint& ref)
//{
//	pt.x = ref.x;
//	pt.y = ref.y;
//	return pt;
//}
//
//Point2d& operator>>(Point2d& pt, CPoint& ref)
//{
//	pt.x = ref.x;
//	pt.y = ref.y;
//	return pt;
//}