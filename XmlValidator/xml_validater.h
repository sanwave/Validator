
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
		/// ��У��XMLһ���ԣ������﷨���󷵻�-1
		/// </summary>
		/// <param name="content">ָ��Xml�ı�</param>
		/// <param name="error">������ϸ��Ϣ</param>
		/// <returns>��ʾһ���Դ�����Ŀ��-1��ʾ���﷨����</returns>
		int ValidateXml(const char * content, XmlValidateError& error)
		{
			std::stack<XmlValidateNode> nodes = std::stack<XmlValidateNode>();
			char * text = const_cast<char*>(content);

			//indexΪ׷�ٴ���ڵ㿪ʼ��ʶ��ȫ���е�λ�ã�index2Ϊ����ڵ������ʶ��ȫ���е�λ��
			off_t index4Open = 0, index4Close = 0;

			//xml�ļ�ͷ��ʼ�ı�־
			off_t head_prefix = strstr(text, "<?") - text;

			//��һ���ڵ㿪ʼ��ʶ'<'�����г��ֵ�λ��
			off_t prefix = strchr(text, '<') - text;

			//��һ���ڵ������ʶ"</"�����г��ֵ�λ��
			off_t close_prefix = strstr(text, "</") - text;

			//�ڵ�����β���ı�ʶ
			off_t suffix = strchr(text, '>') - text;

			while (prefix >= 0)
			{
				if (suffix - prefix - 2 < 0)
				{
					//�﷨����
					error.SetCount(-1);
					return -1;
				}
				else if (prefix == head_prefix)
				{
					//xml�ļ�ͷУ��
					head_prefix = NULL;
				}
				else if (*(text + suffix - 1) == '/')
				{
					;//<****/>�ڵ�ֱ�ӽ��������账��
				}
				else if (0 == (strncmp(text + prefix, "<!--", 4)))
				{
					//#����ע��
					suffix = strstr(text, "-->") - text + 2;
				}
				//��Ϊ�ڵ㿪ʼ��ʶ�����ڵ���ѹջ
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
				//��Ϊ�ڵ������ʶ���������ջ�Ľڵ����Ա�
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
						//׷�ٳ���ڵ������ʶ��λ��
						index4Close += prefix + 2;
						//ֻ��¼��һ�������λ��
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
		/// ��ȡָ���ڵ���
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
		/// ��ȡָ���ַ�������������ֵ
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
