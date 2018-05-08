#ifndef _CONSISTENT_
#define _CONSISTENT_
#include "md5.h"
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <algorithm>
using namespace std;
class HaItem;
struct ServerNode
{
	vector<string> mVirNode;
	int mSerfd;
	ServerNode(int fd):mSerfd(fd)
	{

	}
};
struct HaItem
{
	ServerNode *mSerNode;
	string Md5;//md5
	int fd;
	HaItem(ServerNode *mser,string str,int mfd);

};
class HashCon
{
	private:
		map<string,HaItem*> HaC;
		vector<ServerNode*> HacNode;
	public:
		string Md5Work(const string &str);
		void AddHaNode(int fd, string ip,short port,int count);
		void DelHaNode(int fd);
		void AddVirtual(string ip,short port,ServerNode *node,int count);
		void DelVirNode(ServerNode *mSer,int count);
		int GetServer(string ip,short port);
};
#endif
