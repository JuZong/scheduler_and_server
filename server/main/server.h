#ifndef SERVER_H
#define SERVER_H

/*
 * Server main task:
 * 		1.accept the connection of the scheduler
 * 		2.read msg from the TcpRecvBuff
 * 		3.deal packet 
 * 		4.send json to the MsgQueue;
 * child process:
 * 		5.read json from the MsgQueue;
 * 		6.parse json ,get http-header
 * 		7.handle the request http-header
 * 		8.compound the http-http-header
 * 		9.compound the json
 * 		10.send the json to the scheduler
 */
#include <string>
using namespace std;

const int CHILD_PROCESS_NUM = 3;
const int MAX_BUFF_SIZE = 512;
class Server
{
public:
	Server(const string &ip,int port);
	~Server();
	void run();
private:
	int mlisten_fd;
	int mscheduler_fd;

	string mip;
	string mport;

	int mmsg_id;
	pid_t mpid[CHILD_PROCESS_NUM];
};

#endif
