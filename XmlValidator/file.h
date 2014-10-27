
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
			int encoding = Matrix::TextEncoder::DetectEncode(content);
			if (encoding == CP_UTF8)
			{
				return Matrix::TextEncoder::Utf8ToUnicode(content.c_str());
			}
			else
			{
				return Matrix::TextEncoder::AnsiToUnicode(content.c_str());
			}
		}
		

	private:

	};
}

#endif