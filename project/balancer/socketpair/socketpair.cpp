#include"socketpair.h"
Sockpair::Sockpair()
{
	int error = socketpair(AF_UNIX,SOCK_STREAM,0,sv);
	if(error == -1)
	{
//		log->Error("init socketpair for thread communication failed");	
	}
}
void Sockpair::send_sp(int fd)
{
	char sendbuf[7] = "";
	sprintf(sendbuf,"%d#",fd);
	int error = send(sv[1],sendbuf,strlen(sendbuf),0);
	if(error == -1)
	{
//		log->Error("Send fd into socketpair failed");
	}
}
void Sockpair::recv_sp(int *pfd)
{//加锁成功返回0

	char recvbuff[7] = "";
	//recv 网络中断返回0，失败返回-1
	//errno被设置为EAGAIN:套接字已被标记为非阻塞，而接收操作被阻塞或接受超时
	//当对侧没有send，
	int error = recv(sv[0],recvbuff,6,MSG_PEEK);//处理粘包
	if(error==-1 && errno != EAGAIN)//缓冲区中无数据
	{
//		log->Error("=>Recv fd from socketpair failed with PEEK flg");
	}
	else if(error > 0)
	{
//		log->Warn(recvbuff);
		char *p = strchr(recvbuff,'#');
		if(p == NULL)
		{
//			log->Error("Not get pipe #");
		}
		sscanf(recvbuff,"%d",pfd);
		int len = p - recvbuff + 1;
		
		error = recv(sv[0],recvbuff,len,0);
		if(error == -1)
		{
//			log->Error("Recv fd from socketpair failed with not PEEK");
		}
	}
}
void Sockpair::close_send()
{
	close(sv[1]);
}
void Sockpair::close_recv()
{
	close(sv[0]);
}
