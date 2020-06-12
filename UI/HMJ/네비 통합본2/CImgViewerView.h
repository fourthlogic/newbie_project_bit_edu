
// MFCparamView.h: CMFCparamView 클래스의 인터페이스
//

#pragma once
#include "CircleDection.h"
using namespace cv;
using namespace std;
enum DrawMode	//도형 종류
{
	None = -1, DPoint, DLine, DEllipse, DRectangle
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
	
	//CMainFrame* pFrame;		//메인 프레임

	// 이미지 처리용
	CircleDection m_Algorithm;
	Mat result_mat;
	HBITMAP result_bmp;

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
	//int draw();		//현재 그리는 도형 출력
	void drawShape(CDC* dc, int penType, MyShape a);	//저장된 도형 출력
	void undo();
	void redo();


	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnFileOpen();
	void paraChanged();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDrawPoint();
	afx_msg void OnDrawLine();
	afx_msg void OnDrawEllpse();
	afx_msg void OnDrawRect();
	afx_msg void OnModeDraw();
	afx_msg void OnModeSelect();
	afx_msg void OnThick1();
	afx_msg void OnThick2();
	afx_msg void OnThick3();
	afx_msg void OnThick4();
	afx_msg void OnThick5();
	afx_msg void OnThick10();
	afx_msg void OnContextCopy();
	afx_msg void OnContextPaste();
	afx_msg void OnContextDelete();
	afx_msg void OnContextLinecolor();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnFileSaveWithshape();
	afx_msg void OnFileSaveOnlyimg();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	void CImgViewerView::imgViewer2Navigator();
	void CImgViewerView::GetImgPos(float Navigator_x, float Navigator_y);
};

#ifndef _DEBUG  // MFCparamView.cpp의 디버그 버전
inline CImgViewerDoc* CImgViewerView::GetDocument() const
   { return reinterpret_cast<CImgViewerDoc*>(m_pDocument); }
#endif

