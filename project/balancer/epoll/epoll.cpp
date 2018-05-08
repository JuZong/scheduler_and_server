#include "epoll.h"
#include <stdlib.h>
Epoll::Epoll()
{
	m_epollfd=epoll_create(1024);
	if(m_epollfd==-1)
	{
		exit(0);
	}	
}
int  Epoll::SetNonBlocking(int fd)
{
	int old_option=fcntl(fd,F_GETFL);
	int new_option=old_option | O_NONBLOCK;
	fcntl(fd,F_SETFL,new_option);
	return old_option;	
}
void Epoll::AddFd(int fd)
{
	
	epoll_event event;
	event.data.fd=fd;
	event.events=EPOLLIN | EPOLLET;
	SetNonBlocking(fd);
	epoll_ctl(m_epollfd,EPOLL_CTL_ADD,fd,&event);
}
void Epoll::DelFd(int fd)
{
	epoll_ctl(m_epollfd,EPOLL_CTL_DEL,fd,NULL);
}
int Epoll::EpollWait()
{
 	return epoll_wait(m_epollfd,events,1024,-1);	
}
