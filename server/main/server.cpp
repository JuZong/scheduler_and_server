#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>//socket,bind,listen
#include <sys/types.h>//各种类型。size_t,信号量
#include <arpa/inet.h>//inet_addr
#include <netinet/in.h>//
#include <fcntl.h>//修改fd的属性
#include <errno.h>
#include <sys/msg.h>//消息队列
#include <sys/wait.h>
#include <sys/stat.h>
#include "json/json.h"

#include "../log/log.h"
#include "server.h"
#include "../semaphore/semaphore.h"
#include "../redis/redis.h"
#include "../http_parser/http_parser.h"
#include "../http_splicer/http_splicer.h"

extern Logger *log;

struct MsgType
{
	long mtype;
	char content[MAX_BUFF_SIZE];
};

//三个信号量
struct SemSet
{
	Semaphore *mutex;
	Semaphore *full;
	Semaphore *empty;
};

int dispose_packet(int fd)//粘包处理
{
	char buff[21] = "";
	int ret = recv(fd,buff,20,MSG_PEEK);
	if(ret == 0)
	{
		return 0;
	}
	else if(ret == -1 || errno == EAGAIN)
	{
		return -1;
	}
	char *p = strstr(buff,"#");
   if(p == NULL)
   {
	   return -2;
   }	   
   int left_len = p - buff;
   int packet_size = 0;
   sscanf(buff,"%d",&packet_size);
   recv(fd,buff,left_len+1,0);//读出#和它前面的数字
   return packet_size;
}


//计算用户所请求页面文件的大小
int get_file_size(string file_name)
{
	char buff[128] = "../doc";//文件位置
	strcat(buff,file_name.c_str());
	cout<<buff<<endl;
	struct stat fst;
	if(lstat(buff,&fst) == -1)
	{
		return -1;
	}
	return fst.st_size;
}

string compound_json(int cli_fd,string file_name,int file_size,string http_header,int status)
{
	Json::Value value;
	value["cli_fd"] = cli_fd;
	value["file_name"] = file_name;
	value["file_size"] = file_size;
	value["http_header"] = http_header;
	value["status"] = status;
	value["cmd"]="get";
	return value.toStyledString();
}

string compound_packet(const string &str)
{
	int size = str.size();
	char buff[21] = "";
	sprintf(buff,"%d#",size);
	string ret_str(buff);
	ret_str.append(str);
	return ret_str;
}

//打印各个信号量的值
void print_semvalue(Semaphore &mutex,Semaphore &full, Semaphore &empty)
{
	char buff[1024] = "";
	sprintf(buff,"\nmutex:%d\nfull:%d\nempty:%d\n",mutex.get_sem_value(),full.get_sem_value(),empty.get_sem_value());
	log->warn(buff);
}

// child process
void child_process(int scheduler_fd,int index,int msg_id,SemSet *sem_setp)
{
	char log_buff[128] = "";
	sprintf(log_buff,"child_process[%d] create sunccessfully",index);
	log->notice(log_buff);
	
	log->notice("child process ready to recv msg from MsgQueue");
	//while(1), to recv buff form msg queue,and deal it,at last send it to scheduler
	Semaphore &mutex = *(sem_setp->mutex);
	Semaphore &full = *(sem_setp->full);
	Semaphore &empty = *(sem_setp->empty);

	log->notice("chid process init get the semaphore value");
	print_semvalue(mutex,full,empty);
	Http_parser parser;
	Http_splicer splicer;
	while(true)
	{
		log->warn("child process try to get semaphore");
		full.sem_p();
		mutex.sem_p();
		log->warn("child process enter lock");
		print_semvalue(mutex,full,empty);

		MsgType msg;
		memset(&msg,0,sizeof(msg));
		msg.mtype = 1;
		int ret = msgrcv(msg_id,(void *)&msg,MAX_BUFF_SIZE,1,0);
		if(ret == -1)
		{
			if(errno == E2BIG)
			{
				log->warn("the msg size is so big");
				continue;
			}
			log->warn("child process recv buff from the MsgQueue error");
			break;	
		}

		mutex.sem_v();
		empty.sem_v();
		log->notice("child process extern unlock");
		print_semvalue(mutex,full,empty);
		memset(log_buff,0,128);
		sprintf(log_buff,"child process[%d] recv one msg from MsgQueue",index);
		log->notice(log_buff);
		log->notice(msg.content);//打印从消息队列中接受到的json串
		
		Json::Reader reader;
		Json::Value json_from_scheduler;
		if(reader.parse(msg.content,json_from_scheduler)== -1)
		{
			log->warn("child process: recv Json parse error");
			continue;
		}
		log->notice("json_from_scheduler  parse  successfully");
		int cli_fd = json_from_scheduler["cli_fd"].asInt();
		string http_header = json_from_scheduler["http_header"].asString();

		//解析http报头
		if(!parser.start_parse(http_header.c_str()))
		{
			log->warn("http_header parse error");
			continue;
		}
		log->notice("http_header parse successfully");
		int status_code = 200;
		string mothd = parser.get_mothd();
		string file_name = parser.get_file_name();
		int  status = 0;
		if(mothd.compare("GET") == 0)
		{
			if(!parser.is_file_available())//无用户申请的文件爱你
			{
				status_code = 404;
			}
			
		}
		else if(mothd.compare("POST") == 0)
		{
			
		}
		else
		{
			status_code = 403;
		}
		//封装http报头
		splicer.start_splice();
		splicer.splice_statues_line(parser.get_protocol(),status_code);
		splicer.splice_msg_header();
		if(status_code == 200)
		{
			splicer.splice_content_length(get_file_size(file_name));
			status = 1;
		}
		splicer.splice_empty_line();
		log->notice("compound http_header over");
		log->notice("begin compound json and packet");

		string json_str = compound_json(cli_fd,file_name,get_file_size(file_name),splicer.get_result(),status);
		log->notice("compound json_str successfuly");
		string packet_str = compound_packet(json_str);
		log->notice("compound packet successfully");
		log->notice(packet_str);
		log->notice("over, send msg to the scheduler");
		ret = send(scheduler_fd,packet_str.c_str(),packet_str.size(),0);
		if(ret == -1)
		{
			log->error("send packet to scheduler error");
			break;
		}
		if(ret == 0)
		{
			log->error("scheduler closed... please check it");
			break;
		}
		log->notice("send packet scheduler successfully");
	}
}

Server::Server(const  string &ip, int port)
{
	mip = ip;
	mport = port;
	log->notice("server log begin");
	mlisten_fd = socket(AF_INET,SOCK_STREAM,0);
	if(mlisten_fd == -1)
	{
		log->error("server socket create error");
		exit(0);
	}
	log->notice("server socket create successfully");
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ip.c_str());
	server.sin_port = htons(port);
	int err = bind(mlisten_fd,(sockaddr *)&server,sizeof(server));
	if(err == -1)
	{
		log->error("server socket bind error");
		exit(0);
	}
	log->notice("server socket bind successfully");
	
	err = listen(mlisten_fd,5);
	if(err == -1)
	{
		log->error("server socket listen error");
		exit(0);
	}
	log->notice("server socket listen ... ...");

	log->notice("server socket init over");
}

Server::~Server()
{
	log->notice("close server");
	close(mlisten_fd);
}

/*run:
 * 1.init the MsgQueue
 *   Main process:
 *   	2.while(1) , read the TcpReecvBuffer and dispose packet
 *   	3.send json to  MsgQueue()
 *   child process:
 *   	4.recv json from MsgQueue
 *   	5.parse json
 *   	6.parse http-header
 *   		(get).call scheduler sendfile to clifd
 *   		(post).get the source from redis
 *   	7.compound http-header
 *   	8.compound json
 *   	9.plus length 
 *   	10.send to the scheduler
 */
 void Server::run()
{
	log->notice("a server is running...");

	//连接调度器
	sockaddr_in scheduler;
	socklen_t len;
	mscheduler_fd = accept(mlisten_fd,(sockaddr *)&scheduler,&len);
	if(mscheduler_fd == -1)
	{
		log->error("server socket accept scheduler error");
	}
	log->notice("scheduler connect successfully");

	log->notice("init semaphore...");
	/* 信号量
	 * mutex：互斥
	 * full：缓冲区满
	 * empty：缓冲区空
	*/
	Semaphore mutex((key_t)1*1234);
	Semaphore full((key_t)2*1234);
	Semaphore empty((key_t)3*1234);

	mutex.init_sem(1);
	full.init_sem(0);
	empty.init_sem(30);
	
	SemSet sem_set;
	sem_set.mutex = &mutex;
	sem_set.full = &full;
	sem_set.empty = &empty;

	log->notice("semaphore init successfully");

	log->notice("init Message Queue");
	mmsg_id = msgget((key_t)1234,0666|IPC_CREAT); 
	if(mmsg_id == -1)
	{
		log->error("msgqueue create error");
	}
	log->notice("init msgqueue successfully");

	log->notice("start child process to deal message ");
	for(int i=0; i<CHILD_PROCESS_NUM; i++)
	{
		mpid[i] = fork();
		if(mpid[i] == 0)//子进程执行自己的东西
		{
			log->notice("fork child process successfully");
			child_process(mscheduler_fd,i,mmsg_id,&sem_set);
			log->warn("a child process exit");
		}
		else if(mpid[i] == -1)//
		{
			log->error("fork child process error");
			return;
		}
		else//主进程继续fork
		{
			continue;
		}
	}

	//recv packet from scheduler
	while(1)
	{
		int packet_size = dispose_packet(mscheduler_fd);
		if(packet_size == 0)
		{
			log->warn("scheduler close, break");
			break;
		}
		
		if(packet_size == -1)
		{
				log->warn("recv error,  again");
				continue;
		}
		log->notice("get one new packet");
		log->notice("get the packet size,recv it now ...");

		string recv_buff;
		//防止包太长，所以while循环读
		while(packet_size != 0)
		{
			char buff[1024] = "";
			int recv_size = recv(mscheduler_fd,buff,1024>packet_size?packet_size:1024,0);
			cout<<buff<<endl;
			packet_size -= recv_size;
			if(packet_size < 0)
			{
				log->error("main process recv  error");
				break;
			}
			recv_buff.append(buff);
		}
		if(packet_size != 0)
		{
			log->error("packet recv error,check");
			continue;
		}
		log->notice("recv one packet successfully ...");

		log->notice("send the packet to the MsgQueue");
		
		empty.sem_p();
		mutex.sem_p();
		log->warn("main process enter lock");
		print_semvalue(mutex,full,empty);
		//向子进程发送一条消息
		MsgType msg;
		msg.mtype = 1;
		strcpy(msg.content,recv_buff.c_str());
		msgsnd(mmsg_id,(void *)&msg,MAX_BUFF_SIZE,0);

		mutex.sem_v();
		full.sem_v();
		log->warn("main process extern unlock");
		print_semvalue(mutex,full,empty);
	}
	log->warn("server main process has down,waiting for the child process");
	//wait for child exit
	wait(NULL);
	log->warn("server will closed...");

}

