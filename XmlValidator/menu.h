
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

#include "dialog.h"

namespace Matrix
{
	class MainWindow;

	class Menu
	{
	public:
		Menu()
		{}

		int Init(HWND hwnd, HINSTANCE hinst)
		{
			HMENU menu = LoadMenu(hinst, MAKEINTRESOURCE(IDR_MENU));
			SetMenu(hwnd, menu);
			CheckMenuItem(menu, IDM_WRAP, MF_CHECKED);
			CheckMenuItem(menu, IDM_AUTOVALIDATE, MF_UNCHECKED);
			EnableMenuItem(menu, IDM_PRINT, MF_GRAYED);
			EnableMenuItem(menu, IDM_REPLACE, MF_GRAYED);
			return 0;
		}

		int HandleMessage(HWND hwnd, SciEditor *editor, WPARAM wParam, LPARAM lParam)
		{
			HMENU h_menu = GetMenu(hwnd);
			bool checked = true;
			UINT wmId = 0, wmEvent = 0;
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			// �����˵�ѡ��: 
			switch (wmId)
			{
			case IDM_ABOUT:
				DialogBox(NULL, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, Dialog::About);
				break;

			case IDM_NEW:
				editor->New();
				break;

			case IDM_OPEN:
				editor->OpenFileDlg(hwnd);
				break;

			case IDM_SAVE:
				editor->Save();
				break;

			case IDM_SAVEAS:
				editor->SaveAs();
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
				CheckMenuItem(h_menu, IDM_WRAP, (!checked ? MF_CHECKED : MF_UNCHECKED));
				editor->SetWrap(!checked);				
				break;

			case IDM_VALIDATE:
				editor->ValidateXml();
				break;

			case IDM_AUTOVALIDATE:
				checked = MF_CHECKED == GetMenuState(h_menu, IDM_AUTOVALIDATE, MF_BYCOMMAND);
				CheckMenuItem(h_menu, IDM_AUTOVALIDATE, !checked ? MF_CHECKED : MF_UNCHECKED);
				editor->SetAutoValidate(!checked);
				break;

			default:
				return 0;
			}
			return 1;
		}		
		
	};
}

#endif