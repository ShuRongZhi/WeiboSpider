/*
*������ǹ����࣬��װ��һЩ���õģ�С�͵ĺ���
*����������ʽ�������ַ���ת����
*ԭ����Ӧ�ý����ָ���������ӵ����൱��
*������2013-2-20
*/



#include "StdAfx.h"
#include "Tools.h"
#include "boost\regex.hpp"
#include <Windows.h>
#include "SinaWeiboSpiderDlg.h"

#define Time 5000

//������̬���Ա�󣬱����������ʼ��������ᱨlnk 2001 ����
bool Tools::IsAutoSleep = false;

Tools::Tools(void)
{
}


Tools::~Tools(void)
{
}

//*************************������ʽ��صĺ���***************************/
bool Tools::FormatString(CString &Origin)
{
	/*
	*Bulid Data 2013/2/20 16:30
	*�˺�������ɾ������Ŀո񣬻س�
	*/
	/*
	2013-2-28 ��BUG�������ַ��������ģ�ɾ���ո�ᵼ������
	*/
	if(Origin.IsEmpty())
	{
		AfxMessageBox(_T("FormatString\n���󣺲��ܴ��ݿղ���"),MB_ICONWARNING);
		return false;
	}
	//�ҵ�������1�����ϵĿո�
	CString Rule_Space("\\s{2,}\\s");
	//�ҵ�������1�����ϵĻس�
	//CString Rule_Enter("\\r{2,}\\r");

	boost::regex reg_space(Rule_Space);
	//boost::regex reg_enter(Rule_Enter);

	std::string Result;
	std::string origin(Origin);
	//������ո񼰻س��滻��һ��
	Result = boost::regex_replace(origin,reg_space,"");
	//Result = boost::regex_replace(Result,reg_enter,"\\n");
	//������д�뵽ʵ����(����)
	Origin = Result.c_str();
	return true;
}

bool Tools::ReplaceString(CString &Origin,CString String,CString Rule)
{
	/*
	*Bulid 2013-2-20 17:31
	*�˺��������滻�ַ���
	*�Բ���3ָ�����滻�����ò���2�滻����1����ַ���
	*/
	if(Origin.IsEmpty() || Rule.IsEmpty())
	{
		AfxMessageBox(_T("ReplaceString\n���󣺲��ܴ��ݿղ���"),MB_ICONWARNING);
		return false;
	}
	boost::regex reg(Rule);
	std::string origin(Origin);
	std::string Result;
	Result = boost::regex_replace(origin,reg,String.GetBuffer());
	Origin = Result.c_str();
	return true;
}

bool Tools::ParseString(CString &Origin,CString &Result,CString Rule)
{
	/*
	*Bulid 2013-2-20 17:47
	*�˺������ڽ����ַ���
	*�ڲ���1�в����ַ�������д�뵽����2�У����ҹ����ɲ���3ָ��
	*/
	if(Origin.IsEmpty() || Rule.IsEmpty())
	{
		AfxMessageBox(_T("ParseString Function\n���󣺲��ܴ��ݿղ���"),MB_ICONWARNING);
		return false;
	}
	//��ʼ��������
	Result = "";
	std::string origin(Origin);
	boost::regex reg(Rule);
	boost::smatch what;
	std::string::const_iterator beg,end;
	beg = origin.begin();
	end = origin.end();
	//��һ�����ҵ����������ж��Ƿ��ҵ�������Ҳ���������false
	if(!boost::regex_search(beg,end,what,reg))
	{
		Result = "";
		return false;
	}
	//����ƥ�������ѭ����ӡ������
	while(boost::regex_search(beg,end,what,reg))
	{
		std::string msg(what[0]);
		Result += msg.c_str();
		beg = what[0].second;
	}
	return true;
}

bool Tools::ParseString(CString &Origin,CString &Result,CString Rule,int)
{
	/*
	*Bulid 2013-2-20 17:49
	*�˺������ڽ����ַ���������ÿ�����������ӻ���
	*�ڲ���1�в����ַ�������д�뵽����2�У����ҹ����ɲ���3ָ��
	*/
	if(Origin.IsEmpty() || Rule.IsEmpty())
	{
		AfxMessageBox(_T("ParseString Function with Add Enter Version\n���󣺲��ܴ��ݿղ���"),MB_ICONWARNING);
		return false;
	}
	//��ʼ��������
	Result = "";
	std::string origin(Origin);
	boost::regex reg(Rule);
	boost::smatch what;
	std::string::const_iterator beg,end;
	beg = origin.begin();
	end = origin.end();
	//��һ�����ҵ����������ж��Ƿ��ҵ�������Ҳ���������false
	if(!boost::regex_search(beg,end,what,reg))
	{
		Result = "";
		return false;
	}
	//����ƥ�������ѭ����ӡ������
	while(boost::regex_search(beg,end,what,reg))
	{
		std::string msg(what[0]);
		//��ÿ������������һ���س�
		msg = msg + '\n';
		Result += msg.c_str();
		beg = what[0].second;
	}
	return true;
}


bool Tools::FindFirstString(CString &Origin,CString &Exchange,CString Rule)
{
	/*
	*Bulid 2013-3-5 15:03
	*�˺������ڽ����ַ��������ص�һ������Ҫ����ַ���
	*�ڲ���1�в����ַ�������д�뵽����2�У����ҹ����ɲ���3ָ��
	*/
	if(Origin.IsEmpty() || Rule.IsEmpty())
	{
		AfxMessageBox(_T("FindFirstString Function\n���󣺲��ܴ��ݿղ���"),MB_ICONWARNING);
		return false;
	}
	//��ʼ��������
	Exchange = "";
	std::string origin(Origin);
	boost::regex reg(Rule);
	boost::smatch what;
	std::string::const_iterator beg,end;
	beg = origin.begin();
	end = origin.end();
	//��һ�����ҵ����������ж��Ƿ��ҵ�������Ҳ���������false
	if(!boost::regex_search(beg,end,what,reg))
	{
		Exchange = "";
		return false;
	}
	//����ƥ����������ص�һ�����
	boost::regex_search(beg,end,what,reg);
	std::string msg(what[0]);
	Exchange = msg.c_str();
	return true;
}

std::vector<CString> Tools::ParseString(CString &Source,CString Rule)
{
	/*
	*Bulid 2013-3-6 17:52
	*�˺������ڽ����ַ����������ϵĽ��ѹ��vector��
	*�ڲ���1�в����ַ��������ҹ����ɲ���2ָ��
	*/
	std::vector<CString> Res;
	if(Source.IsEmpty() || Rule.IsEmpty())
	{
		AfxMessageBox(_T("ParseString Function\n���󣺲��ܴ��ݿղ���"),MB_ICONWARNING);
		return Res;
	}
	std::string origin(Source);
	boost::regex reg(Rule);
	boost::smatch what;
	std::string::const_iterator beg,end;
	beg = origin.begin();
	end = origin.end();
	//��һ�����ҵ����������ж��Ƿ��ҵ�������Ҳ���������false
	if(!boost::regex_search(beg,end,what,reg))
	{
		return Res;
	}
	//����ƥ�������ѭ����ӡ������
	while(boost::regex_search(beg,end,what,reg))
	{
		std::string msg(what[0]);
		//�����ѹ��vector��
		Res.push_back(msg.c_str());
		beg = what[0].second;
	}
	return Res;
}

bool Tools::SearchString(CString &Origin,CString Rule)
{
	/*
	*Bulid 2013-2-20 17:50
	*�˺������ڲ����ַ���
	*�Բ���2ָ���Ĺ����ڲ���1�в��ң��ҵ�����true�����򷵻�false
	*/
	if(Origin.IsEmpty() || Rule.IsEmpty())
	{
		AfxMessageBox(_T("SearchString Function\n���󣺲��ܴ��ݿղ���"),MB_ICONWARNING);
		return false;
	}
	std::string origin(Origin);
	boost::regex reg(Rule);
	boost::smatch what;
	std::string::const_iterator beg,end;
	beg = origin.begin();
	end = origin.end();
	if(!boost::regex_search(beg,end,what,reg))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool Tools::RemoveString(CString &Origin,CString Rule)
{
	/*
	*Bulid 2013-2-20 18:10
	*�˺�������ɾ���ַ���
	*����2ָ��ɾ�������ڲ���1��ִ�в���
	*/
	if(Origin.IsEmpty() || Rule.IsEmpty())
	{
		AfxMessageBox(_T("RemoveString Function\n���󣺲��ܴ��ݿղ���"),MB_ICONWARNING);
		return false;
	}
	boost::regex reg(Rule);
	std::string origin(Origin);
	std::string res;
	res = boost::regex_replace(origin,reg,"");
	if(0 == res.length())
	{
		AfxMessageBox(_T("RemoveString Function\n����Boost::Regex_replace���ؿ��ַ���"));
		return false;
	}
	Origin = res.c_str();
	return true;
}

//*********************������ʽ��صĺ���****************************

CString Tools::UTF8ToANSI(CString utf8)
{
	/*
	*�˺����ο����ϴ���
	*Thanks   bbs.sciencenet.cn/thread-107913-1-1.html
	*/
	if(utf8.IsEmpty())
	{
		AfxMessageBox(_T("UTF8ToANSI\n���󣺲��ܴ��ݿղ���"),MB_ICONERROR);
		return "";
	}
	int srcCodeLen=0;   
	srcCodeLen=MultiByteToWideChar(CP_UTF8,NULL,utf8,utf8.GetLength(),NULL,0);   
	wchar_t* result_t=new wchar_t[srcCodeLen+1];   
	MultiByteToWideChar(CP_UTF8,NULL,utf8,utf8.GetLength(),result_t,srcCodeLen); //utf-8ת��ΪUnicode  
	result_t[srcCodeLen]='\0';   
	srcCodeLen=WideCharToMultiByte(CP_ACP,NULL,result_t,wcslen(result_t),NULL,0,NULL,NULL);   
	char* result=new char[srcCodeLen+1];   
	WideCharToMultiByte(CP_ACP,NULL,result_t,wcslen(result_t),result,srcCodeLen,NULL,NULL);//Unicodeת��ΪANSI   
	result[srcCodeLen]='\0';   
	delete result_t;   
	return result; 
}

std::vector<CString> Tools::SplitString(CString Origin)
{
	std::vector<CString> Res;
	if(Origin.IsEmpty())
	{
		AfxMessageBox(_T("SplitString Function Say��\n���󣺲��ܴ��ݿղ���"),MB_ICONERROR);
		return Res;
	}
	CString str;
	while(true)
	{
		char *szBuffer = new char[Origin.GetLength()];
		if(!szBuffer)
		{
			AfxMessageBox(_T("SplitString Function Say:\n�����ڴ�ʧ�ܣ�"),MB_ICONERROR);
			return Res;
		}
		//�ָ��ַ�
		if(!sscanf(Origin.GetBuffer(),"%s",szBuffer))
		{
			if(!szBuffer)
			{
				delete []szBuffer;
			}
			szBuffer = NULL;
			AfxMessageBox(_T("SplitString Function Say:\n sscanf ������������"));
			return Res;
		}
		Origin.ReleaseBuffer();
		str = szBuffer;
		if(!Origin.Replace(str,NULL))
		{
			if(!szBuffer)
			{
				delete []szBuffer;
			}
			szBuffer = NULL;
			break;
		}
		Res.push_back(str);
		if(!szBuffer)
		{
			delete []szBuffer;
		}
		szBuffer = NULL;
	}
	return Res;
}


std::vector<CString> Tools::SplitString(CString Origin,CString Rule)
{
	std::vector<CString> Res;
	if(Origin.IsEmpty())
	{
		AfxMessageBox(_T("SplitString Function Say��\n���󣺲��ܴ��ݿղ���"),MB_ICONERROR);
		return Res;
	}
	std::string origin(Origin);
	boost::regex reg(Rule);
	boost::smatch what;
	std::string::const_iterator beg,end;
	beg = origin.begin();
	end = origin.end();
	//��һ�����ҵ����������ж��Ƿ��ҵ�������Ҳ���������false
	if(!boost::regex_search(beg,end,what,reg))
	{
		return Res;
	}
	//����ƥ�������ѭ����ӡ������
	while(boost::regex_search(beg,end,what,reg))
	{
		std::string msg(what[0]);
		Res.push_back(msg.c_str());
		beg = what[0].second;
	}
	return Res;
}

void Tools::Sleep()
{
	//�˺������������̣߳�������ʹ��챻����
	//����û�ѡ���Զ����ߣ���ʹ��������ɵ���ֵ��������
	//���û�У���ʹ�ù̶�����ֵ��������
	if(IsAutoSleep)
	{
		SleepTime = rand() % 900 * 100;
		UpdateInfo.Format(_T("�߳�˯����......˯��ʱ��Ϊ%d���룬�����ĵȴ�"),SleepTime);
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		//˯�ߣ�ģ���޹��ɵĲ��������ⱻ����
		::Sleep(SleepTime);
	}
	//δѡ���Զ����ߣ�Ĭ��5��
	else
	{
		UpdateInfo.Format(_T("�߳�˯����......˯��ʱ��Ϊ%d���룬�����ĵȴ�"),Time);
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		//˯��
		::Sleep(Time);
	}
}