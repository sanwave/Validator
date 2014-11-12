
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

#ifndef MATRIX
#include <iostream>
#include <string>
#include <stack>
#else
#include "common.h"
#endif

namespace Matrix
{
	class XmlValidateNode
	{
	public:
		XmlValidateNode()
			:m_name(NULL)
			, m_value(NULL)
		{}

		XmlValidateNode(int index, const char * name)
			:m_name(NULL)
			, m_value(NULL)
		{
			m_index = index;
			if (NULL != m_name)
			{
				delete m_name;
				m_name = NULL;
			}
			if (NULL != name)
			{
				int len = strlen(name);
				m_name = new char[len*2 + 1];
				strncpy(m_name, name, len + 1);
			}
			else
			{
				m_name = NULL;
			}
		}

		~XmlValidateNode()
		{
			if (NULL != m_name)
			{
				//delete m_name;
				m_name = NULL;
			}
			if (NULL != m_value)
			{
				delete m_value;
				m_value = NULL;
			}
		}

		int Index()
		{
			return m_index;
		}

		std::string Name()
		{
			return std::string(m_name);
		}

		int Close(const char * name)
		{
			return std::string(m_name) == std::string(name) ? 1 : -1;
			//return strcmp(m_name, name) == 0 ? 1 : -1;
		}

	private:
		int m_index;
		char * m_name;
		char * m_value;
	};

	class Position
	{
	public:
		int Line;
		int Row;
		int Index;
		Position() :Line(1), Row(1), Index(0)
		{}
	};

	class XmlValidateError
	{
	public:
		Position Open, Close;
		char * m_openname, *m_closename;
		int Count;
		XmlValidateError() 
			:m_openname(NULL)
			, m_closename(NULL)
		{
			Open = Position();
			Close = Position();
			Count = 0;
		}

		~XmlValidateError()
		{
			if (NULL != m_openname)
			{
				delete m_openname;
				m_openname = NULL;
			}
			if (NULL != m_closename)
			{
				delete m_closename;
				m_closename = NULL;
			}
		}

		char * OpenName()
		{
			return m_openname;
		}

		char * CloseName()
		{
			return m_closename;
		}

		void SetOpenName(const char * name)
		{
			if (NULL != m_openname)
			{
				delete m_openname;
			}
			if (NULL != name)
			{
				int len = strlen(name);
				m_openname = new char[len + 1];
				strncpy(m_openname, name, len+1);
			}
			else
			{
				m_openname = NULL;
			}
		}

		void SetCloseName(const char * name)
		{
			if (NULL != m_closename)
			{
				delete m_closename;
			}
			if (NULL != name)
			{
				int len = strlen(name);
				m_closename = new char[len + 1];
				strncpy(m_closename, name, len+1);
			}
			else
			{
				m_closename = NULL;
			}
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
		int ValidateXml(const char * content, XmlValidateError& error)
		{
			std::stack<XmlValidateNode> nodes = std::stack<XmlValidateNode>();
			char * text = const_cast<char*>(content);

			//index为追踪错误节点开始标识在全文中的位置，index2为错误节点结束标识在全文中的位置
			off_t index4Open = 0, index4Close = 0;

			//xml文件头开始的标志
			off_t iHeadPrefix = strstr(text, "<?")-text;

			//下一个节点开始标识'<'在文中出现的位置
			off_t iPrefix = strchr(text, '<') - text;

			//下一个节点结束标识"</"在文中出现的位置
			off_t iClosePrefix = strstr(text, "</")-text;

			//节点名字尾部的标识
			off_t iSuffix = strchr(text, '>') - text;

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
					iHeadPrefix = NULL;
				}
				else if (*(text + iSuffix - 1) == '/')
				{
					;//<****/>节点直接结束，不予处理
				}
				else if (0 == (strncmp(text + iPrefix, "<!--", 4)))
				{
					//#跳过注释
					iSuffix = strstr(text, "-->") - text + 2;
				}
				//若为节点开始标识，将节点名压栈
				else if (iPrefix != iClosePrefix)
				{
					const char * node_label = substr(text, iPrefix + 1, iSuffix - iPrefix - 1);
					const char * node_name = GetNodeName(node_label);
					XmlValidateNode node(index4Open + iPrefix + 1, node_name);
					nodes.push(node);
					if (NULL != node_label)
					{
						delete node_label;
						node_label = NULL;
					}
					if (node_label != node_name)
					{
						delete node_name;
						node_name = NULL;
					}
				}
				//若为节点结束标识，将其与出栈的节点名对比
				else
				{
					if (nodes.empty())
					{
						return error.Count = -1;
					}
					XmlValidateNode node = nodes.top();
					

					const char * close_label = substr(text, iPrefix + 2, iSuffix - iPrefix - 2);
					const char * close_name = GetNodeName(close_label);
					if (0!=node.Name().compare(close_name))
					{
						//追踪出错节点结束标识的位置
						index4Close += iPrefix + 2;
						//只记录第一处错误的位置
						error.Count += 1;
						if (1 == error.Count)
						{
							GetPosition(content, node.Index(), error.Open);
							GetPosition(content, index4Close, error.Close);
							error.SetOpenName(node.Name().c_str());
							error.SetCloseName(close_name);
						}
					}

					if (NULL != close_label)
					{
						delete close_label;						
						close_label = NULL;
					}
					if (close_label != close_name)
					{
						delete close_name;
						close_name = NULL;
					}

					nodes.pop();
				}
				text += iSuffix;

				index4Open += iSuffix;
				index4Close = index4Open;

				iPrefix = strchr(text, '<') - text;
				iClosePrefix = strstr(text, "</") - text;
				iSuffix = strchr(text+ 1, '>') - text;
			}
			return error.Count;
		}

	private:
		/// <summary>
		/// 获取指定节点名
		/// </summary>
		const char * GetNodeName(const char * node)
		{
			int i = strchr(node, ' ') - node;
			if (i > 0)
			{
				return substr(node, 0, i);
			}
			else
			{
				int len = strlen(node);
				char * name = new char[len + 1];
				strncpy(name, node, len + 1);
				return name;
			}
		}

		char * substr(const char * src, off_t off, size_t len)
		{
			char * str = new char[len + 1];
			strncpy(str, src + off, len);
			str[len] = 0;
			return str;
		}

		/// <summary>
		/// 获取指定字符串索引的行列值
		/// </summary>
		void GetPosition(const char * content, int index, Position& position)
		{
			position = Position();
			char * text = substr(content, 0, index);
			int i = strchr(text, '\n') - text;
			while (i >= 0)
			{
				++position.Line;
				text += i + 1;
				i = strchr(text, '\n') - text;
			}

			position.Row += strlen(text);
			position.Index = index;

			if (NULL != text)
			{
				delete text;
				text = NULL;
			}
		}
	};

}

#endif
