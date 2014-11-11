

#include "ribbon_framework.h"

IUIFramework *g_pFramework = NULL;  // Reference to the Ribbon framework.
IUIApplication *g_pApplication = NULL;  // Reference to the Application object.

Matrix::SciEditor * MainFrame::Editor;
HWND MainFrame::Win;


bool MainFrame::InitializeFramework(HWND hWnd, Matrix::SciEditor * editor)
{
    // Here we instantiate the Ribbon framework object.
    HRESULT hr = CoCreateInstance(CLSID_UIRibbonFramework, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_pFramework));
    if (FAILED(hr))
    {
        return false;
    }   

    hr = CApplication::CreateInstance(&g_pApplication);
    if (FAILED(hr))
    {
        return false;
    } 

    hr = g_pFramework->Initialize(hWnd, g_pApplication);
    if (FAILED(hr))
    {
        return false;
    }

    hr = g_pFramework->LoadUI(GetModuleHandle(NULL), L"RIBBON_UI_RIBBON");
    if (FAILED(hr))
    {
        return E_FAIL;
    }

	Win = hWnd;
	Editor = editor;

    return true;
}


void MainFrame::DestroyFramework()
{
    if (g_pFramework)
    {
        g_pFramework->Destroy();
        g_pFramework->Release();
        g_pFramework = NULL;
    }

    if (g_pApplication)
    {
        g_pApplication->Release();
        g_pApplication = NULL;
    }
}

void MainFrame::UpdateLayout()
{
	RECT rect;
	GetClientRect(Win,&rect);
	Editor->SetPos(rect, CApplication::RibbonHeight());
}
