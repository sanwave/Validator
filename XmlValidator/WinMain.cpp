
/*
*
*
*
*	Program Entry		In Matrix
*
*	Created by Bonbon	2014.09.27
*
*/


#include "MainWindow.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(pCmdLine);

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		return FALSE;
	}

	Matrix::MainWindow win;

	if (!win.Create(L"Matrix", WS_OVERLAPPEDWINDOW,hInstance))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

	// Run the message loop.

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CoUninitialize();

	return (int)msg.wParam;
}
