
/*
*
*
*
*	Xml Class		In Matrix
*
*	Created by Bonbon	2014.09.25
*
*	Updated by Bonbon	2014.10.31
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
#define Char wchar_t
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
			: m_name(const_cast<wchar_t*>(L""))
			, m_value(const_cast<wchar_t*>(L""))
		{
		}
		
		~XmlBase()
		{
			if ( m_name != NULL)
			{
				delete m_name;
			}
			if ( m_value != NULL)
			{
				delete m_value;
			}
		}

		const wchar_t* Name() const
		{
			return m_name;
		}

		void SetName(const wchar_t* name)
		{
			if ( NULL != m_name)
			{
				delete[] m_name;
			}
			if (NULL != name)
			{
				m_name = new wchar_t[wcslen(name) + 1];
				wcscpy(m_name, name);
			}
			else
			{
				m_name = NULL;
			}
		}

		const wchar_t*	String() const
		{
			return m_value;
		}

		void SetString(const wchar_t* value)
		{
			if (m_value != NULL)
			{
				delete[] m_value;
			}
			if (NULL != value)
			{
				m_value = new wchar_t[wcslen(value) + 1];
				wcscpy(m_value, value);
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

		void AssignString(wchar_t * &str, wchar_t * buffer, size_t length, bool transferCharacter)
		{
			const wchar_t * found = NULL;
			if (!transferCharacter || (found = wmemchr(buffer, L'&', length)) == NULL)
			{
				str = buffer;
				str[length] = 0;
				return;
			}
			std::wstring temp;
			for (; found != NULL; found = wmemchr(buffer, L'&', length))
			{
				temp.append(buffer, found - buffer);
				length -= (found - buffer + 1);
				buffer = const_cast<wchar_t *>(found + 1);
				if (length >= 5)
				{
					if (wcsncmp(buffer, L"quot", 4) == 0)
					{
						buffer += 4;
						length -= 4;
						temp.append(1, L'\"');
						continue;
					}
					if (wcsncmp(found + 1, L"apos", 4) == 0)
					{
						buffer += 4;
						length -= 4;
						temp.append(1, L'\'');
						continue;
					}
				}
				if (length >= 4)
				{
					if (wcsncmp(buffer, L"amp", 3) == 0)
					{
						buffer += 3;
						length -= 3;
						temp.append(1, L'&');
						continue;
					}
				}
				if (length >= 3)
				{
					if (wcsncmp(buffer, L"lt", 2) == 0)
					{
						buffer += 2;
						length -= 2;
						temp.append(1, L'<');
						continue;
					}
					else if (wcsncmp(buffer, L"gt", 2) == 0)
					{
						buffer += 2;
						length -= 2;
						temp.append(1, L'>');
						continue;
					}
				}
				temp.append(1, L'&');
			}
			temp.append(buffer, length);
			size_t actualLength = temp.length();
			memcpy(str, temp.c_str(), sizeof(wchar_t) * actualLength);
			str[actualLength] = 0;
		}

	protected:
		wchar_t*	m_name;
		wchar_t*	m_value;
	};

	enum NodeType
	{
		DOCUMENT = 0,
		ELEMENT,
		COMMENT,
		DECLARATION
	};

	class XmlAttribute : public XmlBase
	{
	};

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
			return (!HasChild() && (m_value == NULL || m_value[0] == 0));
		}

		XmlNode* Parent() const
		{
			return m_parent;
		}

		bool HasChild() const
		{
			return !m_children.empty();
		}

		XmlNode* getFirstChild(NodeIterator& iter) const
		{
			iter = m_children.begin();
			if (iter != m_children.end())
			{
				return *iter;
			}
			return NULL;
		}

		XmlNode* getChild(NodeIterator iter) const
		{
			if (iter != m_children.end())
			{
				return *iter;
			}
			return NULL;
		}

		size_t ChildCount() const
		{
			return m_children.size();
		}
		
		size_t ChildCount(const wchar_t * name) const
		{
			size_t count = 0;
			for (NodeIterator iter = m_children.begin();
			iter != m_children.end();
				++iter)
			{
				XmlNode* child = *iter;
				if (wcscmp(child->m_name, name) == 0)
				{
					++count;
				}
			}
			return count;
		}

		void RemoveChild(XmlNode* node)
		{
			for (NodeList::iterator iter = m_children.begin();
			iter != m_children.end();
				++iter)
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
			for (NodeList::iterator iter = m_children.begin();
			iter != m_children.end();
				++iter)
			{
				XmlNode* child = *iter;
				delete child;
			}
			m_children.clear();
		}

		XmlNode* AddChild(const wchar_t * name = NULL, NodeType type = ELEMENT)
		{
			if (type != COMMENT && type != ELEMENT)
			{
				return NULL;
			}
			XmlNode* child = new XmlNode(type, this);
			if (name != NULL)
			{
				child->SetName(name);
			}
			m_children.push_back(child);
			return child;
		}

		bool HasAttribute() const
		{

		}

		XmlAttribute* Attribute(const wchar_t* name) const
		{
			for (AttributeList::const_iterator iter = m_attributes.begin();
			iter != m_attributes.end();
				++iter)
			{
				XmlAttribute* attribute = *iter;
				if (wcscmp(attribute->Name(), name) == 0)
				{
					return attribute;
				}
			}
			return NULL;
		}

		XmlAttribute* getFirstAttribute(AttributeIterator& iter) const
		{
			iter = m_attributes.begin();
			if (iter != m_attributes.end())
			{
				return *iter;
			}
			return NULL;
		}

		XmlAttribute* getNextAttribute(AttributeIterator& iter) const
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
			for (AttributeList::iterator iter = m_attributes.begin();
			iter != m_attributes.end();
				++iter)
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
			for (AttributeList::iterator iter = m_attributes.begin();
			iter != m_attributes.end();
				++iter)
			{
				delete *iter;
			}
			m_attributes.clear();
		}

		XmlAttribute* AddAttribute(const wchar_t * name = NULL, const wchar_t * value = NULL)
		{
			XmlAttribute* attribute = new XmlAttribute;
			if (name != NULL)
			{
				attribute->SetName(name);
			}
			if (value != NULL)
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
			, m_buffer(NULL)
		{
		}

		~XmlDocument()
		{
			if (m_buffer != NULL)
			{
				delete[] m_buffer;
			}
		}

		bool LoadFromFile(const wchar_t* filename)
		{
			//wchar_t * filename = const_cast<wchar_t *>(filename);
			Matrix::File file(filename);
			size_t file_len = 0;
			const wchar_t * text = file.Text(0, &file_len);

			return Parse(const_cast<wchar_t *>(text), file_len);
		}

		bool Parse(wchar_t* input, size_t size)
		{
			wchar_t * cur = input;
			wchar_t * end = input + size;

			wchar_t * label = NULL;
			size_t labelSize = 0;
			int depth = 0;
			XmlNode* currentNode = this;

			while (cur < end)
			{
				wchar_t * lastPos = cur;
				if (!FindLabel(cur, end - cur, label, labelSize))
				{
					break;
				}
				switch (*label)
				{
				case L'/':	//node ending
					if (depth < 1)
					{
						return false;
					}
					if (currentNode->Type() == ELEMENT && !currentNode->HasChild())
					{
						currentNode->AssignString(currentNode->m_value, lastPos, label - lastPos - 1, true);
					}
					currentNode = currentNode->Parent();
					--depth;
					break;
				case L'?':	//xml define node, ignore
					break;
				case L'!':	//comment node
				{
					//ignore !-- and --
					if (labelSize < 5)
					{
						return false;
					}
					XmlNode* comment = currentNode->AddChild(NULL, COMMENT);
					comment->AssignString(comment->m_name, label + 3, labelSize - 5, false);
				}
					break;
				default:	//node start
				{
					XmlNode* newNode = currentNode->AddChild(NULL, ELEMENT);

					bool emptyNode = ParseLabel(newNode, label, labelSize);

					if (!emptyNode)
					{
						currentNode = newNode;
						++depth;
					}
				}
				}
			} // while(cur < end)

			if (depth != 0)
			{
				return false;
			}
			return true;
		}


	private:

		bool FindLabel(wchar_t* &begin, size_t size, wchar_t* &label, size_t &labelSize)
		{
			label = (wchar_t*)wmemchr(begin, L'<', size);
			if (label == NULL)
			{
				return false;
			}
			++label;
			size -= (label - begin);

			//comment is special, won't end without "-->"
			if (size > 6 //Strlen(T("!---->"))
				&& label[0] == L'!'
				&& label[1] == L'-'
				&& label[2] == L'-')
			{
				//buffer is not NULL-terminated, so we can't use strstr, shit! is there a "safe" version of strstr?
				wchar_t* cur = label + 3;	//skip !--
				size -= 5; //(Strlen(T("!---->")) - 1);
				while (true)
				{
					wchar_t* end = (wchar_t*)wmemchr(cur, L'-', size);
					if (end == NULL)
					{
						return false;
					}
					if (*(end + 1) == L'-' && *(end + 2) == L'>')
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
			begin = (wchar_t*)wmemchr(label, L'>', size);
			if (begin == NULL)
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

		bool ParseLabel(XmlNode* node, wchar_t* label, size_t labelSize)
		{
			wchar_t* cur = label;
			while (*cur != L' ' && *cur != L'/' && *cur != L'>')
			{
				++cur;
			}
			wchar_t next = *cur;
			node->AssignString(node->m_name, label, cur - label, true);
			if (next != L' ')
			{
				return next == L'/';
			}
			//get attributes
			wchar_t* end = label + labelSize;
			++cur;
			while (cur < end)
			{
				while (*cur == L' ')
				{
					++cur;
				}
				//attribute name
				wchar_t* attrName = cur;
				while (*cur != L' ' && *cur != L'=' && *cur != L'/' && *cur != L'>')
				{
					++cur;
				}
				next = *cur;
				size_t attrNameSize = cur - attrName;

				//attribute value
				cur = (wchar_t*)wmemchr(cur, L'"', end - cur);
				if (cur == NULL)
				{
					break;
				}
				wchar_t* attrValue = ++cur;
				cur = (wchar_t*)wmemchr(cur, L'"', end - cur);
				if (NULL == cur)
				{
					return false;
				}
				size_t attrValueSize = cur - attrValue;
				XmlAttribute* attribute = node->AddAttribute();
				attribute->AssignString(attribute->m_name, attrName, attrNameSize, true);
				attribute->AssignString(attribute->m_value, attrValue, attrValueSize, true);
				++cur;
			}
			return next == L'/';
		}

	private:
		char*	m_buffer;
	};

}

#endif 
