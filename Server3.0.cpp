#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <thread>
#include <list> 
#include <mutex>

#define MYPORT 9999 //通信的端口号
#define IP "0.0.0.0"//一般用0.0.0.0表示允许接收任何IP的连接
#define BACKLOG 10 //accept队列的最大等候数
#define MAXUSERNUM 20//聊天室里的最大用户数（每多一个用户就要多创建一个接收线程）
#define getLen(zero) sizeof(zero)/sizeof(zero[0]) //获取数组大小
#define MAXDATASIZE 1000 //最大接收和发送的字节数

using namespace std;

class Users//用此结构体来存储用户的昵称和对应的套接字的标识号
{
public:
	int fd;
	char* name;
	Users(int num, const char str[])//带参构造
	{
		fd = num;
		name = new char[strlen(str) + 1];
		strcpy(name, str);
	}
	Users(const Users& other)//拷贝构造
	{
		fd = other.fd;
		name = new char[strlen(other.name) + 1];
		strcpy(name, other.name);
	}
	Users(Users&& other)//移动构造
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

list<Users> Clientfd;//用链表来存储当前连接的用户
mutex List_Mutex;

string gettime()//用此函数来记录接受到消息的时间
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

void AllSend(const char str[], Users& new_user)//每当接受到消息，就群发给所有群友，除了发送者本身
{
	string temp("*** ");//用temp来制订一些格式
	temp += gettime();
	temp += new_user.name;
	temp += " 说:";
	temp += str;
	lock_guard<mutex> objshuo(List_Mutex);//接下来都要操作链表，先锁住，离开作用域自动释放锁
	auto it = Clientfd.begin();
	while (it != Clientfd.end())//遍历链表群发
	{
		if (it->fd == new_user.fd)//如果是发送者本身，就跳过不发了
		{
			++it;
			continue;
		}
		if (send(it->fd, temp.c_str(), temp.size(), 0) == -1)//new_fd是accept返回套接字文件描述符，str就是你要发送的数据，数据长度，对于最后一位参数，0可看作默认参数，还有别的例如
	/*
		MSG_DONTROUTE 绕过路由器查找
		MSG_DONTROUTE 仅本操作非阻塞
		MSG_OOB 发送或接收带外数据
		MSG_PEEL 窥看外来消息
		MSG_WAITALL 等待所有数据
	*/
		{
			perror("send():");//返回-1则发送失败
			close(it->fd);//发送失败就关闭通讯
			Clientfd.erase(it);
		}
		++it;
	}
}

void Whenleave(Users& new_user);

void RecvThread(Users new_user)//接收线程
{
	while (1)
	{
		char buf[MAXDATASIZE];///缓存接收内容
		int numbytes = 0;//接受到的消息的字节数
		if ((numbytes = recv(new_user.fd, buf, MAXDATASIZE, 0)) == -1)//套接字文件描述符，接收到这字符串里，最大长度,最后一个参数同服务端
		{
			perror("recv():");//接收失败则重新来
			continue;
		}
		if (numbytes >= MAXDATASIZE)//如果发送字节过多，则只存储最大缓存的
			buf[MAXDATASIZE - 1] = '\0';
		else
			buf[numbytes] = '\0';
		if (numbytes == 0 || !strcmp(buf, "ends"))//接受到ends就结束通讯
			break;
		AllSend(buf, new_user);
	}
	close(new_user.fd);//关闭与该用户的套接字
	unique_lock<mutex> objshuo(List_Mutex);//unique_lock比lock_guard更多功能，可以在作用域里提前解锁，如果突然离开作用域也能析构解锁
	auto it = Clientfd.begin();
	while (it != Clientfd.end())//在链表中删除该用户
	{
		if (it->fd == new_user.fd)
		{
			Clientfd.erase(it);
			break;
		}
		++it;
	}
	objshuo.unlock();//解锁
	Whenleave(new_user);//当执行到这里，表明该线程的接收对象，已经下线了，就执行此函数广播该用户已离线的通知
}

Users UserInit(int new_fd)//用户初始化，第一次接收的消息是用户昵称
{
	int numbytes = 0;
	char buf[MAXDATASIZE];
	if ((numbytes = recv(new_fd, buf, MAXDATASIZE, 0)) == -1)//套接字文件描述符，接收到这字符串里，最大长度,最后一个参数同服务端
	{
		perror("recv():");
	}
	buf[numbytes] = '\0';
	Users temp(new_fd, buf);
	return std::move(temp);
}

void WelCome(Users& new_user)//有用户加入时，广播信息
{
	string temp("*** ");
	temp += gettime();
	temp += "欢迎 ";
	temp += new_user.name;
	temp += " 加入群聊,目前群聊有: ";
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
		if (send(it->fd, temp.c_str(), temp.size(), 0) == -1)//new_fd是accept返回套接字文件描述符，str就是你要发送的数据，数据长度，对于最后一位参数，0可看作默认参数，还有别的例如
		/*
			MSG_DONTROUTE 绕过路由器查找
			MSG_DONTROUTE 仅本操作非阻塞
			MSG_OOB 发送或接收带外数据
			MSG_PEEL 窥看外来消息
			MSG_WAITALL 等待所有数据
		*/
		{
			perror("send():");//返回-1则发送失败
			close(it->fd);//发送失败就关闭通讯
			Clientfd.erase(it);
		}
		++it;
	}
}

void Whenleave(Users& new_user)//当用户离开时，广播信息
{
	string temp("*** ");
	temp += gettime();
	temp += " ";
	temp += new_user.name;
	temp += " 离开群聊,目前群聊仅剩有: ";
	lock_guard<mutex> objshuo(List_Mutex);//加锁
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
		if (send(it->fd, temp.c_str(), temp.size(), 0) == -1)//new_fd是accept返回套接字文件描述符，str就是你要发送的数据，数据长度，对于最后一位参数，0可看作默认参数，还有别的例如
		/*
			MSG_DONTROUTE 绕过路由器查找
			MSG_DONTROUTE 仅本操作非阻塞
			MSG_OOB 发送或接收带外数据
			MSG_PEEL 窥看外来消息
			MSG_WAITALL 等待所有数据
		*/
		{
			perror("send():");//返回-1则发送失败
			close(it->fd);//发送失败就关闭通讯
			Clientfd.erase(it);
		}
		++it;
	}
}

void BeforeLeave(int sockfd)//程序退出时关闭所有套接字（一般来说应该用不上）
{
	unique_lock<mutex> objshuo(List_Mutex);
	while (Clientfd.size())
	{
		close(Clientfd.front().fd);
		Clientfd.pop_front();
	}
	//关闭已建立的套接字
	close(sockfd);
}

int main()
{
	//创建socket
	int sockfd;
	sockaddr_in my_addr;
	puts("SERVER:");
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)//如果socket函数出错就会返回-1，否则返回套接字文件描述符
	{
		perror("socket():");//显示错误
		return 0;
	}
	my_addr.sin_family = AF_INET;//通讯在IPV4网络通信范围内
	my_addr.sin_port = htons(MYPORT);//端口号
	my_addr.sin_addr.s_addr = inet_addr(IP);//将点分法的IP地址变成无符号整型存储
	bzero(&(my_addr.sin_zero), getLen(my_addr.sin_zero));//sin_zero是为了让sockaddr与sockaddr_in两个数据结构保持大小相同而保留的空字节

	//绑定
	if (bind(sockfd, (sockaddr*)&my_addr, sizeof(sockaddr)) == -1)//将端口、ip和socket绑定起来
	{
		perror("bind():");//绑定错误
		return 0;
	}

	//监听
	if (listen(sockfd, BACKLOG) == -1)//启动监听，等待接入请求，BACKLOG是进入队列中允许的连接数 
	{
		perror("listen()");//监听失败
		return 0;
	}
	while (1)
	{
		int new_fd;
		socklen_t sin_size;
		sockaddr_in their_addr;//用来装客户端的socket信息
		if ((new_fd = accept(sockfd, (sockaddr*)&their_addr, &sin_size)) == -1)//accept是堵塞函数，如果一直没收到客户端连接则会一直卡在这里等待，当收到不正确的客户端的时候就返回-1，反之会返回其他值
		{
			perror("accept():");//accept错误
			continue;
		}
		if (Clientfd.size() < MAXUSERNUM)
		{
			Users temp(UserInit(new_fd));
			unique_lock<mutex> objshuo(List_Mutex);//加锁
			Clientfd.push_back(temp);//往链表添加元素
			objshuo.unlock();//解锁
			WelCome(temp);
			thread Mythread(RecvThread, temp);//为该用户创建接收线程
			cout << "server: got connection from " << inet_ntoa(their_addr.sin_addr) << endl;//输出客户端的点分十进制后的ip地址
			Mythread.detach();//分离线程，交由系统托管
		}
		else
			cout << "People is full" << endl;
	}
	BeforeLeave(sockfd);//一般不会执行到这里，摆设罢了
	return 0;
}
