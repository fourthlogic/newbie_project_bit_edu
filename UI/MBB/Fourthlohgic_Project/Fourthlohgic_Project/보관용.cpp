// ChildView.cpp: CChildView Ŭ������ ����
//

#include "pch.h"
#include "framework.h"
#include "Fourthlohgic_Project.h"
#include "ChildView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CFourthlohgicProjectApp theApp;
// CChildView

CChildView::CChildView()
{
	if (!image.IsNull())
		image.Destroy();
	image.Load(theApp.sFilename);

	m_background.Attach(image.Detach());
	m_background.GetObject(sizeof(BITMAP), (LPVOID)&m_Bitmap);

	m_ePt = CPoint(0, 0);


	m_bkgBrush.CreateSolidBrush(0x00000000);

	m_Zoom = 1.f;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()



// CChildView �޽��� ó����

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

	return TRUE;
}

void CChildView::OnPaint()
{
	CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	//---------------------------------------------------
	CDC* pDC = GetDC();

	// �̹� ����� OnInitDialog() Ȥ�� OnInitialUpdate()���� �ε�Ǿ� �����Ƿ� �ٽ� �� �ʿ�� ����.
	memDC.CreateCompatibleDC(pDC);
	mdcOffScreen.CreateCompatibleDC(pDC);

	// ȭ�� ũ��� ������� ���۸� ���� �Ѵ�.
	bmpOffScreen.CreateCompatibleBitmap(pDC, m_Bitmap.bmWidth, m_Bitmap.bmHeight);
	// ���� dmemDC�� �޸𸮿��� �ƹ��� �׸��� ����.
	// ���� � ����� ä����� �Ѵٸ� FillRect() �Լ������� Ư�������� ĥ�� �� �ִ�.
	// �׷��� ������ ��� �׸��� �ε��ϹǷ� �ʿ���� ���̴�.

	oldbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);
	// ���� �޸� �غ�� ������. ���� ���ʹ� �׸��� �׸���.
	//�켱 ��� �׸��� �� ���̹Ƿ� ����� �޸𸮿� ���� �Ѵ�.

	memDC.SelectObject(&m_background);   // ��� �׸��� �����ϰ�

	mdcOffScreen.SetStretchBltMode(COLORONCOLOR);


	mdcOffScreen.StretchBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight,
		&memDC, m_ePt.x - m_Zoom, m_ePt.y - m_Zoom, m_Bitmap.bmWidth / m_Zoom, m_Bitmap.bmHeight / m_Zoom, SRCCOPY);
	// ==> ����� �޸𸮹��ۿ� ���� �Ѵ�. ���� ȭ�鿡�� ��Ÿ���� �ʴ´�.
	//���� �׸��� ȭ�鿡 ��Ÿ���� �ʰ�, ������� �����.
	//������� �Ͱ� �Ѵٸ�
	//pDC->BitBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &memDC, 0, 0, SRCCOPY); 
	//    ���� �� �־� ȭ���� Ȯ���ϰ� ������� ������ ���� �Ѵ�.
   // �̹����� BitBlt ���� �ٸ� ������ �Լ��� ���� �޸𸮿� ���� �Ѵ�.
	//----------
	//mdcOffScreen.SetTextColor((COLORREF)0x00FFFFFF);
	//==> �̰��� pDC->SetTextColor( (COLORREF) 0x00FFFFFF );�ʹ� ������ �������
	//mdcOffScreen.SetBkMode(TRANSPARENT);   // ������ ������ ���ش�.
	//-----------
	// ������� ��� �׸��� �ϼ��Ǿ� ����, ���� ǥ�� ���ۿ� ��µ� ���°� �ƴϴ�. ������� �غ��� ���� �׸��� ǥ�õ��� �ʴ´�.
	// ���������� ǥ�� ȭ�� �޸𸮿� ���� �Ѵ�.

	pDC->BitBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &mdcOffScreen, 0, 0, SRCCOPY);

	// �̶����� ȭ�鿡 �׸��� ��Ÿ����.
	memDC.DeleteDC();
	mdcOffScreen.SelectObject(oldbitmap);
	mdcOffScreen.DeleteDC();
	bmpOffScreen.DeleteObject();
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (m_Bitmap.bmWidth < m_bgRect.right) {
		RECT rect2;
		rect2.left = m_Bitmap.bmWidth;
		rect2.right = m_bgRect.right;
		rect2.top = 0;
		rect2.bottom = m_Bitmap.bmHeight;
		//m_ePt.x = 0;
		pDC->FillRect(&rect2, &m_bkgBrush);  // B���� ĥ�ϱ�
	}
	if (m_Bitmap.bmHeight < m_bgRect.bottom) {
		RECT rect2;
		rect2.left = 0;
		rect2.right = m_bgRect.right;
		rect2.top = m_Bitmap.bmHeight;
		rect2.bottom = m_bgRect.bottom;
		//m_ePt.y = 0;
		pDC->FillRect(&rect2, &m_bkgBrush); // C���� ĥ�ϱ�
	}

	//return CWnd::OnEraseBkgnd(pDC);
	return false;
}


void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CWnd::OnMouseMove(nFlags, point);

	if (nFlags & MK_LBUTTON)
	{
		// �̵� ũ�� ���ϱ�
		/*CSize offset = point - m_sPt;
		CDC* pDC = GetDC();*/


		int width = m_bgRect.right - m_bgRect.left;
		int height = m_bgRect.bottom - m_bgRect.top;

		// A������ �簢�� �׸��� //m_sPt�� ���� ����Ʈ, point�� �̵�
		if (m_sPt.x < point.x) // ���������� ��������
		{
			m_ePt.x -= point.x - m_sPt.x;
			if (m_ePt.x < 0)
				m_ePt.x = 0;
		}
		else // �������� ��������
		{
			m_ePt.x += m_sPt.x - point.x;
			if (m_Bitmap.bmWidth > m_bgRect.right)
			{
				if (m_ePt.x > m_Bitmap.bmWidth - width)
					m_ePt.x = m_Bitmap.bmWidth - width;
			}
		}

		// B������ �簢�� �׸���
		if (m_sPt.y < point.y) // ���� �÷�����
		{
			m_ePt.y -= point.y - m_sPt.y;
			if (m_ePt.y < 0)
				m_ePt.y = 0;
		}
		else // �Ʒ��� ��������
		{
			m_ePt.y += m_sPt.y - point.y;
			if (m_Bitmap.bmHeight > m_bgRect.bottom)
			{
				if (m_ePt.y > m_Bitmap.bmHeight - height)
					m_ePt.y = m_Bitmap.bmHeight - height;
			}
		}

		m_sPt = point;
		Invalidate();
	}
}


void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CWnd::OnLButtonDown(nFlags, point);
	m_sPt = point;
}


void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	m_bgRect.left = 0;
	m_bgRect.top = 0;
	m_bgRect.right = cx;
	m_bgRect.bottom = cy;
}


BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (zDelta <= 0)
	{
		m_Zoom += 1.25f;
	}
	else
	{
		m_Zoom -= 0.8f;
	}

	m_ePt = pt;

	Invalidate(0);
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}



if (zDelta <= 0)
{
	m_Zoom += 1.25f;
	m_ePt.x = (m_Bitmap.bmWidth / 2) - (m_Bitmap.bmWidth / m_Zoom);
	m_ePt.y = (m_Bitmap.bmHeight / 2) - (m_Bitmap.bmHeight / m_Zoom);
}
else
{
	m_Zoom -= 0.8f;
	m_ePt.x = (m_Bitmap.bmWidth / 2) - (m_Bitmap.bmWidth * m_Zoom);
	m_ePt.y = (m_Bitmap.bmHeight / 2) - (m_Bitmap.bmHeight * m_Zoom);
}


int width = m_bgRect.right - m_bgRect.left;
int height = m_bgRect.bottom - m_bgRect.top;

//���� ����
m_ePt.x = pt.x - ((pt.x - m_ePt.x) * m_Zoom);
m_ePt.y = pt.y - ((pt.y - m_ePt.y) * m_Zoom);

//������ ����

m_aPt.x = pt.x - ((width - pt.x) * m_Zoom);
m_aPt.y = pt.y - ((width - pt.y) * m_Zoom);