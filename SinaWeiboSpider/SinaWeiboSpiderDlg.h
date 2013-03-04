
// SinaWeiboSpiderDlg.h : ͷ�ļ�
//

#pragma once
#include "resource.h"

// CSinaWeiboSpiderDlg �Ի���
class CSinaWeiboSpiderDlg : public CDialogEx
{
// ����
public:
	CSinaWeiboSpiderDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SINAWEIBOSPIDER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedLogin();
	CString m_UserName;
	CString m_Password;
	BOOL m_IsTreaveFans;
	BOOL m_IsTreaveFollow;
	BOOL m_IsTreaveWeibo;
	CString SaveFilePath;
	CString m_FansUrl;
	CString m_FollowUrl;
	CString m_WeiboUrl;
	CStatic m_LoginStatus;
	afx_msg void OnStnClickedPath();
	BOOL m_AutoSleep;
	CStatic m_ChooseStatus;
protected:
//	afx_msg LRESULT OnUpDateText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpDateMissionInfo(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedClearedit();
};

struct DlgData
{
	struct MissionChoose
	{
		bool IsTraversalWeibo;
		bool IsTraversalFans;
		bool IsTraversalFollow;
	}MissionChoose;

	CString m_Username;
	CString m_Password;
	CString m_WeiboUrl;
	CString m_FansUrl;
	CString m_FollowUrl;
	CString m_ResultPath;
	//ָ���¼״̬�ؼ�
	CStatic *m_LoginStatus;
	CStatic *m_ChooseStatus;
};
