
// MFCparamView.h: CMFCparamView 클래스의 인터페이스
//

#pragma once
#include "CircleDection.h"
using namespace cv;
using namespace std;
enum DrawMode	//도형 종류
{
	None = -1, DPoint, DLine, DEllipse, DRectangle, DTriangle, DCrossHair
};

enum RollBackMode {	//롤백 명령어
	Create, Delete, Update
};

struct MyShape
{
	int shapeType; // 도형 모양	
	double penWidth;
	COLORREF shapeColor; // 색상
	vector<Point2d> pts; // 좌표
	vector<Point2d> RotatePts; // 회전 좌표
	Point2d Center; // 센터
	Point2d Rotate; // 회전
	double theta;  // 각
	double R_theta; // 회전각
	double radin[2]; // 크기
	CRect Rect[4];
};

struct RollbackInfo {	//롤백 정보
	int rollbackmode;	//롤백 명령어
	int idx;			//도형 인덱스
	MyShape redoShape;	//다음 도형
	MyShape undoShape;	//이전 도형
};


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

	// 이미지 처리용
	CircleDection m_Algorithm; // 알고리즘 처리 클래스
	Mat result_mat; // 결과 이미지
	HBITMAP result_bmp; // 결과 이미지 BITMAP 타입

	CDC memDC;				// 처리 CDC을 지정 한다.
	CDC mdcOffScreen;		// 더블버퍼링을 위한 메모리 그림버퍼
	CBitmap bmpOffScreen;	// 더블버퍼링을 위한 비트맵 객체를 만든다.
	CBitmap m_background;	//배경 이미지
	BITMAP m_Bitmap;		//이미지 정보

	//패닝 && 줌
	CPoint m_sPt;	//클릭 시작 좌표(이미지 좌표계)
	CPoint z_pos;	//줌 좌표(이미지 좌표계)
	CPoint m_ePt;	//클라이언트 좌표
	CPoint m_pos;	//무브 좌표(이미지 좌표계)
	CRect m_bgRect;			// 화면 전체의 크기 - client 윈도의 전체크기

	double zoomWidth, zoomHeight;	//줌 크기
	double PWidth, PHeight;			//픽셀 사이즈

	float zoomView; //줌 비율
	float rectScale; //확대/축소 비율
	float printWidth; //출력 크기
	float printHeight; //출력 크기

	POINTF start_pos; //메모리 버퍼 시작 좌표
	POINTF before_Image_pos; //확대/축소 전 마우스 원본에서 위치
	POINTF before_Pixel_pos; //확대/축소 전 마우스 있는 픽셀에서의 위치
	POINTF after_Image_pos; //확대/축소 후 마우스 원본에서 위치
	POINTF after_Pixel_pos; //확대/축소 후 마우스 있는 픽셀에서의 위치
	
	//그리기
	int idx;			//이미지 선택 인덱스
	int drawStyle;		//현재 도형 종류
	CPoint d_sPt;		//그리기 시작값
	CPoint mov_Pt;		//그리기 끝값
	MyShape shape;		// 도형 값을 저장하기 위한 구조체 변수 선언
	CArray<MyShape, MyShape&> data;	//도형 리스트
	int l_width = 1;
	COLORREF color = RGB(0, 0, 0);


	//선택
	CPoint p_pt;			//붙여넣기 좌표
	bool ctrl;		//컨트롤 키 상태
	bool iscopy;	//복사 상태
	MyShape copyShape;	// 복사한 도형
	vector<int> zOrder;		//zOrder 리스트

	//되돌리기
	int rollbackIndex;	//롤백 인덱스
	MyShape temp;		// 변환 전 도형
	vector<RollbackInfo> rollback;	//롤백 리스트

	// 도형 
	BOOL drawID = FALSE; // 그리기 모드
	BOOL penID = FALSE; // 선택 모드
	BOOL drawShapeID = FALSE; // 도형 시작
	BOOL selectID = FALSE; // 선택 
	BOOL EdgeSelect = FALSE; // 엣지 선택
	BOOL rotateID = FALSE; // 회전 선택

	int RectCount = 0; // 좌표 개수
	int SelectIndex; // 선택 이미지 인덱스
	int EdgeIndex; // 엣지 선택 인덱스



	//사용자 설정 함수
	// ================================================================================
	void PrintText(CDC* pDC);	//RGB, Pixel 값 출력
	void DrawTextEx(CDC* pDC, const CString& str, CRect rect, UINT nFormat);	//RGB 센터 출력
	void paraChanged(); // 파라미터 업데이트

	
	// 도형 그리기 함수 
	void draw(CDC* pDC);
	void drawShape(CDC* pDC, MyShape& shape);
	void SelectDrawShape(CDC* pDC, MyShape& shape);
	void MyEllipsePS_DOT(CDC* pDC, Point2d Center, int radinX, int radinY, double theta);
	void MyEllipseR(CDC* pDC, Point2d Center, int radinX, int radinY, double theta, COLORREF parm_color);
	void SelectShapeUpdate(); // 도형 정보 업데이트
	Point2d Intersection(Point2d& pt, Point2d& LinePt1, Point2d& LinePt2); // 

	// 좌표 도형 내부 검사
	int isLeft(Point2d linePt1, Point2d linePt2, CPoint pos);
	int isContainPolygon(CPoint pos, vector<Point2d> vertices);

	// 네비게이터
	void imgViewer2Navigator();
	void GetImgPos(float Navigator_x, float Navigator_y);

	// 롤백 함수
	void undo();
	void redo();

	// 가상
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// 메세지 처리 
	// 생성 및 실행
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC); // 백그라운드 이미지 초기화
	afx_msg void OnFileOpen(); // 파일 OPEN
	afx_msg void OnSize(UINT nType, int cx, int cy); // 클라이언트 크기 
	afx_msg void OnPaint();
	// 마우스
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

	// 컨텍스트 메뉴
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnContextCopy();
	afx_msg void OnContextPaste();
	afx_msg void OnContextDelete();
	afx_msg void OnContextLinecolor();
	// 저장
	afx_msg void OnFileSaveWithshape();
	afx_msg void OnFileSaveOnlyimg();
	// 도형
	afx_msg void OnDrawPoint();
	afx_msg void OnDrawLine();
	afx_msg void OnDrawEllpse();
	afx_msg void OnDrawRect();
	// 모드 선택
	afx_msg void OnModeDraw();
	afx_msg void OnModeSelect();

	afx_msg void OnSelectLw();
	afx_msg void OnSelectColor();
};

#ifndef _DEBUG  // MFCparamView.cpp의 디버그 버전
inline CImgViewerDoc* CImgViewerView::GetDocument() const
   { return reinterpret_cast<CImgViewerDoc*>(m_pDocument); }
#endif

