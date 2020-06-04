
// ChildView.h: CChildView 클래스의 인터페이스
//
#pragma once
// CChildView 창

enum DrawMode	//도형 종류
{
	DPoint, DLine, DEllipse, DRectangle
};

enum RollBackMode {	//롤백 명령어
	Create, Delete, Update
};
struct MyShape
{
	int shapeType; // 도형 모양		
	int penWidth; // 펜의 두깨
	CString shapeText; // 도형 문자열
	int textSize; // 도형 문자열 크기		
	CRect rect; // 도형 그리기 좌표
	COLORREF fgColor, bgColor; // 전경색과 배경색 저장
	bool isClicked = false;	//도형 클릭 여부
};

struct RollbackInfo {	//롤백 정보
	int rollbackmode;	//롤백 명령어
	int idx;			//도형 인덱스
	MyShape redoShape;	//다음 도형
	MyShape undoShape;	//이전 도형
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
	DECLARE_MESSAGE_MAP()
public:
	CMainFrame* pFrame;		//메인 프레임


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
	float m_Zoom;			//줌 비율
	double zoomWidth, zoomHeight;	//줌 크기
	double PWidth, PHeight;			//픽셀 사이즈

	//정발산기슭곰발냄새타령부인사잘해
	//그리기
	bool drawID;		//그리기 상태
	int l_width;		//도형 선 굵기
	int idx;			//이미지 선택 인덱스
	int drawStyle;		//현재 도형 종류
	CPoint d_sPt;		//그리기 시작값
	CPoint mov_Pt;		//그리기 끝값
	CPoint pts;			//도형 그리는 좌표
	CPoint sPt, ePt;	//LBoutton Down, Up 좌표
	MyShape shape;		// 도형 값을 저장하기 위한 구조체 변수 선언
	CArray<MyShape, MyShape&> data;	//도형 리스트

	//선택
	int choiceIdx;		//도형 선택 인덱스
	bool panID;			//선택 상태
	bool m_lbtn;	//LButton 상태
	CPoint p_pt;			//붙여넣기 좌표
	CRect mRect[4];			//선택 사각형 꼭짓점
	int resize;			//크기 변환 인덱스
	int pasteW, pasteH;		//붙여넣기 크기
	bool ctrl;		//컨트롤 키 상태
	bool iscopy;	//복사 상태
	MyShape copyShape;	// 복사한 도형
	vector<int> zOrder;		//zOrder 리스트

	//되돌리기
	int rollbackIndex;	//롤백 인덱스
	MyShape temp;		// 변환 전 도형
	vector<RollbackInfo> rollback;	//롤백 리스트

	//사용자 설정 함수
	void PrintText(CDC* pDC);	//RGB, Pixel 값 출력
	void DrawTextEx(CDC* pDC, const CString& str, CRect rect, UINT nFormat);	//RGB 센터 출력
	int draw();		//현재 그리는 도형 출력
	void drawShape(int shapeNum, int penWd, int sx, int sy, int ex, int ey, COLORREF fgcolor);	//저장된 도형 출력


	//메세지 처리 함수
	BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);

	//메뉴 이벤트 처리 함수
	afx_msg void OnPoint();		//점
	afx_msg void OnLine();		//선
	afx_msg void OnEllpse();	//원
	afx_msg void OnRectangle();	//사각형
	afx_msg void OnDrawid();	//그리기 모드
	afx_msg void OnPanid();		//선택 모드
	afx_msg void OnLwidth1();	//선 굵기 1
	afx_msg void OnLwidth2();	//선 굵기 2
	afx_msg void On32797();		//선 굵기 3
	afx_msg void On32798();		//선 굵기 4
	afx_msg void On32799();		//선 굵기 5
	afx_msg void On32800();		//선 굵기 10
	afx_msg void OnCopy();		//복사
	afx_msg void OnPaste();		//붙여 넣기
	afx_msg void OnDelete();	//삭제
	afx_msg void OnLinecolor();	//도형 색 변경
};