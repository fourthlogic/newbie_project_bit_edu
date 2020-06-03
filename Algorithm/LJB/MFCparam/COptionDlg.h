#pragma once
#include "COptionFormView.h"
#include <vector>
// COptionDlg 대화 상자

class COptionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COptionDlg)

public:
	COptionDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~COptionDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_OPTION_OPEN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl listView;
	CString fileName;
	CString listDist;
	CString listRadMax;
	CString listRadMin;
	CString listBGV;
	//CString listThMax;
	//CString listThMin;

	int nRow;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonOptionOpen();
};
