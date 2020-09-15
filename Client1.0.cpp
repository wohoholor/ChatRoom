#include<iostream>
#include<Windows.h>
#include<ctime>
#include<thread>
#include <string>

#pragma comment(lib,"ws2_32.lib")

#define PORT 9999 // ����������ŵĶ˿ںţ��ǵ÷������ķ���ǽ�Ͱ�ȫ���Զ�Ҫ���Ÿö˿�Ŷ
#define MAXDATASIZE 1000 //ÿ�ο��Խ��յ�����ֽ�
#define IP "124.71.170.151"//��������Ĺ���IP


using namespace std;


void gettime()//��������Ϣʱ���õ� һ�������ǰʱ��ĺ���
{
	time_t rawtime;
	tm* ptminfo = new tm();
	time(&rawtime);
	localtime_s(ptminfo, &rawtime);
	printf("*** %02d-%02d %02d:%02d ",
		ptminfo->tm_mon + 1, ptminfo->tm_mday,
		ptminfo->tm_hour, ptminfo->tm_min);
	delete ptminfo;
}

void RecvThread(int sockfd)//���������������һ�����շ�������Ϣ���߳�
{
	while (1)
	{
		int numbytes = 0;//���������͵��ֽ���
		char buf[MAXDATASIZE];//���ڱ����������Ϣ���ַ�������
		if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
		{
			return;//���recv���ظ�һ���ʾ����ʧ�ܣ�һ���Ƿ��������������⣬�򷵻�
		}
		if (numbytes == 0)//������ܵ�0���ֽ�Ҳ��ʾ���������������⣬break�󷵻�
			break;
		if (numbytes >= MAXDATASIZE)//������������͵��ֽ�������
			buf[MAXDATASIZE - 1] = '\0';//ֻ��ȡ�������ֵǰ���ֽ�
		else
			buf[numbytes] = '\0';//�ַ����������ַ�
		cout << buf << endl;//��ӡ�ַ���
	}
}

void GetYourName(string & name)//����������Ҫ�����ǳ�
{
	cout << "���������ǳƣ�";
	getline(cin, name);
}

void InitCorrespond(const string& name,int sockfd)//��һ��ͨ�Ž������ǳƣ�������Ҳ��ѵ�һ��ͨ�ŵ����ݵ����û��ǳ�
{
	for (int i = 0; i < 9; ++i)//���ԾŴΣ�������Ͳ��ɹ��ͷ���
	{
		int r = send(sockfd, name.c_str(), name.size(), NULL);
		if (r > 0)
			break;
		else
			continue;
	}
}


int wherey()//��ȡ��ǰ����̨�����������
{
	CONSOLE_SCREEN_BUFFER_INFO pBuffer;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &pBuffer);
	return (pBuffer.dwCursorPosition.Y + 1);
}
void gotoxy(int x, int y)//������ƶ���y��x�У�ĳ�����������Ҳ�����ƺ�����
{
	COORD c;
	c.X = x - 1;
	c.Y = y - 1;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
void clear()//��ν�������̨ĳһ�У����ǽ�����ƶ���ĳһ������д��������ֻ��Ҫ�ص���һ�У����û�������Ϣ�س����ͺ󣬾����������ۼ�
{
	int y;
	y = wherey();
	gotoxy(1, y - 1);
}

int main()
{
	string name("");
	GetYourName(name);//��ȡ�û��ǳ�
	WORD sockVersion = MAKEWORD(2, 2);//Э��汾2.2
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;//�������õ�Э��汾����2.2��ʧ��
	}

	int sockfd;//sockfd��������׽��ֵı�ʶ��
	sockaddr_in their_addr;//������ṹ����װ�׽��ֵ���Ϣ


	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)//�����׽��֣������׽��ֱ�ʶ�Ÿ�sockfd
	{
		cout << WSAGetLastError() << endl;//�������ʧ�������������Ϣ
		perror("socket():");
		return 0;
	}

	their_addr.sin_family = AF_INET;//IP/TCPЭ��
	their_addr.sin_port = htons(PORT);//ת���˿ںŵ��������Ͳ��洢�ڽṹ����
	their_addr.sin_addr.S_un.S_addr = inet_addr(IP);//ת��IP���������Ͳ��洢�ڽṹ����
	memset(their_addr.sin_zero, 0, sizeof(their_addr.sin_zero));//��sockaddr_inת����sockaddrǰҪ����
	if (connect(sockfd, (sockaddr*)&their_addr, sizeof(sockaddr)) == -1)//��������׽��ֺ��׽�����Ϣȥ��������
	{
		perror("connect():");//����ʧ�����������Ϣ
		return 0;
	}
	InitCorrespond(name,sockfd);//�ɹ����������ȷ����ǳ�
	thread mythread(RecvThread, sockfd);//����������Ϣ�߳�
	while(1)//����
	{
		string str("");
		getline(cin,str);
		clear();//һ���س�����������������ĺۼ�
		int r = send(sockfd, str.c_str(), str.size(), NULL);//�����������Ϣ
		if (r <= 0)
		{
			perror("send():");//����ʧ�ܣ����������Ϣ��break�˳�
			break;
		}
		gettime();//��ӡ������Ϣ����Ϣ
		cout << "��˵: "<< str << endl;//��ӡ�㷢��ȥ����Ϣ
		if (str == "ends")//����ends����ͨ��
			break;
	}
	closesocket(sockfd);//�ر�sockfd
	mythread.join();//�ȴ������̷߳��أ��ر�sockfd���߳̾ͻ��Զ�����
	return 0;
}