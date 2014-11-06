
/*
*
*
*
*	XmlValidater Class		In Matrix
*
*	Created by Bonbon	2014.09.25
*
*/


#ifndef _XML_VALIDATER_H_
#define _XML_VALIDATER_H_

#include "common.h"
#include <stack>

namespace Matrix
{
	class XmlValidaterNode
	{
	public:
		int Index()
		{
			return m_index;
		}

		std::wstring Name()
		{
			return m_name;
		}

		XmlValidaterNode(int index, std::wstring name)
		{
			m_index = index;
			m_name = name;
		}

		int Close(std::wstring name)
		{
			return m_name == name ? 1 : -1;
		}

	private:
		int m_index;
		std::wstring m_name;
		std::wstring m_value;
	};

	class Position
	{
	public:
		int Line;
		int Row;
		int Index;
		Position() :Line(1), Row(1), Index(0)
		{
		}
	};

	class XmlValidateError
	{
	public:
		Position Open, Close;
		std::wstring OpenName, CloseName;
		int Count;
		XmlValidateError()
		{
			Open = Position();
			Close = Position();
			Count = 0;
			OpenName = CloseName = L"";
		}
	};

	class XMLValidater
	{
	public:
		XMLValidater(){}

		/// <summary>
		/// 简单校验XML一致性，部分语法错误返回-1
		/// </summary>
		/// <param name="content">指定Xml文本</param>
		/// <param name="error">错误详细消息</param>
		/// <returns>表示一致性错误数目，-1表示有语法错误</returns>
		int ValidateXml(const std::wstring& content, XmlValidateError& error)
		{
			std::stack<XmlValidaterNode> nodes = std::stack<XmlValidaterNode>();
			std::wstring text = content;
			//error = XmlValidateError();

			//index为追踪错误节点开始标识在全文中的位置，index2为错误节点结束标识在全文中的位置
			int index4Open = 0, index4Close = 0;			

			//xml文件头开始的标志
			int iHeadPrefix = text.find(L"<?");

			//下一个节点开始标识'<'在文中出现的位置
			int iPrefix = text.find(L"<");

			//下一个节点结束标识"</"在文中出现的位置
			int iClosePrefix = text.find(L"</");

			//节点名字尾部的标识
			int iSuffix = text.find('>');

			while (iPrefix >= 0)
			{
				if (iSuffix - iPrefix - 2 < 0)
				{
					//语法错误
					return error.Count = -1;
				}
				else if (iPrefix == iHeadPrefix)
				{
					//xml文件头校验
					iHeadPrefix = -1;
				}
				else if (text.at(iSuffix - 1) == '/')
				{
					;//<****/>节点直接结束，不予处理
				}
				else if (text.substr(iPrefix, 4) == L"<!--")
				{
					//#跳过注释
					iSuffix = text.find(L"-->") + 2;
				}
				//若为节点开始标识，将节点名压栈
				else if (iPrefix != iClosePrefix)
				{
					std::wstring strNodeName = GetNodeName(text.substr(iPrefix + 1, iSuffix - iPrefix - 1));
					nodes.push(XmlValidaterNode(index4Open + iPrefix + 1, strNodeName));
				}
				//若为节点结束标识，将其与出栈的节点名对比
				else
				{
					if (nodes.empty())
					{
						return error.Count = -1;
					}
					XmlValidaterNode node = nodes.top();
					nodes.pop();

					std::wstring strNodeCloseName = text.substr(iPrefix + 2, iSuffix - iPrefix - 2);
					if (node.Name() != strNodeCloseName)
					{
						//追踪出错节点结束标识的位置
						index4Close += iPrefix + 2;
						//只记录第一处错误的位置
						error.Count += 1;
						if (1 == error.Count)
						{
							GetPosition(content, node.Index(), error.Open);
							GetPosition(content, index4Close, error.Close);
							error.OpenName = node.Name();
							error.CloseName = strNodeCloseName;
						}
					}
				}
				text = text.substr(iSuffix);

				index4Open += iSuffix;
				index4Close = index4Open;

				iPrefix = text.find(L"<");
				iClosePrefix = text.find(L"</");
				iSuffix = text.substr(1).find('>') + 1;
			}
			return error.Count;
		}

	private:
		/// <summary>
		/// 获取指定节点名
		/// </summary>
		std::wstring GetNodeName(const std::wstring& node)
		{
			int i = node.find(' ');
			if (i > 0)
			{
				return node.substr(0, i);
			}
			else
			{
				return node;
			}
		}

		/// <summary>
		/// 获取指定字符串索引的行列值
		/// </summary>
		void GetPosition(const std::wstring& content, int index, Position& position)
		{
			position = Position();
			std::wstring text = content.substr(0, index);
			int i = text.find('\n');
			while (i >= 0)
			{
				++position.Line;
				text = text.substr(i + 1);
				i = text.find(L"\n");
			}

			position.Row += text.length();
			position.Index = index;
		}
	};

}

#endif
