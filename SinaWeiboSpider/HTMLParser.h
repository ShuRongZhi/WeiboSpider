#pragma once
#include <map>
#include <vector>
#include "Tools.h"

// CHTMLParser
//HTML������
class CHTMLParser
{
public:
	CHTMLParser();
	virtual ~CHTMLParser();
	//TODO:��CString�л��HTMLԴ���룬����һЩ��ʼ������
	bool ReadHTML(CString&);
	//TODO:������Դ���ͷ��ڴ�
	bool Destory();
	//TODO:����HTML
	bool ParserHTML();
	//TODO:���������ǩ����  ����1ָ��Ҫ�ı�ǩ
	std::vector<CString> GetTag(CString);
	//TODO:��ñ�ǩ�µ�����
	CString GetText(CString);
private:
	Tools m_Tools;
	//���HTML
	CString m_HTMLCode;
	//���head�ڵ�
	CString m_HeadNode;
	//���body�ڵ�
	CString m_BodyNode;
	//����û���Ϣ�Ľڵ�
	CString m_UserInfoNode;
	//���div�ڵ�
	std::multimap<CString,CString> m_DivNode;
};


