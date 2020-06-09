#pragma once


// LINE_WIDTH 대화 상자

class LINE_WIDTH : public CDialogEx
{
	DECLARE_DYNAMIC(LINE_WIDTH)

public:
	LINE_WIDTH(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~LINE_WIDTH();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LINE_WIDTH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int l_width;
	CSliderCtrl m_ctrSlider;
	virtual BOOL OnInitDialog();
	CEdit m_lwidthtxt;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedBtnok();
};
