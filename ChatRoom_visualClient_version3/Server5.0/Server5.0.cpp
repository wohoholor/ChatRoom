#include "HandleMsg.h"

#define MYPORT 9999 //通信的端口号
#define IP "0.0.0.0"//一般用0.0.0.0表示允许接收任何IP的连接
#define BACKLOG 10 //accept队列的最大等候数
#define MAXUSERNUM 20//聊天室里的最大用户数（每多一个用户就要多创建一个接收线程）
#define getLen(zero) sizeof(zero)/sizeof(zero[0]) //获取数组大小
#define SIGNIN "#SIGNIN"
#define SIGNUP "#SIGNUP"
/*登录格式：#SIGNIN,user_id,user_password*/

using namespace std;

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
		socklen_t sin_size = 1;
		sockaddr_in their_addr;//用来装客户端的socket信息
		if ((new_fd = accept(sockfd, (sockaddr*)&their_addr, &sin_size)) == -1)//accept是堵塞函数，如果一直没收到客户端连接则会一直卡在这里等待，当收到不正确的客户端的时候就返回-1，反之会返回其他值
		{
			perror("accept():");//accept错误
			continue;
		}
		if (Clientfd.size() < MAXUSERNUM)
		{
			Users temp(UserInit(new_fd));
			if (temp.fd == -1)
			{
				SendMsgTo(new_fd, "兄弟，你账号密码不对劲，请关掉程序重新登录\n");
				continue;
			}
			else if (temp.fd == -2)
			{
				SendMsgTo(new_fd, "兄弟，该账号已经存在啦，请关掉程序重新登录\n");
				continue;
			}
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