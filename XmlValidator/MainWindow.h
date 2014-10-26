
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

	MainWindow() :m_auto_validate(true)
	{}

	PCWSTR  ClassName() const { return L"Main Window"; }

	/// <summary>
	/// Windows��Ϣ����
	/// </summary>
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	/// <summary>
	/// ��ʼ������
	/// </summary>
	void InitWindow()
	{
		LoadComponent();
		HMENU menu;
		menu = GetMenu(m_hwnd);
		CheckMenuItem(menu, IDM_WRAP, MF_CHECKED);
		CheckMenuItem(menu, IDM_AUTOVALIDATE, m_auto_validate ? MF_CHECKED : MF_UNCHECKED);
	}

	/// <summary>
	/// �����������
	/// </summary>
	void LoadComponent()
	{
		editor.Create(m_hwnd);
		editor.Init();
	}

	/// <summary>
	/// ����Ӧ�������С
	/// </summary>
	void AdaptSize(RECT &rect)
	{
		editor.SetPos(rect);
	}

	/// <summary>
	/// �����Ϸ��ļ�
	/// </summary>
	int DropFile(HDROP hDropInfo)
	{
		if (hDropInfo)
		{
			//MAX_PATHΪWin������ļ�ȫ������󳤶ȣ�Ϊ260
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
	/// ʹ�ô򿪶Ի�����ļ�
	/// </summary>
	int OpenFileDlg()
	{
		FileDialog dlg = FileDialog();
		LPCWSTR tFileName=dlg.Open();
		if (tFileName != NULL)
		{
			LoadFile(tFileName);
		}
		return 0;
	}

	/// <summary>
	/// �ڴ����ϻ���ָ���ı�
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
		//Matrix::File tFile;
		//LPCWSTR szContent = tFile.ReadFile(tFile.UnicodeToAnsi(fileName));
		Matrix::Document document;
		document.LoadFromFile(Matrix::File().UnicodeToAnsi(filename));
		LPCWSTR szContent = document.m_buffer;


		editor.SetText(Matrix::File().UnicodeToUTF8(szContent));

		if (m_auto_validate)
		{
			ValidateXml(szContent);
		}

		return 0;

		//this->DrawText(szContent);
	}

	/// <summary>
	/// ��У��XML�ļ�һ���Դ���
	/// </summary>
	int ValidateXml(LPCWSTR documnet)
	{
		char *content = NULL;
		if (NULL == documnet)
		{
			int nlen = editor.SendEditor(SCI_GETLENGTH);
			content = new char[nlen + 1];
			editor.SendEditor(SCI_GETTEXT, nlen, (sptr_t)content);
			if (NULL == *content)
			{
				return -1;
			}
			else
			{
				documnet = Matrix::File().Utf8ToUnicode(content);
			}
		}
		
		Matrix::XML tXml;
		Matrix::XmlValidateError tError;
		tXml.ValidateXml(std::wstring(documnet), tError);

		TCHAR err[BUFSIZ];
		wsprintf(err, L"��%d��%d��%s���%d��%d��%s��ƥ��",
			tError.Open.Line, tError.Open.Row, tError.OpenName.c_str(),
			tError.Close.Line, tError.Close.Row, tError.CloseName.c_str());

		if (tError.Count == -1)
		{
			MessageBox(m_hwnd, L"Xml�﷨����", L"Error", MB_ICONERROR | MB_OK);
		}
		else if (tError.Count > 0)
		{
			MessageBox(m_hwnd, err, L"Error", MB_ICONERROR|MB_OK);
			editor.SendEditor(SCI_GOTOLINE, 0 );
			editor.SetFocus();
		}
		else
		{
			MessageBox(m_hwnd, L"δ������", L"Info", MB_ICONINFORMATION|MB_OK);
		}
		return 0;
	}


private:
	//m_ps��m_hdcΪPaint����ʱ�������������н���Ϊ�����ܿ���
	PAINTSTRUCT m_ps;
	HDC m_hdc;

	SciEditor editor;
	bool m_auto_validate;
};

/// <summary>
/// Windows��Ϣ����
/// </summary>
LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT wmId = 0, wmEvent = 0;
	HMENU h_menu;
	h_menu = GetMenu(m_hwnd);
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
		// �����˵�ѡ��: 
		switch (wmId)
		{
		case IDM_ABOUT:
			//DialogBox(nullptr, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;

		case IDM_OPEN:
			OpenFileDlg();
			break;

		case IDM_PRINT:
			break;

		case IDM_EXIT:
			DestroyWindow(m_hwnd);
			break;

		case IDM_WRAP:
			checked = MF_CHECKED == GetMenuState(h_menu, IDM_WRAP, MF_BYCOMMAND);
			editor.SetWrap(!checked);
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

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	}
	return TRUE;
}


#endif