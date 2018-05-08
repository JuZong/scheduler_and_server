

#include "scheuler.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <json/json.h>
#include "../../log/log.h"
#define PATHFILE "../../server/doc"
void* DealClientMess(void* arg);
void* DealServerMess(void* arg);
Scheuler *s;
Scheuler::Scheuler(string ip,short port)
{
	m_listenfd=socket(AF_INET,SOCK_STREAM,0);
	m_log=Logger::get_log(string("../../logfile/SeversLog.txt"));
	s=this;
	if(m_listenfd==-1)
	{
		m_log->error(string("Create m_listenfd is fail"));
		exit(0);
	}
	sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(port);
	saddr.sin_addr.s_addr=inet_addr(ip.c_str());
	int res=bind(m_listenfd,(struct sockaddr*)&saddr,sizeof(saddr));
	if(res==-1)
	{
		m_log->error(string("Bind mListen is Fail"));
		exit(0);
	}
	listen(m_listenfd,5);
	m_log->notice("Server Listen Start is Ok ");

	if(!LoadEtcFile("../../etc/server.etc"))
	{
			m_log->error("No Etc File");
			exit(0);
	}	
	pthread_t pid[4];
	/*create child phtread */
	for(int i=0;i<3;i++)
	{
		if(pthread_create(&pid[i],NULL,DealClientMess,&pipe[i])!=0)
		{
			m_log->notice("Create CliThread Ok");
		}
	}
	pthread_create(&pid[3],NULL,DealServerMess,this);	
}
int Scheuler::ConnectServer(string ip,short port)
{
	int Confd=socket(AF_INET,SOCK_STREAM,0);
	sockaddr_in caddr;
	memset(&caddr,0,sizeof(caddr));
	caddr.sin_family=AF_INET;
	caddr.sin_port=htons(port);
	caddr.sin_addr.s_addr=inet_addr(ip.c_str());
	int res= connect(Confd,(sockaddr*)&caddr,sizeof(caddr));
	if(res<0) {return -1;}
	return Confd;
}
bool Scheuler::LoadEtcFile(string etcpath)
{
	int count=0;
	Json::Reader reader;
	Json::Value root;
	char etcfile[256]="";
	int fd=open(etcpath.c_str(),O_RDONLY);
	if(fd==-1){ return false; }
	int filenum=read(fd,etcfile,255);

	//parse etcfile and connect ser
	reader.parse(etcfile,root);
	string ip;
	short port;
	m_log->warn("a Server Connecting");
	int sernums=root.size();
	for(int i=0;i<sernums;i++)
	{
		ip=root[i]["ip"].asString();
		port=root[i]["port"].asInt();
		int clifd;		
		if((clifd=ConnectServer(ip,port))>0)
		{

			m_log->notice("a Server Connect ok");
			m_hac.AddHaNode(clifd,ip,port,100);
			m_serfd.push_back(clifd);	
			count++;
		}
	}
	close(fd);
	if(count==0)
	{
		m_log->error("No Server Connect Success");	
		exit(0);
	}
	return true;
}
void Scheuler::Run()
{
	m_log->notice("Run()");
	Epoll ep;
	ep.AddFd(m_listenfd);
	int count=0;
	sockaddr_in caddr;
	socklen_t len=sizeof(caddr);
	while(1)
	{	
		int ret=ep.EpollWait();
		if(ret<0)
		{
			break;
		}
		for(int i=0;i<ret;i++)
		{
			int socketfd=ep.events[i].data.fd;
			if(socketfd==m_listenfd)
			{
				int fd=accept(socketfd,(sockaddr*)&caddr,&len);
				m_log->notice("A Client Connect");
				pipe[count++%3].send_sp(fd);
			}

		}
	}	
}
void RecvServerTmp(int fd)
{
	s->RecvSerMess(fd);
}
int Scheuler::RecvSerMess(int fd)
{
	cout<<"server recving message"<<endl;
	char buff[1024]="";
	Json::Value root;
	Json::Reader reader;
	int count=recv(fd,buff,30,MSG_PEEK);
	if(count<=0)
	{
		return -1;
	}
	int len;
	sscanf(buff,"%d",&len);//json length
	char *pos=strchr(buff,'#');
	recv(fd,buff,1+(pos-buff),0);
	recv(fd,buff,len,0);
	cout<<buff<<endl;

    reader.parse(buff,root);
	int clientfd=root["cli_fd"].asInt();
	string cmd=root["cmd"].asString();
	string data=root["http_header"].asString();
	int status=root["status"].asInt();
	send(clientfd,data.c_str(),data.size(),0);
	if(cmd=="get"&&status==1)
	{
		len=root["file_size"].asInt();
		string file=PATHFILE;
		file=file+root["file_name"].asString();
		cout<<file<<endl;
		int filefd=open(file.c_str(),O_RDONLY);//this do to;
		sendfile(clientfd,filefd,NULL,len);
		close(filefd);
	}
	close(clientfd);
	return 1;
	
}
void* DealServerMess(void *arg)
{

	Scheuler *s=(Scheuler*)arg;
	Epoll ep;
	for(int i=0;i<s->m_serfd.size();i++)
	{
		ep.AddFd(s->m_serfd[i]);
	}
	char buff[4]="";
	while(1)
	{
		s->m_log->notice("Deal Server Data");	
		int ret=ep.EpollWait(); //this is wrong
		if(ret<0)
		{
			break;
		}
		for(int i=0;i<ret;i++)
		{
			int serfd=ep.events[i].data.fd;
			int count=recv(serfd,buff,4,MSG_PEEK);
			if(count==0)
			{
				//close sering
					
				s->m_hac.DelHaNode(serfd);
				ep.DelFd(serfd);
				close(serfd);
				vector<int>::iterator it=s->m_serfd.begin();
				for(;it!=s->m_serfd.end();it++)
				{
					if(*it==serfd)
					{
						s->m_serfd.erase(it);
						break;
					}
				}
				if(s->m_serfd.size()==0)
				{
					s->m_log->error("All Servers is Fail");
					exit(0);
				}
				continue;
			}
			s->threadpool.append(serfd);
		}
	}	
}	
void* DealClientMess(void *arg)
{
	Sockpair* p=(Sockpair*)arg;	
	Epoll ep;	
	int pipefd=(p->sv[0]);
	ep.AddFd(pipefd);
	while(1)
	{
		//s->m_log->notice("Deal Client Data");
		int ret=ep.EpollWait();
		if(ret<0){break;}
		for(int i=0;i<ret;i++)
		{
			int socketfd=ep.events[i].data.fd;
			if(socketfd==pipefd)
			{
				int fd;
				p->recv_sp(&socketfd);
				ep.AddFd(socketfd);
			}
			else
			{
				int i=s->RecvCliMess(socketfd);
				if(i==-1)		
				{

	//				cout<<"a client exit"<<endl;
	//				close(socketfd);
	//				ep.DelFd(socketfd);
				}
		     }
		}	
	}
}

int Scheuler::RecvCliMess(int fd)
{
	Json::Value root;
	root["cli_fd"]=fd;
	char httpbuff[1024]="";
	int count=recv(fd,httpbuff,1024,0);
	if(count<=0)
	{
			// client closing
		return -1;	
	}

	root["http_header"]=httpbuff;
    string str=root.toStyledString();
	int len=str.size();
	char tmp1[20]="";
	sprintf(tmp1,"%d#",len);
	string str1=tmp1;
	str1=str1+str;
	struct sockaddr_in saddr;
	socklen_t length=sizeof(saddr);
	getpeername(fd,(sockaddr*)&saddr,&length);
	string ip=inet_ntoa(saddr.sin_addr);
	int port=ntohs(saddr.sin_port);
	int Serfd=m_hac.GetServer(ip,port);
	send(Serfd,str1.c_str(),str1.size(),0);
	return 1;
}
