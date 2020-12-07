#ifndef OPERATE_MYSQL_H
#define OPERATE_MYSQL_H

#include <mysql/mysql.h>
#include <string.h>
#include <iostream>
#include <sstream> 

using namespace std;

#define MYSQL_USER "Ho"
#define MYSQL_PASSWORD "1q23lyc45j"
#define DATABASE_NAME "chatting_room"

bool Is_User_Exist(string User_id);
string HandleStr(const char str[], string& User_id, string& User_password);
bool VerifyUser(const char str[], string& name);
bool RegisterUser(const char str[],string &name);


#endif // !OPERATE_MYSQL_H