
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
#include "file.h"
#include "xml.h"
#include "document.h"

class MainWindow : public BaseWindow<MainWindow>
{
public:
	PCWSTR  ClassName() const { return L"Main Window"; }

	/// <summary>
	/// Windows��Ϣ����
	/// </summary>
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
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
				ValidateXmlFile(fileName);
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
			ValidateXmlFile(tFileName);
		}
		return 0;
	}

	/// <summary>
	/// �ڴ����ϻ���ָ���ı�
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
	/// ��У��XML�ļ�һ���Դ���
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
		this->DrawText(szContent);

		TCHAR err[BUFSIZ];
		wsprintf(err, L"��%d��%d��%s���%d��%d��%s��ƥ��",
			tError.Open.Line, tError.Open.Row, tError.OpenName.c_str(),
			tError.Close.Line, tError.Close.Row, tError.CloseName.c_str());

		if (tError.Count == -1)
		{
			MessageBox(m_hWnd, L"Xml�﷨����", L"", 0);
		}
		else if (tError.Count > 0)
		{
			MessageBox(m_hWnd, err, L"", 0);
		}
		else
		{
			MessageBox(m_hWnd, L"δ������", L"", 0);
		}
		return 0;
	}


private:
	//m_ps��m_hdcΪPaint����ʱ�������������н���Ϊ�����ܿ���
	PAINTSTRUCT m_ps;
	HDC m_hdc;
};

/// <summary>
/// Windows��Ϣ����
/// </summary>
LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT wmId = 0, wmEvent = 0;

	switch (uMsg)
	{

	case WM_CREATE:
		DragAcceptFiles(m_hWnd, TRUE);
		break;

	case WM_SHOWWINDOW:
		break;

	case WM_DROPFILES:
		DropFile((HDROP)wParam);
		break;

	case WM_PAINT:
		m_hdc = BeginPaint(m_hWnd, &m_ps);
		FillRect(m_hdc, &m_ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(m_hWnd, &m_ps);
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