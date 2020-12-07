#pragma once
#include <fstream>
#include <thread>
#include <list> 
#include <mutex>
#include "UserInit.h"

using namespace std;
#define MYFILENAME "recording.log"
extern list<Users> Clientfd;//用链表来存储当前连接的用户
extern mutex List_Mutex;

void RecordLog(const string& str);
void SendLog(string& temp);
void ReadRecord();

