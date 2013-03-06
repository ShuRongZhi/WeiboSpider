// HTMLParser.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SinaWeiboSpider.h"
#include "HTMLParser.h"


// CHTMLParser

CHTMLParser::CHTMLParser()
{

}

CHTMLParser::~CHTMLParser()
{
}


bool CHTMLParser::ReadHTML(CString &Source)
{
	if(Source.IsEmpty())
	{
		return false;
	}
	if(Source.Find("html"))
	{
		//�Ⲣ����һ��HTML�ļ�
		return false;
	}
	//���Ƶ���Ա������
	m_HTMLCode = Source;
	return true;
}

bool CHTMLParser::Destory()
{
	m_HTMLCode.Empty();
	return true;
}

//ȡ��ͷ�ڵ�
CString CHTMLParser::GetHeadNode()
{
	CString Result;
	//ȡ��ͷ�ڵ�
	m_Tools.FindFirstString(m_HTMLCode,Result,"<head>.+?<//head.>");
	if(Result.IsEmpty())
	{
		//û���ҵ����ϵ��ַ���
		return;
	}
	else
	{
		return Result;
	}
}

//ȡ��body�ڵ�
CString CHTMLParser::GetBodyNode()
{
	CString Result;
	//ȡ��body�ڵ�
	//ע�⣬body�����ݿ��ܻ�ܳ��������ڴ濪���Ǹ�����
	m_Tools.FindFirstString(m_HTMLCode,Result,"<body>.+?<//body.>");
	if(Result.IsEmpty())
	{
		//û���ҵ����ϵ��ַ���
		return;
	}
	else
	{
		return Result;
	}
}

std::vector<CString> CHTMLParser::GetTag(CString TagName)
{
	std::vector<CString> _Res;
	//���ϱ�ǩ���Ľڵ���ܲ�ֹһ�������ذ������з��Ͻ��
	if(TagName.IsEmpty())
	{
		//��ǩ����Ϊ��
		return;
	}
	CString Rule;
	//����ǩ��ת����������ʽ�﷨
	Rule.Format(_T("<%s>.+?<//%s>"),TagName,TagName);
	_Res = m_Tools.ParseString(m_HTMLCode,Rule);
	if(_Res.empty())
	{
		//����Ϊ��
		return _Res;
	}
	else
	{
		return _Res;
	}
}

CString CHTMLParser::GetText(CString TagName)
{
	CString _Res;
	//���ϱ�ǩ���Ľڵ���ܲ�ֹһ�������ذ������з��Ͻ��
	if(TagName.IsEmpty())
	{
		//��ǩ����Ϊ��
		return;
	}
	CString Rule;
	//����ǩ��ת����������ʽ�﷨
	Rule.Format(_T("<%s>.+?<//%s>"),TagName,TagName);
	if(_Res.empty())
	{
		//����Ϊ��
		return _Res;
	}
	else
	{
		return _Res;
	}
}