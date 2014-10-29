
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

#define FBUFSIZ 4096
#define FPAGESIZ 1024 * 1024

namespace Matrix
{
	enum FilePos
	{
		HEAD,
		INSIDE,
		END
	};


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
			lstrcpynW(m_filename, filename, ulen + 1);
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

		const char * AnsiText(int page = 0)
		{
			const wchar_t * utext = ReadAsText(m_filename, page);
			const char * atext = Matrix::TextEncoder::UnicodeToAnsi(utext);
			delete utext;
			return atext;
		}

		const char * Utf8Text(int page = 0)
		{
			const wchar_t * utext = ReadAsText(m_filename, page);
			const char * u8text = Matrix::TextEncoder::UnicodeToUTF8(utext);
			delete utext;
			return u8text;
		}

		const char * Binary(int page = 0)
		{
			return ReadAsBinary(m_filename, page);
		}

		TextEncode Encode()
		{
			return Matrix::TextEncoder::DetectEncode(ReadAsBinary(m_filename));
		}

		static const wchar_t * ReadAsText(const char *filename, int page = 0)
		{
			const char * ufilename = Matrix::TextEncoder(filename).Ansi();
			const wchar_t * utext = ReadAsText(ufilename, page);
			delete ufilename;
			return utext;
		}

		static const wchar_t * ReadAsText(const wchar_t *filename, int page = 0)
		{
			const char * text = ReadBlock(filename, FPAGESIZ * page);
			const wchar_t * utext = Matrix::TextEncoder(text).Unicode();
			delete text;
			return utext;
		}

		static const char * ReadAsBinary(const wchar_t * filename, int page = 0)
		{
			return ReadBlock(filename, FPAGESIZ * page);
		}

		static const char * ReadAsBinary(const char * filename, int page = 0)
		{
			const wchar_t * ufilename = Matrix::TextEncoder(filename).Unicode();
			const char * buffer = ReadBlock(ufilename, FPAGESIZ * page);
			delete ufilename;
			return buffer;
		}

		static const char * ReadBlock(const wchar_t * filename, off_t off = 0, size_t read_size = FPAGESIZ)
		{
			std::fstream file;
			file.open(filename, std::ios_base::in | std::ios_base::binary);
			if (!file.is_open())
			{
				return NULL;
			}

			file.seekg(0, std::ios::end);
			size_t size = static_cast<size_t>(file.tellg());			

			if (off>= size)
			{
				return NULL;
			}
			else if (read_size <= 0 || off + read_size > size)
			{
				read_size = size - off;
			}

			file.seekg(off);
			char* buffer = new char[read_size + 1];
			file.read(buffer, read_size);
			buffer[read_size] = '\0';
			file.close();

			return buffer;
		}

	private:
		wchar_t *m_filename;
	};
}

#endif