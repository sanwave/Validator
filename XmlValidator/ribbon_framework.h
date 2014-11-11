
#pragma once

#include <windows.h>
#include <UIRibbon.h>

#include "application.h"
#include "sci_editor.h"

extern IUIFramework *g_pFramework;  // Reference to the Ribbon framework.
extern IUIApplication *g_pApplication;  // Reference to the Application object.

class MainFrame :IUIFramework
{
public:
	static bool InitializeFramework(HWND hWnd, Matrix::SciEditor *);
	static void DestroyFramework();
	static void UpdateLayout();

	static Matrix::SciEditor * Editor;
	static HWND Win;
};

