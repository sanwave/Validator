
/*
*
*
*
*	Document Class		In Matrix
*
*	Created by Bonbon	2014.10.23
*
*/

#ifndef _DOCUMENT_H_
#define _DOCUMENT_H_

#include "common.h"
#include "text_encoder.h"

namespace Matrix
{

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
			
			bool multiBytes = false;
			TextEncode encode = Matrix::TextEncoder().DetectEncode(buffer, size, multiBytes);

			if (encode == UTF_8 || encode == UTF_8_NO_MARK)
			{
				m_buffer = Matrix::TextEncoder().Utf8ToUnicode(buffer);
			}
			else
			{
				m_buffer = Matrix::TextEncoder().AnsiToUnicode(buffer);
			}
			

			return true;
		}		
		
	};
}

#endif
