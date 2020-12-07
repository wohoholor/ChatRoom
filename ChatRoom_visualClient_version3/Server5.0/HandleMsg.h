#pragma once
#include <thread>
#include "UserInit.h"
#include "Loading_record.h"

string gettime();//用此函数来记录接受到消息的时间
void AllSend(const char str[], Users& new_user);//每当接受到消息，就群发给所有群友，除了发送者本身
void Whenleave(Users& new_user);
void RecvThread(Users new_user);//接收线程
void WelCome(Users& new_user);
void BeforeLeave(int sockfd);
void SendMsgTo(int sockfd,const char msg[]);

using namespace std;