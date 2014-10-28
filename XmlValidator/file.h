
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

#include "common.h"
#include "text_encoder.h"

namespace Matrix
{
	class File
	{
	public:

		File() :m_buffer(NULL)
		{

		}

		/// <summary>
		/// 以文本方式读取指定文件内容
		/// </summary>
		/// <param name="fileName">指定文件名</param>
		/// <returns>文件内容的Unicode字符串</returns>
		static const wchar_t * ReadAsText(const char *filename)
		{
			const char *text = ReadAsBinary(filename);
			return Matrix::TextEncoder(text).Unicode();
		}

		static const wchar_t * ReadAsText(const wchar_t *filename)
		{
			const char *text = ReadAsBinary(Matrix::TextEncoder::UnicodeToAnsi(filename));
			return Matrix::TextEncoder(text).Unicode();
		}

		/// <summary>
		/// 以二进制方式读取指定文本文件内容
		/// </summary>
		/// <param name="fileName">指定文件名</param>
		/// <returns>文件文本内容</returns>
		static const char * ReadAsBinary(const char * filename)
		{
			assert(filename != NULL);
			std::fstream file;
			file.open(filename, std::ios_base::in | std::ios_base::binary);
			if (!file.is_open())
			{
				return false;
			}

			file.seekg(0, std::ios::end);
			size_t size = static_cast<size_t>(file.tellg());
			file.seekg(0, std::ios::beg);

			char* buffer = new char[size + 1];
			file.read(buffer, size);
			buffer[size] = '\0';
			
			return buffer;
		}

		static const char * ReadAsBinary(const wchar_t * filename)
		{
			return ReadAsBinary(Matrix::TextEncoder::UnicodeToAnsi(filename));
		}

	private:
		const wchar_t *m_buffer;
	};
}

#endif