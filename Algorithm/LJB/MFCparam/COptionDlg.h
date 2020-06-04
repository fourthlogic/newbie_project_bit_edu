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
	// 리스트 컨트롤 변수
	CListCtrl listView; // 설정 파일 리스트 컨트롤
	CString fileName; // 설정값의 이름
	CString listDist; // 거리
	CString listRadMax; // 최대 반지름
	CString listRadMin; // 최소 반지름
	CString listBGV; // BGV
	//int nRow; // 

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonOptionOpen();
};
