#pragma once


// CSelectLineWidth 대화 상자

class CSelectLineWidth : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectLineWidth)

public:
	CSelectLineWidth(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CSelectLineWidth();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CSelect_LINE_WIDTH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int l_width;	//선 굵기
protected:
	CSliderCtrl m_Sliderctr;
	CEdit m_lwidth;
public:
	// 가상
	virtual BOOL OnInitDialog();

	//이벤트
	afx_msg void OnBnClickedOk();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeEditLw();
};
