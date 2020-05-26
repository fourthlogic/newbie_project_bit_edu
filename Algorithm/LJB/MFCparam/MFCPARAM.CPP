#include "pch.h"
#include "MFCparam.h"


void CMFCparamApp::OnFileNew()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CMultiDocTemplate* pDocTemplate = NULL;
	POSITION pos = GetFirstDocTemplatePosition();
	pDocTemplate = (CMultiDocTemplate*)GetNextDocTemplate(pos);
	pDocTemplate = (CMultiDocTemplate*)GetNextDocTemplate(pos);
	pDocTemplate->OpenDocumentFile(NULL);
}
