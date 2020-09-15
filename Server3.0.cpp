#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <thread>
#include <list> 
#include <mutex>

#define MYPORT 9999 //ͨ�ŵĶ˿ں�
#define IP "0.0.0.0"//һ����0.0.0.0��ʾ��������κ�IP������
#define BACKLOG 10 //accept���е����Ⱥ���
#define MAXUSERNUM 20//�������������û�����ÿ��һ���û���Ҫ�ഴ��һ�������̣߳�
#define getLen(zero) sizeof(zero)/sizeof(zero[0]) //��ȡ�����С
#define MAXDATASIZE 1000 //�����պͷ��͵��ֽ���

using namespace std;

class Users//�ô˽ṹ�����洢�û����ǳƺͶ�Ӧ���׽��ֵı�ʶ��
{
public:
	int fd;
	char* name;
	Users(int num, const char str[])//���ι���
	{
		fd = num;
		name = new char[strlen(str) + 1];
		strcpy(name, str);
	}
	Users(const Users& other)//��������
	{
		fd = other.fd;
		name = new char[strlen(other.name) + 1];
		strcpy(name, other.name);
	}
	Users(Users&& other)//�ƶ�����
	{
		fd = other.fd;
		name = other.name;
		other.name = nullptr;
	}
	~Users()
	{
		delete[] name;
	}
};

list<Users> Clientfd;//���������洢��ǰ���ӵ��û�
mutex List_Mutex;

string gettime()//�ô˺�������¼���ܵ���Ϣ��ʱ��
{
	char str[13];
	time_t rawtime;
	struct tm* ptminfo;
	time(&rawtime);
	ptminfo = localtime(&rawtime);
	sprintf(str, "%02d-%02d %02d:%02d ", ptminfo->tm_mon + 1, ptminfo->tm_mday,
		ptminfo->tm_hour, ptminfo->tm_min);
	string c(str);
	return c;
}

void AllSend(const char str[], Users& new_user)//ÿ�����ܵ���Ϣ����Ⱥ��������Ⱥ�ѣ����˷����߱���
{
	string temp("*** ");//��temp���ƶ�һЩ��ʽ
	temp += gettime();
	temp += new_user.name;
	temp += " ˵:";
	temp += str;
	lock_guard<mutex> objshuo(List_Mutex);//��������Ҫ������������ס���뿪�������Զ��ͷ���
	auto it = Clientfd.begin();
	while (it != Clientfd.end())//��������Ⱥ��
	{
		if (it->fd == new_user.fd)//����Ƿ����߱���������������
		{
			++it;
			continue;
		}
		if (send(it->fd, temp.c_str(), temp.size(), 0) == -1)//new_fd��accept�����׽����ļ���������str������Ҫ���͵����ݣ����ݳ��ȣ��������һλ������0�ɿ���Ĭ�ϲ��������б������
	/*
		MSG_DONTROUTE �ƹ�·��������
		MSG_DONTROUTE ��������������
		MSG_OOB ���ͻ���մ�������
		MSG_PEEL ����������Ϣ
		MSG_WAITALL �ȴ���������
	*/
		{
			perror("send():");//����-1����ʧ��
			close(it->fd);//����ʧ�ܾ͹ر�ͨѶ
			Clientfd.erase(it);
		}
		++it;
	}
}

void Whenleave(Users& new_user);

void RecvThread(Users new_user)//�����߳�
{
	while (1)
	{
		char buf[MAXDATASIZE];///�����������
		int numbytes = 0;//���ܵ�����Ϣ���ֽ���
		if ((numbytes = recv(new_user.fd, buf, MAXDATASIZE, 0)) == -1)//�׽����ļ������������յ����ַ������󳤶�,���һ������ͬ�����
		{
			perror("recv():");//����ʧ����������
			continue;
		}
		if (numbytes >= MAXDATASIZE)//��������ֽڹ��࣬��ֻ�洢��󻺴��
			buf[MAXDATASIZE - 1] = '\0';
		else
			buf[numbytes] = '\0';
		if (numbytes == 0 || !strcmp(buf, "ends"))//���ܵ�ends�ͽ���ͨѶ
			break;
		AllSend(buf, new_user);
	}
	close(new_user.fd);//�ر�����û����׽���
	unique_lock<mutex> objshuo(List_Mutex);//unique_lock��lock_guard���๦�ܣ�����������������ǰ���������ͻȻ�뿪������Ҳ����������
	auto it = Clientfd.begin();
	while (it != Clientfd.end())//��������ɾ�����û�
	{
		if (it->fd == new_user.fd)
		{
			Clientfd.erase(it);
			break;
		}
		++it;
	}
	objshuo.unlock();//����
	Whenleave(new_user);//��ִ�е�����������̵߳Ľ��ն����Ѿ������ˣ���ִ�д˺����㲥���û������ߵ�֪ͨ
}

Users UserInit(int new_fd)//�û���ʼ������һ�ν��յ���Ϣ���û��ǳ�
{
	int numbytes = 0;
	char buf[MAXDATASIZE];
	if ((numbytes = recv(new_fd, buf, MAXDATASIZE, 0)) == -1)//�׽����ļ������������յ����ַ������󳤶�,���һ������ͬ�����
	{
		perror("recv():");
	}
	buf[numbytes] = '\0';
	Users temp(new_fd, buf);
	return std::move(temp);
}

void WelCome(Users& new_user)//���û�����ʱ���㲥��Ϣ
{
	string temp("*** ");
	temp += gettime();
	temp += "��ӭ ";
	temp += new_user.name;
	temp += " ����Ⱥ��,ĿǰȺ����: ";
	lock_guard<mutex> objshuo(List_Mutex);
	auto iter = Clientfd.begin();
	while (iter != Clientfd.end())
	{
		temp += (iter->name);
		if (++iter != Clientfd.end())
			temp += ",";
	}
	auto it = Clientfd.begin();
	while (it != Clientfd.end())
	{
		if (send(it->fd, temp.c_str(), temp.size(), 0) == -1)//new_fd��accept�����׽����ļ���������str������Ҫ���͵����ݣ����ݳ��ȣ��������һλ������0�ɿ���Ĭ�ϲ��������б������
		/*
			MSG_DONTROUTE �ƹ�·��������
			MSG_DONTROUTE ��������������
			MSG_OOB ���ͻ���մ�������
			MSG_PEEL ����������Ϣ
			MSG_WAITALL �ȴ���������
		*/
		{
			perror("send():");//����-1����ʧ��
			close(it->fd);//����ʧ�ܾ͹ر�ͨѶ
			Clientfd.erase(it);
		}
		++it;
	}
}

void Whenleave(Users& new_user)//���û��뿪ʱ���㲥��Ϣ
{
	string temp("*** ");
	temp += gettime();
	temp += " ";
	temp += new_user.name;
	temp += " �뿪Ⱥ��,ĿǰȺ�Ľ�ʣ��: ";
	lock_guard<mutex> objshuo(List_Mutex);//����
	auto iter = Clientfd.begin();
	while (iter != Clientfd.end())
	{
		temp += (iter->name);
		if (++iter != Clientfd.end())
			temp += ",";
	}
	auto it = Clientfd.begin();
	while (it != Clientfd.end())
	{
		if (send(it->fd, temp.c_str(), temp.size(), 0) == -1)//new_fd��accept�����׽����ļ���������str������Ҫ���͵����ݣ����ݳ��ȣ��������һλ������0�ɿ���Ĭ�ϲ��������б������
		/*
			MSG_DONTROUTE �ƹ�·��������
			MSG_DONTROUTE ��������������
			MSG_OOB ���ͻ���մ�������
			MSG_PEEL ����������Ϣ
			MSG_WAITALL �ȴ���������
		*/
		{
			perror("send():");//����-1����ʧ��
			close(it->fd);//����ʧ�ܾ͹ر�ͨѶ
			Clientfd.erase(it);
		}
		++it;
	}
}

void BeforeLeave(int sockfd)//�����˳�ʱ�ر������׽��֣�һ����˵Ӧ���ò��ϣ�
{
	unique_lock<mutex> objshuo(List_Mutex);
	while (Clientfd.size())
	{
		close(Clientfd.front().fd);
		Clientfd.pop_front();
	}
	//�ر��ѽ������׽���
	close(sockfd);
}

int main()
{
	//����socket
	int sockfd;
	sockaddr_in my_addr;
	puts("SERVER:");
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)//���socket��������ͻ᷵��-1�����򷵻��׽����ļ�������
	{
		perror("socket():");//��ʾ����
		return 0;
	}
	my_addr.sin_family = AF_INET;//ͨѶ��IPV4����ͨ�ŷ�Χ��
	my_addr.sin_port = htons(MYPORT);//�˿ں�
	my_addr.sin_addr.s_addr = inet_addr(IP);//����ַ���IP��ַ����޷������ʹ洢
	bzero(&(my_addr.sin_zero), getLen(my_addr.sin_zero));//sin_zero��Ϊ����sockaddr��sockaddr_in�������ݽṹ���ִ�С��ͬ�������Ŀ��ֽ�

	//��
	if (bind(sockfd, (sockaddr*)&my_addr, sizeof(sockaddr)) == -1)//���˿ڡ�ip��socket������
	{
		perror("bind():");//�󶨴���
		return 0;
	}

	//����
	if (listen(sockfd, BACKLOG) == -1)//�����������ȴ���������BACKLOG�ǽ������������������� 
	{
		perror("listen()");//����ʧ��
		return 0;
	}
	while (1)
	{
		int new_fd;
		socklen_t sin_size;
		sockaddr_in their_addr;//����װ�ͻ��˵�socket��Ϣ
		if ((new_fd = accept(sockfd, (sockaddr*)&their_addr, &sin_size)) == -1)//accept�Ƕ������������һֱû�յ��ͻ����������һֱ��������ȴ������յ�����ȷ�Ŀͻ��˵�ʱ��ͷ���-1����֮�᷵������ֵ
		{
			perror("accept():");//accept����
			continue;
		}
		if (Clientfd.size() < MAXUSERNUM)
		{
			Users temp(UserInit(new_fd));
			unique_lock<mutex> objshuo(List_Mutex);//����
			Clientfd.push_back(temp);//���������Ԫ��
			objshuo.unlock();//����
			WelCome(temp);
			thread Mythread(RecvThread, temp);//Ϊ���û����������߳�
			cout << "server: got connection from " << inet_ntoa(their_addr.sin_addr) << endl;//����ͻ��˵ĵ��ʮ���ƺ��ip��ַ
			Mythread.detach();//�����̣߳�����ϵͳ�й�
		}
		else
			cout << "People is full" << endl;
	}
	BeforeLeave(sockfd);//һ�㲻��ִ�е�����������
	return 0;
}
