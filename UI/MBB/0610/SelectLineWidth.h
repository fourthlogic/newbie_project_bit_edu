#pragma once


// SelectLineWidth 대화 상자

class SelectLineWidth : public CDialogEx
{
	DECLARE_DYNAMIC(SelectLineWidth)

public:
	SelectLineWidth(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~SelectLineWidth();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LINE_WIDTH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int l_width;
	CSliderCtrl m_Sliderctr;
	CEdit m_lwidth;
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedOk();
};
