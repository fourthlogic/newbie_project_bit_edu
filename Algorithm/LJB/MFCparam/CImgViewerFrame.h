
// ChildFrm.h: CChildFrame 클래스의 인터페이스
//

#pragma once

class CImgViewerFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CImgViewerFrame)
public:
	CImgViewerFrame() noexcept;

// 특성입니다.
protected:
	CSplitterWnd m_wndSplitter;
public:

// 작업입니다.
public:

// 재정의입니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CImgViewerFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
