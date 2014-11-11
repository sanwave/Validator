
/*
*
*
*
*	Scintilla Editor Class		In Matrix
*
*	Created by Bonbon	2014.10.26
*
*	Updated by Bonbon	2014.11.11
*
*/

#ifndef _SCI_EDITOR_
#define _SCI_EDITOR_


#include "common.h"
#include <scintilla.h> 
#include <scilexer.h> 
#include "file.h"
#include "xml_validater.h"
#include "xml.h"

namespace Matrix
{
	static class Color
	{
	public:
		static const COLORREF red = RGB(0xFF, 0x00, 0x00);
		static const COLORREF green = RGB(0x00, 0xFF, 0x00);
		static const COLORREF blue = RGB(0x00, 0x00, 0xFF);
		static const COLORREF dark_green = RGB(0, 0x80, 0);
		static const COLORREF yellow = red | green;
		static const COLORREF white = red | green | blue;
		static const COLORREF black = red & green & blue;
		 
		static const COLORREF xml_string_blue = RGB(0x80, 0x00, 0xFF);		//字符串	
		static const COLORREF tag_blue = blue;		//节点
		static const COLORREF attr_red = red;
		 
		static const COLORREF cpp_string_gray = RGB(0xC8, 0xC8, 0xC8);
		static const COLORREF keywords_blude = RGB(0x56, 0x9C, 0xD6);
		static const COLORREF preprocessor_purple = RGB(0xBD, 0x63, 0xC5);		//宏
		 
		static const COLORREF fore_white = RGB(0xC8, 0xC8, 0xC8);			//黑背景下的文字
		static const COLORREF back_black = RGB(0x1E, 0x1E, 0x1E);			//背景
		static const COLORREF caret_white = RGB(0xDC, 0xDC, 0xDC);
		static const COLORREF caret_black = back_black;
		static const COLORREF current_line_black = RGB(0x0F, 0x0F, 0x0F);
		static const COLORREF current_line_yellow = RGB(0xFF, 0xFF, 0x80);
		static const COLORREF line_number_blue = RGB(0x2B, 0x91, 0xAF);		//行号
		static const COLORREF line_number_black = RGB(0x2B, 0x2B, 0x2B);
		static const COLORREF selection_blue = RGB(0x26, 0x4F, 0x78);
		static const COLORREF selection_wight = fore_white;
		static const COLORREF comment_green = RGB(0x57, 0xA6, 0x4A);		// 注释	
	};

	class SciEditor
	{
	public:

		SciEditor() :
			m_line_wrap(true),
			m_auto_validate(false),
			m_current_page(0),
			m_filename(NULL),
			m_file_pos(Matrix::FilePos::HEAD),
			m_vscroll_pos(0),
			m_vscroll_size(0)
		{
		}

		~SciEditor()
		{
			if (NULL != m_filename)
			{
				delete m_filename;
				m_filename = NULL;
			}
		}

		bool AutoValidate()
		{
			return m_auto_validate;
		}

		void SetAutoValidate(bool value)
		{
			m_auto_validate = (value == 0) ? false : true;
		}

		bool LineWrap()
		{
			return m_line_wrap;
		}

		void SetWrap(bool iflag)
		{
			m_line_wrap = iflag;
			SendEditor(SCI_SETWRAPMODE, (WPARAM)(m_line_wrap == 0 ? 0 : 1));
		}

		wchar_t * FileName()
		{
			return m_filename;
		}

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
				SendEditor(SCI_STYLESETFORE, STYLE_DEFAULT, Color::back_black);
				SendEditor(SCI_STYLESETBACK, STYLE_DEFAULT, Color::white);
				SendEditor(SCI_STYLECLEARALL);
				SendEditor(SCI_SETSELBACK, true, Color::selection_wight);
				SendEditor(SCI_SETCARETFORE, Color::caret_black);	//光标

				SendEditor(SCI_SETLEXER, SCLEX_HTML);
				SendEditor(SCI_STYLESETFORE, SCE_H_XMLSTART, Color::red);
				SendEditor(SCI_STYLESETBACK, SCE_H_XMLSTART, Color::yellow);
				SendEditor(SCI_STYLESETFORE, SCE_H_XMLEND, Color::red);
				SendEditor(SCI_STYLESETBACK, SCE_H_XMLEND, Color::yellow);

				SendEditor(SCI_STYLESETFORE, SCE_D_DEFAULT, Color::black);
				SendEditor(SCI_STYLESETBACK, SCE_D_DEFAULT, Color::white);
				SendEditor(SCI_STYLESETBOLD, SCE_D_DEFAULT, true);

				SendEditor(SCI_STYLESETFORE, SCE_D_NUMBER, Color::red);

				SendEditor(SCI_STYLESETFORE, SCE_H_TAG, Color::tag_blue);
				SendEditor(SCI_STYLESETFORE, SCE_H_TAGEND, Color::tag_blue);
				SendEditor(SCI_STYLESETFORE, SCE_H_TAGUNKNOWN, Color::tag_blue);

				SendEditor(SCI_STYLESETFORE, SCE_H_ATTRIBUTE, Color::attr_red);
				SendEditor(SCI_STYLESETFORE, SCE_H_ATTRIBUTEUNKNOWN, Color::attr_red);

				SendEditor(SCI_STYLESETFORE, SCE_H_DOUBLESTRING, Color::xml_string_blue);
				SendEditor(SCI_STYLESETFORE, SCE_H_SINGLESTRING, Color::xml_string_blue);

				SendEditor(SCI_STYLESETFORE, SCE_H_COMMENT, Color::comment_green);

				SendEditor(SCI_SETCARETLINEVISIBLE, TRUE);
				SendEditor(SCI_SETCARETLINEBACK, Color::current_line_yellow);
			}
			else if (SCLEX_CPP == style)
			{				
				SendEditor(SCI_STYLESETFORE, STYLE_DEFAULT, Color::fore_white);
				SendEditor(SCI_STYLESETBACK, STYLE_DEFAULT, Color::back_black);
				SendEditor(SCI_STYLECLEARALL);

				SendEditor(SCI_STYLESETFORE, STYLE_LINENUMBER, Color::line_number_blue);
				SendEditor(SCI_STYLESETBACK, STYLE_LINENUMBER, Color::line_number_black);
				SendEditor(SCI_SETCARETFORE, Color::caret_white);	//光标

				//试图同步高亮，失败
				//int pos = SendEditor(SCI_GETCURRENTPOS, 0, 0);
				//SendEditor(SCI_STYLESETBACK, STYLE_BRACELIGHT, red);
				//SendEditor(SCI_BRACEHIGHLIGHT, 20, 10);
				//SendEditor(SCI_SETHIGHLIGHTGUIDE, 20);

				SendEditor(SCI_SETSELBACK, true, Color::selection_blue);

				SendEditor(SCI_SETLEXER, SCLEX_CPP);
				SendEditor(SCI_SETKEYWORDS, 0, (sptr_t)cpp_keywords);//设置关键字 
				SendEditor(SCI_SETKEYWORDS, 1, (sptr_t)cpp_type_keywords);//设置关键字 
				
				SendEditor(SCI_STYLESETFORE, SCE_C_DEFAULT, Color::fore_white);
				SendEditor(SCI_STYLESETBACK, SCE_C_DEFAULT, Color::back_black);
				// 下面设置各种语法元素风格 
				SendEditor(SCI_STYLESETFORE, SCE_C_WORD, Color::keywords_blude);   //关键字 
				SendEditor(SCI_STYLESETFORE, SCE_C_WORD2, Color::keywords_blude);   //关键字 
				SendEditor(SCI_STYLESETBOLD, SCE_C_WORD2, TRUE);   //关键字 
				SendEditor(SCI_STYLESETFORE, SCE_C_STRING, Color::cpp_string_gray); //字符串 
				SendEditor(SCI_STYLESETFORE, SCE_C_CHARACTER, Color::cpp_string_gray); //字符 
				SendEditor(SCI_STYLESETFORE, SCE_C_PREPROCESSOR, Color::preprocessor_purple);//预编译开关 

				SendEditor(SCI_STYLESETFORE, SCE_C_COMMENT, Color::comment_green);//块注释 
				SendEditor(SCI_STYLESETFORE, SCE_C_COMMENTLINE, Color::comment_green);//行注释 
				SendEditor(SCI_STYLESETFORE, SCE_C_COMMENTDOC, Color::comment_green);//文档注释（/**开头） 
				
				SendEditor(SCI_SETCARETLINEVISIBLE, TRUE);
				SendEditor(SCI_SETCARETLINEBACK, Color::current_line_black);
			}
			else
			{
				SendEditor(SCI_STYLESETFORE, STYLE_DEFAULT, Color::back_black);
				SendEditor(SCI_STYLESETBACK, STYLE_DEFAULT, Color::white);
				SendEditor(SCI_STYLECLEARALL);
				SendEditor(SCI_SETCARETFORE, Color::caret_black);	//光标
				SendEditor(SCI_SETSELBACK, true, Color::selection_wight);
				SendEditor(SCI_STYLESETSIZE, STYLE_DEFAULT, 14);

				SendEditor(SCI_SETCARETLINEVISIBLE, TRUE);
				SendEditor(SCI_SETCARETLINEBACK, Color::current_line_yellow);
			}
			
		}

		void SetPos(RECT rect, int menu_height = 0)
		{
			SetWindowPos(m_hwnd, HWND_TOP, 0, menu_height,
				rect.right - rect.left - 11, rect.bottom - rect.top - menu_height, SWP_SHOWWINDOW);
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
		
		int New()
		{
			if (NULL != m_filename)
			{
				delete m_filename;
				m_filename = NULL;
			}
			SendEditor(SCI_SETTEXT, NULL, (sptr_t)"");
			return 0;
		}

		/// <summary>
		/// 使用打开对话框打开文件
		/// </summary>
		int OpenFileDlg(HWND win)
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
				LoadFile(win, filename);
			}
			//GetSaveFileName
			return 0;
		}

		int LoadFile(HWND win, LPCWSTR filename)
		{
			LoadFromFile(filename);

			SetWindowText(win, std::wstring(filename).append(L" - Matrix").c_str());

			if (m_auto_validate)
			{
				ValidateXml();
			}
			return 0;
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

		int Save()
		{
			int nlen = SendEditor(SCI_GETLENGTH);
			char * content = new char[nlen + 1];
			SendEditor(SCI_GETTEXT, nlen + 1, (sptr_t)content);
			if (NULL == m_filename)
			{
				return SaveAs();
			}
			else if (NULL == *content)
			{
				return -1;
			}
			else
			{
				int res = Matrix::File(m_filename).OverWrite(content, nlen);
			}
			return 0;
		}

		int SaveAs()
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

			if (GetSaveFileName(&ofn))
			{				
				size_t ulen = wcslen(ofn.lpstrFile);
				wchar_t * filename = new wchar_t[ulen + 1];
				lstrcpynW(filename, ofn.lpstrFile, ulen + 1);

				int nlen = SendEditor(SCI_GETLENGTH);
				char * content = new char[nlen + 1];
				SendEditor(SCI_GETTEXT, nlen+1, (sptr_t)content);
				bool is_saved = false;
				if (0 == Matrix::File(filename).WriteText(content, nlen, false))
				{
					if (IDYES == MessageBoxW(m_hwnd, L"该文件已存在，要覆盖吗？", L"Warning", MB_YESNO))
					{
						Matrix::File(filename).OverWrite(content, nlen);
						is_saved = true;
					}
				}
				else
				{
					is_saved = true;
				}

				if (is_saved)
				{
					if (NULL != m_filename)
					{
						delete m_filename;
					}
					m_filename = filename;
				}
				else
				{
					delete filename;
					filename = NULL;
				}
			}
			//GetSaveFileName
			return 0;
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
				const char * text = Matrix::File(m_filename).Utf8Text(++m_current_page, &size);
				if (NULL == text)
				{
					m_file_pos = Matrix::FilePos::END;
					return --m_current_page;
				}
				m_file_pos = Matrix::FilePos::INSIDE;
				SendEditor(SCI_APPENDTEXT, size, (sptr_t)text);
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

		/// <summary>
		/// 简单校验XML文件一致性错误
		/// </summary>
		int ValidateXml(LPCWSTR document = NULL)
		{
			char *content = NULL;
			if (NULL == document)
			{
				int nlen = SendEditor(SCI_GETLENGTH);
				content = new char[nlen + 1];
				SendEditor(SCI_GETTEXT, nlen, (sptr_t)content);
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
			//xml.Parse(content, strlen(content));
			if (NULL != content)
			{
				delete content;
				content = NULL;
			}
			//MessageBox(NULL,xml.Name(), L"", MB_OK);

			Matrix::XMLValidater tXml;
			Matrix::XmlValidateError tError;
			tXml.ValidateXml(content, tError);

			if (NULL != document)
			{
				delete document;
				document = NULL;
			}

			CHAR err[BUFSIZ];
			sprintf(err, "第%d行%d列%s与第%d行%d列%s不匹配",
				tError.Open.Line, tError.Open.Row, tError.OpenName(),
				tError.Close.Line, tError.Close.Row, tError.CloseName());

			if (tError.Count == -1)
			{
				MessageBox(m_hwnd, L"Xml语法错误", L"Error", MB_ICONERROR | MB_OK);
			}
			else if (tError.Count > 0)
			{
				MessageBoxA(m_hwnd, err, "Error", MB_ICONERROR | MB_OK);
				SendEditor(SCI_GOTOLINE, 0);
				SetFocus();
			}
			else
			{
				MessageBox(m_hwnd, L"未见错误", L"Info", MB_ICONINFORMATION | MB_OK);
			}
			return 0;
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
		bool m_auto_validate;
		int m_vscroll_size;
		int m_vscroll_pos;
		wchar_t * m_filename;
		int m_current_page;
		
		Matrix::FilePos m_file_pos;		
	};
}

#endif
