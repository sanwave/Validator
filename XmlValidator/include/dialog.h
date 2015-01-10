
/*
*
*
*
*	Dialog Class		In Matrix
*
*	Created by Bonbon	2014.11.15
*
*	Updated by Bonbon	2014.11.15
*
*/


#ifndef _DIALOG_H_
#define _DIALOG_H_

#include "common.h"

namespace Matrix
{
	class Dialog
	{
	public:
		static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
		{
			UNREFERENCED_PARAMETER(lParam);
			switch (message)
			{
			case WM_INITDIALOG:
				RECT rect;
				ZeroMemory(&rect, sizeof(rect));
				GetWindowRect(hDlg, &rect);
				//rect.top += 200;
				//rect.left += 100;
				//SetWindowPos(hDlg, HWND_TOP, rect.left+400, rect.top+200, rect.right, rect.bottom, SWP_SHOWWINDOW);
				
				return (INT_PTR)TRUE;

			case WM_COMMAND:
				if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
				{
					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;
				}
				break;
			}
			return (INT_PTR)FALSE;
		}

		/// <summary>
		/// 在窗口上绘制指定文本
		/// </summary>
		static int DrawText(HWND hwnd, LPCWSTR text)
		{
			HDC m_hdc = GetDC(hwnd);
			RECT rect;

			HFONT font = CreateFontW(18, 0, 0, 0, 300, false, false, false, DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI");

			HFONT hFontOld = (HFONT)SelectObject(m_hdc, font);

			GetClientRect(hwnd, &rect);
			SetTextColor(m_hdc, 0x00000000);
			SetBkMode(m_hdc, TRANSPARENT);
			rect.left = 12;
			rect.right -= 12;
			rect.top = 20;
			FillRect(m_hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
			::DrawText(m_hdc, text, -1, &rect, DT_NOCLIP | DT_WORDBREAK | DT_EDITCONTROL);
			SelectObject(m_hdc, hFontOld);
			ReleaseDC(hwnd, m_hdc);

			return 1;
		}

	};
}

#endif
