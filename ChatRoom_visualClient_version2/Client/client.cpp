#include "client.h"


Client::Client(const QString &n,QObject *parent) : QObject(parent)
{
    name = n;
    MyColor = true;
    WORD sockVersion = MAKEWORD(2, 2);//协议版本2.2
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        //return 0;//如果申请得到协议版本不是2.2则失败
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)//申请套接字，并将套接字标识号给sockfd
    {
        qDebug() << WSAGetLastError();//如果申请失败则输出错误信息
        qDebug() << "socket():错误";
    }
    their_addr.sin_family = AF_INET;//IP/TCP协议
    their_addr.sin_port = htons(PORT);//转换端口号的数据类型并存储在结构体内
    their_addr.sin_addr.S_un.S_addr = inet_addr(IP);//转换IP的数据类型并存储在结构体内
    memset(their_addr.sin_zero, 0, sizeof(their_addr.sin_zero));//将sockaddr_in转换成sockaddr前要补零
    if (::connect(sockfd, (sockaddr*)&their_addr, sizeof(sockaddr)) == -1)//用申请的套接字和套接字信息去建立连接
    {
        qDebug() << "connect():错误";
    }
    for (int i = 0; i < 9; ++i)//尝试九次，如果发送不成功就放弃
    {
        int r = send(sockfd, name.toUtf8().constData(), name.toUtf8().size(), NULL);
        if (r > 0)
            break;
        else
            continue;
    }
    //std::thread mythread(&Client::RecvThread,this);//创建接收消息线程
    QFuture<void> mythread = QtConcurrent::run(this,&Client::RecvThread);
}

void Client::RecvThread()//将以这个函数创建一个接收服务器消息的线程
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
        //qDebug() << buf;//打印字符串
        Currmsg = buf;
        MyColor = true;
        emit this->GetMsg();
    }
}

void Client::SendMsg(const string &str)
{
    int r = send(sockfd, str.c_str(), str.size(), NULL);//发送输入的信息
    if (r <= 0)
    {
        qDebug() << "send():";//发送失败，输出错误信息，break退出
    }
//    if (str == "ends" || str.empty())//输入ends结束通信
//        emit this->closeconnect();
    string temp = "*** ";
    temp+=gettime();
    temp+="你 ***\n  ";
    temp+=str;
    Currmsg = temp.c_str();
    MyColor = false;
    emit this->GetMsg();
}

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
