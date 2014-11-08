
/*
*
*
*
*	MainWindow Class		In Matrix
*
*	Created by Bonbon	2014.09.27
*
*/


#ifndef _MAINWIN_H_
#define _MAINWIN_H_


#include "basewin.h"
#include <commdlg.h>
#include "application.h"

//#include "file.h"
#include "xml_validater.h"
#include "sci_editor.h"
#include "xml.h"


namespace Matrix
{

	class MainWindow : public BaseWindow<MainWindow>
	{
	public:

		MainWindow() : m_auto_validate(false)
		{}

		PCWSTR  ClassName() const { return L"Main Window"; }

		/// <summary>
		/// Windows消息处理
		/// </summary>
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

		/// <summary>
		/// 初始化窗体
		/// </summary>
		int InitWindow()
		{
			LoadComponent();
			HMENU menu;
			menu = GetMenu(m_hwnd);
			CheckMenuItem(menu, IDM_WRAP, MF_CHECKED);
			CheckMenuItem(menu, IDM_AUTOVALIDATE, m_auto_validate ? MF_CHECKED : MF_UNCHECKED);

			bool init_success = true;
			//init_success= InitializeFramework(m_hwnd);
			if (!init_success)
			{
				return -1;
			}
		}

		/// <summary>
		/// 载入所需组件
		/// </summary>
		void LoadComponent()
		{
			//m_editor = &(SciEditor());
			m_editor.Create(m_hwnd);
			m_editor.Init();
		}

		/// <summary>
		/// 自适应父窗体大小
		/// </summary>
		void AdaptSize(RECT &rect)
		{
			m_editor.SetPos(rect);
		}

		/// <summary>
		/// 处理拖放文件
		/// </summary>
		int DropFile(HDROP hDropInfo)
		{
			if (hDropInfo)
			{
				//MAX_PATH为Win定义的文件全名的最大长度，为260
				LPWSTR fileName = new TCHAR[BUFSIZ];
				UINT fileNum = ::DragQueryFile(hDropInfo, 0xffffffff, NULL, 0);
				for (UINT i = 0; i < fileNum; i++)
				{
					::DragQueryFile(hDropInfo, i, fileName, MAX_PATH);
					LoadFile(fileName);
				}
				::DragFinish(hDropInfo);
				delete fileName;
			}
			return 0;
		}

		/// <summary>
		/// 使用打开对话框打开文件
		/// </summary>
		int OpenFileDlg()
		{
			OPENFILENAME ofn;      // 公共对话框结构。     
			wchar_t filename[MAX_PATH]; // 保存获取文件名称的缓冲区。               
			// 初始化选择文件对话框。
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ZeroMemory(filename, MAX_PATH);
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = m_hwnd;
			ofn.lpstrFile = filename;
			ofn.nMaxFile = sizeof(filename);
			ofn.lpstrFilter = L"All(*.*)\0*.*\0Xml(*.xml)\0*.xml\0Header(*.h)\0*.h\0Text(*.txt)\0*.TXT\0\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			//ofn.lpTemplateName =  MAKEINTRESOURCE(ID_TEMP_DIALOG);    
			// 显示打开选择文件对话框。     

			if (GetOpenFileName(&ofn))
			{
				LoadFile(filename);
			}
			//GetSaveFileName
			return 0;
		}
		
		//有问题
		int Search(wchar_t * text)
		{
			/*FINDREPLACE fr;
			text = new wchar_t[100];
			ZeroMemory(text, 200);
			fr.lpstrFindWhat = text;
			fr.lStructSize = sizeof(FINDREPLACE);
			fr.hwndOwner = m_hwnd;
			FindText(&fr);*/
			return 0;
		}

		/// <summary>
		/// 在窗口上绘制指定文本
		/// </summary>
		void DrawText(LPCWSTR text)
		{
			m_hdc = GetDC(m_hwnd);
			RECT rect;

			HFONT font = CreateFontW(18, 0, 0, 0, 300, false, false, false, DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI");

			HFONT hFontOld = (HFONT)SelectObject(m_hdc, font);

			GetClientRect(m_hwnd, &rect);
			SetTextColor(m_hdc, 0x00000000);
			SetBkMode(m_hdc, TRANSPARENT);
			rect.left = 12;
			rect.right -= 12;
			rect.top = 20;
			FillRect(m_hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
			::DrawText(m_hdc, text, -1, &rect, DT_NOCLIP | DT_WORDBREAK | DT_EDITCONTROL);
			SelectObject(m_hdc, hFontOld);
			ReleaseDC(m_hwnd, m_hdc);
		}

		int LoadFile(LPCWSTR filename)
		{
			m_editor.LoadFromFile(filename);

			SetWindowText(m_hwnd, std::wstring(filename).append(L" - Matrix").c_str());

			if (m_auto_validate)
			{
				//ValidateXml();
			}
			return 0;
		}

		void RemoveBorder()
		{
			//SetWindowLong(m_hwnd, GWL_STYLE, GetWindowLong(m_hwnd, GWL_STYLE) & ~WS_CAPTION &~WS_BORDER);
			//SetWindowLong(m_hwnd, GWL_STYLE, GetWindowLong(m_hwnd, GWL_STYLE) | (WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP));

			SetWindowLong(m_hwnd, GWL_STYLE, GetWindowLong(m_hwnd, GWL_STYLE) &	(~(WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)));
			SetWindowPos(m_hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
		}

		/// <summary>
		/// 简单校验XML文件一致性错误
		/// </summary>
		int ValidateXml(LPCWSTR document = NULL)
		{
			char *content = NULL;
			if (NULL == document)
			{
				int nlen = m_editor.SendEditor(SCI_GETLENGTH);
				content = new char[nlen + 1];
				m_editor.SendEditor(SCI_GETTEXT, nlen, (sptr_t)content);
				if (NULL == *content)
				{
					return -1;
				}
				else
				{
					document = Matrix::TextEncoder::Utf8ToUnicode(content);
				}
			}

			//Matrix::XmlDocument xml;
			//wchar_t * wdocument = const_cast<wchar_t *>(document);
			//xml.LoadFromFile(m_editor.FileName());
			//xml.parse(wdocument, wcslen(document));
			//MessageBox(NULL,xml.Name(), L"", MB_OK);

			Matrix::XMLValidater tXml;
			Matrix::XmlValidateError tError;
			tXml.ValidateXml(std::wstring(document), tError);

			TCHAR err[BUFSIZ];
			wsprintf(err, L"第%d行%d列%s与第%d行%d列%s不匹配",
				tError.Open.Line, tError.Open.Row, tError.OpenName.c_str(),
				tError.Close.Line, tError.Close.Row, tError.CloseName.c_str());

			if (tError.Count == -1)
			{
				MessageBox(m_hwnd, L"Xml语法错误", L"Error", MB_ICONERROR | MB_OK);
			}
			else if (tError.Count > 0)
			{
				MessageBox(m_hwnd, err, L"Error", MB_ICONERROR | MB_OK);
				m_editor.SendEditor(SCI_GOTOLINE, 0);
				m_editor.SetFocus();
			}
			else
			{
				MessageBox(m_hwnd, L"未见错误", L"Info", MB_ICONINFORMATION | MB_OK);
			}
			return 0;
		}


	private:
		//m_ps与m_hdc为Paint中临时变量，放在类中仅仅为了性能考虑
		PAINTSTRUCT m_ps;
		HDC m_hdc;

		Matrix::SciEditor m_editor;
		bool m_auto_validate;
	};
}

	/// <summary>
	/// Windows消息处理
	/// </summary>
LRESULT Matrix::MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT wmId = 0, wmEvent = 0;
	HMENU h_menu = GetMenu(m_hwnd);
	SCNotification *notify = NULL;
	bool checked = true;

	switch (uMsg)
	{

	case WM_CREATE:
		InitWindow();
		DragAcceptFiles(m_hwnd, TRUE);
		break;

	case WM_SHOWWINDOW:
		break;

	case WM_DROPFILES:
		DropFile((HDROP)wParam);
		break;

	case WM_SIZE:
		RECT rect;
		ZeroMemory(&rect, sizeof(rect));
		GetWindowRect(m_hwnd, &rect);
		AdaptSize(rect);
		break;

	case WM_PAINT:
		m_hdc = BeginPaint(m_hwnd, &m_ps);
		FillRect(m_hdc, &m_ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(m_hwnd, &m_ps);
		break;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_ABOUT:
			//DialogBox(nullptr, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;

		case IDM_OPEN:
			OpenFileDlg();
			break;

		case IDM_SAVE:
			m_editor.Save();
			break;

		case IDM_PRINT:
			break;

		case IDM_EXIT:
			DestroyWindow(m_hwnd);
			break;

		case IDM_FIND:
			Search(NULL);
			break;

		case IDM_WRAP:
			checked = MF_CHECKED == GetMenuState(h_menu, IDM_WRAP, MF_BYCOMMAND);
			m_editor.SetWrap(!checked);
			CheckMenuItem(h_menu, IDM_WRAP, (!checked ? MF_CHECKED : MF_UNCHECKED));
			break;

		case IDM_VALIDATE:
			ValidateXml(NULL);
			break;

		case IDM_AUTOVALIDATE:
			m_auto_validate = MF_CHECKED != GetMenuState(h_menu, IDM_AUTOVALIDATE, MF_BYCOMMAND);
			CheckMenuItem(h_menu, IDM_AUTOVALIDATE, m_auto_validate ? MF_CHECKED : MF_UNCHECKED);
			break;

		default:
			return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
		}
		break;

	case WM_NOTIFY:	
		notify = (SCNotification*)lParam;
		//if (notify->nmhdr.hwndFrom == &m_editor)
		//{
			m_editor.HandleMsg(notify, wParam);
		//}
		break;

	case WM_VSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_BOTTOM:
			break;

		default:
			break;
		}
		break;

	case WM_DESTROY:
		DestroyFramework();
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	}
	return TRUE;
}

#endif