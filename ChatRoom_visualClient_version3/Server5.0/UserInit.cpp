#include "UserInit.h"

Users UserInit(int new_fd)//用户初始化，第一次接收的消息是用户昵称
{
	int numbytes = 0;
	char buf[MAXDATASIZE];
	Users NewUser(-1, "\0");
	if ((numbytes = recv(new_fd, buf, MAXDATASIZE, 0)) == -1)//套接字文件描述符，接收到这字符串里，最大长度,最后一个参数同服务端
	{
		perror("recv():");
	}
	buf[numbytes] = '\0';//#signin
	string name("");
	if (buf[5] == 'I')
	{
		if (VerifyUser(buf, name))
		{
			Users temp(new_fd, name.c_str());
			NewUser = std::move(temp);
		}
		//密码错了就会返回-1的User
	}
	else if (buf[5] == 'U')
	{
		if (RegisterUser(buf, name))
		{
			Users temp(new_fd, name.c_str());
			NewUser = std::move(temp);
		}
		else
			NewUser.fd = -2;
	}
	else
		cout << "客户端传输数据的格式不对" << endl;
	return std::move(NewUser);
}