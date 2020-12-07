#include "Operate_Mysql.h"

string HandleStr(const char str[],string& User_id, string& User_password)
{
	string User_name("");
	auto ptr = str + 8;
	int data_count = 3;
	while (*ptr != '\0')
	{
		if (*ptr == ',')
		{
			if (*(ptr + 1) == ',')
			{
				switch (data_count)
				{
				case 3:
					User_id.push_back(*ptr);
					break;
				case 2:
					User_password.push_back(*ptr);
					break;
				case 1:
					User_name.push_back(*ptr);
					break;
				default:
					return "";
				}
				ptr = ptr + 2;
				continue;
			}
			--data_count;
		}
		else
		{
			switch (data_count)
			{
			case 3:
				User_id.push_back(*ptr);
				break;
			case 2:
				User_password.push_back(*ptr);
				break;
			case 1:
				User_name.push_back(*ptr);
				break;
			default:
				return "";
			}
		}
		++ptr;
	}
	return User_name;
}

bool Is_User_Exist(string User_id)
{
	string mysql_cmd("select User_password,User_name from user_tab where User_id=");
	mysql_cmd += User_id;
	MYSQL conn;
	int res;
	mysql_init(&conn);
	if (mysql_real_connect(&conn, "localhost", MYSQL_USER, MYSQL_PASSWORD, DATABASE_NAME, 0, NULL, CLIENT_FOUND_ROWS))
	{
		mysql_query(&conn, "set names utf8");
		res = mysql_query(&conn, mysql_cmd.c_str());
		if (res)
			printf("查找指令执行失败\n");
		else
		{
			printf("数据库指令执行成功\n");
			MYSQL_RES* result = mysql_store_result(&conn);
			MYSQL_ROW row = mysql_fetch_row(result);
			int num = mysql_num_fields(result);
			if (row)
			{
				mysql_close(&conn);
				return true;
			}
			else
			{
				cout << "没有该账号" << endl;
				mysql_close(&conn);
				return false;
			}
		}
		mysql_close(&conn);
	}
	cout << "出现别的问题" << endl;
	return false;
}

bool VerifyUser(const char str[], string& name)
{
	string User_id("");
	string User_password("");
	HandleStr(str, User_id, User_password);
	string mysql_cmd("select User_password,User_name from user_tab where User_id=");
	mysql_cmd += User_id;
	MYSQL conn;
	int res;
	mysql_init(&conn);
	if (mysql_real_connect(&conn, "localhost", MYSQL_USER,MYSQL_PASSWORD , DATABASE_NAME, 0, NULL, CLIENT_FOUND_ROWS))
	{
		printf("成功连接\n");
		mysql_query(&conn, "set names utf8");
		res = mysql_query(&conn, mysql_cmd.c_str());
		if (res)
			printf("error\n");
		else
		{
			printf("数据库指令执行成功\n");
			MYSQL_RES* result = mysql_store_result(&conn);
			MYSQL_ROW row = mysql_fetch_row(result);
			int num = mysql_num_fields(result);
			if (row)
			{
				if (row[0] == User_password)
				{
					cout << "账号密码验证正确" << endl;
					name = row[1];
					mysql_close(&conn);
					return true;
				}
				else
				{
					cout << "账号密码验证错误！！" << endl;
					mysql_close(&conn);
					return false;
				}
			}
			else
			{
				cout << "没有该账号" << endl;
				mysql_close(&conn);
				return false;
			}
		}
		mysql_close(&conn);
	}
	cout << "出现别的问题" << endl;
	return false;
}


bool RegisterUser(const char str[],string &name)
{
	string User_id("");
	string User_password("");
	name = HandleStr(str, User_id, User_password);
	if (Is_User_Exist(str))
		return false;
	stringstream temp_cmd;
	temp_cmd << "insert into user_tab(User_id,User_password,User_name,Register_time) values(" << User_id << ",\"" << User_password << "\",\"" << name << "\"," << "NOW())";
	string mysql_cmd = temp_cmd.str(); 
	//string mysql_cmd("insert into user_tab(User_id,User_password,User_name,Register_time) values(%s,%s,%s,%s)");
	//mysql_cmd += User_id;
	MYSQL conn;
	int res;
	mysql_init(&conn);
	if (mysql_real_connect(&conn, "localhost", MYSQL_USER, MYSQL_PASSWORD, DATABASE_NAME, 0, NULL, CLIENT_FOUND_ROWS))
	{
		printf("成功连接\n");
		mysql_query(&conn, "set names utf8");
		res = mysql_query(&conn, mysql_cmd.c_str());
		if (res)
			printf("error\n");
		else
		{
			printf("数据库指令执行成功（注册成功）\n"); 
			mysql_close(&conn);
			return true;
		}
		mysql_close(&conn);
	}
	cout << "出现别的问题" << endl;
	return false;
}