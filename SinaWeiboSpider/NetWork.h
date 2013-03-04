#pragma once
#include "Tools.h"
#include <map>
#include "DataStruct.h"



class NetWork
{
public:
	NetWork(void);
	~NetWork(void);
	//��öԻ�����������PostMessage��
	void SetDlgHwnd(HWND);
	BOOL InitSocket();
	BOOL ConnectionServer(CString);
	BOOL ConnectionServer();
	BOOL Login(CString,CString);
	int SendMsg(CString&);
	BOOL RecvMsg(CString&);
	void CloseSocket();
	bool ResetSocket();
	void SetSaveFilePath(CString);
	//TODO: ����1:Ҫ������΢����ע�б��ַ
	bool TraversalFollow(CString);
	bool TraversalFans(CString);
	bool TraversalWeibo(CString);


	unsigned int Follow_Page;
	unsigned int Weibo_Page;
	unsigned int Fans_Page;
	unsigned int WeiboID;
private:
	HWND hwnd;
	CFile DebugOut;
	CFile m_Result;
	Tools tools;
	sockaddr_in ServerAddr;
	WSADATA data;
	SOCKET Client;
	std::map<CString,CString> m_FollowList;
	std::map<CString,CString>::iterator m_iter;
	std::map<CString,CString> m_FansList;

	std::map<unsigned int,DataStruct,std::greater<unsigned int> > m_WeiboList;
	DataStruct WeiboInfo;
	//���ڸ���UI
	CString UpdateInfo;
	//���password�ֶε����ֵ
	CString m_RandomPassword;
	//���VK�����ֵ
	CString m_vk;
	//�����ת��URL
	CString m_location;
	//��ŵ�½���cookie
	CString m_cookie;
	//������һ�ε���connectionServer��IP,��ResetSocket������ʹ��
	CString IP;
	CString DebugInfo;
	CString SystemTime;
	int Result;
	bool IsInit;
	bool IsConnection;
	bool IsLogin;
	bool IsSaveFilePathSet;
};

