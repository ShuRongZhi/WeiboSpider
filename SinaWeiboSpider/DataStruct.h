#pragma once
class DataStruct
{
public:
	DataStruct(void);
	~DataStruct(void);
	//΢������
	CString Text;
	//����ʱ��
	CString Time;
	//��Դ
	CString From;
	//������
	unsigned int Comment;
	//ת����
	unsigned int Resport;
	//ͼƬURL
	CString PicUrl;
	//�Ƿ�ת��
	bool IsResport;
	//�Ƿ��ͼƬ
	bool HasPic;
};

