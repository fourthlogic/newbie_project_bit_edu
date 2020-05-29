
// MainFrm.h: CMainFrame 클래스의 인터페이스
//

#pragma once


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

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	DECLARE_MESSAGE_MAP()
public:
	//CChildView* m_imageView;
	vector<imageData> imageList;
};


