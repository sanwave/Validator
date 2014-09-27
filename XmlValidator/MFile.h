
/*
*
*
*
*	File Class		In Matrix
*
*	Created by Bonbon	2014.09.25
*
*/


#ifndef _MFILE_H_
#define _MFILE_H_

#include "Common.h"

namespace Matrix
{
	class File
	{
	public:

		/// <summary>
		/// ��ȡָ���ļ����ݣ��˺����������ļ�����
		/// </summary>
		/// <param name="fileName">ָ���ļ���</param>
		/// <returns>�ļ����ݵ�ANSI�ַ���</returns>
		std::string Read(LPCSTR fileName)
		{
			std::ifstream fin(fileName);
			std::string content, tmp;

			while (std::getline(fin, tmp))
			{
				content += tmp + "\r\n";
			}
			return content;
		}

		/// <summary>
		/// ��ȡָ���ı��ļ����ݣ�����Bom��UTF8�����ص�ʶ���ļ�����
		/// </summary>
		/// <param name="fileName">ָ���ļ���</param>
		/// <returns>�ļ��ı�����</returns>
		LPCWSTR ReadFile(LPCSTR fileName)
		{
			std::string content = Read(fileName);
			int encoding = GetEncoding(content);
			if (encoding == CP_UTF8)
			{
				return Utf8ToUnicode(content.c_str());
			}
			else
			{
				return AnsiToUnicode(content.c_str());
			}
		}

		/// <summary>
		/// ��ָ��ANSI��������ת��ΪUnicode����
		/// </summary>
		/// <param name="ansiText">ָ��ANSI�����ı�</param>
		/// <returns>ת�����Unicode�ı�</returns>
		LPWSTR AnsiToUnicode(LPCSTR ansiText)
		{
			//Ԥת�����õ�����ռ�Ĵ�С 
			int uLen = ::MultiByteToWideChar(CP_ACP, NULL, ansiText, strlen(ansiText), NULL, 0);
			//����ռ�Ҫ��'\0'�����ռ䣬MultiByteToWideChar�����'\0'�ռ� 
			LPWSTR uText = new TCHAR[uLen + 1];
			//ת�� 
			::MultiByteToWideChar(CP_ACP, NULL, ansiText, strlen(ansiText), uText, uLen);
			//������'\0' 
			uText[uLen] = '\0';
			return uText;
		}

		/// <summary>
		/// ��ָ��UTF8��������ת��ΪUnicode����
		/// </summary>
		/// <param name="u8Text">ָ��UTF8�����ı�</param>
		/// <returns>ת�����Unicode�ı�</returns>
		LPWSTR Utf8ToUnicode(LPCSTR u8Text)
		{
			//Ԥת�����õ�����ռ�Ĵ�С 
			int uLen = ::MultiByteToWideChar(CP_UTF8, NULL, u8Text, strlen(u8Text), NULL, 0);
			//����ռ�Ҫ��'\0'�����ռ䣬MultiByteToWideChar�����'\0'�ռ� 
			LPWSTR uText = new TCHAR[uLen + 1];
			//ת�� 
			::MultiByteToWideChar(CP_UTF8, NULL, u8Text, strlen(u8Text), uText, uLen);
			//������'\0' 
			uText[uLen] = '\0';

			return uText;
		}


		/// <summary>
		/// ��ָ��Unicode��������ת��ΪANSI����
		/// </summary>
		/// <param name="uText">ָ��Unicode�����ı�</param>
		/// <returns>ת�����ANSI�ı�</returns>
		LPSTR UnicodeToAnsi(LPCWSTR uText)
		{
			//Ԥת�����õ�����ռ�Ĵ�С������õĺ��������������෴ 
			int ansiLen = ::WideCharToMultiByte(CP_ACP, NULL, uText, wcslen(uText), NULL, 0, NULL, NULL);
			//ͬ�ϣ�����ռ�Ҫ��'\0'�����ռ� 
			LPSTR ansiText = new char[ansiLen + 1];
			//unicode���Ӧ��strlen��wcslen 
			::WideCharToMultiByte(CP_ACP, NULL, uText, wcslen(uText), ansiText, ansiLen, NULL, NULL);
			//������'\0' 
			ansiText[ansiLen] = '\0';

			return ansiText;
		}

		/// <summary>
		/// ��ָ��Unicode��������ת��ΪUTF8����
		/// </summary>
		/// <param name="uText">ָ��Unicode�����ı�</param>
		/// <returns>ת�����UTF8�ı�</returns>
		LPSTR UnicodeToUTF8(LPCWSTR uText)
		{
			//Ԥת�����õ�����ռ�Ĵ�С������õĺ��������������෴ 
			int u8Len = ::WideCharToMultiByte(CP_UTF8, NULL, uText, wcslen(uText), NULL, 0, NULL, NULL);
			//ͬ�ϣ�����ռ�Ҫ��'\0'�����ռ� 
			//UTF8��Ȼ��Unicode��ѹ����ʽ����Ҳ�Ƕ��ֽ��ַ��������Կ�����char����ʽ���� 
			char* u8Text = new char[u8Len + 1];
			//unicode���Ӧ��strlen��wcslen 
			::WideCharToMultiByte(CP_UTF8, NULL, uText, wcslen(uText), u8Text, u8Len, NULL, NULL);
			//������'\0' 
			u8Text[u8Len] = '\0';
			return u8Text;
		}

		/// <summary>
		/// ��ȡָ���ı������ʽ���б�ʧЧʱ����UTF8
		/// </summary>
		/// <param name="text">ָ���ı�����</param>
		/// <returns>��Ӧ����ĺ�</returns>
		int GetEncoding(std::string text)
		{
			if ((text[0] == 0xEF)
				&& (text[1] == 0xBB)
				&& (text[2] == 0xBF))
			{
				return CP_UTF8;//UTF8 With Bom
			}
			else if ((text[0] == 0xFF)
				&& (text[1] == 0xFE))
			{
				return 0;//UTF16 Сͷ
			}
			else if ((text[0] == 0xFE)
				&& (text[0] == 0xFF))
			{
				return 0;//UTF16 ��ͷ
			}
			else
			{
				return AnsiOrUTF8(text);
			}
		}


		/// <summary>
		/// ʶ����Bom UTF8��ANSI�����ı����б�ʧЧʱ����UTF8
		/// </summary>
		/// <param name="text">ָ���ı�����</param>
		/// <returns>��Ӧ����ĺ�</returns>
		int AnsiOrUTF8(std::string text)
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
							return CP_ACP;
						}
					}
					index += u8Count;
					i = 0;
				}
				else
				{
					return CP_ACP;
				}
			}
			//Ĭ��UTF8
			return CP_UTF8;
		}

	private:

	};
}

#endif