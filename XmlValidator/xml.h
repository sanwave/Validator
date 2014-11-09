
/*
*
*
*
*	Xml Class		In Matrix
*
*	Created by Bonbon	2014.11.05
*
*	Updated by Bonbon	2014.11.06
*
*/


#ifndef _XML_H_
#define _XML_H_

#ifndef MATRIX
#include <iostream>
#include <string>
#include <list>
#include <istream>
#include <algorithm>
#else
#include "common.h"
#endif

#include "file.h"
/*
#ifdef UNICODE
#define Char char
//#define String std::wstring
#else
#define Char char
#define String std::string
#endif*/

namespace Matrix
{

	class XmlAttribute;
	class XmlNode;

	typedef std::list<XmlAttribute*> AttributeList;
	typedef std::list<XmlNode*> NodeList;

	typedef AttributeList::const_iterator AttributeIterator;
	typedef NodeList::const_iterator NodeIterator;

	class XmlBase
	{
		friend class XmlDocument;
	public:
		XmlBase()
			: m_name(NULL)
			, m_value(NULL)
		{
		}
		
		~XmlBase()
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

		const char* Name() const
		{
			return m_name;
		}

		void SetName(const char* name)
		{
			if ( NULL != m_name)
			{
				delete[] m_name;
			}
			if (NULL != name)
			{
				size_t ulen = strlen(name);
				m_name = new char[ulen + 1];
				strncpy(m_name, name, ulen+1);
			}
			else
			{
				m_name = NULL;
			}
		}

		const char*	String() const
		{
			return m_value;
		}

		void SetString(const char* value)
		{
			if (NULL != m_value)
			{
				delete[] m_value;
			}
			if (NULL != value)
			{
				size_t ulen = strlen(value);
				m_value = new char[ulen + 1];
				strncpy(m_value, value, ulen);
			}
			else
			{
				m_value = NULL;
			}
		}

		void SetString(const std::wstring& value)
		{
			SetString(value.c_str());
		}

		void AssignString(char * &str, char * buffer, size_t length, bool transferCharacter)
		{
			const char * found = NULL;
			if (!transferCharacter || (found = (char *)memchr(buffer, '&', length)) == NULL)
			{
				if (NULL != str)
				{
					delete str;
				}
				if (NULL != buffer)
				{
					int ulen = strlen(buffer);
					str = new char[ulen + 1];
					strncpy(str, buffer, ulen + 1);
				}
				else
				{
					str = NULL;
				}
				return;
			}
			std::string temp;
			for (; NULL != found; found = (char *)memchr(buffer, '&', length))
			{
				temp.append(buffer, found - buffer);
				length -= (found - buffer + 1);
				buffer = const_cast<char *>(found + 1);
				if (length >= 5)
				{
					if (strncmp(buffer, "quot", 4) == 0)
					{
						buffer += 4;
						length -= 4;
						temp.append(1, '\"');
						continue;
					}
					if (strncmp(found + 1, "apos", 4) == 0)
					{
						buffer += 4;
						length -= 4;
						temp.append(1, '\'');
						continue;
					}
				}
				if (length >= 4)
				{
					if (strncmp(buffer, "amp", 3) == 0)
					{
						buffer += 3;
						length -= 3;
						temp.append(1, '&');
						continue;
					}
				}
				if (length >= 3)
				{
					if (strncmp(buffer, "lt", 2) == 0)
					{
						buffer += 2;
						length -= 2;
						temp.append(1, '<');
						continue;
					}
					else if (strncmp(buffer, "gt", 2) == 0)
					{
						buffer += 2;
						length -= 2;
						temp.append(1, '>');
						continue;
					}
				}
				temp.append(1, '&');
			}
			temp.append(buffer, length);
			if (NULL != str)
			{
				delete str;
			}
			if (!temp.empty())
			{
				int ulen = temp.length();
				str = new char[ulen + 1];
				strncpy(str, temp.c_str(), ulen + 1);
			}
			else
			{
				str = NULL;
			}
		}

	protected:
		char*	m_name;
		char*	m_value;
	};

	enum NodeType
	{
		DOCUMENT = 0,
		ELEMENT,
		COMMENT,
		DECLARATION
	};

	class XmlAttribute : public XmlBase
	{};

	class XmlNode : public XmlBase
	{
	public:
		XmlNode(NodeType type, XmlNode* parent)
			: m_type(type)
			, m_parent(parent)
		{
		}

		~XmlNode()
		{
			ClearAttribute();
			ClearChild();
		}

	public:
		NodeType Type() const
		{
			return m_type;
		}

		bool Empty() const
		{
			return (!HasChild() && (NULL == m_value));
		}

		XmlNode* Parent() const
		{
			return m_parent;
		}

		bool HasChild() const
		{
			return !m_children.empty();
		}

		XmlNode* FirstChild(NodeIterator& iter) const
		{
			iter = m_children.begin();
			if (iter != m_children.end())
			{
				return *iter;
			}
			return NULL;
		}

		XmlNode* NextChild(NodeIterator iter) const
		{
			if (iter != m_children.end())
			{
				++iter;
				if (iter != m_children.end())
				{
					return *iter;
				}
			}
			return NULL;
		}

		size_t ChildCount() const
		{
			return m_children.size();
		}
		
		size_t ChildCount(const char * name) const
		{
			size_t count = 0;
			for (NodeIterator iter = m_children.begin(); iter != m_children.end(); ++iter)
			{
				XmlNode* child = *iter;
				if (strcmp(child->m_name, name) == 0)
				{
					++count;
				}
			}
			return count;
		}

		void RemoveChild(XmlNode* node)
		{
			for (NodeList::iterator iter = m_children.begin(); iter != m_children.end(); ++iter)
			{
				if (*iter == node)
				{
					delete node;
					m_children.erase(iter);
					return;
				}
			}
		}

		void ClearChild()
		{
			NodeList::iterator iter = m_children.begin();
			for (; iter != m_children.end(); ++iter)
			{
				delete *iter;
			}
			m_children.clear();
		}

		XmlNode* AddChild(const char * name = NULL, NodeType type = ELEMENT)
		{
			if (type != COMMENT && type != ELEMENT)
			{
				return NULL;
			}
			XmlNode* child = new XmlNode(type, this);
			if (NULL != name)
			{
				child->SetName(name);
			}
			m_children.push_back(child);
			return child;
		}

		bool HasAttribute() const
		{
			return !m_attributes.empty();
		}

		XmlAttribute* Attribute(const char* name) const
		{
			for (AttributeList::const_iterator iter = m_attributes.begin(); iter != m_attributes.end(); ++iter)
			{
				XmlAttribute* attribute = *iter;
				if (strcmp(attribute->Name(), name) == 0)
				{
					return attribute;
				}
			}
			return NULL;
		}

		XmlAttribute* FirstAttribute(AttributeIterator& iter) const
		{
			iter = m_attributes.begin();
			if (iter != m_attributes.end())
			{
				return *iter;
			}
			return NULL;
		}

		XmlAttribute* NextAttribute(AttributeIterator& iter) const
		{
			if (iter != m_attributes.end())
			{
				++iter;
				if (iter != m_attributes.end())
				{
					return *iter;
				}
			}
			return NULL;
		}

		void RemoveAttribute(XmlAttribute* attribute)
		{
			for (AttributeList::iterator iter = m_attributes.begin(); iter != m_attributes.end(); ++iter)
			{
				if (*iter == attribute)
				{
					delete attribute;
					m_attributes.erase(iter);
					return;
				}
			}
		}

		void ClearAttribute()
		{
			AttributeList::iterator iter = m_attributes.begin();
			for (; iter != m_attributes.end(); ++iter)
			{
				delete *iter;
			}
			m_attributes.clear();
		}

		XmlAttribute* AddAttribute(const char * name = NULL, const char * value = NULL)
		{
			XmlAttribute* attribute = new XmlAttribute;
			if (NULL != name)
			{
				attribute->SetName(name);
			}
			if (NULL != value)
			{
				attribute->SetString(value);
			}
			m_attributes.push_back(attribute);
			return attribute;
		}

	private:
		NodeType		m_type;
		AttributeList	m_attributes;
		XmlNode*		m_parent;
		NodeList		m_children;
	};

	class XmlDocument : public XmlNode
	{
	public:
		XmlDocument()
			: XmlNode(DOCUMENT, NULL)
		{
		}

		~XmlDocument()
		{
		}

		bool LoadFromFile(const char* filename)
		{
			Matrix::File file(filename);
			size_t file_len = 0;
			const char * text = file.Utf8Text(0, &file_len);

			bool res = Parse(const_cast<char *>(text), file_len);
			if (NULL != text)
			{
				delete text;
			}
		}

		bool Parse(char* input, size_t size)
		{
			char * cur = input;
			char * end = input + size;

			char * label = NULL;
			size_t label_size = 0;
			int depth = 0;
			XmlNode* currentNode = this;

			while (cur < end)
			{
				char * last_pos = cur;
				if (!FindLabel(cur, end - cur, label, label_size))
				{
					break;
				}
				switch (*label)
				{
				case '/':	//node ending
					if (depth < 1)
					{
						return false;
					}
					if (currentNode->Type() == ELEMENT && !currentNode->HasChild())
					{
						currentNode->AssignString(currentNode->m_value, last_pos, label - last_pos - 1, true);
					}
					currentNode = currentNode->Parent();
					--depth;
					break;

				case '?':	//xml define node, ignore
					break;

				case '!':	//comment node
				{
					//ignore !-- and --
					if (label_size < 5)
					{
						return false;
					}
					XmlNode* comment = currentNode->AddChild(NULL, COMMENT);
					comment->AssignString(comment->m_name, label + 3, label_size - 5, false);
				}
					break;

				default:	//node start
				{
					XmlNode* new_node = currentNode->AddChild(NULL, ELEMENT);

					bool empty = ParseLabel(new_node, label, label_size);

					if (!empty)
					{
						currentNode = new_node;
						++depth;
					}
				}

				}
			} //end of while(cur < end)

			if (depth != 0)
			{
				return false;
			}
			return true;
		}


	private:

		bool FindLabel(char* &begin, size_t size, char* &label, size_t &labelSize)
		{
			label = (char*)memchr(begin, '<', size);
			if (NULL == label)
			{
				return false;
			}
			++label;
			size -= (label - begin);

			//comment is special, won't end without "-->"
			if (size > 6 //Strlen(T("!---->"))
				&& label[0] == '!'
				&& label[1] == '-'
				&& label[2] == '-')
			{
				//buffer is not NULL-terminated, so we can't use strstr, shit! is there a "safe" version of strstr?
				char* cur = label + 3;	//skip !--
				size -= 5; //(Strlen(T("!---->")) - 1);
				while (true)
				{
					char* end = (char*)memchr(cur, '-', size);
					if (NULL == end)
					{
						return false;
					}
					if (*(end + 1) == '-' && *(end + 2) == '>')
					{
						//get it
						labelSize = end - label + 2;
						begin = end + 3;
						return true;
					}
					size -= (end - cur + 1);
					cur = end + 1;
				}
			}
			begin = (char*)memchr(label, '>', size);
			if (NULL == begin)
			{
				return false;
			}
			labelSize = begin - label;
			++begin;
			if (labelSize == 0)
			{
				return false;
			}
			return true;
		}

		bool ParseLabel(XmlNode* node, char* label, size_t labelSize)
		{
			char* cur = label;
			while (*cur != ' ' && *cur != '/' && *cur != '>')
			{
				++cur;
			}
			char next = *cur;
			node->AssignString(node->m_name, label, cur - label, true);
			if (next != ' ')
			{
				return next == '/';
			}
			//get attributes
			char* end = label + labelSize;
			++cur;
			while (cur < end)
			{
				while (*cur == ' ')
				{
					++cur;
				}
				//attribute name
				char* attrName = cur;
				while (*cur != ' ' && *cur != '=' && *cur != '/' && *cur != '>')
				{
					++cur;
				}
				next = *cur;
				size_t attr_name_size = cur - attrName;

				//attribute value
				cur = (char*)memchr(cur, '"', end - cur);
				if (NULL == cur)
				{
					break;
				}
				char* attrValue = ++cur;
				cur = (char*)memchr(cur, '"', end - cur);
				if (NULL == cur)
				{
					return false;
				}
				size_t attr_value_size = cur - attrValue;
				XmlAttribute* attribute = node->AddAttribute();
				attribute->AssignString(attribute->m_name, attrName, attr_name_size, true);
				attribute->AssignString(attribute->m_value, attrValue, attr_value_size, true);
				++cur;
			}
			return next == '/';
		}

	};

}

#endif 
