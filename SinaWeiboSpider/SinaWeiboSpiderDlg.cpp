
// SinaWeiboSpiderDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SinaWeiboSpider.h"
#include "SinaWeiboSpiderDlg.h"
#include "afxdialogex.h"
#include "NetWork.h"
#include <vector>
#include "Tools.h"
#include <fstream>
#include "HTMLParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//ȫ�ֶ���DlgData,�����������߳��з���
DlgData Dlgdata;
NetWork nt;
BOOL IsLogin = false;
bool IsLoginThreadRun = false;
bool IsMissionThreadRun = false;

//����һЩ����
#define InitFail -1
#define LoginFail -2
#define ConnectionFail -3
#define TraversalFollowFail -4
#define TraversalWeiboFail -5
#define TraversalFansFail -6


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
	//	afx_msg LRESULT OnUpDateText(WPARAM wParam, LPARAM lParam);
public:
	CStatic m_Pic;
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CC, m_Pic);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	//	ON_MESSAGE(WM_UPDATETEXT, &CAboutDlg::OnUpDateText)
END_MESSAGE_MAP()


// CSinaWeiboSpiderDlg �Ի���




CSinaWeiboSpiderDlg::CSinaWeiboSpiderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSinaWeiboSpiderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_UserName = _T("");
	m_Password = _T("");
	m_FansUrl = _T("");
	m_FollowUrl = _T("");
	m_WeiboUrl = _T("");
	Dlgdata.MissionChoose.IsTraversalFans = false;
	Dlgdata.MissionChoose.IsTraversalFollow = false;
	Dlgdata.MissionChoose.IsTraversalFollow = false;
}

void CSinaWeiboSpiderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USER_NAME, m_UserName);
	DDV_MaxChars(pDX, m_UserName, 256);
	DDX_Text(pDX, IDC_USER_PASSWORD, m_Password);
	DDV_MaxChars(pDX, m_Password, 512);
	DDX_Check(pDX, IDC_TRAVEFANS, m_IsTreaveFans);
	DDX_Check(pDX, IDC_TRAVEFOLLOW, m_IsTreaveFollow);
	DDX_Check(pDX, IDC_TRAVEWEIBO, m_IsTreaveWeibo);
	DDX_Text(pDX, IDC_FANSURL, m_FansUrl);
	DDV_MaxChars(pDX, m_FansUrl, 512);
	DDX_Text(pDX, IDC_FOLLOWURL, m_FollowUrl);
	DDV_MaxChars(pDX, m_FollowUrl, 512);
	DDX_Text(pDX, IDC_WEIBOURL, m_WeiboUrl);
	DDV_MaxChars(pDX, m_WeiboUrl, 512);
	DDX_Control(pDX, IDC_ISLOGIN, m_LoginStatus);
	DDX_Check(pDX, IDC_AUTOSLEEP, m_AutoSleep);
	DDX_Control(pDX, IDC_STATUS, m_ChooseStatus);
}

BEGIN_MESSAGE_MAP(CSinaWeiboSpiderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CSinaWeiboSpiderDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_LOGIN, &CSinaWeiboSpiderDlg::OnBnClickedLogin)
	ON_STN_CLICKED(IDC_PATH, &CSinaWeiboSpiderDlg::OnStnClickedPath)
	//	ON_MESSAGE(WM_UPDATETEXT, &CSinaWeiboSpiderDlg::OnUpDateText)
	ON_MESSAGE(WM_UPDATEMISSIONINFO, &CSinaWeiboSpiderDlg::OnUpDateMissionInfo)
	ON_BN_CLICKED(IDC_CLEAREDIT, &CSinaWeiboSpiderDlg::OnBnClickedClearedit)
END_MESSAGE_MAP()


// CSinaWeiboSpiderDlg ��Ϣ�������

BOOL CSinaWeiboSpiderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	IsLogin = false;
	//���Ի���ľ�����Ƹ�NetWork�࣬�������
	nt.SetDlgHwnd(m_hWnd);
	//���Զ�˯������Ϊfalse
	m_AutoSleep = false;
	Tools::IsAutoSleep = false;
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CSinaWeiboSpiderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSinaWeiboSpiderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSinaWeiboSpiderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//��¼�߳�
DWORD WINAPI LoginThread(LPVOID lpParameter);
//ִ�������߳�
DWORD WINAPI MissionThread(LPVOID lpParameter);

void CSinaWeiboSpiderDlg::OnBnClickedStart()
{
	//�ж������߳��Ƿ�����
	if(IsMissionThreadRun)
	{
		MessageBox(_T("�߳�����������ȴ��߳����"));
		return;
	}
	if(!IsLogin)
	{
		MessageBox(_T("���󣺱����¼����ܽ�����һ�������ȵ�¼"));
		return;
	}

	//��������ȡ���ݲ�У��Ϸ���
	UpdateData(true);
	if(m_IsTreaveWeibo)
	{

		if(m_WeiboUrl.IsEmpty())
		{
			MessageBox(_T("�����ѡ����΢����������д΢����ַ������"));
			return;
		}
		Dlgdata.MissionChoose.IsTraversalWeibo = true;
		Dlgdata.m_WeiboUrl = m_WeiboUrl;
	}
	if(m_IsTreaveFans)
	{

		if(m_FansUrl.IsEmpty())
		{
			MessageBox(_T("�����ѡ������˿��������д��˿��ַ������"));
			return;
		}
		Dlgdata.MissionChoose.IsTraversalFans = true;
		Dlgdata.m_FansUrl = m_FansUrl;
	}
	if(m_IsTreaveFollow)
	{

		if(m_FollowUrl.IsEmpty())
		{
			MessageBox(_T("�����ѡ������ע��������д��ע��ַ������"));
			return;
		}
		Dlgdata.MissionChoose.IsTraversalFollow = true;
		Dlgdata.m_FollowUrl = m_FollowUrl;
	}
	if(!(m_IsTreaveWeibo || m_IsTreaveFans || m_IsTreaveFollow))
	{
		MessageBox(_T("���󣺱���ѡ������һ����������"));
		return;
	}
	if(!m_AutoSleep)
	{
		BOOL m_Res;
		m_Res = MessageBox(_T("����:�������ѡ��ѡ��п�����Ϊ�����ٶȹ���������ʺű��������������\n���鹴ѡ��ѡ��Ƿ�ѡ��"),0,MB_YESNO| MB_ICONWARNING);
		if(IDYES == m_Res)
		{
			m_AutoSleep = true;
			Tools::IsAutoSleep = true;
			UpdateData(false);
		}
		else
		{
			Tools::IsAutoSleep = false;
		}
	}
	Dlgdata.m_ChooseStatus = &m_ChooseStatus;
	//���������߳�
	HANDLE hd;
	hd = CreateThread(NULL,0,MissionThread,NULL,0,NULL);
	CloseHandle(hd);
}

void CSinaWeiboSpiderDlg::OnBnClickedLogin()
{
	//����Ŀ¼
	nt.SetSaveFilePath(SaveFilePath);
	if(IsLoginThreadRun)
	{
		MessageBox(_T("��¼�߳�����������ȴ��߳����"));
		return;
	}
	if(IsLogin)
	{
		int Res;
		Res = MessageBox(_T("��ǰ�Ѿ���¼��ȷ��Ҫ���µ�¼��"),0,MB_YESNO);
		if(IDNO == Res)
		{
			return;
		}
	}
	//�����������ݲ�У��Ϸ���
	UpdateData(true);
	if(m_UserName.IsEmpty())
	{
		MessageBox(_T("�����û�������Ϊ��"),0,MB_ICONWARNING);
		return;
	}
	if(m_Password.IsEmpty())
	{
		MessageBox(_T("�������벻��Ϊ��"),0,MB_ICONWARNING);
		return;
	}
	//���û����������뾲̬�ı��ؼ���ֵ��ȫ�ֽṹ��Dlgdata,�Ա����߳��з���
	Dlgdata.m_Password = m_Password;
	Dlgdata.m_Username = m_UserName;
	Dlgdata.m_LoginStatus = &m_LoginStatus;

	m_LoginStatus.SetWindowTextA(_T("���ڵ�¼�����Ե�"));
	//������¼�߳�
	HANDLE hd;
	hd = CreateThread(NULL,0,LoginThread,NULL,0,NULL);
	CloseHandle(hd);
}


void CSinaWeiboSpiderDlg::OnStnClickedPath()
{
#if 0
	// TODO: �ڴ���ӿؼ�֪ͨ����������}
	CFileDialog SaveFileDlg(false,".txt","������",6UL,"�ı���ʽ|*.txt||");
	if(SaveFileDlg.DoModal()==IDOK)
	{
		//ȡ������·��
		SaveFilePath = SaveFileDlg.GetPathName();
	}
	CString Out;
	Out.Format(_T("���ô�ŵ�Ŀ¼Ϊ:%s"),SaveFilePath);
	GetDlgItem(IDC_PATH)->SetWindowTextA(Out);
	nt.SetSaveFilePath(SaveFilePath);
#endif
	CFile fp;
	fp.Open(_T("D:\\xmll.txt"),CFile::modeReadWrite);
	char *pStr = new char[fp.GetLength()+1];
	fp.Read(pStr,fp.GetLength());
	CString szStr(pStr);
	delete []pStr;
	CHTMLParser ps;
	ps.ReadHTML(szStr);
	ps.ParserHTML();
	Sleep(100);
}

//��¼�߳�
DWORD WINAPI LoginThread(LPVOID lpParameter)
{
	IsLoginThreadRun =  true;
	CString Msg;
	if(!nt.InitSocket())
	{
		Msg.Format(_T("��ʼ��Socketʧ��"));
		Dlgdata.m_LoginStatus->SetWindowTextA(Msg);
		IsLoginThreadRun = false;
		return InitFail;
	}
	if(!nt.ConnectionServer(_T("221.179.175.244")))
	{
		Msg.Format(_T("���ӷ�����ʧ��"));
		Dlgdata.m_LoginStatus->SetWindowTextA(Msg);
		IsLoginThreadRun = false;
		return ConnectionFail;
	}
	if(!nt.Login(Dlgdata.m_Username,Dlgdata.m_Password))
	{
		Msg.Format(_T("��¼ʧ��"));
		Dlgdata.m_LoginStatus->SetWindowTextA(Msg);
		IsLoginThreadRun = false;
		return LoginFail;
	}
	else
	{
		//���õ�¼״̬
		IsLogin = true;
		Msg.Format(_T("��ǰ�ѵ�¼����¼�ʺ�Ϊ: %s"),Dlgdata.m_Username);
		Dlgdata.m_LoginStatus->SetWindowTextA(Msg);
		Msg.Empty();
	}
	IsLoginThreadRun = false;
	return true;
}

//�����߳�
DWORD WINAPI MissionThread(LPVOID lpParameter)
{
	bool IsMissionSuccess = true;
	IsMissionThreadRun = true;
	if(Dlgdata.MissionChoose.IsTraversalFans)
	{
		Dlgdata.m_ChooseStatus->SetWindowTextA(_T("���ڱ�����˿......"));
		if(!nt.TraversalFans(Dlgdata.m_FansUrl))
		{
			IsMissionSuccess = false;
			Dlgdata.m_ChooseStatus->SetWindowTextA(_T("������˿ʧ��"));
		}
		else
		{
			Dlgdata.m_ChooseStatus->SetWindowTextA(_T("������˿���"));
		}
	}
	if(Dlgdata.MissionChoose.IsTraversalFollow)
	{
		Dlgdata.m_ChooseStatus->SetWindowTextA(_T("���ڱ�����ע......"));
		if(!nt.TraversalFollow(Dlgdata.m_FollowUrl))
		{
			IsMissionSuccess = false;
			Dlgdata.m_ChooseStatus->SetWindowTextA(_T("������עʧ��"));
		}
		else
		{
			Dlgdata.m_ChooseStatus->SetWindowTextA(_T("������ע���"));
		}
	}
	if(Dlgdata.MissionChoose.IsTraversalWeibo)
	{
		Dlgdata.m_ChooseStatus->SetWindowTextA(_T("���ڱ���΢��......"));
		if(!nt.TraversalWeibo(Dlgdata.m_WeiboUrl))
		{
			IsMissionSuccess = false;
			Dlgdata.m_ChooseStatus->SetWindowTextA(_T("����΢��ʧ��"));
		}
		else
		{
			Dlgdata.m_ChooseStatus->SetWindowTextA(_T("����΢�����"));
		}
	}
	//�ж�����״̬
	if(IsMissionSuccess)
	{
		Dlgdata.m_ChooseStatus->SetWindowTextA(_T("������������ɣ��ȴ���ָ����......"));
	}
	else
	{
		Dlgdata.m_ChooseStatus->SetWindowTextA(_T("������һ������ʧ�ܣ��ȴ���ָ����......"));
	}
	IsMissionThreadRun = false;
	return true;
}

afx_msg LRESULT CSinaWeiboSpiderDlg::OnUpDateMissionInfo(WPARAM wParam, LPARAM lParam)
{

	int index;
	CEdit *pEdit;
	pEdit = (CEdit *)GetDlgItem(IDC_MISSIONINFO);
	index = pEdit->GetWindowTextLengthA();
	pEdit->SetSel(index,index);
	CString Temp((LPCTSTR)wParam);
	pEdit->ReplaceSel(Temp + _T("\n"));
	return 0;
}


void CSinaWeiboSpiderDlg::OnBnClickedClearedit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CEdit *pEdit;
	pEdit = (CEdit*)GetDlgItem(IDC_MISSIONINFO);
	pEdit->SetWindowTextA(_T(""));
}

//������ڶԻ����CCЭ��ͼ��
CImage image;
BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//���ж�image�Ƿ�������
	//���û����ֱ�����룬����Ѿ������ˣ�����image����������
	if(!image)
	{
		image.Load(_T("res\\by-nc-sa.png"));
		m_Pic.SetBitmap(image);
	}
	else
	{
		image.Destroy();
		image.Load(_T("res\\by-nc-sa.png"));
		m_Pic.SetBitmap(image);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
