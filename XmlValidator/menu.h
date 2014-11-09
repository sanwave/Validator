
/*
*
*
*
*	Menu Class		In Matrix
*
*	Created by Bonbon	2014.11.09
*
*	Updated by Bonbon	2014.11.09
*
*/


#ifndef _MENU_H_
#define _MENU_H_

#ifndef MATRIX
#include <iostream>
#include <string>
#else
#include "common.h"
#endif

#include "sci_editor.h"

namespace Matrix
{
	class MainWindow;

	class Menu
	{
	public:
		Menu() : m_auto_validate(false)
		{}

		bool AutoValidate()
		{
			return m_auto_validate;
		}

		int HandleMessage(HWND hwnd, SciEditor *editor, WPARAM wParam, LPARAM lParam)
		{
			HMENU h_menu = GetMenu(hwnd);
			bool checked = true;
			UINT wmId = 0, wmEvent = 0;
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			// ·ÖÎö²Ëµ¥Ñ¡Ôñ: 
			switch (wmId)
			{
			case IDM_ABOUT:
				//DialogBox(nullptr, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;

			case IDM_OPEN:
				editor->OpenFileDlg(hwnd,m_auto_validate);
				break;

			case IDM_SAVE:
				editor->Save();
				break;

			case IDM_PRINT:
				break;

			case IDM_EXIT:
				DestroyWindow(hwnd);
				break;

			case IDM_FIND:
				editor->Search(NULL);
				break;

			case IDM_WRAP:
				checked = MF_CHECKED == GetMenuState(h_menu, IDM_WRAP, MF_BYCOMMAND);
				editor->SetWrap(!checked);
				CheckMenuItem(h_menu, IDM_WRAP, (!checked ? MF_CHECKED : MF_UNCHECKED));
				break;

			case IDM_VALIDATE:
				editor->ValidateXml(NULL);
				break;

			case IDM_AUTOVALIDATE:
				m_auto_validate = MF_CHECKED != GetMenuState(h_menu, IDM_AUTOVALIDATE, MF_BYCOMMAND);
				CheckMenuItem(h_menu, IDM_AUTOVALIDATE, m_auto_validate ? MF_CHECKED : MF_UNCHECKED);
				break;

			default:
				return 0;
			}
			return 1;
		}

	private:
		bool m_auto_validate;
		
	};
}

#endif