
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
#include "dialog.h"
//#include "file.h"
#include "xml.h"
#include "document.h"
#include "sci_editor.h"

class MainWindow : public BaseWindow<MainWindow>
{
public:
	PCWSTR  ClassName() const { return L"Main Window"; }

	/// <summary>
	/// Windows消息处理
	/// </summary>
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	/// <summary>
	/// 载入所需组件
	/// </summary>
	void LocaComponent()
	{
		editor.Create(m_hWnd);
		editor.Init();
	}

	/// <summary>
	/// 自适应父窗体大小
	/// </summary>
	void AdaptSize(RECT &rect)
	{
		editor.SetPos(rect);
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
				ValidateXmlFile(fileName);
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
		FileDialog dlg = FileDialog();
		LPCWSTR tFileName=dlg.Open();
		if (tFileName != NULL)
		{
			ValidateXmlFile(tFileName);
		}
		return 0;
	}

	/// <summary>
	/// 在窗口上绘制指定文本
	/// </summary>
	void DrawText(LPCWSTR text)
	{
		m_hdc = GetDC(m_hWnd);
		RECT rect;

		HFONT font = CreateFontW(18, 0, 0, 0, 300, false, false, false, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI");

		HFONT hFontOld = (HFONT)SelectObject(m_hdc, font);

		GetClientRect(m_hWnd, &rect);
		SetTextColor(m_hdc, 0x00000000);
		SetBkMode(m_hdc, TRANSPARENT);
		rect.left = 12;
		rect.right -= 12;
		rect.top = 20;
		FillRect(m_hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
		::DrawText(m_hdc, text, -1, &rect, DT_NOCLIP | DT_WORDBREAK | DT_EDITCONTROL);
		SelectObject(m_hdc, hFontOld);
		ReleaseDC(m_hWnd, m_hdc);
	}

	/// <summary>
	/// 简单校验XML文件一致性错误
	/// </summary>
	int ValidateXmlFile(LPCWSTR fileName)
	{
		//Matrix::File tFile;
		//LPCWSTR szContent = tFile.ReadFile(tFile.UnicodeToAnsi(fileName));
		Matrix::Document document;
		document.LoadFromFile(Matrix::File().UnicodeToAnsi(fileName));
		LPCWSTR szContent = document.m_buffer;
		Matrix::XML tXml;
		Matrix::XmlValidateError tError;
		tXml.ValidateXml(std::wstring(szContent), tError);

		editor.SetText(Matrix::File().UnicodeToUTF8(szContent));
		this->DrawText(szContent);

		TCHAR err[BUFSIZ];
		wsprintf(err, L"第%d行%d列%s与第%d行%d列%s不匹配",
			tError.Open.Line, tError.Open.Row, tError.OpenName.c_str(),
			tError.Close.Line, tError.Close.Row, tError.CloseName.c_str());

		if (tError.Count == -1)
		{
			MessageBox(m_hWnd, L"Xml语法错误", L"", 0);
		}
		else if (tError.Count > 0)
		{
			MessageBox(m_hWnd, err, L"", 0);
		}
		else
		{
			MessageBox(m_hWnd, L"未见错误", L"", 0);
		}
		return 0;
	}


private:
	//m_ps与m_hdc为Paint中临时变量，放在类中仅仅为了性能考虑
	PAINTSTRUCT m_ps;
	HDC m_hdc;

	SciEditor editor;
};

/// <summary>
/// Windows消息处理
/// </summary>
LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT wmId = 0, wmEvent = 0;

	switch (uMsg)
	{

	case WM_CREATE:
		LocaComponent();
		DragAcceptFiles(m_hWnd, TRUE);
		break;

	case WM_SHOWWINDOW:
		break;

	case WM_DROPFILES:
		DropFile((HDROP)wParam);
		break;

	case WM_SIZE:
		RECT rect;
		ZeroMemory(&rect, sizeof(rect));
		GetWindowRect(m_hWnd, &rect);
		AdaptSize(rect);
		break;

	case WM_PAINT:
		m_hdc = BeginPaint(m_hWnd, &m_ps);
		FillRect(m_hdc, &m_ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(m_hWnd, &m_ps);
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

		case IDM_EXIT:
			DestroyWindow(m_hWnd);
			break;

		default:
			return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	}
	return TRUE;
}


#endif