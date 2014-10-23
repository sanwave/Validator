
/*
*
*
*
*	Document Class		In Matrix
*
*	Created by Bonbon	2014.10.23
*
*/

#ifndef _MDOCUMENT_H_
#define _MDOCUMENT_H_

#include "Common.h"
#include "MFile.h"

namespace Matrix{

	enum Encode
	{
		ANSI = 0,
		UTF_8,
		UTF_8_NO_MARK,
		UTF_16,
		UTF_16_BIG_ENDIAN,
		DefaultEncode = UTF_8
	};

	class Document
	{
	public:

		TCHAR *m_buffer;

		Document() :m_buffer(NULL){}
		~Document()
		{
			if (m_buffer != NULL)
			{
				delete[] m_buffer;
				m_buffer = NULL;
			}
		}

		bool LoadFromFile(const char* filename)
		{
			assert(filename != NULL);
			std::fstream file;
			file.open(filename, std::ios_base::in | std::ios_base::binary);
			if (!file.is_open())
			{
				return false;
			}
			bool succeeded = LoadFromStream(file);
			file.close();
			return succeeded;
		}

		bool LoadFromStream(std::istream& input)
		{
			input.seekg(0, std::ios::end);
			size_t size = static_cast<size_t>(input.tellg());
			input.seekg(0, std::ios::beg);

			char* buffer = new char[size + 1];
			input.read(buffer, size);
			buffer[size] = '\0';

			bool succeeded = ReallyLoadFromMemory(buffer, size);
			if (!succeeded)
			{
				if (m_buffer != NULL)
				{
					delete[] m_buffer;
					m_buffer = NULL;
				}
			}
			return succeeded;
		}

		bool ReallyLoadFromMemory(char* buffer, size_t size)
		{
			if (m_buffer != NULL)
			{
				delete[] m_buffer;
				m_buffer = NULL;
			}
			if (size < 3)
			{
				return false;
			}
			//get encode
			Encode encode = ANSI;
			bool multiBytes = false;
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
				encode = DetectEncode(buffer, size, multiBytes);
			}
			
			if (encode == UTF_8 || encode == UTF_8_NO_MARK)
			{
				m_buffer = Matrix::File().Utf8ToUnicode(buffer);
			}
			else
			{
				m_buffer = Matrix::File().AnsiToUnicode(buffer);
			}

			return true;
		}
		
		Encode DetectEncode(const char* str, size_t size, bool& multiBytes)
		{
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
					multiBytes = true;
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
						if ((*(str + 1) & 0xc0) != 0x80 || (*(str + 2) & 0xc0) != 0x80 || (*(str + 3) & 0xc0) != 0x80)
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
	};
}

#endif
