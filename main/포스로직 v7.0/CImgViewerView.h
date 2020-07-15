
// MFCparamView.h: CMFCparamView 클래스의 인터페이스
//

#pragma once
#include "CircleDetection.h"
using namespace cv;
using namespace std;

enum DrawMode	//도형 종류
{
	None = -1, DPoint, DLine, DEllipse, DRectangle, DTriangle
};

enum RollBackMode {	//롤백 명령어
	Create, Delete, Update, GroupStart, GroupEnd
};


struct MyShape
{
	int shapeType; // 도형 모양	
	double penWidth;
	COLORREF shapeColor; // 색상
	vector<Point2d> pts; // 좌표
	vector<Point2d> RotatePts; // 회전 좌표
	Point2d Center; // 센터
	double theta;  // 각
	double R_theta; // 회전각
	int MoveCount; // 회전시 이동
	bool isflag; // 알고리즘 도형 or 사용자 제작 도형 (true 알고리즘, false 사용자 정의)
	Point tri_idx; // 삼각형 인덱스 계산 변수 
};

struct CopyShape {
	MyShape copy; // 복사용 shape
	vector<Point2d> pts; // 좌표
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
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

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
	vector<Vec3f> CircleCenter; // 원 검출
	vector<pair<Point2d, Point2d>> CirLinePoints; // 원의 중심점 최소제곱법 직선의 점
	Point2d IntersectionPoint;
	Mat result_mat; // 결과 이미지
	HBITMAP result_bmp; // 결과 이미지 BITMAP 타입

	CDC memDC;				// 처리 CDC을 지정 한다.
	CDC mdcOffScreen;		// 더블버퍼링을 위한 메모리 그림버퍼
	CBitmap bmpOffScreen;	// 더블버퍼링을 위한 비트맵 객체를 만든다.
	CBitmap m_background;	//배경 이미지
	BITMAP m_Bitmap;		//이미지 정보

	//패닝 && 줌
	Point2d m_sPt;	//클릭 시작 좌표(이미지 좌표계)
	CPoint z_pos;	//줌 좌표(이미지 좌표계)
	Point2d m_ePt;	//클라이언트 좌표
	Point2d m_pos;	//무브 좌표(이미지 좌표계)
	CRect m_bgRect;			// 화면 전체의 크기 - client 윈도의 전체크기

	double zoomWidth, zoomHeight;	//줌 크기
	double PWidth, PHeight;			//픽셀 사이즈

	double zoomView; //줌 비율
	double rectScale; //확대/축소 비율
	double printWidth; //출력 크기
	double printHeight; //출력 크기

	Point2d start_pos; //메모리 버퍼 시작 좌표
	Point2d before_Image_pos; //확대/축소 전 마우스 원본에서 위치
	Point2d before_Pixel_pos; //확대/축소 전 마우스 있는 픽셀에서의 위치
	Point2d after_Image_pos; //확대/축소 후 마우스 원본에서 위치
	Point2d after_Pixel_pos; //확대/축소 후 마우스 있는 픽셀에서의 위치



	CMenu* pMenu;
	//그리기
	int idx;			//이미지 선택 인덱스
	int drawStyle;		//현재 도형 종류
	Point2d d_sPt;		//그리기 시작값
	Point2d mov_Pt;		//그리기 끝값
	MyShape shape;		// 도형 값을 저장하기 위한 구조체 변수 선언
	CArray<MyShape, MyShape&> data;	//도형 리스트
	UINT m_nFlags; // 마우스버튼 확인
	int l_width = 1; // 선 굵기
	COLORREF color = RGB(0, 0, 0); // 도형 색

	// 엣지 좌표
	Point2d pts_0; 
	Point2d pts_1;
	Point2d pts_2;
	Point2d pts_3;

	// 이미지의 좌표
	Point2d d_img_pos;
	Point2d d_pix_pos;
	Point2d u_img_pos;
	Point2d u_pix_pos;
	Point2d d_pos;
	Point2d u_pos;
	Point2d rotatePts;

	//선택
	CPoint p_pt;			//붙여넣기 좌표
	bool ctrl;		//컨트롤 키 상태
	bool iscopy;	//복사 상태
	CopyShape copyShape;	// 복사한 도형
	vector<int> zOrder;			//zOrder 리스트
	vector<int> GroupList;		//Group 선택 리스트
	vector<CopyShape> copyList;	//복사 리스트
	vector<MyShape> ctrlCopyList; // copy 리스트
	double s_radian = 5; //  엣지 작은 원 반지름
	double b_radian = 12; //  엣지 큰 원 반지름

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
	BOOL ctrlSelect = FALSE;//컨트롤 누르고 선택

	// 볼 카운터 
	int BallCount;  // 볼 개수
	int BallIndex = -1; // 볼카운터 인덱스

	int RectCount = 0; // 좌표 개수
	int SelectIndex; // 선택 이미지 인덱스
	int EdgeIndex; // 엣지 선택 인덱스
	BOOL Save_Shape;	// 도형 포함 저장
	CString fileSave;			// 저장 파일경로 
	int ctrlv; // 컨트롤 v


	// 생성 및 실행
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC); // 백그라운드 이미지 초기화
	afx_msg void OnFileOpen(); // 파일 OPEN
	afx_msg void OnSize(UINT nType, int cx, int cy); // 클라이언트 크기 
	afx_msg void OnPaint(); // 클라이언트 그리기

	// 마우스
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

	// 컨텍스트 메뉴
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	afx_msg void OnContextCopy();
	afx_msg void OnContextPaste();
	afx_msg void OnContextDelete();
	afx_msg void OnContextLinecolor();
	afx_msg void OnContextLinewidth();

	// 저장
	afx_msg void OnFileSaveWithshape();
	afx_msg void OnFileSaveOnlyimg();
	void DrawShape_FileSave(CDC* pDC, MyShape& shape);

	// 도형
	afx_msg void OnDrawPoint(); // 크로스 헤어
	afx_msg void OnDrawLine(); // 직선
	afx_msg void OnDrawEllpse(); // 원
	afx_msg void OnDrawRect(); // 사각형
	afx_msg void OnDrawTri(); // 삼각형

	// 모드 선택
	afx_msg void OnModeSelect(); // 선택 모드

	// 선 굵기 & 선 색 선택
	afx_msg void OnSelectLw(); // 도형 굵기 변경
	afx_msg void OnSelectColor(); // 컬러 변경


	//사용자 설정 함수
	// ================================================================================
	//픽셀, RGB 값 출력
	void PrintText(CDC* pDC);	//RGB, Pixel 값 출력
	void DrawTextEx(CDC* pDC, const CString& str, CRect rect, UINT nFormat);	//RGB 센터 출력
	void paraChanged(bool isRotateCheck); // 파라미터 업데이트


	// 도형 그리기 함수 
	void draw(CDC* pDC); // move 그리기
	void drawShape(CDC* pDC, MyShape& shape); // 도형
	void SelectDrawShape(CDC* pDC, MyShape& shape); // 선택 도형 그리기 (회전 이동)
	void SelectShapeUpdate(); // 도형 정보 업데이트
	Point2d Intersection(Point2d& pt, Point2d& LinePt1, Point2d& LinePt2); // 회전 교점
	template <typename T>
	T my_greater(T num1, T num2, T& smaller);	// 좌표 비교
	
	// 도형 객체 추가
	void AddShapeCircle(vector<Vec3f>& circleCenter); // 원
	void AddShapeLines(vector<pair<Point2d, Point2d>>& cirLinePoints); // 선
	void AddShapePoint(Point2d interPoint); // 교점


	// 롤백 함수
	void undo();	// ctrl + Z
	void redo();	// ctrl + Y

	// 도형 내부 확인  
	template <typename T>
	BOOL IsContainVector(T data, vector<T> list);
	template <typename T>
	BOOL IsContainVector(T data, vector<T> list, int& idx);


	// 좌표 도형 내부 검사
	double isLeft(Point2d linePt1, Point2d linePt2, Point2d pos);
	int isContainPolygon(Point2d pos, vector<Point2d> vertices);
	int isContainPolygon(Point2d pos, vector<Point2d> vertices, int shapeType);
	BOOL polygon_points_inside(vector<Point2d> rc, vector<Point2d> V);
	BOOL lineCircleIntersection(Point2d AP1, Point2d AP2, MyShape& sh);
	BOOL lineLineIntersection(Point2d A, Point2d B, Point2d C, Point2d D);
	BOOL polygon_edges_overlap(vector<Point2d> rc, vector<Point2d> V);
	BOOL overlap(vector<Point2d> rc, vector<Point2d> V);

	void SaveShape(string);
	void LoadShape(string);
	// 네비게이터
	void imgViewer2Navigator();
	void GetImgPos(double Navigator_x, double Navigator_y);

	afx_msg void OnContextmenuBallcount();
};

#ifndef _DEBUG  // MFCparamView.cpp의 디버그 버전
inline CImgViewerDoc* CImgViewerView::GetDocument() const
{
	return reinterpret_cast<CImgViewerDoc*>(m_pDocument);
}
#endif

