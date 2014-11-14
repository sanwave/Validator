
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
	class Color
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

		SciEditor();

		~SciEditor();

        inline static SciEditor * CurrentPtr()
        {
            return m_this;
        }
        inline static void SetCurrentPtr(SciEditor * value)
        {
            m_this = value;
        }

		inline bool AutoValidate() const
		{
			return m_auto_validate;
		}
		inline void SetAutoValidate(short value)
		{
			m_auto_validate = (value == 0) ? false : true;
		}

		inline wchar_t * FileName() const
		{
			return m_filename;
		}
		
		void Create(HWND hwndParent);
		void Init();
		void HandleMsg(SCNotification * msg, WPARAM wParam);

		bool LineWrap() const
		{
			return m_line_wrap;
		}
		void SetWrap(short iflag);
		void SetLineNumber();
		void SetTextStyle(int style);
		void SetPos(RECT rect, int menu_height = 0);
		void SetFocus();
		void GetScrollSize();
		
		//有问题
		int Search(wchar_t * text);
		
		int New();
		int OpenFileDlg(HWND win);
		int LoadFile(HWND win, LPCWSTR filename);
		int LoadFromFile(const wchar_t *filename);
		int Save();
		int SaveAs();

		int NextPage();
		int ValidateXml();

	private:
		inline sptr_t SendEditor(unsigned int iMessage, uptr_t wParam = 0, sptr_t lParam = 0);

		HWND m_hwnd;
		SciFnDirect m_fnDirect;
		sptr_t m_ptrDirect;

		bool m_line_wrap;
		bool m_auto_validate;
		int m_vscroll_size;
		int m_vscroll_pos;
		wchar_t * m_filename;
		int m_current_page;

		static SciEditor * m_this;		
		Matrix::FilePos m_file_pos;		
	};
}

#endif
