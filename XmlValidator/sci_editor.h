
/*
*
*
*
*	Scintilla Editor Class		In Matrix
*
*	Created by Bonbon	2014.10.26
*
*/


#include "common.h"
#include <scintilla.h> 
#include <scilexer.h> 
#include "file.h"

namespace Matrix
{
	class SciEditor
	{
	public:

		SciEditor() :
			m_line_wrap(true), 
			m_current_page(0),
			m_filename(NULL),
			m_file_pos(Matrix::FilePos::HEAD),
			m_vscroll_pos(0),
			m_vscroll_size(0)
		{}

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
			SendEditor(SCI_SETCARETLINEVISIBLE, TRUE);
			SendEditor(SCI_SETCARETLINEBACK, 0xb0ffff);

			SendEditor(SCI_SETTABWIDTH, 4);
			SendEditor(SCI_SETCODEPAGE, SC_CP_UTF8);
			SendEditor(SCI_STYLECLEARALL);
			SetLineNumber();
			SetWrap(true);
			SetTextStyle(SCLEX_XML);
		}

		void SetLineNumber()
		{
			SendEditor(SCI_SETMARGINTYPEN, 2, SC_MARGIN_NUMBER);
			SendEditor(SCI_SETMARGINWIDTHN, 2, 29);
		}

		void SetTextStyle(int style)
		{
			if (SCLEX_XML == style)
			{
				SendEditor(SCI_SETLEXER, SCLEX_XML);
				SendEditor(SCI_STYLESETFORE, SCE_H_TAG, 0x00FF0000);
				SendEditor(SCI_STYLESETFORE, SCE_H_TAGUNKNOWN, 0x00FF0000);
				SendEditor(SCI_STYLESETFORE, SCE_H_ATTRIBUTE, 0x000000FF);
				SendEditor(SCI_STYLESETFORE, SCE_H_ATTRIBUTEUNKNOWN, 0x0000FF00);
				SendEditor(SCI_STYLESETBOLD, SCE_H_DEFAULT, true);
				SendEditor(SCI_STYLESETFORE, SCE_H_DOUBLESTRING, 0x00F08000);
				SendEditor(SCI_STYLESETFORE, SCE_H_COMMENT, 0x0000A000);
			}
			SendEditor(SCI_STYLESETFONT, STYLE_DEFAULT, (sptr_t)"Consolas");
			SendEditor(SCI_STYLESETSIZE, STYLE_DEFAULT, 12);
		}

		void SetPos(RECT rect)
		{
			SetWindowPos(m_hwnd, HWND_TOP, 0, 0,
				rect.right - rect.left - 11, rect.bottom - rect.top - 50, SWP_SHOWWINDOW);
		}

		int LoadFromFile(const wchar_t *filename)
		{
			m_current_page = 0;

			size_t ulen = wcslen(filename);
			m_filename = new wchar_t[ulen + 1];
			lstrcpynW(m_filename, filename, ulen + 1);

			const char * text = Matrix::File(filename).Utf8Text(m_current_page);
			m_file_pos = Matrix::FilePos::HEAD;
			SendEditor(SCI_SETTEXT, NULL, (sptr_t)text);
			delete text;
			text = NULL;
			GetScrollSize();
			return m_current_page;
		}

		/*int LastPage()
		{
		if (Matrix::FilePos::HEAD == m_file_pos)
		{
		return -2;
		}
		else if (NULL != m_filename)
		{
		const char * text = Matrix::File(m_filename).Utf8Text(--m_current_page);
		if (m_current_page == 0)
		{
		m_file_pos = Matrix::FilePos::HEAD;
		}
		else
		{
		m_file_pos = Matrix::FilePos::INSIDE;
		}
		SendEditor(SCI_SETTEXT, NULL, (sptr_t)text);
		delete text;
		text = NULL;
		return m_current_page;
		}
		else
		{
		return -1;
		}
		}*/

		int NextPage()
		{
			if (Matrix::FilePos::END == m_file_pos)
			{
				return -2;
			}
			else if (NULL != m_filename)
			{
				size_t size = 0;
				const char * text = Matrix::File(m_filename).Utf8Text(++m_current_page,&size);
				if (NULL == text)
				{
					m_file_pos = Matrix::FilePos::END;
					return --m_current_page;
				}
				m_file_pos = Matrix::FilePos::INSIDE;
				SendEditor(SCI_APPENDTEXT,  size, (sptr_t)text);				
				delete text;
				text = NULL;
				GetScrollSize();
				return m_current_page;
			}
			else
			{
				return -1;
			}
		}

		void SetWrap(bool iflag)
		{
			SendEditor(SCI_SETWRAPMODE, (WPARAM)iflag ? 1 : 0);
		}

		sptr_t SendEditor(unsigned int iMessage, uptr_t wParam = 0, sptr_t lParam = 0)
		{
			return m_fnDirect(m_ptrDirect, iMessage, wParam, lParam);
		}

		void SetFocus()
		{
			::SetFocus(m_hwnd);
		}

		void GetScrollSize()
		{
			SCROLLINFO si;
			ZeroMemory(&si, sizeof(si));
			si.cbSize = sizeof(si);
			si.fMask = SIF_ALL;
			::GetScrollInfo(m_hwnd, SB_VERT, &si);
			m_vscroll_pos = si.nPos;
			m_vscroll_size = si.nMax;
			return;
		}

		void HandleMsg(SCNotification * msg, WPARAM wParam)
		{
			
			int i = LOWORD(wParam);
			int vpos = 0; 

			switch (msg->nmhdr.code)
			{
			case SCN_UPDATEUI:
				switch (msg->updated)
				{
				case SC_UPDATE_V_SCROLL:
					GetScrollSize();
					vpos = GetScrollPos(m_hwnd, SB_VERT);
					if (m_vscroll_size <= m_vscroll_pos+50)
					{
						//MessageBox(m_hwnd, L"已经是最后了", L"Info", MB_ICONINFORMATION | MB_OK);
						NextPage();
					}
					break;

				default:
					break;
				}
				break;

			default:
				break;
			}
		}

	private:
		HWND m_hwnd;
		SciFnDirect m_fnDirect;
		sptr_t m_ptrDirect;

		bool m_line_wrap;
		wchar_t * m_filename;
		int m_current_page;
		Matrix::FilePos m_file_pos;
		int m_vscroll_size;
		int m_vscroll_pos;
	};
}