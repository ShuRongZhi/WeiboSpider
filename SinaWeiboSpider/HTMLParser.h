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
	//TODO:���head�ڵ� ʧ�ܷ���NULL
	CString GetHeadNode();
	//���body�ڵ� ʧ�ܷ���NULL
	CString GetBodyNode();
	//TODO:���������ǩ����  ����1ָ��Ҫ�ı�ǩ
	std::vector<CString> GetTag(CString);
	//TODO:��ñ�ǩ�µ�����
	CString GetText(CString);
private:
	Tools m_Tools;
	CString m_HTMLCode;
};


