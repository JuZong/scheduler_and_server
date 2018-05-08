#ifndef _EPOLL_
#define _EPOLL_
#include <sys/epoll.h>
#include <fcntl.h>
class Epoll
{
	public:
		Epoll();
		int SetNonBlocking(int fd);
		void AddFd(int fd);
		void DelFd(int fd);
		int  EpollWait();
		int m_epollfd;
		epoll_event events[1024];
		friend void* DealClientMess(void *arg);
};
#endif
