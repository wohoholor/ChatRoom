#ifndef USERINIT_H
#define USERINIT_H

#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "Operate_Mysql.h"

using namespace std;
#define MAXDATASIZE 1000 //最大接收和发送的字节数

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
	Users& operator=(Users&& other) noexcept
	{
		fd = other.fd;
		if (name)
			delete name;
		name = other.name;
		other.name = nullptr;
		return *this;
	}
	~Users()
	{
		delete[] name;
	}
};

Users UserInit(int new_fd);

#endif // !USERINIT_H
