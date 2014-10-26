
/*
*
*
*
*	File Class		In Matrix
*
*	Created by Bonbon	2014.09.25
*
*/


#include "common.h"
#include <scintilla.h> 
#include <scilexer.h> 

class SciEditor
{
public:

	void Create(HWND hwndParent)
	{
		if (NULL == LoadLibrary(L"SciLexer.dll"))
		{
			MessageBox(hwndParent, L"Load SciLexer.dll Failed! ", L"Error", MB_OK);
			exit(0);
		}

		m_hwnd = ::CreateWindowExW(0, L"Scintilla",
			L"", WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
			0, 0, 0, 0,
			hwndParent,
			NULL, NULL, NULL);

		m_fnDirect = (SciFnDirect)SendMessage(m_hwnd, SCI_GETDIRECTFUNCTION, 0, 0);
		m_ptrDirect = (sptr_t)SendMessage(m_hwnd, SCI_GETDIRECTPOINTER, 0, 0);
	}

	void Init()
	{
		SendEditor(SCI_SETLEXER, SCLEX_CPP);
		SendEditor(SCI_STYLESETFONT, STYLE_DEFAULT, (sptr_t)"Consolas");
		SendEditor(SCI_STYLESETSIZE, STYLE_DEFAULT, 12);

		SendEditor(SCI_SETCARETLINEVISIBLE, TRUE);
		SendEditor(SCI_SETCARETLINEBACK, 0xb0ffff);

		SendEditor(SCI_SETMARGINTYPEN, 2, SC_MARGIN_NUMBER);
		SendEditor(SCI_SETMARGINWIDTHN, 2, 29);

		SendEditor(SCI_SETTABWIDTH, 4);

		SendEditor(SCI_SETCODEPAGE, SC_CP_UTF8);

		SendEditor(SCI_SETWRAPMODE, (WPARAM)1);

		SendEditor(SCI_STYLECLEARALL);
	}

	void SetPos(RECT rect)
	{
		SetWindowPos(m_hwnd, HWND_TOP, 0, 0, rect.right - rect.left - 11, rect.bottom - rect.top - 50, SWP_SHOWWINDOW);
	}

	void SetText(const char *text)
	{
		SendEditor(SCI_SETTEXT, NULL, (sptr_t)text);
	}

	sptr_t SendEditor(unsigned int iMessage, uptr_t wParam = 0, sptr_t lParam = 0)
	{
		return m_fnDirect(m_ptrDirect, iMessage, wParam, lParam);
	}

private:
	HWND m_hwnd;
	SciFnDirect m_fnDirect;
	sptr_t m_ptrDirect;
};