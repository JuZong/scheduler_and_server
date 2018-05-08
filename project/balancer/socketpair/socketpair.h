#ifndef _SOCKETPAIR_H
#define _SOCKETPAIR_H

#include<unistd.h>//系统调用
#include<sys/types.h> //size_t  pid_t  time_t
#include<sys/socket.h>
#include<iostream> 
//#include"../log/mutex.h"
#include<errno.h> //定义整数变量errno
//#include "../log/log.h"
#include<stdio.h>
#include<string.h>
using namespace std;
class Sockpair
{
public:
	int sv[2];
	Sockpair();
	void send_sp(int fd);
	void recv_sp(int *pfd);
	void close_send();
	void close_recv();
};
  
#endif
