// COptionDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MFCparam.h"
#include "COptionDlg.h"
#include "afxdialogex.h"


// COptionDlg 대화 상자

IMPLEMENT_DYNAMIC(COptionDlg, CDialogEx)

COptionDlg::COptionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_OPTION_OPEN, pParent)
{

}

COptionDlg::~COptionDlg()
{
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_OPTION, listView);
}


BEGIN_MESSAGE_MAP(COptionDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_OPTION_OPEN, &COptionDlg::OnBnClickedButtonOptionOpen)
END_MESSAGE_MAP()


// COptionDlg 메시지 처리기


BOOL COptionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	TCHAR* szText[5] = { _T("File Name"), _T("Distance"), _T("RadMax"), _T("RadMin"), _T("BGV") };
	int nWidth[5] = { 130, 70, 70, 70, 70 };

	LV_COLUMN iCol;
	iCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH; // 각 멤버가 유효한 정보를 가지고 있는지를 명시하는 변수
	iCol.fmt = LVCFMT_LEFT; // 정렬
	listView.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	for (int i = 0; i < 5; i++)
	{
		iCol.pszText = (LPTSTR)szText[i];
		iCol.cx = nWidth[i];
		listView.InsertColumn(i, &iCol);
	}

	listView.Scroll(0); // 처음에 스크롤 바로 나오게 함

	// ==================== db 에서 받아오는 부분 ========================
	CDatabase db;
	CString strCon = _T("Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=D://access test folder//Database2.accdb");
	if (!db.OpenEx(strCon))
	{
		MessageBox(_T("설정값 저장 파일이 연결되지 않았습니다."));
	}
	CRecordset rs(&db);
	try
	{
		rs.Open(CRecordset::dynaset, _T("SELECT * FROM [Params]"));

		int col = rs.GetODBCFieldCount(); // 열의 갯수
		int row = 0;

		TCHAR szText[100] = _T("");
		LVITEM item;
		item.mask = LVIF_TEXT;

		while (!rs.IsEOF())
		{
			item.iItem = row;
			for (int i = 0; i < col; i++)
			{
				item.iSubItem = i;
				CString temp = _T("");
				rs.GetFieldValue(i, temp);
				wsprintf(szText, _T("%s"), temp);
				item.pszText = (LPTSTR)szText;
				if (i == 0)
					listView.InsertItem(&item);
				else
					listView.SetItem(&item);
				wsprintf(szText, _T(""));
			}
			row++;
			rs.MoveNext();
		}
		//nRow = row;
		//nRow = rs.GetRecordCount();
		db.Close();
	}
	catch (CDBException* e)
	{
		//e->ReportError();
		AfxMessageBox(_T("Database error: ") + e->m_strError);

	}
	// ==================== db 에서 받아오는 부분 ========================

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void COptionDlg::OnBnClickedButtonOptionOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	COptionFormView* pView = (COptionFormView*)pFrame->pOptionView;
	
	int count = listView.GetItemCount();
	
	for (int i = 0; i < count; i++)
	{
		if (LVIS_SELECTED == listView.GetItemState(i, LVIS_SELECTED))
		{
			pView->strFilename = listView.GetItemText(i, 0);
			pView->m_strDist = listView.GetItemText(i, 1);
			pView->m_strRadMax = listView.GetItemText(i, 2);
			pView->m_strRadMin = listView.GetItemText(i, 3);
			pView->m_strBGV = listView.GetItemText(i, 4);
		}
	}
	pView->paraChanged2();
	OnOK();
}

