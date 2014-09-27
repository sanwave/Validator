
/*
*
*
*
*	Xml Class		In Matrix
*
*	Created by Bonbon	2014.09.25
*
*/


#ifndef _MXML_H_
#define _MXML_H_

#include "Common.h"
#include <stack>

namespace Matrix
{
	class XmlNode
	{
	public:
		int Index;
		std::wstring Name;

		XmlNode(int index, std::wstring name)
		{
			Index = index;
			Name = name;
		}

		int Close(std::wstring name)
		{
			return Name == name ? 1 : -1;
		}
	};

	class Position
	{
	public:
		int Line;
		int Row;
		Position()
		{
			Line = Row = 1;
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

	class XML
	{
	public:
		XML(){}

		/// <summary>
		/// ��У��XMLһ���ԣ������﷨���󷵻�-1
		/// </summary>
		/// <param name="content">ָ��Xml�ı�</param>
		/// <param name="error">������ϸ��Ϣ</param>
		/// <returns>��ʾһ���Դ�����Ŀ��-1��ʾ���﷨����</returns>
		int XML::ValidateXml(const std::wstring& content, XmlValidateError& error)
		{
			std::stack<XmlNode> nodes = std::stack<XmlNode>();
			std::wstring text = content;
			//error = XmlValidateError();

			//indexΪ׷�ٴ���ڵ㿪ʼ��ʶ��ȫ���е�λ�ã�index2Ϊ����ڵ������ʶ��ȫ���е�λ��
			int index4Open = 0, index4Close = 0;			

			//xml�ļ�ͷ��ʼ�ı�־
			int iHeadPrefix = text.find(L"<?");

			//��һ���ڵ㿪ʼ��ʶ'<'�����г��ֵ�λ��
			int iPrefix = text.find(L"<");

			//��һ���ڵ������ʶ"</"�����г��ֵ�λ��
			int iClosePrefix = text.find(L"</");

			//�ڵ�����β���ı�ʶ
			int iSuffix = text.find('>');

			while (iPrefix >= 0)
			{
				if (iSuffix - iPrefix - 2 < 0)
				{
					//�﷨����
					return error.Count = -1;
				}
				else if (iPrefix == iHeadPrefix)
				{
					//xml�ļ�ͷУ��
					iHeadPrefix = -1;
				}
				else if (text.at(iSuffix - 1) == '/')
				{
					;//<****/>�ڵ�ֱ�ӽ��������账��
				}
				//��Ϊ�ڵ㿪ʼ��ʶ�����ڵ���ѹջ
				else if (iPrefix != iClosePrefix)
				{
					std::wstring strNodeName = GetNodeName(text.substr(iPrefix + 1, iSuffix - iPrefix - 1));
					nodes.push(XmlNode(index4Open + iPrefix + 1, strNodeName));
				}
				//��Ϊ�ڵ������ʶ���������ջ�Ľڵ����Ա�
				else
				{
					if (nodes.empty())
					{
						return error.Count = -1;
					}
					XmlNode node = nodes.top();
					nodes.pop();

					std::wstring strNodeCloseName = text.substr(iPrefix + 2, iSuffix - iPrefix - 2);
					if (node.Name != strNodeCloseName)
					{
						//׷�ٳ���ڵ������ʶ��λ��
						index4Close += iPrefix + 2;
						//ֻ��¼��һ�������λ��
						error.Count += 1;
						if (1 == error.Count)
						{
							GetPosition(content, node.Index, error.Open);
							GetPosition(content, index4Close, error.Close);
							error.OpenName = node.Name;
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
		/// ��ȡָ���ڵ���
		/// </summary>
		std::wstring XML::GetNodeName(const std::wstring& node)
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
		/// ��ȡָ���ַ�������������ֵ
		/// </summary>
		void XML::GetPosition(const std::wstring& content, int index, Position& position)
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
		}
	};

}


#endif