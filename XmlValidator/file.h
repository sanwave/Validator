
/*
*
*
*
*	File Class		In Matrix
*
*	Created by Bonbon	2014.09.25
*
*/


#ifndef _FILE_H_
#define _FILE_H_

#ifndef MATRIX
#include <iostream>
#include <string>
#else
#include "common.h"
#endif
#include "text_encoder.h"

namespace Matrix
{
	class File
	{
	public:

		File(const char *filename)
		{
			m_filename = Matrix::TextEncoder(filename).Unicode();
		}

		File(const wchar_t *filename)
		{
			size_t ulen = wcslen(filename);
			m_filename = new wchar_t[ulen + 1];
			lstrcpynW(m_filename, filename, ulen+1);
		}

		~File()
		{
			delete m_filename;
			m_filename = NULL;
		}

		const wchar_t * Text()
		{
			return ReadAsText(m_filename);
		}

		const char * AnsiText()
		{
			const wchar_t * utext = ReadAsText(m_filename);
			const char * atext = Matrix::TextEncoder::UnicodeToAnsi(utext);
			delete utext;
			return atext;
		}

		const char * Utf8Text()
		{
			const wchar_t * utext = ReadAsText(m_filename);
			const char * u8text = Matrix::TextEncoder::UnicodeToUTF8(utext);
			delete utext;
			return u8text;
		}

		const char * Binary()
		{
			return ReadAsBinary(m_filename);
		}

		TextEncode Encode()
		{
			return Matrix::TextEncoder::DetectEncode(ReadAsBinary(m_filename));
		}

		/// <summary>
		/// 以文本方式读取指定文件内容
		/// </summary>
		/// <param name="fileName">指定文件名</param>
		/// <returns>文件内容的Unicode字符串</returns>
		static const wchar_t * ReadAsText(const char *filename)
		{
			const char * ufilename = Matrix::TextEncoder(filename).Ansi();
			const wchar_t * utext = ReadAsText(ufilename);
			delete ufilename;
			return utext;
		}

		static const wchar_t * ReadAsText(const wchar_t *filename)
		{
			const char * text = ReadAsBinary(filename);
			const wchar_t * utext = Matrix::TextEncoder(text).Unicode();
			delete text;
			return utext;
		}

		/// <summary>
		/// 以二进制方式读取指定文本文件内容
		/// </summary>
		/// <param name="fileName">指定文件名</param>
		/// <returns>文件文本内容</returns>
		static const char * ReadAsBinary(const wchar_t * filename)
		{
			std::fstream file;
			file.open(filename, std::ios_base::in | std::ios_base::binary);
			if (!file.is_open())
			{
				return NULL;
			}

			file.seekg(0, std::ios::end);
			size_t size = static_cast<size_t>(file.tellg());
			file.seekg(0, std::ios::beg);

			char* buffer = new char[size + 1];
			file.read(buffer, size);
			buffer[size] = '\0';
			file.close();

			return buffer;
		}

		static const char * ReadAsBinary(const char * filename)
		{
			const wchar_t * ufilename = Matrix::TextEncoder(filename).Unicode();
			const char * buffer = ReadAsBinary(ufilename);
			delete ufilename;
			return buffer;
		}

	private:
		wchar_t *m_filename;
	};
}

#endif