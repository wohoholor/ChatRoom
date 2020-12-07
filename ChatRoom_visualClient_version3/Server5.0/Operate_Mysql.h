#ifndef OPERATE_MYSQL_H
#define OPERATE_MYSQL_H

#include <mysql/mysql.h>
#include <string.h>
#include <iostream>
#include <sstream> 

using namespace std;

#define MYSQL_USER "数据库用户名"
#define MYSQL_PASSWORD "数据库密码"
#define DATABASE_NAME "数据库名"

bool Is_User_Exist(string User_id);
string HandleStr(const char str[], string& User_id, string& User_password);
bool VerifyUser(const char str[], string& name);
bool RegisterUser(const char str[],string &name);


#endif // !OPERATE_MYSQL_H
