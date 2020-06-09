// LINE_WIDTH.cpp: 구현 파일
//

#include "pch.h"
#include "MFCparam.h"
#include "LINE_WIDTH.h"
#include "afxdialogex.h"


// LINE_WIDTH 대화 상자

IMPLEMENT_DYNAMIC(LINE_WIDTH, CDialogEx)

LINE_WIDTH::LINE_WIDTH(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LINE_WIDTH, pParent)
{

}

LINE_WIDTH::~LINE_WIDTH()
{
}

void LINE_WIDTH::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LINEWIDTH, m_ctrSlider);
    DDX_Control(pDX, IDC_LWIDTHTXT, m_lwidthtxt);
}


BEGIN_MESSAGE_MAP(LINE_WIDTH, CDialogEx)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_BtnOK, &LINE_WIDTH::OnBnClickedBtnok)
END_MESSAGE_MAP()


// LINE_WIDTH 메시지 처리기


BOOL LINE_WIDTH::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    //CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();  // 프레임 윈도우 포인터 구하고...
    //CImgViewerView* pView = (CImgViewerView*)pFrame->GetActiveView(); // 활성화된 뷰의 포인터 구한다.
    //int a = pView->l_width;
    m_ctrSlider.SetRange(1, 10);

    // 최소 값 설정.
    m_ctrSlider.SetRangeMin(1);

    // 최대 값을 설정.
    m_ctrSlider.SetRangeMax(10);

    // 위치 설정.
    m_ctrSlider.SetPos(1);

    // 눈금 간격을 설정한다.
    // 속성의 Tick Marks와 Auto Ticks가 True로 되어 있어야 한다.
    m_ctrSlider.SetTicFreq(1);

    // 키보드 커서키로 슬라이더를 움직일때의 증가 크기를 설정
    m_ctrSlider.SetLineSize(1);

    // 키보드의 PgUp, PgDn키를 누르거나 마우스로 슬라이더의 몸동을 클릭시 움직일 크기
    m_ctrSlider.SetPageSize(1);

    // 선택영역 지운기.
    m_ctrSlider.ClearSel();
    int iPos = m_ctrSlider.GetPos();
    CString sPos;
    sPos.Format(_T("%d"), iPos);
    m_lwidthtxt.SetWindowTextW(sPos);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void LINE_WIDTH::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (IDC_LINEWIDTH == pScrollBar->GetDlgCtrlID())
    {
        int iPos = m_ctrSlider.GetPos();
        CString sPos;
        sPos.Format(_T("%d"), iPos);
        m_lwidthtxt.SetWindowTextW(sPos);

    }

    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void LINE_WIDTH::OnBnClickedBtnok()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    l_width = m_ctrSlider.GetPos();

    OnOK();
}
