
/*
*
*
*
*	XmlValidater Class		In Matrix
*
*	Created by Bonbon	2014.09.25
*
*	Updated by Bonbon	2014.11.13
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
            SetName(name);
		}

        XmlValidateNode(const XmlValidateNode& src)
			:m_name(NULL)
			, m_value(NULL)
        {
            m_index = src.Index();
            SetName(src.Name());
			SetValue(src.Value());
        }

		~XmlValidateNode()
		{
			if (NULL != m_name)
			{
				delete m_name;
				m_name = NULL;
			}
			if (NULL != m_value)
			{
				delete m_value;
				m_value = NULL;
			}
		}

		int Index() const
		{
			return m_index;
		}

        int SetIndex(int value)
        {
            m_index = value;
        }

		const char * Name() const
		{
			return m_name;
		}

        void SetName(const char * name)
        {
            if (NULL != m_name)
            {
                delete m_name;
                m_name = NULL;
            }
            if (NULL != name)
            {
                int len = strlen(name);
                m_name = new char[len + 1];
                strncpy(m_name, name, len + 1);
            }
            else
            {
                m_name = NULL;
            }
        }

		const char * Value() const
		{
			return m_value;
		}

		void SetValue(const char * value)
		{
			if (NULL != m_value)
			{
				delete m_value;
				m_value = NULL;
			}
			if (NULL != value)
			{
				int len = strlen(value);
				m_value = new char[len * 2 + 1];
				strncpy(m_value, value, len + 1);
			}
			else
			{
				m_value = NULL;
			}
		}

		int Close(const char * name)
		{
			return strcmp(m_name, name) == 0 ? 1 : -1;
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
		Position() 
			:Line(1)
			, Row(1)
			, Index(0)
		{}
	};

	class XmlValidateError
	{
	public:
		Position Open, Close;
		
		XmlValidateError() 
			:m_openname(NULL)
			, m_closename(NULL)
		{
			Open = Position();
			Close = Position();
			m_count = 0;
		}

		XmlValidateError(const XmlValidateError& src)
			:m_openname(NULL)
			, m_closename(NULL)
		{
			Open = src.Open;
			Close = src.Close;
			m_count = src.Count();
			SetOpenName(src.OpenName());
			SetCloseName(src.CloseName());
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

		int Count() const
		{
			return m_count;
		}

		void SetCount(int value)
		{
			m_count = value;
		}

		char * OpenName() const
		{
			return m_openname;
		}

		char * CloseName() const
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

	private:
		char * m_openname, *m_closename;
		int m_count;
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
			off_t head_prefix = strstr(text, "<?") - text;

			//下一个节点开始标识'<'在文中出现的位置
			off_t prefix = strchr(text, '<') - text;

			//下一个节点结束标识"</"在文中出现的位置
			off_t close_prefix = strstr(text, "</") - text;

			//节点名字尾部的标识
			off_t suffix = strchr(text, '>') - text;

			while (prefix >= 0)
			{
				if (suffix - prefix - 2 < 0)
				{
					//语法错误
					error.SetCount(-1);
					return -1;
				}
				else if (prefix == head_prefix)
				{
					//xml文件头校验
					head_prefix = NULL;
				}
				else if (*(text + suffix - 1) == '/')
				{
					;//<****/>节点直接结束，不予处理
				}
				else if (0 == (strncmp(text + prefix, "<!--", 4)))
				{
					//#跳过注释
					suffix = strstr(text, "-->") - text + 2;
				}
				//若为节点开始标识，将节点名压栈
				else if (prefix != close_prefix)
				{
					const char * node_label = Substr(text, prefix + 1, suffix - prefix - 1);
					const char * node_name = GetNodeName(node_label);
					XmlValidateNode node = XmlValidateNode(index4Open + prefix + 1, node_name);					
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
						error.SetCount(-1);
						return -1;
					}
					XmlValidateNode node = nodes.top();
					nodes.pop();

					const char * close_label = Substr(text, prefix + 2, suffix - prefix - 2);
					const char * close_name = GetNodeName(close_label);
					if (0 != strcmp(node.Name(), close_name))
					{
						//追踪出错节点结束标识的位置
						index4Close += prefix + 2;
						//只记录第一处错误的位置
						error.SetCount(error.Count() + 1);
						if (1 == error.Count())
						{
							GetPosition(content, node.Index(), error.Open);
							GetPosition(content, index4Close, error.Close);
							error.SetOpenName(node.Name());
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
				}
				text += suffix;

				index4Open += suffix;
				index4Close = index4Open;

				prefix = strchr(text, '<') - text;
				close_prefix = strstr(text, "</") - text;
				suffix = strchr(text + 1, '>') - text;
			}
			return error.Count();
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
				return Substr(node, 0, i);
			}
			else
			{
				int len = strlen(node);
				char * name = new char[len + 1];
				strncpy(name, node, len + 1);
				return name;
			}
		}

		char * Substr(const char * src, off_t off, size_t len)
		{
			char * str = new char[len + 1];
			strncpy(str, src + off, len);
			str[len] = '\0';
			return str;
		}

		/// <summary>
		/// 获取指定字符串索引的行列值
		/// </summary>
		void GetPosition(const char * content, int index, Position& position)
		{
			char * subcontent = Substr(content, 0, index);
            char * text = subcontent;
            char * line_wrap = strchr(text, '\n');
            while (NULL != line_wrap)
            {
                ++position.Line;
                text = line_wrap + 1;
                line_wrap = strchr(text, '\n');
            }

            position.Row += strlen(text);
			position.Index = index;

            if (NULL != subcontent)
			{
                delete subcontent;
                subcontent = NULL;
			}
		}
	};

}

#endif
