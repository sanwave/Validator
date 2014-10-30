
/*
*
*
*
*	File Class		In Matrix
*
*	Created by Bonbon	2014.09.25
*
*	Updated by Bonbon	2014.10.29
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
			//ÅÐ¶ÏÎÄ¼þ´æÔÚ
			if (NULL == filename)
			{
				m_filename = NULL;
			}
			else
			{
				m_filename = Matrix::TextEncoder(filename).Unicode();
			}
			
		}

		File(const wchar_t *filename)
		{
			if (NULL == filename)
			{
				m_filename = NULL;
			}
			else
			{
				size_t ulen = wcslen(filename);
				m_filename = new wchar_t[ulen + 1];
				lstrcpynW(m_filename, filename, ulen + 1);
			}
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

		const char * AnsiText(int page = 0,size_t *size=NULL)
		{
			const wchar_t * utext = ReadAsText(m_filename, page);
			if (NULL == utext)
			{
				return NULL;
			}
			else if (NULL != size)
			{
				*size = wcslen(utext);
			}			
			const char * atext = Matrix::TextEncoder::UnicodeToAnsi(utext);
			if (NULL != utext)
			{
				delete utext;
				utext = NULL;
			}
			return atext;
		}

		const char * Utf8Text(int page = 0, size_t *size=NULL)
		{
			const wchar_t * utext = ReadAsText(m_filename, page);			
			if (NULL == utext)
			{
				return NULL;
			}
			else if (NULL != size)
			{
				*size = wcslen(utext);
			}			
			const char * u8text = Matrix::TextEncoder::UnicodeToUTF8(utext);
			if (NULL != utext)
			{
				delete utext;
				utext = NULL;
			}
			return u8text;
		}

		const char * Binary(int page = 0, size_t *size = NULL)
		{
			if (NULL != size)
			{
				*size = FBUFSIZ;
			}
			return ReadAsBinary(m_filename, page);
		}

		TextEncode Encode()
		{
			if (NULL == m_filename)
			{
				return Matrix::TextEncode::UNKNOWN;
			}
			const char * sample = ReadBlock(m_filename, 0, FBUFSIZ);
			TextEncode encode = Matrix::TextEncoder::DetectEncode(sample);
			if (NULL != sample)
			{
				delete sample;
				sample = NULL;
			}			
			return encode;
		}

		static const wchar_t * ReadAsText(const char *filename, int page = 0)
		{
			if (NULL == filename)
			{
				return NULL;
			}
			const char * ufilename = Matrix::TextEncoder(filename).Ansi();
			const wchar_t * utext = ReadAsText(ufilename, page);
			if (NULL != ufilename)
			{
				delete ufilename;
				ufilename = NULL;
			}
			return utext;
		}

		static const wchar_t * ReadAsText(const wchar_t *filename, int page = 0)
		{
			const char * text = ReadBlock(filename, FPAGESIZ * page);
			if (NULL == text)
			{
				return NULL;
			}
			const wchar_t * utext = Matrix::TextEncoder(text).Unicode();
			if (NULL != text)
			{
				delete text;
				text = NULL;
			}			
			return utext;
		}

		static const char * ReadAsBinary(const char * filename, int page = 0)
		{
			if (NULL == filename)
			{
				return NULL;
			}
			const wchar_t * ufilename = Matrix::TextEncoder(filename).Unicode();
			const char * buffer = ReadBlock(ufilename, FPAGESIZ * page);
			if (NULL != ufilename)
			{
				delete ufilename;
				ufilename = NULL;
			}			
			return buffer;
		}

		static const char * ReadAsBinary(const wchar_t * filename, int page = 0)
		{
			return ReadBlock(filename, FPAGESIZ * page);
		}

		static const char * ReadBlock(const wchar_t * filename, off_t off = 0, size_t read_size = FPAGESIZ)
		{
			std::fstream file;
			if (NULL == filename)
			{
				return NULL;
			}
			file.open(filename, std::ios_base::in | std::ios_base::binary);
			if (!file.is_open())
			{
				return NULL;
			}

			file.seekg(0, std::ios::end);
			size_t size = static_cast<size_t>(file.tellg());

			if (off >= size)
			{
				file.close();
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