
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
#include "sci_editor.h"
#include "application.h"
#include "menu.h"


namespace Matrix
{
	class MainWindow : public BaseWindow<MainWindow>
	{
	public:
		MainWindow() :m_use_ribbon(false)
		{

		}

		PCWSTR  ClassName() const { return L"Main Window"; }

		/// <summary>
		/// Windows消息处理
		/// </summary>
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

		/// <summary>
		/// 载入所需组件
		/// </summary>
		void LoadComponent()
		{
			m_editor.Create(m_hwnd);			
		}

		/// <summary>
		/// 初始化窗体
		/// </summary>
		int InitWindow()
		{
			//m_use_ribbon = true;

			if (true == m_use_ribbon)
			{
				bool init_success = MainFrame::InitializeFramework(m_hwnd, &m_editor);
				if (!init_success)
				{
					return -1;
				}
			}
			else
			{
				m_menu.Init(m_hwnd, m_hinst);
			}

			DragAcceptFiles(m_hwnd, TRUE);			
			m_editor.Init();
			return 0;
		}
		
		/// <summary>
		/// 自适应父窗体大小
		/// </summary>
		void AdaptSize(RECT &rect)
		{
			if (m_use_ribbon)
			{
				MainFrame::UpdateLayout();
			}
			else
			{
				rect.bottom -= 45;
				m_editor.SetPos(rect);
			}			
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
					m_editor.LoadFile(m_hwnd,fileName);
				}
				::DragFinish(hDropInfo);
				delete fileName;
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

	private:
		//m_ps与m_hdc为Paint中临时变量，放在类中仅仅为了性能考虑
		PAINTSTRUCT m_ps;
		HDC m_hdc;

		Matrix::SciEditor m_editor;
		Matrix::Menu m_menu;

		bool m_use_ribbon;

	};
}

	/// <summary>
	/// Windows消息处理
	/// </summary>
LRESULT Matrix::MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	SCNotification *notify = NULL;

	switch (uMsg)
	{
	case WM_CREATE:
		LoadComponent();
		InitWindow();		
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
		if (0 == m_menu.HandleMessage(m_hwnd, &m_editor, wParam, lParam))
		{
			return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
		}
		break;

	case WM_NOTIFY:	
		notify = (SCNotification*)lParam;
		if (notify->nmhdr.hwndFrom == &m_editor)
		{
			m_editor.HandleMsg(notify, wParam);
		}
		break;

	case WM_DESTROY:
		if (m_use_ribbon)
		{
			MainFrame::DestroyFramework();
		}
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	}
	return TRUE;
}

#endif