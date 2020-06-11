
// MainFrm.h: CMainFrame 클래스의 인터페이스
//

#pragma once
#include "MFCparam.h"

#include "COptionDoc.h"
#include "COptionFormView.h"
#include "COptionFrame.h"

#include "CircleDection.h"

#include "CNavigatorFrame.h"
#include "CNavigatorDoc.h"
#include "CNavigatorView.h"

class imageData {
	std::string fileName;
	Mat src;
	HBITMAP image;
public:
	std::string getFileName() { return fileName; }
	void setFileName(std::string fileName) { this->fileName = fileName; }

	Mat getSrc() { return this->src; }
	void setImage() {
		this->image = getHbitmap();
	}
	HBITMAP getImage() {
		return image;
	}
	HBITMAP getHbitmap();
};

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame() noexcept;

// 특성입니다.
public:
	

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	// declare our pane
	CDockablePane m_wndPane;


// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	CircleDection Cir;

	CRect m_rect;
	//POSITION posTemplate = NULL, posDocument = NULL, posView = NULL, posFrame = NULL;
	//CMultiDocTemplate* pDocTemplate = NULL;
	//COptionDoc* pOptionDoc = NULL;
	//COptionFormView* pOptionView = NULL;
	//COptionFrame* pOptionFrame = NULL;
	//CImgViewerDoc* pImgViewerDoc = NULL;
	//CImgViewerView* pImgViewerView = NULL;
	//CImgViewerFrame* pImgViewerFrame = NULL;
	//CNavigatorDoc* pNavigatorDoc = NULL;
	//CNavigatorView* pNavigatorView = NULL;
	//CNavigatorFrame* pNavigatorFrame = NULL;

	vector<imageData> imageList;
	COLORREF color;
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnClose();
	afx_msg void OnOptionSave();
};


