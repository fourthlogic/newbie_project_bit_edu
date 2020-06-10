// SelectLineWidth.cpp: 구현 파일
//

#include "pch.h"
#include "MFCparam.h"
#include "SelectLineWidth.h"
#include "afxdialogex.h"


// SelectLineWidth 대화 상자

IMPLEMENT_DYNAMIC(SelectLineWidth, CDialogEx)

SelectLineWidth::SelectLineWidth(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LINE_WIDTH, pParent)
{

}

SelectLineWidth::~SelectLineWidth()
{
}

void SelectLineWidth::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_LW, m_Sliderctr);
	DDX_Control(pDX, IDC_EDIT_LW, m_lwidth);
}


BEGIN_MESSAGE_MAP(SelectLineWidth, CDialogEx)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDOK, &SelectLineWidth::OnBnClickedOk)
END_MESSAGE_MAP()


// SelectLineWidth 메시지 처리기


BOOL SelectLineWidth::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
    l_width = 1;
    m_Sliderctr.SetRange(1, 10);

    // 최소 값 설정.
    m_Sliderctr.SetRangeMin(1);

    // 최대 값을 설정.
    m_Sliderctr.SetRangeMax(10);

    // 위치 설정.
    m_Sliderctr.SetPos(1);

    // 눈금 간격을 설정한다.
    // 속성의 Tick Marks와 Auto Ticks가 True로 되어 있어야 한다.
    m_Sliderctr.SetTicFreq(1);

    // 키보드 커서키로 슬라이더를 움직일때의 증가 크기를 설정
    m_Sliderctr.SetLineSize(1);

    // 키보드의 PgUp, PgDn키를 누르거나 마우스로 슬라이더의 몸동을 클릭시 움직일 크기
    m_Sliderctr.SetPageSize(1);

    // 선택영역 지운기.
    //m_Sliderctr.ClearSel();
    l_width = m_Sliderctr.GetPos();
    CString sPos;
    sPos.Format(_T("%d"), l_width);
    m_lwidth.SetWindowTextW(sPos);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void SelectLineWidth::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (IDC_SLIDER_LW == pScrollBar->GetDlgCtrlID())
    {
        l_width = m_Sliderctr.GetPos();
        CString sPos;
        sPos.Format(_T("%d"), l_width);
        m_lwidth.SetWindowTextW(sPos);

    }
    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void SelectLineWidth::OnBnClickedOk()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    l_width = m_Sliderctr.GetPos();
    CDialogEx::OnOK();
}
