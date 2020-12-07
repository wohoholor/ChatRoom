#include "Loading_record.h"
list<Users> Clientfd{};//用链表来存储当前连接的用户
mutex List_Mutex;

void RecordLog(const string& str)
{
	ofstream outfile(MYFILENAME, ios::app);
	outfile << str << endl;
	outfile.close();
	return;
}

void SendLog(string& temp)
{
	lock_guard<mutex> objshuo(List_Mutex);//接下来都要操作链表，先锁住，离开作用域自动释放锁
	auto it = Clientfd.begin();
	temp += "\n";
	while (it != Clientfd.end())//遍历链表群发
	{
		if (send(it->fd, temp.c_str(), temp.size(), 0) == -1)//new_fd是accept返回套接字文件描述符，str就是你要发送的数据，数据长度，对于最后一位参数，0可看作默认参数，还有别的例如
		{
			perror("send():");//返回-1则发送失败
			close(it->fd);//发送失败就关闭通讯
			Clientfd.erase(it);
		}
		++it;
	}
}

void ReadRecord()
{
	ifstream recordfile(MYFILENAME, ios::ate);
	if (!recordfile.is_open())
	{
		cout << "未成功打开文件" << endl;
		return;
	}
	// 先倒回文件末尾两个字符
	recordfile.seekg(-2, recordfile.cur);
	// 假定反向读第3行的记录
	int lineCount = 16;
	for (int i = 0; i < lineCount; i++)
	{
		// 查看前一个字符是否为回车符
		while (recordfile.peek() != recordfile.widen('\n'))
		{
			//auto t = recordfile.peek();
			recordfile.seekg(-1, recordfile.cur);
			//if (t == recordfile.peek())
				//break;
		}
		// 走到这里表示跳过一行了，所以继续向前跳直到到3行
		recordfile.seekg(-1, recordfile.cur);
	}

	recordfile.seekg(2, recordfile.cur);
	std::string  line;
	//如果想将倒数lineCount行都输出，咋加入循环while
	getline(recordfile, line);//输出为‘\n’
	while (getline(recordfile, line))
	{
		SendLog(line);
	}
	recordfile.clear();
	recordfile.close();
	string lastlog("\n\n\n*** 以下为新消息 ***\n");
	SendLog(lastlog);
}