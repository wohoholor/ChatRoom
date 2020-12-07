#include "HandleMsg.h"
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
	temp += " ***\n  ";
	temp += str;
	RecordLog(temp);
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


void WelCome(Users& new_user)//有用户加入时，广播信息
{
	ReadRecord();

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

void SendMsgTo(int sockfd,const char msg[])
{
	send(sockfd, msg, strlen(msg)+1, 0);
	close(sockfd);
}