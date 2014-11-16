
#include "sci_editor.h"


namespace Matrix
{
	Matrix::SciEditor * Matrix::SciEditor::m_this = NULL;

	SciEditor::SciEditor() :
		m_line_wrap(true),
		m_auto_validate(false),
		m_current_page(0),
		m_filename(NULL),
		m_file_pos(Matrix::FilePos::HEAD),
		m_vscroll_pos(0),
		m_vscroll_size(0),
		m_read_all(true),
		m_black_theme(false)
	{
		if (NULL == m_this)
		{
			m_this = this;
		}
	}

	SciEditor::~SciEditor()
	{
		if (NULL != m_filename)
		{
			delete m_filename;
			m_filename = NULL;
		}
	}

	void SciEditor::SetCurrentPtr(SciEditor * value)
	{
		m_this = value;
	}

	void SciEditor::SetAutoValidate(short value)
	{
		m_auto_validate = (value == 0) ? false : true;
	}

	void SciEditor::SetWrap(short iflag)
	{
		m_line_wrap = (0 == iflag) ? false : true;
		SendEditor(SCI_SETWRAPMODE, (WPARAM)(m_line_wrap == 0 ? 0 : 1));
	}

	void SciEditor::Create(HWND hwndParent)
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

	void SciEditor::Init()
	{
		SendEditor(SCI_SETTABWIDTH, 4);
		SendEditor(SCI_SETCODEPAGE, SC_CP_UTF8);
		SetLineNumber();
		SetWrap(m_line_wrap);		
		SetBlackTheme(m_black_theme);
	}

	void SciEditor::SetLineNumber()
	{
		SendEditor(SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
		SendEditor(SCI_SETMARGINWIDTHN, 0, 30);
	}

	void SciEditor::SetBlackTheme(bool value)
	{
		m_black_theme = value;
		int text_style = 0;
		text_style = SendEditor(SCI_GETLEXER);

		SendEditor(SCI_STYLESETFONT, STYLE_DEFAULT, (sptr_t)"Microsoft Yahei");
		SendEditor(SCI_STYLESETFONT, STYLE_DEFAULT, (sptr_t)"Consolas");
		SendEditor(SCI_STYLESETSIZE, STYLE_DEFAULT, 12);

		if (m_black_theme)
		{
			SendEditor(SCI_STYLESETFORE, STYLE_DEFAULT, Color::fore_white);
			SendEditor(SCI_STYLESETBACK, STYLE_DEFAULT, Color::back_black);
			SendEditor(SCI_STYLECLEARALL);
			
			SendEditor(SCI_STYLESETFORE, STYLE_LINENUMBER, Color::line_number_blue);
			SendEditor(SCI_STYLESETBACK, STYLE_LINENUMBER, Color::line_number_black);

			SendEditor(SCI_SETFOLDMARGINCOLOUR, true, Color::line_number_black);
			SendEditor(SCI_SETFOLDMARGINHICOLOUR, true, Color::line_number_blue);
			
			SendEditor(SCI_SETCARETFORE, Color::caret_white);	//���
			SendEditor(SCI_SETSELBACK, true, Color::selection_blue);
			SendEditor(SCI_SETCARETLINEVISIBLE, TRUE);
			SendEditor(SCI_SETCARETLINEBACK, Color::current_line_black);
		}
		else
		{
			SendEditor(SCI_STYLESETFORE, STYLE_DEFAULT, Color::back_black);
			SendEditor(SCI_STYLESETBACK, STYLE_DEFAULT, Color::white);
			SendEditor(SCI_STYLECLEARALL);

			SendEditor(SCI_SETFOLDMARGINCOLOUR, true, Color::margin_light_back);
			SendEditor(SCI_SETFOLDMARGINHICOLOUR, true, Color::margin_light_back);

			SendEditor(SCI_SETCARETFORE, Color::caret_black);	//���
			SendEditor(SCI_SETSELBACK, true, Color::selection_white);
			SendEditor(SCI_SETCARETLINEVISIBLE, TRUE);
			SendEditor(SCI_SETCARETLINEBACK, Color::current_line_yellow);			
		}

		SetTextStyle(text_style);
	}

	void SciEditor::SetTextStyle(int style)
	{
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

		if (SCLEX_XML == style || SCLEX_HTML == style)
		{
			SendEditor(SCI_SETLEXER, SCLEX_HTML);
			SendEditor(SCI_STYLESETFORE, SCE_H_XMLSTART, Color::red);
			SendEditor(SCI_STYLESETBACK, SCE_H_XMLSTART, Color::yellow);
			SendEditor(SCI_STYLESETFORE, SCE_H_XMLEND, Color::red);
			SendEditor(SCI_STYLESETBACK, SCE_H_XMLEND, Color::yellow);

			//SendEditor(SCI_STYLESETFORE, SCE_D_DEFAULT, Color::black);
			//SendEditor(SCI_STYLESETBACK, SCE_D_DEFAULT, Color::white);
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
		}
		else if (SCLEX_CPP == style)
		{
			//��ͼͬ��������ʧ��
			//int pos = SendEditor(SCI_GETCURRENTPOS, 0, 0);
			//SendEditor(SCI_STYLESETBACK, STYLE_BRACELIGHT, red);
			//SendEditor(SCI_BRACEHIGHLIGHT, 20, 10);
			//SendEditor(SCI_SETHIGHLIGHTGUIDE, 20);

			SendEditor(SCI_SETLEXER, SCLEX_CPP);
			SendEditor(SCI_SETKEYWORDS, 0, (sptr_t)cpp_keywords);//���ùؼ��� 
			SendEditor(SCI_SETKEYWORDS, 1, (sptr_t)cpp_type_keywords);//���ùؼ��� 

			//SendEditor(SCI_STYLESETFORE, SCE_C_DEFAULT, Color::fore_white);
			//SendEditor(SCI_STYLESETBACK, SCE_C_DEFAULT, Color::back_black);

			// �������ø����﷨Ԫ�ط�� 
			SendEditor(SCI_STYLESETFORE, SCE_C_WORD, Color::keywords_blude);   //�ؼ��� 
			SendEditor(SCI_STYLESETFORE, SCE_C_WORD2, Color::keywords_blude);   //�ؼ��� 
			SendEditor(SCI_STYLESETBOLD, SCE_C_WORD2, TRUE);   //�ؼ��� 
			SendEditor(SCI_STYLESETFORE, SCE_C_STRING, Color::cpp_string_gray); //�ַ��� 
			SendEditor(SCI_STYLESETFORE, SCE_C_CHARACTER, Color::cpp_string_gray); //�ַ� 
			SendEditor(SCI_STYLESETFORE, SCE_C_PREPROCESSOR, Color::preprocessor_purple);//Ԥ���뿪�� 

			SendEditor(SCI_STYLESETFORE, SCE_C_COMMENT, Color::comment_green);//��ע�� 
			SendEditor(SCI_STYLESETFORE, SCE_C_COMMENTLINE, Color::comment_green);//��ע�� 
			SendEditor(SCI_STYLESETFORE, SCE_C_COMMENTDOC, Color::comment_green);//�ĵ�ע�ͣ�/**��ͷ��
		}

		SetFolder();		
	}

	void SciEditor::SetFolder()
	{		
		SendEditor(SCI_SETPROPERTY, (sptr_t)"fold", (sptr_t)"1");
		SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.compact", (LPARAM)"0");
		SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.html", (LPARAM)"1");
		SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.html.preprocessor", (LPARAM)"1");

		
		SendEditor(SCI_SETMARGINTYPEN, 2, SC_MARGIN_SYMBOL);//ҳ������ 
		SendEditor(SCI_SETMARGINMASKN, 2, SC_MASK_FOLDERS); //ҳ������ 
		SendEditor(SCI_SETMARGINWIDTHN, 2, 11); //ҳ�߿��
		
		SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_PIXMAP);
		SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_PIXMAP);
		SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND, SC_MARK_PIXMAP);
		SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_PIXMAP);
		SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNERCURVE);
		SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
		SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNERCURVE);
		// 
		SendEditor(SCI_MARKERDEFINEPIXMAP, SC_MARKNUM_FOLDER, (sptr_t)plus_xpm);
		SendEditor(SCI_MARKERDEFINEPIXMAP, SC_MARKNUM_FOLDEROPEN, (sptr_t)minus_xpm);
		SendEditor(SCI_MARKERDEFINEPIXMAP, SC_MARKNUM_FOLDEREND, (sptr_t)plus_xpm);
		SendEditor(SCI_MARKERDEFINEPIXMAP, SC_MARKNUM_FOLDEROPENMID, (sptr_t)minus_xpm);

		// �۵���ǩ��ɫ
		if (m_black_theme)
		{
			SendEditor(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERSUB, Color::line_number_blue);
			SendEditor(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERMIDTAIL, Color::line_number_blue);
			SendEditor(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERTAIL, Color::line_number_blue);
		}		

		SendEditor(SCI_SETFOLDFLAGS, 16, 0); //����۵������۵��е����¸���һ������ 

		SendEditor(SCI_SETMARGINSENSITIVEN, 2, TRUE); //��Ӧ�����Ϣ		
	}

	void SciEditor::SetPos(RECT rect, int menu_height)
	{
		SetWindowPos(m_hwnd, HWND_TOP, 0, menu_height,
			rect.right - rect.left - 11, rect.bottom - rect.top - menu_height, SWP_SHOWWINDOW);
	}

	void SciEditor::SetReadAll(bool value)
	{
		m_read_all = value;
		if (NULL != m_filename)
		{
			LoadFromFile(m_filename);
		}
	}
	
	int SciEditor::Search(wchar_t * text)
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

	int SciEditor::New()
	{
		if (NULL != m_filename)
		{
			delete m_filename;
			m_filename = NULL;
		}
		SendEditor(SCI_SETTEXT, NULL, (sptr_t)"");
		return 0;
	}

	int SciEditor::OpenFileDlg(HWND win)
	{
		OPENFILENAME ofn;      // �����Ի���ṹ��     
		wchar_t filename[MAX_PATH]; // �����ȡ�ļ����ƵĻ�������               
		// ��ʼ��ѡ���ļ��Ի���
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
		// ��ʾ��ѡ���ļ��Ի���     

		if (GetOpenFileName(&ofn))
		{
			LoadFile(win, filename);
		}
		//GetSaveFileName
		return 0;
	}

	int SciEditor::LoadFile(HWND win, LPCWSTR filename)
	{
		LoadFromFile(filename);

		SetWindowText(win, std::wstring(filename).append(L" - Matrix").c_str());

		if (m_auto_validate)
		{
			ValidateXml();
		}
		return 0;
	}

	int SciEditor::LoadFromFile(const wchar_t *filename)
	{
		if (NULL == filename)
		{
			return -2;
		}

		m_current_page = (m_read_all == false) ? 0 : -1;

		size_t ulen = wcslen(filename);
		m_filename = new wchar_t[ulen + 1];
		lstrcpynW(m_filename, filename, ulen + 1);

		_wcslwr_s(m_filename, ulen + 1);

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
	
	int SciEditor::Save()
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

	int SciEditor::SaveAs()
	{
		OPENFILENAME ofn;      // �����Ի���ṹ��     
		wchar_t filename[MAX_PATH]; // �����ȡ�ļ����ƵĻ�������               
		// ��ʼ��ѡ���ļ��Ի���
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
		// ��ʾ��ѡ���ļ��Ի���     

		if (GetSaveFileName(&ofn))
		{
			size_t ulen = wcslen(ofn.lpstrFile);
			wchar_t * filename = new wchar_t[ulen + 1];
			lstrcpynW(filename, ofn.lpstrFile, ulen + 1);

			int nlen = SendEditor(SCI_GETLENGTH);
			char * content = new char[nlen + 1];
			SendEditor(SCI_GETTEXT, nlen + 1, (sptr_t)content);
			bool is_saved = false;
			if (0 == Matrix::File(filename).WriteText(content, nlen, false))
			{
				if (IDYES == MessageBoxW(m_hwnd, L"���ļ��Ѵ��ڣ�Ҫ������", L"Warning", MB_YESNO))
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

	int SciEditor::Cut()
	{
		SendEditor(SCI_CUT);
		return 1;
	}

	int SciEditor::Copy()
	{
		SendEditor(SCI_COPY);
		return 1;
	}

	int SciEditor::Paste()
	{
		SendEditor(SCI_PASTE);
		return 1;
	}

	int SciEditor::NextPage()
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
	
	int SciEditor::ValidateXml()
	{
		char *content = NULL;
		int nlen = SendEditor(SCI_GETLENGTH);
		content = new char[nlen + 1];
		SendEditor(SCI_GETTEXT, nlen + 1, (sptr_t)content);

		Matrix::XMLValidater tXml;
		Matrix::XmlValidateError tError;
		tXml.ValidateXml(content, tError);

		if (NULL != content)
		{
			delete content;
			content = NULL;
		}

		CHAR err[BUFSIZ];
		sprintf(err, "��%d��%d��%s���%d��%d��%s��ƥ��",
			tError.Open.Line, tError.Open.Row, tError.OpenName(),
			tError.Close.Line, tError.Close.Row, tError.CloseName());

		if (tError.Count() == -1)
		{
			MessageBox(m_hwnd, L"Xml�﷨����", L"Error", MB_ICONERROR | MB_OK);
		}
		else if (tError.Count() > 0)
		{
			MessageBoxA(m_hwnd, err, "Error", MB_ICONERROR | MB_OK);
			SendEditor(SCI_GOTOLINE, 0);
			SetFocus();
		}
		else
		{
			MessageBox(m_hwnd, L"δ������", L"Info", MB_ICONINFORMATION | MB_OK);
		}
		return 0;
	}
	
	void SciEditor::SetFocus()
	{
		::SetFocus(m_hwnd);
	}
	
	void SciEditor::GetScrollSize()
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

	void SciEditor::HandleMsg(SCNotification * msg, WPARAM wParam)
	{
		int i = LOWORD(wParam);
		int vpos = 0;
		int line_number = 0;

		switch (msg->nmhdr.code)
		{
		case SCN_UPDATEUI:
			switch (msg->updated)
			{
			case SC_UPDATE_V_SCROLL:
				GetScrollSize();
				vpos = GetScrollPos(m_hwnd, SB_VERT);
				if (m_vscroll_size <= m_vscroll_pos + 50)
				{
					NextPage();
				}
				break;

			default:
				break;
			}
			break;

		case SCN_MARGINCLICK:
			line_number = SendEditor(SCI_LINEFROMPOSITION, msg->position);
			SendEditor(SCI_TOGGLEFOLD, line_number);
			break;			

		default:
			break;
		}
	}

	sptr_t SciEditor::SendEditor(unsigned int iMessage, uptr_t wParam, sptr_t lParam)
	{
		return m_fnDirect(m_ptrDirect, iMessage, wParam, lParam);
	}
}

