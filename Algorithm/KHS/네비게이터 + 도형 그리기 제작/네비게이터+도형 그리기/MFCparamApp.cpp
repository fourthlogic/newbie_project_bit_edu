
// MFCparam.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "MFCparam.h"
#include "MainFrm.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCparamApp

BEGIN_MESSAGE_MAP(CMFCparamApp, CWinApp)
	ON_COMMAND(ID_FILE_OPEN, &CMFCparamApp::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE_WITHSHAPE, &CMFCparamApp::OnFileSaveWithshape)
	ON_COMMAND(ID_FILE_SAVE_ONLYIMG, &CMFCparamApp::OnFileSaveOnlyimg)
	ON_COMMAND(ID_APP_EXIT, &CMFCparamApp::OnAppExit)
	ON_COMMAND(ID_OPTION_OPEN, &CMFCparamApp::OnOptionOpen)
	ON_COMMAND(ID_APP_ABOUT, &CMFCparamApp::OnAppAbout)
	ON_COMMAND(ID_OPTION_SAVE, &CMFCparamApp::OnOptionSave)
END_MESSAGE_MAP()


// CMFCparamApp 생성

CMFCparamApp::CMFCparamApp() noexcept
{
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 애플리케이션을 공용 언어 런타임 지원을 사용하여 빌드한 경우(/clr):
	//     1) 이 추가 설정은 다시 시작 관리자 지원이 제대로 작동하는 데 필요합니다.
	//     2) 프로젝트에서 빌드하려면 System.Windows.Forms에 대한 참조를 추가해야 합니다.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 아래 애플리케이션 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("MFCparam.AppID.NoVersion"));

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 CMFCparamApp 개체입니다.

CMFCparamApp theApp;


// CMFCparamApp 초기화

BOOL CMFCparamApp::InitInstance()
{
	// 애플리케이션 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit 컨트롤을 사용하려면 AfxInitRichEdit2()가 있어야 합니다.
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 애플리케이션 마법사에서 생성된 애플리케이션"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.


	// 애플리케이션의 문서 템플릿을 등록합니다.  문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.

	// Option(설정창) 문서 템플릿 추가
	pDocTemplate1 = new CMultiDocTemplate(IDR_MAINFRAME,
		RUNTIME_CLASS(COptionDoc),
		RUNTIME_CLASS(COptionFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(COptionFormView));
	if (!pDocTemplate1)
		return FALSE;
	AddDocTemplate(pDocTemplate1);

	// 이미지 뷰어 문서 템플릿 추가
	pDocTemplate2 = new CMultiDocTemplate(IDR_MAINFRAME,
		RUNTIME_CLASS(CImgViewerDoc),
		RUNTIME_CLASS(CImgViewerFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CImgViewerView));
	if (!pDocTemplate2)
		return FALSE;
	AddDocTemplate(pDocTemplate2);

	pDocTemplate3 = new CMultiDocTemplate(IDR_MAINFRAME,
		RUNTIME_CLASS(CNavigatorDoc),
		RUNTIME_CLASS(CNavigatorFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CNavigatorView));
	if (!pDocTemplate3)
		return FALSE;
	AddDocTemplate(pDocTemplate3);

	// 주 MDI 프레임 창을 만듭니다.
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	//if (!ProcessShellCommand(cmdInfo))
	//	return FALSE;

	// Child 프레임 창들을 미리 띄우기 위함
	pDocTemplate1->OpenDocumentFile(NULL); // 파라미터 창
	pDocTemplate3->OpenDocumentFile(NULL); // 네비게이터 창
	pDocTemplate2->OpenDocumentFile(NULL); // 이미지 뷰 창

	// 윈도우들의 포인터 저장
	POSITION posDocument = pDocTemplate1->GetFirstDocPosition(); // 도큐먼트 포지션 얻기
	pOptionDoc = (COptionDoc*)pDocTemplate1->GetNextDoc(posDocument); // 설정창 Doc 포인터
	POSITION posView = pOptionDoc->GetFirstViewPosition(); // 뷰 포지션 얻기
	pOptionView = (COptionFormView*)pOptionDoc->GetNextView(posView); // 설정창 View 포인터
	pOptionFrame = (COptionFrame*)pOptionView->GetParentFrame(); // 설정창 Frame 포인터

	posDocument = pDocTemplate2->GetFirstDocPosition();
	pImgViewerDoc = (CImgViewerDoc*)pDocTemplate2->GetNextDoc(posDocument); // 이미지뷰 창 Doc 포인터
	posView = pImgViewerDoc->GetFirstViewPosition();
	pImgViewerView = (CImgViewerView*)pImgViewerDoc->GetNextView(posView); // 이미지뷰 창 View 포인터
	pImgViewerFrame = (CImgViewerFrame*)pImgViewerView->GetParentFrame(); // 이미지뷰 창 Frame 포인터

	posDocument = pDocTemplate3->GetFirstDocPosition();
	pNavigatorDoc = (CNavigatorDoc*)pDocTemplate3->GetNextDoc(posDocument); // 네비게이터 창 Doc 포인터
	posView = pNavigatorDoc->GetFirstViewPosition();
	pNavigatorView = (CNavigatorView*)pNavigatorDoc->GetNextView(posView); // 네비게이터 창 View 포인터
	pNavigatorFrame = (CNavigatorFrame*)pNavigatorView->GetParentFrame(); // 네비게이터 창 Frame 포인터
	
	// 주 창이 초기화되었으므로 이를 표시하고 업데이트합니다.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CMFCparamApp::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

// CMFCparamApp 메시지 처리기


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CMFCparamApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMFCparamApp 메시지 처리기

void CMFCparamApp::OnFileOpen() // 파일 열기
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	//CImgViewerView* pView = pMain->pImgViewerView;
	//pView->OnFileOpen();
	pImgViewerView->OnFileOpen();
	pNavigatorView->OnFileOpen();
	pImgViewerView->imgViewer2Navigator();
}

void CMFCparamApp::OnFileSaveWithshape() // 파일 저장 - 도형 포함
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	//CImgViewerView* pView = pMain->pImgViewerView;
	//pView->OnFileSaveWithshape();
	pImgViewerView->OnFileSaveWithshape();
}

void CMFCparamApp::OnFileSaveOnlyimg() // 파일 저장 - 도형 제외
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	//CImgViewerView* pView = pMain->pImgViewerView;
	//pView->OnFileSaveOnlyimg();
	pImgViewerView->OnFileSaveOnlyimg();
}

void CMFCparamApp::OnAppExit() // 프로그램 종료
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	//COptionFormView* pView = (COptionFormView*)pFrame->pOptionView;
	//pFrame->pOptionView->OnAppExit();
	pOptionView->OnAppExit();
}

void CMFCparamApp::OnOptionOpen() // 설정파일 열기
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	//COptionFormView* pView = (COptionFormView*)pFrame->pOptionView;
	//pFrame->pOptionView->OnOptionOpen();
	pOptionView->OnOptionOpen();
}

void CMFCparamApp::OnOptionSave() // 설정파일 저장
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	//COptionFormView* pView = (COptionFormView*)pFrame->pOptionView;
	//pFrame->pOptionView->OnOptionSave();
	pOptionView->OnOptionSave();
}
