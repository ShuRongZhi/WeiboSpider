#include "StdAfx.h"
#include "NetWork.h"
#include "Tools.h"
#include <fstream>

//����������С
#define MAX 65535

NetWork::NetWork(void)
{
	IsSaveFilePathSet = false;
}

//ȡ�öԻ����������ڷ�����Ϣ���ؼ�
void NetWork::SetDlgHwnd(HWND m_hwnd)
{
	hwnd = m_hwnd;
	//���Ի�������ֵ��tools
	tools.hwnd = m_hwnd;
}

//���ô�Ž����Ŀ¼
void NetWork::SetSaveFilePath(CString Path)
{
	//�����ж��Ƿ��һ�ε���
	if(!IsSaveFilePathSet)
	{
		//���PathΪ�գ���ʾ�û�û��ָ���洢Ŀ¼������Ĭ��Ŀ¼���
		if(Path.IsEmpty())
		{
			if(!DebugOut.Open(_T("D:\\debugout.txt"),CFile::modeCreate | CFile::modeReadWrite))
			{
				AfxMessageBox(_T("NetWork Constructor Function\n�����޷�����Debug���Ŀ¼"),MB_ICONWARNING);
			}
			if(!m_Result.Open(_T("D:\\������.txt"),CFile::modeReadWrite))
			{
				m_Result.Open(_T("D:\\������.txt"),CFile::modeCreate|CFile::modeReadWrite);
			}
			m_Result.SeekToEnd();
		}
		//�û�ָ���˴洢Ŀ¼
		else
		{
			if(!m_Result.Open(Path,CFile::modeReadWrite))
			{
				m_Result.Open(Path,CFile::modeCreate|CFile::modeReadWrite);
			}
			m_Result.SeekToEnd();
		}
		IsSaveFilePathSet = true;
	}
	//��2+�ε��ã����ȹر��ļ����ٴ򿪷���ᱨ��
	//�����ǰ�Ѵ��ļ�����ر�
	else
	{
		m_Result.Close();
		if(!m_Result.Open(Path,CFile::modeReadWrite))
		{
			m_Result.Open(Path,CFile::modeCreate|CFile::modeReadWrite);
		}
		m_Result.SeekToEnd();
		IsSaveFilePathSet = true;
	}
}

NetWork::~NetWork(void)
{
}

//��ʼ��socket
BOOL NetWork::InitSocket()
{
	if(0 != WSAStartup(MAKEWORD(2,2),&data))
	{
		AfxMessageBox(_T("���󣺳�ʼ��WSA�ṹ��ʧ��"),MB_ICONWARNING);
		return false;
	}
	Client = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(Client == INVALID_SOCKET)
	{
		AfxMessageBox(_T("���󣺴����׽���ʧ��"),MB_ICONWARNING);
		return false;
	}
	IsInit = true;
	return true;
}

//���ӷ�����
BOOL NetWork::ConnectionServer(CString IPAddr)
{
	if(IPAddr.IsEmpty())
	{
		AfxMessageBox(_T("ConnectionServer Function Say:\n����IP��ַΪ��"),MB_ICONERROR);
		return false;
	}
	//��ָ����IP��ֵ����Ա����IP
	IP = IPAddr;
	if(!IsInit)
	{
		AfxMessageBox(_T("���󣺱����ȳ�ʼ���׽���"),MB_ICONWARNING);
		return false;
	}
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.S_un.S_addr = inet_addr(IPAddr);

	ServerAddr.sin_port = htons(80);
	memset(ServerAddr.sin_zero,0,sizeof(ServerAddr));
	Result = connect(Client,(sockaddr*)&ServerAddr,sizeof(ServerAddr));
	if(SOCKET_ERROR == Result)
	{
		AfxMessageBox(_T("�������ӷ�����ʧ��"),MB_ICONWARNING);
		return false;
	}
	IsConnection = true;
	return true;
}

//���ӷ��������ذ汾�����ӷ�����IPΪ���һ�����ӵ�IP
BOOL NetWork::ConnectionServer()
{
	if(!IsInit)
	{
		AfxMessageBox(_T("���󣺱����ȳ�ʼ���׽���"),MB_ICONWARNING);
		return false;
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.S_un.S_addr = inet_addr(IP);

	ServerAddr.sin_port = htons(80);
	memset(ServerAddr.sin_zero,0,sizeof(ServerAddr));
	Result = connect(Client,(sockaddr*)&ServerAddr,sizeof(ServerAddr));
	if(SOCKET_ERROR == Result)
	{
		AfxMessageBox(_T("�������ӷ�����ʧ��"),MB_ICONWARNING);
		return false;
	}
	IsConnection = true;
	return true;
}

//�������ݣ����ط����ֽ���
int NetWork::SendMsg(CString &Msg)
{
	if(!IsConnection)
	{
		AfxMessageBox(_T("���󣺱��������ӷ��������ܷ�������"),MB_ICONWARNING);
		return false;
	}

	Result = send(Client,Msg.GetBuffer(),Msg.GetLength(),0);
	if(SOCKET_ERROR == Result)
	{
		AfxMessageBox(_T("���󣺷�������ʧ��"),MB_ICONWARNING);
		return false;
	}
	return Result;
}

//��������
BOOL NetWork::RecvMsg(CString &Exchange)
{
	std::ofstream fp;
	//��ʼ��������
	Exchange = "";
	int Ret = 0;
	char *RecvBuffer = new char[MAX];
	while(true)
	{ 
		memset(RecvBuffer,0,MAX);
		Ret = 0;
		//���ֵ-1
		Ret = recv(Client,RecvBuffer,MAX-1,0);
		if(0 == Ret || SOCKET_ERROR == Ret)
		{
			//����������
			break;
		}
		//��Ϊ����������chunked����ķ���������ҳ�������Ļ���ÿ�ν��ܻ�����һ�������ַ�������0x��ʮ����������
		//Ϊ�˱���ָʾ���ȵ��ַ�Ӱ������Ľ�������ÿ�ν���ʱ���ó��ȱ��ȥ��
		CString Temp(RecvBuffer);
		tools.RemoveString(Temp,"\\b[0-9a-fA-F]{2,8}?\\r");
		Exchange+=Temp;
	}
	if(Exchange.IsEmpty())
	{
		AfxMessageBox(_T("RecvMsg Function\n���󣺷������޷��أ����Ժ�����"),MB_ICONWARNING);
		delete []RecvBuffer;
		return false;
	}
	delete []RecvBuffer;
	return true;
}

//�˺�������socket����
bool  NetWork::ResetSocket()
{
	//�����ж�scket״̬
	if(IsInit)
	{
		if(IsConnection)
		{
			//�����ǰ�����ӣ���ر�socket
			CloseSocket();
			//��������
			if(!InitSocket())
			{
				return false;
			}
			if(!ConnectionServer())
			{
				return false;
			}
			return true;
		}
		//�ѳ�ʼ����δ���ӣ�ֱ�����ӷ���������
		else
		{
			if(!ConnectionServer())
			{
				return false;
			}
		}
	}
	//δ��ʼ����ֱ�ӳ�ʼ��������
	else
	{
		if(!InitSocket())
		{
			return false;
		}
		if(!ConnectionServer())
		{
			return false;
		}
	}
	return true;
}

//�˺����ر�socket
void NetWork::CloseSocket()
{
	try
	{
		closesocket(Client);
		WSACleanup();
		IsInit = false;
		IsConnection = false;
	}
	catch(...)
	{
		AfxMessageBox(_T("�ر��׽���ʧ��"),MB_ICONERROR);
		exit(-1);
	}
}

//�˺�����װ�˵�¼����
BOOL NetWork::Login(CString username,CString password)
{
	/*  ΢����½����
	* 1) ����GET��������3g.sina.com.cn/prog/wapsite/sso/login.php?ns=1&revalid=2&backURL=http%3A%2F%2Fweibo.cn%2F&backTitle=%D0%C2%C0%CB%CE%A2%B2%A9&vt=�����ҳ
	*     �õ�Դ���룬��ȡ��form action=�����URL��password�����ֵ��vk�����ֵ   
	* 2) ����POST���ݰ���������3g.sina.com.cn//prog/wapsite/sso/������ϵ�һ����ȡ��form action�����URL
	*    POST���ݣ�mobile=" + �û��� + "&" + ��һ����ȡ��password���ֵ +"=" + �û����� + "&remember=on&backURL=http%253A%252F%252Fweibo.cn%252F%253Fs2w%"
	*    "253Dlogin&backTitle=%E6%96%B0%E6%B5%AA%E5%BE%AE%E5%8D%9A&vk=" + ��һ����ȡ��vk���ֵ + "&submit=%E7%99%BB%E5%BD%95"
	* 3)������Ӧ�û᷵��HTTP 302 Found�ض���,��ȡLocation�ֶ���Set-Cookie�ֶδ��á��������������HTTP 200 OK��˵���û����������д���
	*
	* 4)���ʵڶ������ص�location������ת
	*
	* 5)�������͵�½�ɹ��ˣ����Է��ʵ�½�����ҳ���ǵô��ϵڶ������ص�cookie
	*
	*
	*!!!!!!!!!!!!!GET���������һ��Ҫ��2���س�����Ȼ��һֱ���ܲ�����Ϣ����������������������������
	*/

	CString Request;
	Request =  "GET /prog/wapsite/sso/login.php?ns=1&revalid=2&backURL=http%3A%2F%2Fweibo.cn%2F&backTitle=%D0%C2%C0%CB%CE%A2%B2%A9&vt= HTTP/1.1\r\n"
		"Host: 3g.sina.com.cn\r\n"
		"User-Agent: Mozilla/5.0 (Windows NT 6.1; rv:16.0) Gecko/20100101 Firefox/16.0\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
		"Accept-Language: zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3\r\n"
		"Referer: http://weibo.cn/pub/\r\n"
		//cookie��ʼ
		"Cookie: ad_user=pos4ddc723cbedd25084b05a30e9d; ad_time=1350873178; U_TRS1=00000078.264e6b92.5097a058.a5dd7659; UOR=lcx.cc,tech.sina.com.cn,;"
		" SINAGLOBAL=5870298401953.488.1352264698102; user_survey=-1; FSINAGLOBAL=5870298401953.488.1352264698102; ULV=1352264699688:1:1:1:5870298401"
		"953.488.1352264698102:; vjuids=-3cf85321e.13ad941edfa.0.6c31fd762931c; vjlast=1352264708; ALF=1353307827; SUR=uid%3D1917215763%26user%3D6174"
		"78860%2540qq.com%26nick%3D617478860%26email%3D%26dob%3D%26ag%3D4%26sex%3D%26ssl%3D0; SUS=SID-1917215763-1352769650-XD-qyhz1-d69fa09fd946ce2b"
		"4097573d62722734; SUE=es%3D608da6a1a04d1ccfda4c66466e200cfe%26ev%3Dv1%26es2%3Dde3c0087d87c14526b3e0165adfd2750%26rs0%3DkYpezbkv4Afm1AUYgxH1o"
		"P8m5i3E5xpcpbKSHp8iyd6Uio2n60sWvy1Rg9EON8P759GDg4rA3Zk3WOBdRdar%252FOLJA0dfvZCNr4Wft0kwVAkBIZEdILoXz3WhmNIAE1WPylcZF66KS7Bdn7lMuWwYZxtkOWOK9"
		"0O%252Bp0w7k9U7g2s%253D%26rv%3D0; SUP=cv%3D1%26bt%3D1352769650%26et%3D1352856050%26d%3D40c3%26i%3D2734%26us%3D1%26vf%3D0%26vt%3D0%26ac%3D0%2"
		"6lt%3D7%26uid%3D1917215763%26user%3D617478860%2540qq.com%26ag%3D4%26name%3D617478860%2540qq.com%26nick%3D617478860%26sex%3D%26ps%3D0%26email"
		"%3D%26dob%3D%26ln%3D%26os%3D%26fmp%3D%26lcp%3D2011-06-15%252020%253A00%253A51\r\n\r\n";

	DebugInfo.Format(_T("\n��һ�����󣺻�ȡ���ֵ\n"));
	DebugOut.Write(DebugInfo,DebugInfo.GetLength());
	DebugOut.Flush();

	//������Ϣ�����ڸ��¿ؼ�
	UpdateInfo.Format(_T("�����ҳ���ֵ��......"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	if(!SendMsg(Request))
	{
		UpdateInfo.Format(_T("��������ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}

	CString ServerReturn_UTF8,ServerReturn_ANSI;
	if(!RecvMsg(ServerReturn_UTF8))
	{
		UpdateInfo.Format(_T("���շ���������ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	/*
	*�Է��������ص��ַ�������ת��
	*����΢���ı���ΪUTF8,������ʽò�Ʋ���֧��unicode���룬���Ա���ת����ANSI����
	*/
	ServerReturn_ANSI = tools.UTF8ToANSI(ServerReturn_UTF8);

	if(ServerReturn_ANSI.IsEmpty())
	{
		AfxMessageBox(_T("Login Function Say:\n����UTF8ToANSI�������ؿ��ַ���"),MB_ICONWARNING);
		UpdateInfo.Format(_T("UTF8ToANSI�������ؿ��ַ���"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	/*
	*ȡ��΢��ҳ��URL�����ֵ�����´���
	*/
	CString RandomURL,m_temp;
	if(!tools.ParseString(ServerReturn_ANSI,RandomURL,"<form action=\".*ns=1\""))
	{
		UpdateInfo.Format(_T("���URL���ֵʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	m_temp = RandomURL;

	if(!tools.ParseString(m_temp,RandomURL,"login.*ns=1"))
	{
		UpdateInfo.Format(_T("���URL���ֵʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	m_temp.Empty();

	DebugInfo.Format(_T("\n\nURL�����ֵ�� :%s\n\n"),RandomURL);
	DebugOut.Write(DebugInfo,DebugInfo.GetLength());
	DebugOut.Flush();

	//ȡ��΢��ҳ��password��������ֵ
	if(!tools.ParseString(ServerReturn_ANSI,m_RandomPassword,"password.*"))
	{
		UpdateInfo.Format(_T("���Password���ֵʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	m_temp = m_RandomPassword;
	if(!tools.ParseString(m_temp,m_RandomPassword,"password_...."))
	{
		return false;
	}
	DebugInfo.Format(_T("\n\nPassword���ֵ��:%s\n\n"),m_RandomPassword);
	DebugOut.Write(DebugInfo,DebugInfo.GetLength());
	DebugOut.Flush();

	//ȡ��΢��ҳ��VK��������ֵ
	if(!tools.ParseString(ServerReturn_ANSI,m_vk,"vk..value..\\w{0,50}"))
	{
		UpdateInfo.Format(_T("���VK���ֵʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	m_temp = m_vk;

	if(!tools.ParseString(m_temp,m_vk,"=\"\\w{0,100}"))
	{
		UpdateInfo.Format(_T("���VK���ֵʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	//ȥ��vk������ַ�
	m_vk.Replace("\"",NULL);
	m_vk.Replace("=",NULL);


	UpdateInfo.Format(_T("�ɹ�"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	CString PostDataLenght;
	CString PostData;
	CString LoginRequest;

	//����socket״̬
	CloseSocket();
	if(!InitSocket())
	{
		return false;
	}
	//ָ����¼��������IP��ַ
	if(!ConnectionServer("221.179.175.244"))
	{
		return false;
	}

	//�������ݰ�
	PostData = "mobile=" + username + "&" + m_RandomPassword +"=" + password  + "&remember=on&backURL=http%253A%252F%252Fweibo.cn%252F%253Fs2w%"
		"253Dlogin&backTitle=%E6%96%B0%E6%B5%AA%E5%BE%AE%E5%8D%9A&vk=" + m_vk + "&submit=%E7%99%BB%E5%BD%95";
	//�������ݰ��ĳ���
	PostDataLenght.Format("Content-Length: %d\r\n\r\n",PostData.GetLength());

	//���������
	LoginRequest = "POST /prog/wapsite/sso/"+RandomURL+" HTTP/1.1\r\n"
		"Host: 3g.sina.com.cn\r\n"
		"User-Agent: Mozilla/5.0 (Windows NT 6.1; rv:16.0) Gecko/20100101 Firefox/16.0\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
		"Accept-Language: zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3\r\n"
		"Referer: http://3g.sina.com.cn/prog/wapsite/sso/login.php?ns=1&revalid=2&backURL=http%3A%2F%2Fweibo.cn%2F%3Fs2w%3Dlogin&backTitle=%D0%C2%C0%CB%CE%A2%B2%A9&vt=\r\n"
		//cookie �п�ʼ
		"Cookie: ad_user=pos4ddc723cbedd25084b05a30e9d; ad_time=1350873178; U_TRS1=00000078.264e6b92.5097a058.a5dd7659; UOR=lcx.cc,tech.sina.com.cn,; SINAGLOBAL=5870298401953.488."
		"1352264698102; user_survey=-1; FSINAGLOBAL=5870298401953.488.1352264698102; ULV=1352264699688:1:1:1:5870298401953.488.1352264698102:; vjuids=-3cf85321e.13ad941edfa.0."
		"6c31fd762931c; vjlast=1352264708; SUS=SID-1917215763-1352703027-XD-vfgtd-bd3b55a5094b6402fd79fadefe693f97; SUE=es%3Dc5e170f1abd289a95794e07e9458f4d8%26ev%3Dv1%26es2%3D1815e128"
		"e69a7575d60b9aa17db3a3ab%26rs0%3DwTZJDGkATabYkGHu8%252FTMgtvVZLN8d%252FkA3Ihu7jDtTAZ1SkeS0FhqLJQAlP4XLA%252FtEbM5G79mWYnJ95nptVOlSSlGwe9c9uROq1zBZMiE6%252FuLRL9xiPTNBsJ2hJjQkn"
		"r0cpreeDv%252BGMpkmaeejcKvHaEq7ktvB1SYY3J1vzTsfDE%253D%26rv%3D0; "
		"SUP=cv%3D1%26bt%3D1352703027%26et%3D1352789427%26d%3D40c3%26i%3D3f97%26us%3D1%26vf%3D0%26vt%3D0%26ac%3D0%26lt%3D1%26uid%3D1917215763%26user%3D617478860%2540qq."
		"com%26ag%3D4%26name%3D617478860%2540qq.com%26nick%3D617478860%26sex%3D%26ps%3D0%26email%3D%26dob%3D%26ln%3D617478860%2540qq.com%26os%3D%26fmp%3D%26lcp%3D2011-06-"
		"15%252020%253A00%253A51; ALF=1353307827; SUR=uid%3D1917215763%26user%3D617478860%2540qq.com%26nick%3D617478860%26email%3D%26dob%3D%26ag%3D4%26sex%3D%26ssl%3D0\r\n"
		//cookie �н���
		"Content-Type: application/x-www-form-urlencoded\r\n"+
		PostDataLenght+PostData;

	UpdateInfo.Format(_T("���͵�¼������......"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	if(!SendMsg(LoginRequest))
	{
		UpdateInfo.Format(_T("ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}

	if(!RecvMsg(ServerReturn_UTF8))
	{
		UpdateInfo.Format(_T("���շ���������ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	if(ServerReturn_UTF8.IsEmpty())
	{
		AfxMessageBox(_T("Login Function Say:\n���󣺷������޷��أ����Ժ�����"),MB_ICONERROR);
		UpdateInfo.Format(_T("�������޷��أ����Ժ�����"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}

	ServerReturn_ANSI = tools.UTF8ToANSI(ServerReturn_UTF8);
	if(ServerReturn_ANSI.IsEmpty())
	{
		AfxMessageBox(_T("Login Function Say:\n����UTF8ToANSI�������ؿ��ַ���"),MB_ICONERROR);
		UpdateInfo.Format(_T("UTF8ToANSI�������ؿ��ַ���"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	//�����¼ʧ�ܣ��������᷵��HTTP 200 OK ���ɹ��򷵻�HTTP 302 Found
	if(tools.SearchString(ServerReturn_ANSI,"HTTP/1.1 200 OK"))
	{
		//Ѱ���������ı��
		if(tools.SearchString(ServerReturn_ANSI,"<div class=\"msgErr\">"))
		{
			UpdateInfo.Format(_T("�û����������������������"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			AfxMessageBox(_T("Login Function\n�û��������������������������"),MB_ICONERROR);
			return false;
		}
		//2013-2-21
		//���û���ҵ��������ı�ǣ���ô�п�����Ҫ������֤�룬���ڲ�������������
		else
		{
			int Res;
			Res = AfxMessageBox(_T("Login Function Say:\n�����������쳣���Ƿ���ʾ������Ϣ��"),MB_YESNO | MB_ICONERROR);
			if(IDYES == Res)
			{
				AfxMessageBox(ServerReturn_ANSI);
				return false;
			}
			else
			{
				AfxMessageBox(_T("Login Function Say:\n���򽫻��˳�......"));
				exit(-1);
			}
		}
	}

	if(tools.SearchString(ServerReturn_ANSI,"HTTP/1.1 302 Found"))
	{
		//Ѱ��Location�ֶ�
		if(!tools.ParseString(ServerReturn_ANSI,m_location,"Location:.http.*"))
		{
			AfxMessageBox(_T("Login Function Say:\n����δ�ҵ�Location�ֶ�"),MB_ICONERROR);
			return false;
		}
		else
		{
			//Ѱ��Set-Cooike�ֶ�
			if(!tools.ParseString(ServerReturn_ANSI,m_cookie,"Set-Cookie.*?;"))
			{
				AfxMessageBox(_T("Login Function\n����:δ�ҵ�Set-Cookie�ֶ�"),MB_ICONWARNING);
				return false;
			}

			//ɾ��Location�ֶε���Ϣ��ֻ������ַ
			m_location.Replace("Location:",NULL);
			m_location.Replace("http://login.sina.cn",NULL);
			//ɾ��Vary��������������
			int cout = m_location.Find("Vary");
			m_location.Delete(cout,10000);
			m_location.Find("\r\n",NULL);
			//ɾ��Set-Cookie�ֶ���Ϣ��ֻ����cookie
			m_cookie.Replace("Set-Cookie:",NULL);
			m_cookie.Replace(";",NULL);
			m_cookie = "_T_WL=1;"+ m_cookie;
			m_cookie.Replace("\r\n",NULL);
		}
	}
	//�������Ȳ�����HTTP 200,Ҳ������HTTP 302
	//Ŀǰ��û�������������
	else
	{
		int Res;
		Res = AfxMessageBox(_T("Login Function Say:\n�����������쳣���Ƿ���ʾ���ؽ����(���ؽ���������Debug�ļ�)"),MB_YESNO | MB_ICONERROR);
		DebugInfo.Format(_T("\n΢����¼ʧ�ܣ����������ؽ���쳣���޷����������������:\n"));
		DebugOut.Write(DebugInfo,DebugInfo.GetLength());
		DebugOut.Write(ServerReturn_ANSI,ServerReturn_ANSI.GetLength());
		if(IDYES == Res)
		{
			AfxMessageBox(ServerReturn_ANSI);
			return false;
		}
		else
		{
			AfxMessageBox(_T("���򽫻��˳�"));
			exit(-1);
		}
	}

	DebugInfo.Format(_T("\n\nCookie�ֶ�ֵ��%s\nLocation�ֶ�ֵ��%s\n\n"),m_cookie,m_location);
	DebugOut.Write(DebugInfo,DebugInfo.GetLength());
	DebugOut.Flush();

	UpdateInfo.Format(_T("�ɹ�"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	//����Socket
	CloseSocket();
	if(!InitSocket())
	{
		return false;
	}
	if(!ConnectionServer("221.179.175.249"))
	{
		return false;
	}

	CString JumpRequest;
	JumpRequest =  "GET " + m_location + "HTTP/1.1\r\n"
		"Host: login.sina.cn\r\n"
		"User-Agent: Mozilla/5.0 (Windows NT 6.1; rv:16.0) Gecko/20100101 Firefox/16.0\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
		"Accept-Language: zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3\r\n"
		"Connection: keep-alive\r\n"
		"Referer: http://3g.sina.com.cn/prog/wapsite/sso/login.php?ns=1&revalid=2&backURL=http%3A%2F%2Fweibo.cn%2F%3Fs2w%3Dlogin&backTitle=%D0%C2%C0%CB%CE%A2%B2%A9&vt=\r\n"
		"Cookie: ad_user=pos4ddc723cbedd25094866b177da; ad_time=1351911019; ustat=4c0077fd3103524f2f1e0748204b897b; vt=4\r\n\r\n";

	UpdateInfo.Format(_T("������ת������......"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	if(!SendMsg(JumpRequest))
	{
		UpdateInfo.Format(_T("ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	if(!RecvMsg(ServerReturn_UTF8))
	{
		UpdateInfo.Format(_T("���շ���������ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	if(ServerReturn_UTF8.IsEmpty())
	{
		AfxMessageBox(_T("Login Function Say:\n���󣺷������޷��أ����Ժ�����"),MB_ICONERROR);
		UpdateInfo.Format(_T("�������޷��أ����Ժ�����"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}

	UpdateInfo.Format(_T("�ɹ�"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	CloseSocket();
	AfxMessageBox(_T("Login Function Say:\n�ף���¼�ɹ�~!"),MB_ICONINFORMATION);

	UpdateInfo.Format(_T("��¼�ɹ�."));

	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
	return true;
}

//������ע
bool NetWork::TraversalFollow(CString URL)
{
	IP = "180.149.139.248";
	ResetSocket();
	DebugInfo.Format(_T("\n\n---------��ʼ������ע-------\n\n"));
	DebugOut.Write(DebugInfo,DebugInfo.GetLength());
	DebugOut.Flush();

	UpdateInfo.Format(_T("��ʼ������ע......"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	if(!tools.SearchString(URL,"follow"))
	{
		UpdateInfo.Format(_T("���󣺵�ַ��Ч"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		AfxMessageBox(_T("TraversalFollow Function Say:\n���������ַò�Ʋ��ǹ�ע�б�ĵ�ַ���봫�ݹ�ע�б�ĵ�ַ"),MB_ICONWARNING);
		return false;
	}

	CString Request,ServerReturn_UTF8,ServerReturn_ANSI;
	//ȥ��ǰ׺
	URL.Replace("http://weibo.cn",NULL);

	Request = "GET " + URL + " HTTP/1.1\r\n"
		"Host:weibo.cn\r\n"
		"User-Agent:Mozilla/5.0 (Windows NT 6.1; rv:16.0) Gecko/20100101 Firefox/16.0\r\n"
		"Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
		"Accept-Language:zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3\r\n"
		"Referer:http://weibo.cn/\r\n"
		"Cookie:"+m_cookie+";\r\n\r\n";

	UpdateInfo.Format(_T("���������ȡ��ҳ��......"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	if(!SendMsg(Request))
	{
		UpdateInfo.Format(_T("��������ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	if(!RecvMsg(ServerReturn_UTF8))
	{
		UpdateInfo.Format(_T("���շ���������ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}

	if(ServerReturn_UTF8.IsEmpty())
	{
		UpdateInfo.Format(_T("���շ���������ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		AfxMessageBox(_T("TraversalFollow Function Say:\n���󣺷������޷��أ����Ժ�����"),MB_ICONWARNING);
		return false;
	}

	UpdateInfo.Format(_T("��ȡ��ҳ�ɹ�\n����ת����......."));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
	ServerReturn_ANSI = tools.UTF8ToANSI(ServerReturn_UTF8);

	if(ServerReturn_ANSI.IsEmpty())
	{
		UpdateInfo.Format(_T("����ת��ʧ�ܣ�UTF8ToANSI�������ؿ��ַ���"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		AfxMessageBox(_T("TraversalFollow Function Say:\n����UTF8ToANSI�������ؿ��ַ���"),MB_ICONERROR);
		return false;
	}

	UpdateInfo.Format(_T("�ɹ�"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
	//���Լ���
	int count = 0;
	while(!tools.SearchString(ServerReturn_ANSI,"HTTP/1.1 200 OK"))
	{
		++count;
		if(5 > count)
		{
			UpdateInfo.Format(_T("��ȡ���ʧ��"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			AfxMessageBox(_T("TraversalFollow Function Say:\n���Դ������࣬���Ժ����ԣ����߻����ʺ�"));
			return false;
		}
		int Res;
		Res = AfxMessageBox(_T("TraversalFollow Function Say:\n���������ؽ�����������ˣ��Ƿ��Ժ����·������ԣ�"),MB_YESNO);
		if(IDYES == Res)
		{
			//������������Ļ���һ�㶼���ٶ�̫������ģ�����ʱ��Ӧ�ó�һ��
			tools.Sleep();
			tools.Sleep();
			tools.Sleep();

			ResetSocket();
			if(!SendMsg(Request))
			{
				return false;
			}
			if(!RecvMsg(ServerReturn_UTF8))
			{
				return false;
			}
			if(ServerReturn_UTF8.IsEmpty())
			{
				UpdateInfo.Format(_T("���շ���������ʧ��"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				AfxMessageBox(_T("TraversalFollow Function Say:\n���󣺷������޷��أ����Ժ�����"),MB_ICONERROR);
				return false;
			}
			ServerReturn_ANSI = tools.UTF8ToANSI(ServerReturn_UTF8);
			if(ServerReturn_ANSI.IsEmpty())
			{
				UpdateInfo.Format(_T("����ת��ʧ�ܣ�UTF8ToANSI�������ؿ��ַ���"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				AfxMessageBox(_T("TraversalFollow Function Say:\n����UTF8ToANSI�������ؿ��ַ���"),MB_ICONERROR);
				return false;
			}
		}
		else
		{
			return false;
		}
	}


	DebugOut.Write(ServerReturn_ANSI,ServerReturn_ANSI.GetLength());

	UpdateInfo.Format(_T("���ҳ����......"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
	//���ҳ��
	CString All_Page,Page_Temp;
	if(!tools.ParseString(ServerReturn_ANSI,Page_Temp,";\\d{1,}.\\d{1,}.</div>"))
	{
		UpdateInfo.Format(_T("��ȡҳ��ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	if(!tools.ParseString(Page_Temp,All_Page,"/\\d{1,}ҳ"))
	{
		UpdateInfo.Format(_T("��ȡҳ��ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	All_Page.Replace("/",NULL);
	All_Page.Replace("ҳ",NULL);
	Follow_Page = atoi(All_Page.GetBuffer());

	CString Reg_res;
	CString Follow_URL,Follow_UID;

	//��ŷָ�>�ַ�������
	unsigned int unt= 0;
	unsigned int Now_Page = 1;

	UpdateInfo.Format(_T("���ҳ���ɹ���ҳ��Ϊ:%d"),Follow_Page);
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
	//��ŷָ����ַ���
	std::vector<CString> SplitStringReturn;
	std::vector<CString>::iterator vec_iter;

	//���ϵͳʱ��
	COleDateTime;
	COleDateTime datetime;  
	datetime=COleDateTime::GetCurrentTime();                                                                               
	SystemTime=datetime.Format("%Y��%m��%d��%Hʱ%M��   "); 
	DebugInfo.Format(_T("\n-------��ʼ������ע����ǰʱ�䣺%s----------\n"),SystemTime);
	m_Result.Write(DebugInfo,DebugInfo.GetLength());

	while(Now_Page < Follow_Page)
	{
		//���m_FollowList��ÿ����һҳдһ���ļ�����ʡ�ڴ�
		m_FollowList.clear();

		while(true)
		{
			//˯��һ��
			tools.Sleep();

			//ɾ��������ַ�
			ServerReturn_ANSI.Replace("?vt=4&amp;gsid=3_5afe4404b77a34da8b085446a50e8af47308&amp;st=5e0f",NULL);
			tools.RemoveString(ServerReturn_ANSI,".st=\\w{1,}");

			//��ù�ע���������
			//��ʽweibo.cn\username>�ǳ�<
			//������weibo.cn\u\userid>�ǳ�<
			if(!tools.ParseString(ServerReturn_ANSI,Reg_res,"weibo.cn...\\w{1,30}\">[^<].+?<",0))
			{
				//��ʱ�򣬼�ʹ��HTTP 200 OK�ķ���ֵ��ҳ��Ҳ�п����ǿյģ���û������
				//�����Ļ����͵�������һҳ����������������������º����ҳ�治�ᱻ��ȡ
				DebugInfo.Format(_T("TraversalFollw Function����%dҳ��ȡ�������ݣ���ת��һҳ"),Now_Page);
				DebugOut.Write(DebugInfo,DebugInfo.GetLength());
				DebugOut.Flush();

				CString JumpNextPage;
				JumpNextPage.Format("?page=%d",++Now_Page);

				if(Now_Page > Follow_Page)
				{
					break;
				}

				UpdateInfo.Format(_T("��ǰ������%dҳ����%dҳ"),Now_Page,Follow_Page);
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

				JumpNextPage = URL+JumpNextPage;
				Request = "GET " + JumpNextPage + " HTTP/1.1\r\n"
					"Host:weibo.cn\r\n"
					"User-Agent:Mozilla/5.0 (Windows NT 6.1; rv:16.0) Gecko/20100101 Firefox/16.0\r\n"
					"Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
					"Accept-Language:zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3\r\n"
					"Connection:keep-alive\r\n"
					//���ĸ�ҳ����ת�ģ���Ȼ���������ûʲô��ϵ������Ϊ�˷�ֹ��Ī������Ĵ��󣬰���������Ϊ��
					"Referer:http://weibo.cn/" +JumpNextPage +"\r\n"
					"Cookie:"+m_cookie+";\r\n\r\n";


				UpdateInfo.Format(_T("����������......"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				ResetSocket();
				if(!SendMsg(Request))
				{
					UpdateInfo.Format(_T("��������ʧ��"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					return false;
				}
				if(!RecvMsg(ServerReturn_UTF8))
				{
					UpdateInfo.Format(_T("���շ���������ʧ��"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					return false;
				}

				if(ServerReturn_UTF8.IsEmpty())
				{
					UpdateInfo.Format(_T("���շ���������ʧ��"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					AfxMessageBox(_T("TraversalFollow Function Say:\n���󣺷������޷��أ����Ժ�����"),MB_ICONWARNING);
					return false;
				}

				UpdateInfo.Format(_T("��ȡ��ҳ�ɹ�\n��ʼ����ת��......"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

				ServerReturn_ANSI = tools.UTF8ToANSI(ServerReturn_UTF8);
				if(ServerReturn_ANSI.IsEmpty())
				{
					UpdateInfo.Format(_T("����ת��ʧ��,UTF8ToANSI�������ؿ��ַ���"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					AfxMessageBox(_T("TraversalFollow Function Say:\n����UTF8ToANSI�������ؿ��ַ���"),MB_ICONERROR);
					return false;
				}
			}
			else
			{
				break;
			}
		}

		Reg_res.Replace("\"",NULL);


		UpdateInfo.Format(_T("��ʼ������ҳ����"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

		//�ָ��ַ���
		SplitStringReturn = tools.SplitString(Reg_res);


		if(SplitStringReturn.empty())
		{
			UpdateInfo.Format(_T("��ҳ���ݽ���ʧ��"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			return false;
		}
		vec_iter = SplitStringReturn.begin();
		while(vec_iter != SplitStringReturn.end())
		{
			//�ҵ��ָ��
			unt = vec_iter->Find(">");
			//���󵽷ָ���֮�������Ϊ�û�url
			Follow_URL = vec_iter->Left(unt);
			//�ӷָ���������Ϊ�û��ǳ�
			Follow_UID = vec_iter->Mid(unt);
			m_FollowList.insert(std::make_pair(Follow_URL,Follow_UID));
			++vec_iter;
		}

		m_iter = m_FollowList.begin();
		CString Out;
		while(m_iter != m_FollowList.end())
		{
			Out.Format("\n�û��ǳƣ�%s ----  �û�URL��%s",m_iter->second,m_iter->first);
			m_Result.Write(Out,Out.GetLength());
			++m_iter;
		}

		CString JumpNextPage;
		//���ж���ҳ���Ƿ�Ϸ���֮ǰ��whileѭ���п��ܽ�ҳ�����������
		if(Now_Page > Follow_Page)
		{
			break;
		}


		JumpNextPage.Format("?page=%d",++Now_Page);
		JumpNextPage = URL+JumpNextPage;
		Request = "GET " + JumpNextPage + " HTTP/1.1\r\n"
			"Host:weibo.cn\r\n"
			"User-Agent:Mozilla/5.0 (Windows NT 6.1; rv:16.0) Gecko/20100101 Firefox/16.0\r\n"
			"Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
			"Accept-Language:zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3\r\n"
			"Connection:keep-alive\r\n"
			//���ĸ�ҳ����ת�ģ���Ȼ���������ûʲô��ϵ������Ϊ�˷�ֹ��Ī������Ĵ��󣬰���������Ϊ��
			"Referer:http://weibo.cn/" +JumpNextPage +"\r\n"
			"Cookie:"+m_cookie+";\r\n\r\n";


		UpdateInfo.Format(_T("��ǰ������%dҳ����%dҳ"),Now_Page,Follow_Page);
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

		ResetSocket();
		if(!SendMsg(Request))
		{
			UpdateInfo.Format(_T("��������ʧ��"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			return false;
		}
		if(!RecvMsg(ServerReturn_UTF8))
		{
			UpdateInfo.Format(_T("���շ���������ʧ��"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			return false;
		}

		if(ServerReturn_UTF8.IsEmpty())
		{
			UpdateInfo.Format(_T("���շ���������ʧ��"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			AfxMessageBox(_T("TraversalFollow Function Say:\n���󣺷������޷��أ����Ժ�����"),MB_ICONWARNING);
			return false;
		}

		UpdateInfo.Format(_T("��ȡ��ҳ�ɹ�\n��ʼ����ת��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

		ServerReturn_ANSI = tools.UTF8ToANSI(ServerReturn_UTF8);

		if(ServerReturn_ANSI.IsEmpty())
		{
			UpdateInfo.Format(_T("����ת��ʧ��,UTF8ToANSI�������ؿ��ַ���"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			AfxMessageBox(_T("TraversalFollow Function Say:\n����UTF8ToANSI�������ؿ��ַ���"),MB_ICONERROR);
			return false;
		}

		//�������Լ���
		count = 0;
		while(!tools.SearchString(ServerReturn_ANSI,"HTTP/1.1 200 OK"))
		{
			++count;
			if(5 > count)
			{
				UpdateInfo.Format(_T("���Դ������࣬���Ժ����ԣ����߻����ʺ�"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				AfxMessageBox(_T("TraversalFollow Function Say:\n���Դ������࣬���Ժ����ԣ����߻����ʺ�"));
				return false;
			}
			int Res;
			Res = AfxMessageBox(_T("TraversalFollow Function Say:\n���������ؽ�����������ˣ��Ƿ��Ժ����·������ԣ�"),MB_YESNO);
			if(IDYES == Res)
			{
				//һ�����������ⶼ�Ƿ��ʹ��죬˯�߾õ�
				tools.Sleep();
				tools.Sleep();
				tools.Sleep();
				ResetSocket();
				UpdateInfo.Format(_T("����������......"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				if(!SendMsg(Request))
				{
					UpdateInfo.Format(_T("��������ʧ��"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					return false;
				}
				if(!RecvMsg(ServerReturn_UTF8))
				{
					UpdateInfo.Format(_T("���շ���������ʧ��"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					return false;
				}
				if(ServerReturn_UTF8.IsEmpty())
				{
					UpdateInfo.Format(_T("���շ���������ʧ��"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					AfxMessageBox(_T("TraversalFollow Function Say:\n���󣺷������޷��أ����Ժ�����"),MB_ICONERROR);
					return false;
				}

				UpdateInfo.Format(_T("��ȡ��ҳ�ɹ�\n��ʼ����ת��......"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

				ServerReturn_ANSI = tools.UTF8ToANSI(ServerReturn_UTF8);
				if(ServerReturn_ANSI.IsEmpty())
				{
					UpdateInfo.Format(_T("����ת��ʧ�ܣ�UTF8ToANSI�������ؿ��ַ���"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					AfxMessageBox(_T("TraversalFollow Function Say:\n����UTF8ToANSI�������ؿ��ַ���"),MB_ICONERROR);
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}
	CloseSocket();
	UpdateInfo.Format(_T("������ע���"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
	return true;
}

//������˿
bool NetWork::TraversalFans(CString URL)
{
	IP = "180.149.139.248";
	DebugInfo.Format(_T("\n-----------��ʼ������˿--------\n"));
	DebugOut.Write(DebugInfo,DebugInfo.GetLength());

	UpdateInfo.Format(_T("��ʼ������˿"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	ResetSocket();

	if(!tools.SearchString(URL,"fans"))
	{
		UpdateInfo.Format(_T("���󣺵�ַ��Ч"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		AfxMessageBox(_T("TreaversalFans Function Say:\n���������ַò�Ʋ��Ƿ�˿�б��ַ���봫�ݷ�˿�б��ַ"),MB_ICONERROR);
		return false;
	}

	CString ServerReturn_UTF8,ServerReturn_ANSI,Request;
	//ɾ��http://weibo.cn��ǰ׺
	URL.Replace("http://weibo.cn",NULL);

	Request  = "GET " + URL + " HTTP/1.1\r\n"
		"Host:weibo.cn\r\n"
		"User-Agent:Mozilla/5.0 (Windows NT 6.1; rv:16.0) Gecko/20100101 Firefox/16.0\r\n"
		"Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
		"Accept-Language:zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3\r\n"
		"Referer:http://weibo.cn/\r\n"
		"Cookie:"+m_cookie+";\r\n\r\n";

	UpdateInfo.Format(_T("����������......"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	if(!SendMsg(Request))
	{
		UpdateInfo.Format(_T("��������ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	if(!RecvMsg(ServerReturn_UTF8))
	{
		UpdateInfo.Format(_T("���շ���������ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}

	if(ServerReturn_UTF8.IsEmpty())
	{
		UpdateInfo.Format(_T("���շ���������ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		AfxMessageBox(_T("TreaversalFans Function Say:\n���󣺷������޷��أ����Ժ�����"),MB_ICONERROR);
		return false;
	}

	UpdateInfo.Format(_T("��ȡ��ҳ�ɹ�����ʼ����ת��"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	ServerReturn_ANSI = tools.UTF8ToANSI(ServerReturn_UTF8);
	if(ServerReturn_ANSI.IsEmpty())
	{
		UpdateInfo.Format(_T("����ת��ʧ�ܣ�UTF8ToANSI�������ؿ��ַ���"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		AfxMessageBox(_T("TreaversalFans Function Say:\n����UTF8ToANSI�������ؿ��ַ���"),MB_ICONERROR);
		return false;
	}
	//�������ü�����
	unsigned int count = 0;
	while(!tools.SearchString(ServerReturn_ANSI,"HTTP/1.1 200 OK"))
	{
		++count;
		if(5 > count)
		{
			UpdateInfo.Format(_T("��ȡ���ʧ��"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			AfxMessageBox(_T("TreaversalFans Function Say:\n�������Դ������࣬���Ժ����ԣ����߻����ʺ�"));
			return false;
		}
		int Res;
		Res = AfxMessageBox(_T("TreaversalFans Function Say:\n�������ķ��ؽ�����������ˣ��Ƿ��Ժ����·�������?"),MB_YESNO);
		if(IDYES == Res)
		{
			//˯�õ�
			tools.Sleep();
			tools.Sleep();
			tools.Sleep();

			ResetSocket();
			UpdateInfo.Format(_T("����������......"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

			if(!SendMsg(Request))
			{
				UpdateInfo.Format(_T("��������ʧ��"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				return false;
			}
			if(!RecvMsg(ServerReturn_UTF8))
			{
				UpdateInfo.Format(_T("���շ���������ʧ��"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				return false;
			}
			if(ServerReturn_UTF8.IsEmpty())
			{
				UpdateInfo.Format(_T("���շ���������ʧ��"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				AfxMessageBox(_T("TraversalFans Function Say:\n���󣺷������޷��أ����Ժ�����"),MB_ICONERROR);
				return false;
			}
			ServerReturn_ANSI = tools.UTF8ToANSI(ServerReturn_UTF8);
			if(ServerReturn_ANSI.IsEmpty())
			{
				UpdateInfo.Format(_T("����ת��ʧ�ܣ�UTF8ToANSI�������ؿ��ַ���"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				AfxMessageBox(_T("TraversalFans Function Say:\n����UTF8ToANSI�������ؿ��ַ���"),MB_ICONERROR);
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	DebugOut.Write(ServerReturn_ANSI,ServerReturn_ANSI.GetLength());

	UpdateInfo.Format(_T("��ʼ��ȡ��˿ҳ��......"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	//��ȡҳ��
	CString All_Page,Page_Temp;
	if(!tools.ParseString(ServerReturn_ANSI,Page_Temp,"\\w{1,}/\\w{1,}ҳ"))
	{
		UpdateInfo.Format(_T("��ȡҳ��ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		AfxMessageBox(_T("TreaversalFans Function Say:\n���󣺻�ȡҳ����ʧ��"),MB_ICONERROR);
		return false;
	}
	if(!tools.ParseString(Page_Temp,All_Page,"/\\d{1,}ҳ"))
	{
		UpdateInfo.Format(_T("��ȡҳ��ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	All_Page.Replace("/",NULL);
	All_Page.Replace("ҳ",NULL);
	Fans_Page = atoi(All_Page.GetBuffer());
	//����ַ���
	All_Page.Empty();

	UpdateInfo.Format(_T("��ȡҳ���ɹ�����%dҳ"),Fans_Page);
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	if(Fans_Page > 300)
	{
		CString Out;
		int Res;
		UpdateInfo.Format(_T("��˿ҳ�����ࣺ%d�����ܻ�Ӱ��Ч��"),Fans_Page);
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		Out.Format(_T("��˿ҳ�����ࣺ%d  ����ʱ����ܻ�ܳ����Ƿ����������"),Fans_Page);
		Res = AfxMessageBox(Out,MB_YESNO);
		if(IDNO == Res)
		{
			UpdateInfo.Format(_T("������˿��ȡ��"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			DebugInfo.Format(_T("�û�����ȡ��������˿"));
			DebugOut.Write(DebugInfo,DebugInfo.GetLength());
			return true;
		}
	}

	CString Reg_res;
	CString Fans_URL,Fans_UID;

	//��ŷָ�>�ַ�������
	unsigned int unt= 0;
	unsigned int Now_Page = 1;

	//��ŷָ����ַ���
	std::vector<CString> SplitStringReturn;
	std::vector<CString>::iterator vec_iter;

	//���ϵͳʱ��
	COleDateTime;
	COleDateTime datetime;  
	datetime=COleDateTime::GetCurrentTime();                                                                               
	SystemTime=datetime.Format("%Y��%m��%d��%Hʱ%M��   "); 
	DebugInfo.Format(_T("\n-------��ʼ������˿����ǰʱ�䣺%s----------\n"),SystemTime);
	m_Result.Write(DebugInfo,DebugInfo.GetLength());

	while(Now_Page < Fans_Page)
	{
		//���m_FansList��ÿ����һҳдһ���ļ�����ʡ�ڴ�
		m_FansList.clear();

		while(true)
		{
			tools.Sleep();
			//ɾ��������ַ�
			tools.RemoveString(ServerReturn_ANSI,".st=\\w{1,}");

			UpdateInfo.Format(_T("��ʼ������ҳ"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			//������˿���ݣ���ʽ���עһ��
			if(!tools.ParseString(ServerReturn_ANSI,Reg_res,"weibo.cn...\\w{1,30}\">[^<].+?<",0))
			{
				//��ʱ�򣬼�ʹ��HTTP 200 OK�ķ���ֵ��ҳ��Ҳ�п����ǿյģ���û������
				//�����Ļ����͵�������һҳ����������������������º����ҳ�治�ᱻ��ȡ
				DebugInfo.Format(_T("TraversalFans Function����%dҳ��ȡ�������ݣ���ת��һҳ"),Now_Page);
				DebugOut.Write(DebugInfo,DebugInfo.GetLength());
				DebugOut.Flush();

				CString JumpNextPage;
				JumpNextPage.Format("?page=%d",++Now_Page);

				if(Now_Page > Fans_Page)
				{
					break;
				}

				UpdateInfo.Format(_T("��ǰҳ%d����ʧ�ܣ���ת����һҳ"),Now_Page);
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

				JumpNextPage = URL+JumpNextPage;
				Request = "GET " + JumpNextPage + " HTTP/1.1\r\n"
					"Host:weibo.cn\r\n"
					"User-Agent:Mozilla/5.0 (Windows NT 6.1; rv:16.0) Gecko/20100101 Firefox/16.0\r\n"
					"Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
					"Accept-Language:zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3\r\n"
					//���ĸ�ҳ����ת�ģ���Ȼ���������ûʲô��ϵ������Ϊ�˷�ֹ��Ī������Ĵ��󣬰���������Ϊ��
					"Referer:http://weibo.cn/" +JumpNextPage +"\r\n"
					"Cookie:"+m_cookie+";\r\n\r\n";

				UpdateInfo.Format(_T("����������......"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

				ResetSocket();
				if(!SendMsg(Request))
				{
					UpdateInfo.Format(_T("��������ʧ��"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					return false;
				}
				if(!RecvMsg(ServerReturn_UTF8))
				{
					UpdateInfo.Format(_T("���շ���������ʧ��"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					return false;
				}

				if(ServerReturn_UTF8.IsEmpty())
				{
					UpdateInfo.Format(_T("���շ���������ʧ��"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					AfxMessageBox(_T("TraversalFans Function Say:\n���󣺷������޷��أ����Ժ�����"),MB_ICONWARNING);
					return false;
				}

				UpdateInfo.Format(_T("��ȡ��ҳ�ɹ�����ʼ����ת��"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				ServerReturn_ANSI = tools.UTF8ToANSI(ServerReturn_UTF8);
				if(ServerReturn_ANSI.IsEmpty())
				{
					UpdateInfo.Format(_T("����ת��ʧ�ܣ�UTF8ToANSI�������ؿ��ַ���"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					AfxMessageBox(_T("TraversalFansFunction Say:\n����UTF8ToANSI�������ؿ��ַ���"),MB_ICONERROR);
					return false;
				}
			}
			else
			{
				break;
			}
		}

		UpdateInfo.Format(_T("��ʼ������ҳ"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		//�ָ��ַ���
		SplitStringReturn = tools.SplitString(Reg_res);

		if(SplitStringReturn.empty())
		{
			UpdateInfo.Format(_T("������ҳʧ��"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			return false;
		}

		vec_iter = SplitStringReturn.begin();
		while(vec_iter != SplitStringReturn.end())
		{
			//�������ݵķ������ע��ͬ
			unt = vec_iter->Find(">");
			Fans_URL = vec_iter->Left(unt);
			Fans_UID = vec_iter->Mid(unt);
			m_FansList.insert(std::make_pair(Fans_URL,Fans_UID));
			++vec_iter;
		}

		m_iter = m_FansList.begin();
		CString Out;
		while(m_iter != m_FansList.end())
		{
			Out.Format("\n�û��ǳƣ�%s ----  �û�URL��%s\n",m_iter->second,m_iter->first);
			m_Result.Write(Out,Out.GetLength());
			++m_iter;
		}


		CString JumpNextPage;
		//�ж�ҳ���Ƿ�Ϸ�
		if(Now_Page > Fans_Page)
		{
			break;
		}

		UpdateInfo.Format(_T("���ڱ�����˿����ǰҳ%d����%d"),Now_Page,Fans_Page);
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

		JumpNextPage.Format("?page=%d",++Now_Page);
		JumpNextPage = URL+JumpNextPage;
		Request = "GET " + JumpNextPage + " HTTP/1.1\r\n"
			"Host:weibo.cn\r\n"
			"User-Agent:Mozilla/5.0 (Windows NT 6.1; rv:16.0) Gecko/20100101 Firefox/16.0\r\n"
			"Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
			"Accept-Language:zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3\r\n"
			//���ĸ�ҳ����ת�ģ���Ȼ���������ûʲô��ϵ������Ϊ�˷�ֹ��Ī������Ĵ��󣬰���������Ϊ��
			"Referer:http://weibo.cn/" +JumpNextPage +"\r\n"
			"Cookie:"+m_cookie+";\r\n\r\n";


		UpdateInfo.Format(_T("����������......"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);


		ResetSocket();
		if(!SendMsg(Request))
		{
			UpdateInfo.Format(_T("��������ʧ��"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			return false;
		}
		if(!RecvMsg(ServerReturn_UTF8))
		{
			UpdateInfo.Format(_T("���շ���������ʧ��"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			return false;
		}

		if(ServerReturn_UTF8.IsEmpty())
		{
			UpdateInfo.Format(_T("���շ���������ʧ��"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			AfxMessageBox(_T("TraversalFans Function Say:\n���󣺷������޷��أ����Ժ�����"),MB_ICONWARNING);
			return false;
		}


		UpdateInfo.Format(_T("��ȡ��ҳ�ɹ�����ʼ����ת��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

		ServerReturn_ANSI = tools.UTF8ToANSI(ServerReturn_UTF8);

		if(ServerReturn_ANSI.IsEmpty())
		{
			UpdateInfo.Format(_T("����ת��ʧ�ܣ�UTF8ToANSI�������ؿ��ַ���"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			AfxMessageBox(_T("TraversalFans Function Say:\n����UTF8ToANSI�������ؿ��ַ���"),MB_ICONERROR);
			return false;
		}

		//�������Լ���
		count = 0;
		while(!tools.SearchString(ServerReturn_ANSI,"HTTP/1.1 200 OK"))
		{
			++count;
			if(5 > count)
			{
				UpdateInfo.Format(_T("���Դ������࣬���Ժ����ԣ����߻����ʺ�"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				AfxMessageBox(_T("TraversalFoans Function Say:\n���Դ������࣬���Ժ����ԣ����߻����ʺ�"));
				return false;
			}
			int Res;
			Res = AfxMessageBox(_T("TraversalFans Function Say:\n���������ؽ�����������ˣ��Ƿ��Ժ����·������ԣ�"),MB_YESNO);
			if(IDYES == Res)
			{
				//˯�õ�
				tools.Sleep();
				tools.Sleep();
				tools.Sleep();
				ResetSocket();

				UpdateInfo.Format(_T("����������......"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

				if(!SendMsg(Request))
				{
					UpdateInfo.Format(_T("��������ʧ��"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					return false;
				}

				if(!RecvMsg(ServerReturn_UTF8))
				{
					UpdateInfo.Format(_T("���շ���������ʧ��"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					return false;
				}
				if(ServerReturn_UTF8.IsEmpty())
				{
					UpdateInfo.Format(_T("���շ���������ʧ��"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					AfxMessageBox(_T("TraversalFans Function Say:\n���󣺷������޷��أ����Ժ�����"),MB_ICONERROR);
					return false;
				}

				UpdateInfo.Format(_T("��ȡ��ҳ�ɹ�����ʼ����ת��"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

				ServerReturn_ANSI = tools.UTF8ToANSI(ServerReturn_UTF8);
				if(ServerReturn_ANSI.IsEmpty())
				{
					UpdateInfo.Format(_T("����ת��ʧ�ܣ�UTF8ToANSI�������ؿ��ַ���"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					AfxMessageBox(_T("TraversalFans Function Say:\n����UTF8ToANSI�������ؿ��ַ���"),MB_ICONERROR);
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}
	CloseSocket();
	UpdateInfo.Format(_T("������˿���"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
	return true;
}

//����΢��
bool NetWork::TraversalWeibo(CString URL)
{
	IP = "180.149.153.216";
	//����socket
	ResetSocket();

	UpdateInfo.Format(_T("��ʼ����΢��"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	URL.Replace("http://weibo.cn",NULL);

	CString ServerReturn_ANSI,ServerReturn_UTF8,Request;

	Request = "GET " + URL + " HTTP/1.1\r\n"
		"Host:weibo.cn\r\n"
		"User-Agent:Mozilla/5.0 (Windows NT 6.1; rv:16.0) Gecko/20100101 Firefox/16.0\r\n"
		"Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
		"Accept-Language:zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3\r\n"
		"Referer:http://weibo.cn/\r\n"
		"Cookie:"+m_cookie+";\r\n\r\n";

	UpdateInfo.Format(_T("����������....."));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	if(!SendMsg(Request))
	{
		UpdateInfo.Format(_T("��������ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	if(!RecvMsg(ServerReturn_UTF8))
	{
		UpdateInfo.Format(_T("���շ���������ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	if(ServerReturn_UTF8.IsEmpty())
	{
		UpdateInfo.Format(_T("���շ���������ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		AfxMessageBox(_T("TreaversalWeibo Function Say:\n���󣺷������޷��أ����Ժ�����"),MB_ICONERROR);
		return false;
	}

	UpdateInfo.Format(_T("��ȡ��ҳ�ɹ�����ʼ����ת��....."));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	//ת��
	ServerReturn_ANSI  = tools.UTF8ToANSI(ServerReturn_UTF8);
	if(ServerReturn_ANSI.IsEmpty())
	{
		UpdateInfo.Format(_T("����ת��ʧ�ܣ�UTF8ToANSI�������ؿ��ַ���"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		AfxMessageBox(_T("TreaversalWeibo Function Say:\nUTF8ToANSI�������ؿ��ַ���"),MB_ICONWARNING);
		return false;
	}
	//���Լ�����������һֱ����
	unsigned int count = 0;
	//�жϷ������Ƿ�ɹ�����
	while(!tools.SearchString(ServerReturn_ANSI,"HTTP/1.1 200 OK"))
	{
		UpdateInfo.Format(_T("��ȡ��ҳ����ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		++count;
		if(5 > count)
		{
			UpdateInfo.Format(_T("���Դ������࣬���Ժ����ԣ����߻����ʺ�"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			AfxMessageBox(_T("TreaversalWeibo Function Say:\n���Դ������࣬���Ժ����ԣ����߻����ʺ�"));
			return false;
		}
		int Res;
		Res = AfxMessageBox(_T("TreaversalWeibo Function Say:\n���������ؽ�����������ˣ��Ƿ��Ժ����·������ԣ�"),MB_YESNO);
		if(IDYES == Res)
		{

			//˯�õ�
			tools.Sleep();
			tools.Sleep();
			tools.Sleep();
			ResetSocket();
			UpdateInfo.Format(_T("����������......"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

			if(!SendMsg(Request))
			{
				UpdateInfo.Format(_T("��������ʧ��"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				return false;
			}

			if(!RecvMsg(ServerReturn_UTF8))
			{
				UpdateInfo.Format(_T("���շ���������ʧ��"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				return false;
			}
			if(ServerReturn_UTF8.IsEmpty())
			{
				UpdateInfo.Format(_T("���շ��񷵻�ʧ��"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				AfxMessageBox(_T("TreaversalWeibo Function Say:\n���󣺷������޷��أ����Ժ�����"),MB_ICONERROR);
				return false;
			}

			UpdateInfo.Format(_T("��ȡ��ҳ�ɹ�����ʼ����ת��"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

			ServerReturn_ANSI = tools.UTF8ToANSI(ServerReturn_UTF8);
			if(ServerReturn_ANSI.IsEmpty())
			{
				UpdateInfo.Format(_T("����ת��ʧ�ܣ�UTF8ToANSI�������ؿ��ַ���"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				AfxMessageBox(_T("TreaversalWeibo Function Say:\n����UTF8ToANSI�������ؿ��ַ���"),MB_ICONERROR);
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	//΢������ҳ����<span class=.ctt.>���������Դ����ж�
	if(!tools.SearchString(ServerReturn_ANSI,"<span class=.ctt.>"))
	{
		UpdateInfo.Format(_T("���󣺵�ַ��Ч"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		AfxMessageBox(_T("TreaversalWeibo Function Say:\n���������ַò�Ʋ���΢���б��봫��΢���б��ַ"),MB_ICONWARNING);
		return false;
	}

	DebugOut.Write(ServerReturn_ANSI,ServerReturn_ANSI.GetLength());

	UpdateInfo.Format(_T("��ʼ��ȡ΢��ҳ��......"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	//���΢��ҳ��
	CString All_Page,Page_Temp;
	if(!tools.ParseString(ServerReturn_ANSI,Page_Temp,";\\d{1,}.\\d{1,}.</div>"))
	{
		UpdateInfo.Format(_T("��ȡ΢��ҳ��ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	if(!tools.ParseString(Page_Temp,All_Page,"/\\d{1,}ҳ"))
	{
		UpdateInfo.Format(_T("��ȡ΢��ҳ��ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	All_Page.Replace("/",NULL);
	All_Page.Replace("ҳ",NULL);
	Weibo_Page = atoi(All_Page.GetBuffer());

	UpdateInfo.Format(_T("��ȡ΢��ҳ���ɹ���΢����%dҳ"),Weibo_Page);
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	UpdateInfo.Format(_T("��ʼ��ȡ΢����"));
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	//��ȡ΢����
	CString WeiboNum_String;
	if(!tools.ParseString(ServerReturn_ANSI,WeiboNum_String,"<span class=.tc.>.*?<"))
	{
		UpdateInfo.Format(_T("��ȡ΢����ʧ��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		return false;
	}
	WeiboNum_String.Replace("<span class=\"tc\">΢��",NULL);
	WeiboNum_String.Replace("[",NULL);
	WeiboNum_String.Replace("]<",NULL);
	WeiboID = atoi(WeiboNum_String.GetBuffer());
	WeiboNum_String.ReleaseBuffer();
	WeiboNum_String.Empty();

	UpdateInfo.Format(_T("��ȡ΢�����ɹ���΢����%d��"),WeiboID);
	SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

	//��ŷָ���΢����ÿ��΢��ռһ��CString
	std::vector<CString> SplitStringReturn;
	std::vector<CString>::iterator vec_iter;


	//���ϵͳʱ��
	COleDateTime;
	COleDateTime datetime;  
	datetime=COleDateTime::GetCurrentTime();                                                                               
	SystemTime=datetime.Format("%Y��%m��%d��%Hʱ%M��   "); 
	DebugInfo.Format(_T("\n-------��ʼ����΢������ǰʱ�䣺%s----------\n"),SystemTime);
	m_Result.Write(DebugInfo,DebugInfo.GetLength());

	CString Reg_res;
	unsigned int Now_Page = 0;

	//��Ϊ10/10������ҳ���ǺϷ��ģ�����Ϊ�˱����ٻ�ȡһҳ��ʹ��<=
	while(Now_Page <= Weibo_Page)
	{
		//ÿ����һҳдһ�Σ������map����ʡ�ڴ�
		m_WeiboList.clear();
		tools.Sleep();
		while(true)
		{
			CString JumpNextPage;
			JumpNextPage.Format("?page=%d",++Now_Page);

			if(Now_Page > Weibo_Page)
			{
				break;
			}
			UpdateInfo.Format(_T("���ڱ���΢������ǰҳ%d����%dҳ"),Now_Page,Weibo_Page);
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

			JumpNextPage = URL+JumpNextPage;
			Request = "GET " + JumpNextPage + " HTTP/1.1\r\n"
				"Host:weibo.cn\r\n"
				"User-Agent:Mozilla/5.0 (Windows NT 6.1; rv:16.0) Gecko/20100101 Firefox/16.0\r\n"
				"Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
				"Accept-Language:zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3\r\n"
				"Connection:keep-alive\r\n"
				//���ĸ�ҳ����ת�ģ���Ȼ���������ûʲô��ϵ������Ϊ�˷�ֹ��Ī������Ĵ��󣬰���������Ϊ��
				"Referer:http://weibo.cn/" +JumpNextPage +"\r\n"
				"Cookie:"+m_cookie+";\r\n\r\n";

			UpdateInfo.Format(_T("����������......"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

			ResetSocket();
			if(!SendMsg(Request))
			{			
				UpdateInfo.Format(_T("��������ʧ��"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				return false;
			}
			if(!RecvMsg(ServerReturn_UTF8))
			{
				UpdateInfo.Format(_T("���շ���������ʧ��"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				return false;
			}

			if(ServerReturn_UTF8.IsEmpty())
			{
				UpdateInfo.Format(_T("���շ���������ʧ��"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				AfxMessageBox(_T("TraversalWeibo Function Say:\n���󣺷������޷��أ����Ժ�����"),MB_ICONWARNING);
				return false;
			}

			UpdateInfo.Format(_T("��ȡҳ��ɹ�����ʼ����ת��"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);

			ServerReturn_ANSI = tools.UTF8ToANSI(ServerReturn_UTF8);
			if(ServerReturn_ANSI.IsEmpty())
			{
				UpdateInfo.Format(_T("����ת��ʧ�ܣ�UTF8ToANSI�������ؿ��ַ���"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				AfxMessageBox(_T("TraversalWeibo Function Say:\n����UTF8ToANSI�������ؿ��ַ���"),MB_ICONERROR);
				return false;
			}

			//�ж��Ƿ��ܹ��ɹ���ȡ��΢������
			if(!tools.ParseString(ServerReturn_ANSI,Reg_res,"<div class=\"c\" id=\".*?\"><div><span class=\".*?\">.+?</span></div></div>",0))
			{
				//��ʱ�򣬼�ʹ��HTTP 200 OK�ķ���ֵ��ҳ��Ҳ�п����ǿյģ���û������
				//�����Ļ����͵�������һҳ����������������������º����ҳ�治�ᱻ��ȡ
				DebugInfo.Format(_T("TraversalWeibo Function����%dҳ��ȡ�������ݣ���ת��һҳ"),Now_Page);
				DebugOut.Write(DebugInfo,DebugInfo.GetLength());
				DebugOut.Flush();
				UpdateInfo.Format(_T("��%dҳ��ȡ�������ݣ���ת����һҳ"),Now_Page);
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			}
			else
			{
				break;
			}
		}//whileѭ���Ľ���������


		UpdateInfo.Format(_T("��ʼ����΢��"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
		//ɾ������Ҫ�ģ������Ժ���ȡ
		//�Ƴ��ö�΢������Ϣ
		//2013-2-27 BUG
		//ɾ�����ö�΢�������ᱻͳ��
		tools.RemoveString(Reg_res,"<div><img alt=\"M\" src=\"http://.+?\" />&nbsp;<a href=\"/member/pay.+?>��ͨ��Ա"
			"</a><br/>\\[<span class=\"kt\">�ö�</span>\\]<span class=\"cmt\">��Ա��Ȩ</span>&nbsp;<span class=\"ctt\">");
		//ɾ�����ޡ�������
		tools.RemoveString(Reg_res,"&nbsp;<a href=\"http://weibo.cn/attitude/.+?>");
		//ɾ����ת����������
		tools.RemoveString(Reg_res,"&nbsp;<a href=\"http://weibo.cn/repost/.+?>");
		//ɾ�������ۡ�������
		tools.RemoveString(Reg_res,"&nbsp;<a href=\"http://weibo.cn/comment/.+?>");
		//ɾ�����ղء�������
		tools.RemoveString(Reg_res,"&nbsp;<a href=\"http://weibo.cn/fav/addFav/.+?>");
		//ɾ�������⡻������
		tools.RemoveString(Reg_res,"<a href=\"http://huati.weibo.cn/.+?>");
		//ɾ����Сͼ��������
		tools.RemoveString(Reg_res,"<a href=\"http://weibo.cn/mblog/pic/.+?</a>&nbsp;");
		//ɾ�����Է���ҳ�ͻ��˵�����
		tools.RemoveString(Reg_res,"<a href=\"http://weibo.cn/sinaurl.+?>");
		//ɾ��&nbsp
		tools.RemoveString(Reg_res,"&nbsp");
		//ɾ��id
		tools.RemoveString(Reg_res,"<div class=\"c\" id=\".*?\"><div>");

		//�����ͰѾ����ǰҳ��΢����ŵ�Reg_res����ַ�����
		//Ȼ�����SplitString�ָ��ַ�������</span></div></div>Ϊ��־�ָ�ÿ��΢��
		SplitStringReturn = tools.SplitString(Reg_res,"<span class=.+?</span></div></div>");

		if(SplitStringReturn.empty())
		{
			UpdateInfo.Format(_T("΢������ʧ��"));
			SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
			return false;
		}

		//�ָ���ַ��������ĵ�����
		vec_iter = SplitStringReturn.begin();

		while(vec_iter != SplitStringReturn.end())
		{
			CString Temp;
			//����ת����΢��
			if(tools.SearchString(*vec_iter,"<span class=\"cmt\">"))
			{
				//���΢����@�����ˣ���Դ�������ʾһ��URL��Ϊ�˺ÿ�������URL�滻��@
				if(!tools.ReplaceString(*vec_iter,"@",";<a href=\"http://weibo.cn/.+?\">"))
				{
					UpdateInfo.Format(_T("���󣡣�����������@�û���URL����"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					DebugInfo.Format(_T("���󣡣�����������@�û���URL����"));
					DebugOut.Write(DebugInfo,DebugInfo.GetLength());
				}
				//ɾ���û�����ı�ʶ����v��֤��
				if(!tools.ReplaceString(*vec_iter,"","</a><img src=\".+?/>;"))
				{
					UpdateInfo.Format(_T("���󣡣����������û���ʶ����"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					DebugInfo.Format(_T("���󣡣����������û���ʶ����"));
					DebugOut.Write(DebugInfo,DebugInfo.GetLength());
				}
				//ɾ��΢������ǰ�ı�ʶ
				if(!vec_iter->Replace("</span><span class=.+?>",NULL))
				{
					UpdateInfo.Format(_T("���󣡣���������</span><span class=\"ctt\">����"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					DebugInfo.Format(_T("\n���󣡣���������</span><span class=\"ctt\">����\n"));
					DebugOut.Write(DebugInfo,DebugInfo.GetLength());
					//return false;
				}

				if(!tools.ParseString(*vec_iter,Temp,"<span class=\"cmt\">.+?<span class=\"cmt\">"))
				{
					UpdateInfo.Format(_T("���󣡣���ȡ����΢�����ģ������ԣ���"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					return false;
				}
				//ɾ��HTML���
				if(!tools.ReplaceString(Temp," ","<.+?>"))
				{
					UpdateInfo.Format(_T("���󣡣���������HTML��ǣ���"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					DebugInfo.Format(_T("���󣡣���������HTML��ǣ���"));
					DebugOut.Write(DebugInfo,DebugInfo.GetLength());
					return false;
				}
				//��������΢�����뵽WeiboInfo��TEXT�ֶ�
				WeiboInfo.Text = Temp;

				//�����ͼƬ����
				if(tools.ParseString(*vec_iter,Temp,"<a href=\"http://weibo.cn/mblog/.+?\">"))
				{
					Temp.Replace("<a href=\"",NULL);
					Temp.Replace("\"",NULL);
					WeiboInfo.PicUrl = Temp;
					WeiboInfo.HasPic = true;
				}
				//��ת���������Ϊtrue
				WeiboInfo.IsResport = true;
			}

			//�����ת����΢��
			else
			{
				//����΢�����ĸ�ʽ
				if(!tools.ParseString(*vec_iter,Temp,"<span class=\"ctt\">.+?��"))
				{
					return false;
				}
				Temp.Replace("<span class=\"ctt\">",NULL);
				Temp.Replace("��",NULL);

				//ɾ��HTML���
				if(!tools.ReplaceString(Temp," ","<.+?>"))
				{
					UpdateInfo.Format(_T("���󣡣���������HTML��ǣ���"));
					SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
					return false;
				}
				//��������΢�����뵽WeiboInfo��TEXT�ֶ�
				WeiboInfo.Text = Temp;

				//�����ͼƬ�����
				if(tools.ParseString(*vec_iter,Temp,"<a href=\"http://weibo.cn/mblog/.+?\">"))
				{
					Temp.Replace("<a href=\"",NULL);
					Temp.Replace("\"",NULL);
					WeiboInfo.PicUrl = Temp;
					WeiboInfo.HasPic = true;
				}

				WeiboInfo.IsResport = false;
			}

			//ɾ��ԭͼ����ʾ
			WeiboInfo.Text.Replace("ԭͼ",NULL);

			//��ȡת����
			if(!tools.ParseString(*vec_iter,Temp,"[^ԭ��]ת��\\[\\d{1,}\\]"))
			{
				UpdateInfo.Format(_T("���󣡣���ȡ����ת��������"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				return false;
			}
			Temp.Replace(">ת��[",NULL);
			Temp.Replace("]",NULL);
			WeiboInfo.Resport = atoi(Temp.GetBuffer());
			Temp.ReleaseBuffer();

			//��ȡ������
			if(!tools.ParseString(*vec_iter,Temp,"[^ԭ��]����\\[\\d{1,}\\]"))
			{
				UpdateInfo.Format(_T("���󣡣���ȡ��������������"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				return false;
			}
			Temp.Replace(">����[",NULL);
			Temp.Replace("]",NULL);
			WeiboInfo.Comment = atoi(Temp.GetBuffer());
			Temp.ReleaseBuffer();

			//��ȡ��Դ
			if(!tools.ParseString(*vec_iter,Temp,"����.*?<"))
			{
				UpdateInfo.Format(_T("���󣡣���ȡ������Դ����"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				return false;
			}
			Temp.Replace("����",NULL);
			Temp.Replace("<",NULL);
			WeiboInfo.From = Temp;

			//��ȡʱ��
			//2013-2-27 BUG
			//Ŀǰֻ������ҳԴ�����е�ʱ����ʾ�������ʱ��
			//���ȡ��ҳ��12�㣬��11:50������΢������ʾ10����ǰ
			//��һ��������ϵͳʱ�������м��㣬��ʾ����ʱ��
			if(!tools.ParseString(*vec_iter,Temp,"<span class=\"ct\">.+?;"))
			{
				UpdateInfo.Format(_T("���󣡣���ȡ��������ʱ�䣡��"));
				SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
				return false;
			}
			Temp.Replace("<span class=\"ct\">",NULL);
			Temp.Replace(";",NULL);
			WeiboInfo.Time = Temp;

			//������õ�΢������map��
			//��΢��ID����
			m_WeiboList.insert(std::make_pair(WeiboID,WeiboInfo));
			//WeiboIDΪ΢������������
			--WeiboID;
			++vec_iter;


		}
		std::map<unsigned int,DataStruct,std::greater<unsigned int> >::iterator  map_iter;
		map_iter = m_WeiboList.begin();
		CString out;
		while(map_iter != m_WeiboList.end())
		{
			if(map_iter->second.HasPic)
			{
				if(map_iter->second.IsResport)
				{
					out.Format("ID:%d\n΢������:%s\n�Ƿ�ת��:��\n������:%d\nת����:%d\nͼƬURL:%s\n����ʱ��:%s\n��Դ:%s\n\n",map_iter->first,map_iter->second.Text,
						map_iter->second.Comment,map_iter->second.Resport,map_iter->second.PicUrl,map_iter->second.Time,
						map_iter->second.From);
				}
				else
				{
					out.Format("ID:%d\n΢������:%s\n�Ƿ�ת��:��\n������:%d\nת����:%d\nͼƬURL:%s\n����ʱ��:%s\n��Դ:%s\n\n",map_iter->first,map_iter->second.Text,
						map_iter->second.Comment,map_iter->second.Resport,map_iter->second.PicUrl,map_iter->second.Time,
						map_iter->second.From);
				}
			}
			else
			{
				if(map_iter->second.IsResport)
				{
					out.Format("ID:%d\n΢������:%s\n�Ƿ�ת��:��\n������:%d\nת����:%d\n\n����ʱ��:%s\n��Դ:%s\n\n",map_iter->first,map_iter->second.Text,
						map_iter->second.Comment,map_iter->second.Resport,map_iter->second.Time,
						map_iter->second.From);
				}
				else
				{
					out.Format("ID:%d\n΢������:%s\n�Ƿ�ת��:��\n������:%d\nת����:%d\n\n����ʱ��:%s\n��Դ:%s\n\n",map_iter->first,map_iter->second.Text,
						map_iter->second.Comment,map_iter->second.Resport,map_iter->second.Time,
						map_iter->second.From);
				}
			}
			m_Result.Write(out,out.GetLength());
			++map_iter;
		}
		UpdateInfo.Format(_T("΢�������ɹ�"));
		SendMessage(hwnd,WM_UPDATEMISSIONINFO,(WPARAM)(LPCTSTR)UpdateInfo,0);
	}
	return true;
}