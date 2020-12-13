#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include<iostream>
#include <stdlib.h>
#include <winsock2.h>
#include<ctime>

#include <string>
#include <QDebug>
#include <QThread>
#include <QtConcurrent/QtConcurrent>


#define PORT 9999 // 你服务器开放的端口号，记得服务器的防火墙和安全策略都要开放该端口哦
#define MAXDATASIZE 1000 //每次可以接收的最大字节
#define IP "111"//你服务器的公网IP

using namespace std;

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(const QString &n = "头号玩家",QObject *parent = nullptr);
    void RecvThread();
    void SendMsg(const string &str);
    int sockfd;
    QString Currmsg;
    bool MyColor;
signals:
    void closeconnect();
    void GetMsg();
private:
    QString VerifyMsg;
    sockaddr_in their_addr;
};

string gettime();

#endif // CLIENT_H
