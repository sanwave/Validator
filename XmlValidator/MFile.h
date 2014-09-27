
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
		/// 获取指定文件内容，此函数不考虑文件编码
		/// </summary>
		/// <param name="fileName">指定文件名</param>
		/// <returns>文件内容的ANSI字符串</returns>
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
		/// 读取指定文本文件内容，根据Bom及UTF8编码特点识别文件编码
		/// </summary>
		/// <param name="fileName">指定文件名</param>
		/// <returns>文件文本内容</returns>
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
		/// 将指定ANSI编码内容转换为Unicode编码
		/// </summary>
		/// <param name="ansiText">指定ANSI编码文本</param>
		/// <returns>转换后的Unicode文本</returns>
		LPWSTR AnsiToUnicode(LPCSTR ansiText)
		{
			//预转换，得到所需空间的大小 
			int uLen = ::MultiByteToWideChar(CP_ACP, NULL, ansiText, strlen(ansiText), NULL, 0);
			//分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间 
			LPWSTR uText = new TCHAR[uLen + 1];
			//转换 
			::MultiByteToWideChar(CP_ACP, NULL, ansiText, strlen(ansiText), uText, uLen);
			//最后加上'\0' 
			uText[uLen] = '\0';
			return uText;
		}

		/// <summary>
		/// 将指定UTF8编码内容转换为Unicode编码
		/// </summary>
		/// <param name="u8Text">指定UTF8编码文本</param>
		/// <returns>转换后的Unicode文本</returns>
		LPWSTR Utf8ToUnicode(LPCSTR u8Text)
		{
			//预转换，得到所需空间的大小 
			int uLen = ::MultiByteToWideChar(CP_UTF8, NULL, u8Text, strlen(u8Text), NULL, 0);
			//分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间 
			LPWSTR uText = new TCHAR[uLen + 1];
			//转换 
			::MultiByteToWideChar(CP_UTF8, NULL, u8Text, strlen(u8Text), uText, uLen);
			//最后加上'\0' 
			uText[uLen] = '\0';

			return uText;
		}


		/// <summary>
		/// 将指定Unicode编码内容转换为ANSI编码
		/// </summary>
		/// <param name="uText">指定Unicode编码文本</param>
		/// <returns>转换后的ANSI文本</returns>
		LPSTR UnicodeToAnsi(LPCWSTR uText)
		{
			//预转换，得到所需空间的大小，这次用的函数和上面名字相反 
			int ansiLen = ::WideCharToMultiByte(CP_ACP, NULL, uText, wcslen(uText), NULL, 0, NULL, NULL);
			//同上，分配空间要给'\0'留个空间 
			LPSTR ansiText = new char[ansiLen + 1];
			//unicode版对应的strlen是wcslen 
			::WideCharToMultiByte(CP_ACP, NULL, uText, wcslen(uText), ansiText, ansiLen, NULL, NULL);
			//最后加上'\0' 
			ansiText[ansiLen] = '\0';

			return ansiText;
		}

		/// <summary>
		/// 将指定Unicode编码内容转换为UTF8编码
		/// </summary>
		/// <param name="uText">指定Unicode编码文本</param>
		/// <returns>转换后的UTF8文本</returns>
		LPSTR UnicodeToUTF8(LPCWSTR uText)
		{
			//预转换，得到所需空间的大小，这次用的函数和上面名字相反 
			int u8Len = ::WideCharToMultiByte(CP_UTF8, NULL, uText, wcslen(uText), NULL, 0, NULL, NULL);
			//同上，分配空间要给'\0'留个空间 
			//UTF8虽然是Unicode的压缩形式，但也是多字节字符串，所以可以以char的形式保存 
			char* u8Text = new char[u8Len + 1];
			//unicode版对应的strlen是wcslen 
			::WideCharToMultiByte(CP_UTF8, NULL, uText, wcslen(uText), u8Text, u8Len, NULL, NULL);
			//最后加上'\0' 
			u8Text[u8Len] = '\0';
			return u8Text;
		}

		/// <summary>
		/// 获取指定文本编码格式，判别失效时优先UTF8
		/// </summary>
		/// <param name="text">指定文本内容</param>
		/// <returns>对应编码的宏</returns>
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
				return 0;//UTF16 小头
			}
			else if ((text[0] == 0xFE)
				&& (text[0] == 0xFF))
			{
				return 0;//UTF16 大头
			}
			else
			{
				return AnsiOrUTF8(text);
			}
		}


		/// <summary>
		/// 识别无Bom UTF8与ANSI编码文本，判别失效时优先UTF8
		/// </summary>
		/// <param name="text">指定文本内容</param>
		/// <returns>对应编码的宏</returns>
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
					continue;//Ansi字符
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
			//默认UTF8
			return CP_UTF8;
		}

	private:

	};
}

#endif