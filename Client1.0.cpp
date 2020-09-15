#include<iostream>
#include<Windows.h>
#include<ctime>
#include<thread>
#include <string>

#pragma comment(lib,"ws2_32.lib")

#define PORT 9999 // 你服务器开放的端口号，记得服务器的防火墙和安全策略都要开放该端口哦
#define MAXDATASIZE 1000 //每次可以接收的最大字节
#define IP "124.71.170.151"//你服务器的公网IP


using namespace std;


void gettime()//当发送消息时调用的 一个输出当前时间的函数
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

void RecvThread(int sockfd)//将以这个函数创建一个接收服务器消息的线程
{
	while (1)
	{
		int numbytes = 0;//服务器发送的字节数
		char buf[MAXDATASIZE];//用于保存服务器消息的字符串数组
		if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
		{
			return;//如果recv返回负一则表示接收失败，一般是服务器出现了问题，则返回
		}
		if (numbytes == 0)//如果接受到0个字节也表示服务器出现了问题，break后返回
			break;
		if (numbytes >= MAXDATASIZE)//如果服务器发送的字节数过大
			buf[MAXDATASIZE - 1] = '\0';//只截取缓存最大值前的字节
		else
			buf[numbytes] = '\0';//字符串后边填空字符
		cout << buf << endl;//打印字符串
	}
}

void GetYourName(string & name)//进入程序后先要输入昵称
{
	cout << "请输入你昵称：";
	getline(cin, name);
}

void InitCorrespond(const string& name,int sockfd)//第一次通信将发送昵称，服务器也会把第一次通信的内容当成用户昵称
{
	for (int i = 0; i < 9; ++i)//尝试九次，如果发送不成功就放弃
	{
		int r = send(sockfd, name.c_str(), name.size(), NULL);
		if (r > 0)
			break;
		else
			continue;
	}
}


int wherey()//获取当前控制台光标所处行数
{
	CONSOLE_SCREEN_BUFFER_INFO pBuffer;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &pBuffer);
	return (pBuffer.dwCursorPosition.Y + 1);
}
void gotoxy(int x, int y)//将光标移动到y行x列（某个函数库好像也有类似函数）
{
	COORD c;
	c.X = x - 1;
	c.Y = y - 1;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
void clear()//所谓清理控制台某一行，就是将光标移动到某一行重新写，我这里只需要回到上一行，当用户输入消息回车发送后，就清理该输入痕迹
{
	int y;
	y = wherey();
	gotoxy(1, y - 1);
}

int main()
{
	string name("");
	GetYourName(name);//获取用户昵称
	WORD sockVersion = MAKEWORD(2, 2);//协议版本2.2
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;//如果申请得到协议版本不是2.2则失败
	}

	int sockfd;//sockfd是申请的套接字的标识号
	sockaddr_in their_addr;//用这个结构体来装套接字的信息


	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)//申请套接字，并将套接字标识号给sockfd
	{
		cout << WSAGetLastError() << endl;//如果申请失败则输出错误信息
		perror("socket():");
		return 0;
	}

	their_addr.sin_family = AF_INET;//IP/TCP协议
	their_addr.sin_port = htons(PORT);//转换端口号的数据类型并存储在结构体内
	their_addr.sin_addr.S_un.S_addr = inet_addr(IP);//转换IP的数据类型并存储在结构体内
	memset(their_addr.sin_zero, 0, sizeof(their_addr.sin_zero));//将sockaddr_in转换成sockaddr前要补零
	if (connect(sockfd, (sockaddr*)&their_addr, sizeof(sockaddr)) == -1)//用申请的套接字和套接字信息去建立连接
	{
		perror("connect():");//连接失败输出错误信息
		return 0;
	}
	InitCorrespond(name,sockfd);//成功建立连接先发送昵称
	thread mythread(RecvThread, sockfd);//创建接收消息线程
	while(1)//发送
	{
		string str("");
		getline(cin,str);
		clear();//一旦回车完成输入则清除输入的痕迹
		int r = send(sockfd, str.c_str(), str.size(), NULL);//发送输入的信息
		if (r <= 0)
		{
			perror("send():");//发送失败，输出错误信息，break退出
			break;
		}
		gettime();//打印发送消息的消息
		cout << "你说: "<< str << endl;//打印你发出去的消息
		if (str == "ends")//输入ends结束通信
			break;
	}
	closesocket(sockfd);//关闭sockfd
	mythread.join();//等待接收线程返回，关闭sockfd该线程就会自动返回
	return 0;
}