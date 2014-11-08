
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
	const COLORREF red = RGB(0xFF, 0x00, 0x00);
	const COLORREF green = RGB(0x00, 0xFF, 0x00);
	const COLORREF blue = RGB(0x00, 0x00, 0xFF);
	const COLORREF dark_green = RGB(0, 0x80, 0);	
	const COLORREF yellow = red | green;
	const COLORREF white = red | green | blue;
	const COLORREF black = red & green & blue;

	const COLORREF xml_string_blue = RGB(0x80, 0x00, 0xFF);		//字符串	
	const COLORREF tag_blue = blue;		//节点
	const COLORREF attr_red = red;

	const COLORREF cpp_string_gray = RGB(0xC8, 0xC8, 0xC8);
	const COLORREF keywords_blude = RGB(0x56, 0x9C, 0xD6);
	const COLORREF preprocessor_purple = RGB(0xBD, 0x63, 0xC5);		//宏

	const COLORREF fore_white = RGB(0xC8, 0xC8, 0xC8);			//黑背景下的文字
	const COLORREF back_black = RGB(0x1E, 0x1E, 0x1E);			//背景
	const COLORREF caret_white = RGB(0xDC, 0xDC, 0xDC);
	const COLORREF caret_black = back_black;
	const COLORREF current_line_black = RGB(0x0F,0x0F,0x0F);
	const COLORREF current_line_yellow = RGB(0xFF, 0xFF, 0x80);
	const COLORREF line_number_blue = RGB(0x2B, 0x91, 0xAF);		//行号
	const COLORREF line_number_black = RGB(0x2B,0x2B,0x2B);
	const COLORREF selection_blue = RGB(0x26,0x4F,0x78);
	const COLORREF selection_wight = fore_white;
	const COLORREF comment_green = RGB(0x57, 0xA6, 0x4A);		// 注释	

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

		wchar_t * FileName()
		{
			return m_filename;
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
			SetTextStyle(0);
		}

		void SetLineNumber()
		{
			SendEditor(SCI_SETMARGINTYPEN, 2, SC_MARGIN_NUMBER);
			SendEditor(SCI_SETMARGINWIDTHN, 2, 29);
		}

		void SetTextStyle(int style)
		{
			SendEditor(SCI_STYLESETFONT, STYLE_DEFAULT, (sptr_t)"Microsoft Yahei");
			SendEditor(SCI_STYLESETFONT, STYLE_DEFAULT, (sptr_t)"Consolas");
			SendEditor(SCI_STYLESETSIZE, STYLE_DEFAULT, 12);
			
			const char* cpp_keywords =
				"asm auto break case catch class const "
				"const_cast continue default delete do double "
				"dynamic_cast else enum explicit extern false "
				"for friend goto if inline mutable "
				"namespace new operator private protected public "
				"register reinterpret_cast return signed "
				"sizeof static static_cast struct switch template "
				"this throw true try typedef typeid typename "
				"union unsigned using virtual volatile while";
			const char* cpp_type_keywords =
				"bool char float int long short void wchar_t";			

			if (SCLEX_XML == style)
			{
				SendEditor(SCI_STYLESETFORE, STYLE_DEFAULT, back_black );
				SendEditor(SCI_STYLESETBACK, STYLE_DEFAULT, white);
				SendEditor(SCI_STYLECLEARALL);
				SendEditor(SCI_SETSELBACK, true, selection_wight);
				SendEditor(SCI_SETCARETFORE, caret_black);	//光标

				SendEditor(SCI_SETLEXER, SCLEX_HTML);
				SendEditor(SCI_STYLESETFORE, SCE_H_XMLSTART, red);
				SendEditor(SCI_STYLESETBACK, SCE_H_XMLSTART, yellow);
				SendEditor(SCI_STYLESETFORE, SCE_H_XMLEND, red);
				SendEditor(SCI_STYLESETBACK, SCE_H_XMLEND, yellow);

				SendEditor(SCI_STYLESETFORE, SCE_D_DEFAULT, black);
				SendEditor(SCI_STYLESETBACK, SCE_D_DEFAULT, white);
				SendEditor(SCI_STYLESETBOLD, SCE_D_DEFAULT, true);

				SendEditor(SCI_STYLESETFORE, SCE_D_NUMBER, red);

				SendEditor(SCI_STYLESETFORE, SCE_H_TAG, tag_blue);
				SendEditor(SCI_STYLESETFORE, SCE_H_TAGEND, tag_blue);
				SendEditor(SCI_STYLESETFORE, SCE_H_TAGUNKNOWN, tag_blue);

				SendEditor(SCI_STYLESETFORE, SCE_H_ATTRIBUTE, attr_red);
				SendEditor(SCI_STYLESETFORE, SCE_H_ATTRIBUTEUNKNOWN, attr_red);

				SendEditor(SCI_STYLESETFORE, SCE_H_DOUBLESTRING, xml_string_blue);
				SendEditor(SCI_STYLESETFORE, SCE_H_SINGLESTRING, xml_string_blue);

				SendEditor(SCI_STYLESETFORE, SCE_H_COMMENT, comment_green);

				SendEditor(SCI_SETCARETLINEVISIBLE, TRUE);
				SendEditor(SCI_SETCARETLINEBACK, current_line_yellow);
			}
			else if (SCLEX_CPP == style)
			{				
				SendEditor(SCI_STYLESETFORE, STYLE_DEFAULT, fore_white);
				SendEditor(SCI_STYLESETBACK, STYLE_DEFAULT, back_black);
				SendEditor(SCI_STYLECLEARALL);

				SendEditor(SCI_STYLESETFORE, STYLE_LINENUMBER, line_number_blue);
				SendEditor(SCI_STYLESETBACK, STYLE_LINENUMBER, line_number_black);
				SendEditor(SCI_SETCARETFORE, caret_white);	//光标

				//试图同步高亮，失败
				//int pos = SendEditor(SCI_GETCURRENTPOS, 0, 0);
				//SendEditor(SCI_STYLESETBACK, STYLE_BRACELIGHT, red);
				//SendEditor(SCI_BRACEHIGHLIGHT, 20, 10);
				//SendEditor(SCI_SETHIGHLIGHTGUIDE, 20);

				SendEditor(SCI_SETSELBACK,true, selection_blue);

				SendEditor(SCI_SETLEXER, SCLEX_CPP);
				SendEditor(SCI_SETKEYWORDS, 0, (sptr_t)cpp_keywords);//设置关键字 
				SendEditor(SCI_SETKEYWORDS, 1, (sptr_t)cpp_type_keywords);//设置关键字 
				
				SendEditor(SCI_STYLESETFORE, SCE_C_DEFAULT, fore_white);
				SendEditor(SCI_STYLESETBACK, SCE_C_DEFAULT, back_black);
				// 下面设置各种语法元素风格 
				SendEditor(SCI_STYLESETFORE, SCE_C_WORD, keywords_blude);   //关键字 
				SendEditor(SCI_STYLESETFORE, SCE_C_WORD2, keywords_blude);   //关键字 
				SendEditor(SCI_STYLESETBOLD, SCE_C_WORD2, TRUE);   //关键字 
				SendEditor(SCI_STYLESETFORE, SCE_C_STRING, cpp_string_gray); //字符串 
				SendEditor(SCI_STYLESETFORE, SCE_C_CHARACTER, cpp_string_gray); //字符 
				SendEditor(SCI_STYLESETFORE, SCE_C_PREPROCESSOR, preprocessor_purple);//预编译开关 

				SendEditor(SCI_STYLESETFORE, SCE_C_COMMENT, comment_green);//块注释 
				SendEditor(SCI_STYLESETFORE, SCE_C_COMMENTLINE, comment_green);//行注释 
				SendEditor(SCI_STYLESETFORE, SCE_C_COMMENTDOC, comment_green);//文档注释（/**开头） 
				
				SendEditor(SCI_SETCARETLINEVISIBLE, TRUE);
				SendEditor(SCI_SETCARETLINEBACK, current_line_black);
			}
			else
			{
				SendEditor(SCI_STYLESETFORE, STYLE_DEFAULT, back_black);
				SendEditor(SCI_STYLESETBACK, STYLE_DEFAULT, white);
				SendEditor(SCI_STYLECLEARALL);
				SendEditor(SCI_SETCARETFORE, caret_black);	//光标
				SendEditor(SCI_SETSELBACK, true, selection_wight);
				SendEditor(SCI_STYLESETSIZE, STYLE_DEFAULT, 14);

				SendEditor(SCI_SETCARETLINEVISIBLE, TRUE);
				SendEditor(SCI_SETCARETLINEBACK, current_line_yellow);
			}
			
		}

		void SetPos(RECT rect)
		{
			SetWindowPos(m_hwnd, HWND_TOP, 0, 160,
				rect.right - rect.left - 11, rect.bottom - rect.top - 50 - 110, SWP_SHOWWINDOW);
		}

		int LoadFromFile(const wchar_t *filename)
		{
			m_current_page = 0;

			size_t ulen = wcslen(filename);
			m_filename = new wchar_t[ulen + 1];
			lstrcpynW(m_filename, filename, ulen + 1);

			if (wcsstr(m_filename, L".xml"))
			{
				SetTextStyle(SCLEX_XML);
			}
			else if (wcsstr(m_filename, L".cpp") || wcsstr(m_filename, L".h"))
			{
				SetTextStyle(SCLEX_CPP);
			}
			else
			{
				SetTextStyle(0);
			}

			const char * text = Matrix::File(filename).Utf8Text(m_current_page);
			m_file_pos = Matrix::FilePos::HEAD;
			SendEditor(SCI_SETTEXT, NULL, (sptr_t)text);
			
			delete text;
			text = NULL;
			GetScrollSize();
			return m_current_page;
		}
		
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

		int Save()
		{
			int nlen = SendEditor(SCI_GETLENGTH);
			char * content = new char[nlen + 1];
			SendEditor(SCI_GETTEXT, nlen, (sptr_t)content);
			if (NULL == m_filename)
			{
				return -2;
			}
			else if (NULL == *content)
			{
				return -1;
			}
			else
			{
				Matrix::File(m_filename).WriteText(content, nlen, true);
			}
			return 0;
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