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
	//���Ƶ���Ա������
	m_HTMLCode = Source;
	return true;
}


bool CHTMLParser::ParserHTML()
{
	//��ʱ���div�ڵ��vector
	std::vector<CString> _TempSaveDivNode;

	//ɾ��HTML�е�CSS��ʽ
	if(!m_Tools.RemoveString(m_HTMLCode,"<style .+?>.+?</style>"))
	{
		//ɾ��css��ʽʧ��
		return false;
	}
	//Ѱ��ͷ�ڵ�
	if(!m_Tools.FindFirstString(m_HTMLCode,m_HeadNode,"<head>.+?</head>"))
	{
		//û���ҵ�head�ڵ�
		return false;
	}

	//Ѱ��body�ڵ�
	if(!m_Tools.ParseString(m_HTMLCode,m_BodyNode,"<body>.+?</body>"))
	{
		//û���ҵ�body�ڵ�
		return false;
	}
	//�ҵ�body�ڵ�
	else
	{
		if(m_Tools.ParseString(m_BodyNode,m_UserInfoNode,"<div class=\"u\">[^<div>].+?</div></div>"))
		{
			//�ҵ��û���Ϣ�ڵ�
			//˵�������΢��ҳ��
			//��body�ڵ����û���Ϣ�Ľڵ��Ƴ�
			m_BodyNode.Replace(m_UserInfoNode,NULL);
		}

		
		//ɾ���ײ�����Ϣ
		CString _TempRemove;
		if(m_Tools.ParseString(m_BodyNode,_TempRemove,"<div class=\"n\".+?>.+?</a></div><div class=\"c\".+?>"))
		{
			//����ҵ�
			//��div class="c" ɾ���������ȡ������һ��΢��
			m_Tools.RemoveString(_TempRemove,"<div class=\"c\".+?>");
			m_BodyNode.Replace(_TempRemove,NULL);
		}
		//��ȡ�ڵ����ݵ�������ʽ <.+?>.+?</.+?><div class="s"></div>
		_TempSaveDivNode = m_Tools.ParseString(m_BodyNode,"<.+?>.+?</.+?><div class=\"s\"></div>");
		if(_TempSaveDivNode.empty())
		{
			//����Ϊ�գ�����
			return false;
		}
		std::vector<CString>::iterator _TempIter;
		_TempIter = _TempSaveDivNode.begin();
		while(_TempIter != _TempSaveDivNode.end())
		{
			CString _TagName;
			//��ȡ��ǩ��
			if(!m_Tools.FindFirstString(*_TempIter,_TagName,"<div class=.+?>"))
			{
				//��ȡ������ǩ��
				return false;
			}
			m_DivNode.insert(std::make_pair(_TagName,*_TempIter));
			++_TempIter;
		}
	}

	CFile fp;
	fp.Open(_T("D:\\ParserOut.txt"),CFile::modeCreate | CFile::modeWrite);
	std::multimap<CString,CString>::iterator  _iter;
	_iter = m_DivNode.begin();
	CString out;
	while(_iter != m_DivNode.end())
	{
		out.Format(_T("\n\nDiv Id is :%s--------\nDiv Content is :%s \n\n\n"),_iter->first,_iter->second);
		fp.Write(out,out.GetLength());
		++_iter;
	}
	return true;
}

bool CHTMLParser::Destory()
{
	m_HTMLCode.Empty();
	return true;
}

std::vector<CString> CHTMLParser::GetTag(CString TagName)
{
	std::vector<CString> _Res;
	//���ϱ�ǩ���Ľڵ���ܲ�ֹһ�������ذ������з��Ͻ��
	if(TagName.IsEmpty())
	{
		//��ǩ����Ϊ��
		return _Res;
	}
	CString Rule;
	//����ǩ��ת����������ʽ�﷨
	Rule.Format(_T("<%s>.+?</%s>"),TagName,TagName);
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
		return NULL;
	}
	CString Rule;
	//����ǩ��ת����������ʽ�﷨
	Rule.Format(_T("<%s>.+?<//%s>"),TagName,TagName);
	if(_Res.IsEmpty())
	{
		return _Res;
	}
	else
	{
		return _Res;
	}
}