#pragma once

// CNavigatorView 보기

class CNavigatorView : public CView
{
	DECLARE_DYNCREATE(CNavigatorView)

protected:
	CNavigatorView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CNavigatorView();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	//CMainFrame* pMain;

	CDC memDC;				// 처리 CDC을 지정 한다.
	CDC mdcOffScreen;		// 더블버퍼링을 위한 메모리 그림버퍼
	CBitmap bmpOffScreen;	// 더블버퍼링을 위한 비트맵 객체를 만든다.
	CBitmap m_background;
	BITMAP bmp;

	int clientWidth, clientHeight;
	int bmpWidth, bmpHeight;

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileOpen();

	void GetRectPos(float x, float y, float Width, float Height);
	float Rect_x;
	float Rect_y;
	float Rect_Width;
	float Rect_Height;
	CRect rect;

	float imgViewer_x, imgViewer_y, imgViewer_Width, imgViewer_Height;
	CPoint m_sPt;
	float client_x, client_y;

	void CNavigatorView::DrawNavigatorRect();
	bool isNavigator_Paint = false;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	void CNavigatorView::Navigator2imgViewer();
};


