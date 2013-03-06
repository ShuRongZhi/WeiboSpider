#pragma once
#include <vector>

//�Զ�����Ϣ�����ڸ�����ʾ������Ϣ�Ŀؼ�
#define WM_UPDATEMISSIONINFO (WM_USER + 100)

class Tools
{
public:
	Tools(void);
	~Tools(void);
	//�����ַ���  ����1:���������ַ���  ����2:��Ž�������ַ���  ����3:��������
	bool ParseString(CString&,CString&,CString);
	//�����ַ���2 ����1:���������ַ��� ����2:��Ž�������ַ��� ����3:�������� ����4:��ʾ��ÿ�����
	//������ӻس�
	bool ParseString(CString&,CString&,CString,int);
	//�����ַ��������ص�һ������Ҫ����ַ���
	//����1:���������ַ���������2:��Ž�������ַ���������3:��������
	bool FindFirstString(CString&,CString&,CString);
	//����1:���������ַ���������2:�������� ���ط��Ͻ����vector����
	std::vector<CString> ParseString(CString&,CString);
	//��ʽ���ַ���������ɾ������Ļس����ո��
	//����1:���������ַ���    ����ʽ������ַ���д�뵽����1��
	bool FormatString(CString&);
	//Ѱ���ַ������ҵ�����ture�����򷵻�false
	//����1:�����ҵ��ַ��� ����2:���ҹ���
	bool SearchString(CString&,CString);
	//�滻�ַ��� �����滻����ַ���д�뵽����1��
	//����1:���滻���ַ��� ����2 �滻�ַ��� ����3 �滻����
	bool ReplaceString(CString&,CString,CString);
	//ɾ���ַ���  ����1����ɾ�����ַ��� ����2��ɾ������
	//��ɾ������ַ���д�뵽����1��
	bool RemoveString(CString&,CString);
	//���տ��ֽ��ַ��������ض��ֽ��ַ���
	CString UTF8ToANSI(CString);
	//�ַ����ָ��  ����CString�ַ������зָ����vector��Ĭ���Կո�ָ�
	std::vector<CString> SplitString(CString);
	//�ַ����ָ�����ذ汾  ����CString�ַ������зָ����vector�������Լ�ָ���ָ����
	std::vector<CString> SplitString(CString,CString);
	void Sleep();
	HWND hwnd;
	static bool IsAutoSleep;
private:
	CString UpdateInfo;
	unsigned int SleepTime;
};