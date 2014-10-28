
/*
*
*
*
*	TextEncoder Class		In Matrix
*
*	Created by Bonbon	2014.10.27
*
*/


#ifndef _TEXT_ENCODER_H_
#define _TEXT_ENCODER_H_

#ifndef MATRIX
#include <iostream>
#include <string>
#else
#include "common.h"
#endif

namespace Matrix
{
	enum TextEncode
	{
		ANSI = 0,
		UTF_8,
		UTF_8_NO_MARK,
		UTF_16,
		UTF_16_BIG_ENDIAN,
		DefaultEncode = UTF_8_NO_MARK
	};

	class TextEncoder
	{
	public:

		TextEncoder(const char *buffer)
		{
			TextEncode encode = DetectEncode(buffer);
			if (ANSI == encode)
			{
				m_buffer = AnsiToUnicode(buffer);
			}
			else if (UTF_8_NO_MARK == encode)
			{
				m_buffer = Utf8ToUnicode(buffer);
			}
			else if (UTF_8 == encode)
			{
				m_buffer = Utf8ToUnicode(buffer + 3);
			}
			else
			{
				m_buffer = NULL;
			}
		}

		TextEncoder(const wchar_t *buffer) :m_buffer(buffer)
		{}

		TextEncoder(std::string buffer)
		{
			TextEncode encode = DetectEncode(buffer);
			if (ANSI == encode)
			{
				m_buffer = AnsiToUnicode(buffer.c_str());
			}
			else if (UTF_8_NO_MARK == encode)
			{
				m_buffer = Utf8ToUnicode(buffer.c_str());
			}
			else if (UTF_8 == encode)
			{
				m_buffer = Utf8ToUnicode(buffer.substr(3).c_str());
			}
			else
			{
				m_buffer = NULL;
			}
		}

		TextEncoder(std::wstring buffer) :m_buffer(buffer.c_str())
		{}

		const char* Ansi()
		{
			return UnicodeToAnsi(m_buffer);
		}

		const char* Utf8()
		{
			return UnicodeToUTF8(m_buffer);
		}

		const wchar_t* Unicode()
		{
			return m_buffer;
		}

		/// <summary>
		/// ��ָ��ANSI��������ת��ΪUnicode����
		/// </summary>
		/// <param name="ansiText">ָ��ANSI�����ı�</param>
		/// <returns>ת�����Unicode�ı�</returns>
		static wchar_t* AnsiToUnicode(const char * atext, size_t size = 0)
		{
			int len = ::MultiByteToWideChar(CP_ACP, NULL, atext, strlen(atext), NULL, 0);
			wchar_t* utext = new wchar_t[len + 1];
			::MultiByteToWideChar(CP_ACP, NULL, atext, strlen(atext), utext, len);
			utext[len] = '\0';
			return utext;
		}

		/// <summary>
		/// ��ָ��UTF8��������ת��ΪUnicode����
		/// </summary>
		/// <param name="u8Text">ָ��UTF8�����ı�</param>
		/// <returns>ת�����Unicode�ı�</returns>
		static wchar_t* Utf8ToUnicode(const char * u8text, size_t size = 0)
		{
			int len = ::MultiByteToWideChar(CP_UTF8, NULL, u8text, strlen(u8text), NULL, 0);
			wchar_t* utext = new wchar_t[len + 1];
			::MultiByteToWideChar(CP_UTF8, NULL, u8text, strlen(u8text), utext, len);
			utext[len] = '\0';
			return utext;
		}


		/// <summary>
		/// ��ָ��Unicode��������ת��ΪANSI����
		/// </summary>
		/// <param name="uText">ָ��Unicode�����ı�</param>
		/// <returns>ת�����ANSI�ı�</returns>
		static char* UnicodeToAnsi(const wchar_t* utext, size_t size = 0)
		{
			int len = ::WideCharToMultiByte(CP_ACP, NULL, utext, wcslen(utext), NULL, 0, NULL, NULL);
			char *atext = new char[len + 1];
			::WideCharToMultiByte(CP_ACP, NULL, utext, wcslen(utext), atext, len, NULL, NULL);
			atext[len] = '\0';
			return atext;
		}

		/// <summary>
		/// ��ָ��Unicode��������ת��ΪUTF8����
		/// </summary>
		/// <param name="uText">ָ��Unicode�����ı�</param>
		/// <returns>ת�����UTF8�ı�</returns>
		static char* UnicodeToUTF8(const wchar_t* utext, size_t size = 0)
		{
			int len = ::WideCharToMultiByte(CP_UTF8, NULL, utext, wcslen(utext), NULL, 0, NULL, NULL);
			char *u8text = new char[len + 1];
			::WideCharToMultiByte(CP_UTF8, NULL, utext, wcslen(utext), u8text, len, NULL, NULL);
			u8text[len] = '\0';
			return u8text;
		}

		/// <summary>
		/// ��ȡָ���ı������ʽ���б�ʧЧʱ����UTF8
		/// </summary>
		/// <param name="text">ָ���ı�����</param>
		/// <returns>��Ӧ����ĺ�</returns>
		static TextEncode DetectEncode(std::string text)
		{
			if ((text[0] == 0xEF) && (text[1] == 0xBB) && (text[2] == 0xBF))
			{
				return UTF_8;//UTF8 With Bom
			}
			else if ((text[0] == 0xFF) && (text[1] == 0xFE))
			{
				return UTF_16;
			}
			else if ((text[0] == 0xFE) && (text[0] == 0xFF))
			{
				return UTF_16_BIG_ENDIAN;
			}
			else
			{
				return DetectAnsiOrUtf8(text);
			}
		}

		/// <summary>
		/// ʶ����Bom UTF8��ANSI�����ı����б�ʧЧʱ����UTF8
		/// </summary>
		/// <param name="text">ָ���ı�����</param>
		/// <returns>��Ӧ����ĺ�</returns>
		static TextEncode DetectAnsiOrUtf8(std::string text)
		{
			int index = -1;
			int u8Count = 0;
			char i = -1;
			char ch;
			while (text[++index])
			{
				ch = text[index];
				if ((ch & 0x80) == 0)
				{
					continue;//Ansi�ַ�
				}
				else if ((ch & 0xC0) == 0xC0)
				{
					if ((ch & 0xFC) == 0xFC)
					{
						u8Count = 5;
					}
					else if ((ch & 0xF8) == 0xF8)
					{
						u8Count = 4;
					}
					else if ((ch & 0xF0) == 0xF0)
					{
						u8Count = 3;
					}
					else if ((ch & 0xE0) == 0xE0)
					{
						u8Count = 2;
					}
					else if ((ch & 0xC0) == 0xC0)
					{
						u8Count = 1;
					}
					while (u8Count > ++i)
					{
						if ((text[index + 1 + i] & 0x80) != 0x80)
						{
							return ANSI;
						}
					}
					index += u8Count;
					i = 0;
				}
				else
				{
					return ANSI;
				}
			}
			//Ĭ��UTF8
			return UTF_8_NO_MARK;
		}

		/// <summary>
		/// ��ȡָ���ı������ʽ���б�ʧЧʱ����UTF8
		/// </summary>
		/// <param name="buffer">ָ���ı�����</param>
		/// <returns>��Ӧ����ĺ�</returns>
		static TextEncode DetectEncode(const char* buffer, size_t size = 0)
		{
			TextEncode encode = ANSI;

			const unsigned char* bom = reinterpret_cast<const unsigned char*>(buffer);
			if (bom[0] == 0xfe && bom[1] == 0xff)
			{
				encode = UTF_16_BIG_ENDIAN;
			}
			else if (bom[0] == 0xff && bom[1] == 0xfe)
			{
				encode = UTF_16;
			}
			else if (bom[0] == 0xef && bom[1] == 0xbb && bom[2] == 0xbf)
			{
				encode = UTF_8;
			}
			else
			{
				encode = DetectAnsiOrUtf8(buffer, size);
			}
			return encode;
		}

		/// <summary>
		/// ʶ����Bom UTF8��ANSI�����ı����б�ʧЧʱ����UTF8
		/// </summary>
		/// <param name="str">ָ���ı�����</param>
		/// <returns>��Ӧ����ĺ�</returns>
		static TextEncode DetectAnsiOrUtf8(const char* str, size_t size = 0)
		{
			if (0 == size)
			{
				size = strlen(str);
			}

			while (size > 0)
			{
				if ((*str & 0x80) == 0)
				{
					//1 byte
					++str;
					--size;
				}
				else
				{
					if ((*str & 0xf0) == 0xe0)
					{
						//3 bytes
						if (size < 3)
						{
							return ANSI;
						}
						if ((*(str + 1) & 0xc0) != 0x80 || (*(str + 2) & 0xc0) != 0x80)
						{
							return ANSI;
						}
						str += 3;
						size -= 3;
					}
					else if ((*str & 0xe0) == 0xc0)
					{
						//2 bytes
						if (size < 2)
						{
							return ANSI;
						}
						if ((*(str + 1) & 0xc0) != 0x80)
						{
							return ANSI;
						}
						str += 2;
						size -= 2;
					}
					else if ((*str & 0xf8) == 0xf0)
					{
						//4 bytes
						if (size < 4)
						{
							return ANSI;
						}
						if ((*(str + 1) & 0xc0) != 0x80 || (*(str + 2) & 0xc0) != 0x80
							|| (*(str + 3) & 0xc0) != 0x80)
						{
							return ANSI;
						}
						str += 4;
						size -= 4;
					}
					else
					{
						return ANSI;
					}
				}
			}
			return UTF_8_NO_MARK;
		}

	private:

		const wchar_t *m_buffer;

	};
}

#endif