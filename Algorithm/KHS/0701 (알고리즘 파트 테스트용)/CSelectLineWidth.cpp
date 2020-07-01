// CSelectLineWidth.cpp: 구현 파일
//

#include "pch.h"
#include "MFCparam.h"
#include "CSelectLineWidth.h"
#include "afxdialogex.h"

// CSelectLineWidth 대화 상자

IMPLEMENT_DYNAMIC(CSelectLineWidth, CDialogEx)

CSelectLineWidth::CSelectLineWidth(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CSelect_LINE_WIDTH, pParent)
{

}

CSelectLineWidth::~CSelectLineWidth()
{
}

void CSelectLineWidth::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_LW, m_Sliderctr);
	DDX_Control(pDX, IDC_EDIT_LW, m_lwidth);
}


BEGIN_MESSAGE_MAP(CSelectLineWidth, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSelectLineWidth::OnBnClickedOk)
	ON_WM_HSCROLL()
    ON_EN_CHANGE(IDC_EDIT_LW, &CSelectLineWidth::OnEnChangeEditLw)
END_MESSAGE_MAP()


// CSelectLineWidth 메시지 처리기

BOOL CSelectLineWidth::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    l_width = 1;

    m_Sliderctr.SetRange(1, 10);

    m_Sliderctr.SetRangeMin(1);

    m_Sliderctr.SetRangeMax(10);

    m_Sliderctr.SetPos(l_width);

    m_Sliderctr.SetTicFreq(1);

    m_Sliderctr.SetLineSize(1);

    m_Sliderctr.SetPageSize(1);

    l_width = m_Sliderctr.GetPos();

    CString sPos;
    sPos.Format(_T("%d"), l_width);
    m_lwidth.SetWindowTextW(sPos);

	return TRUE;
}

void CSelectLineWidth::OnBnClickedOk()
{
    l_width = m_Sliderctr.GetPos();
	CDialogEx::OnOK();
}


void CSelectLineWidth::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    if (IDC_SLIDER_LW == pScrollBar->GetDlgCtrlID())
    {
        l_width = m_Sliderctr.GetPos();
        CString sPos;
        sPos.Format(_T("%d"), l_width);
        m_lwidth.SetWindowTextW(sPos);

    }

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}




void CSelectLineWidth::OnEnChangeEditLw()
{
    if (UpdateData(TRUE)) {
        CString lw;
        m_lwidth.GetWindowTextW(lw);
        l_width = _ttoi(lw);
        m_Sliderctr.SetPos(l_width);
    }
}
