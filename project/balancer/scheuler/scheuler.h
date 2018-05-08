#ifndef _SCHEULER_
#define _SCHEULER_
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/sendfile.h>
#include <string.h>
#include <string>
#include "../epoll/epoll.h"
#include "../socketpair/socketpair.h"
#include "../../log/log.h"
#include "../consis_hash/consistent_hash.h"
#include "../../threadpool/threadpool.h"
using namespace std;
const int DEALTHREAD=3;
struct Node;
class Scheuler
{
	public:
		Scheuler(string ip,short port);
		bool LoadEtcFile(string etcpath);
		int ConnectServer(string ip,short port);//返回fd
		int RecvCliMess(int fd);
		int RecvSerMess(int fd);
		void Run();
	private:
		Logger *m_log;//log object
		HashCon m_hac;//hash cirle
		multimap<string,short> m_serinfo;//保存当前服务器的port和ip
		Sockpair pipe[3];
		vector<int> m_serfd;
		string m_ip;
		short m_port;
		int m_listenfd;
		ThreadPool threadpool;
		friend void* DealClientMess(void *arg);
		friend void *DealServerMess(void *arg);
};
#endif
