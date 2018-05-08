#include<time.h>
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include"log.h"
using namespace std;
Logger *Logger::log =NULL;
Mutex Logger::mutex;
Logger::Logger(const string &filename)
{
	fd = open(filename.c_str(),O_WRONLY|O_APPEND|O_CREAT,0666);
	if(fd == -1)
	{
		perror("open logfile error");
		exit(0);
	}
}
string Logger::get_current_time()
{
	time_t t;
	time(&t);
	struct tm *p_tm = localtime(&t);
	char timer[32]="";
	sprintf(timer,"[%d-%d %d:%d:%d]",1+p_tm->tm_mon,p_tm->tm_mday,p_tm->tm_hour,p_tm->tm_min,p_tm->tm_sec);
	return string(timer);
}

Logger *Logger::get_log(const string &filename)
{
	if(log == NULL)
	{
		mutex.lock();
		if(log == NULL)
		{
			log = new Logger(filename);
		}
		mutex.unlock();
	}
    return log;
}

Logger::~Logger()
{
	close(fd);
	free(log);
}

void Logger::notice(const string &message)
{
	string prefix="[NOTICE:]";
	string log_time = get_current_time();
	mutex.lock();
	int error = write(fd,prefix.c_str(),prefix.size());
	write(fd,message.c_str(),message.size());
	write(fd,log_time.c_str(),log_time.size());
	write(fd,"\n",2);
	mutex.unlock();
	printf("\033[32m%s%s %s\033[0m\n",prefix.c_str(),message.c_str(),log_time.c_str());
}

void Logger::warn(const string &message)
{
	string prefix("[WARN:]");
	string log_time = get_current_time();
	mutex.lock();
	int error = write(fd,prefix.c_str(),prefix.size());
	write(fd,message.c_str(),message.size());
	write(fd,log_time.c_str(),log_time.size());
	write(fd,"\n",2);
	mutex.unlock();
	printf("\033[33m%s%s %s\033[0m\n",prefix.c_str(),message.c_str(),log_time.c_str());
}

void Logger::error(const string &message)
{
	const string prefix("[ERROR:]");
	string log_time = get_current_time();
	mutex.lock();
	int error = write(fd,prefix.c_str(),prefix.size());
	write(fd,message.c_str(),message.size());
	write(fd,log_time.c_str(),log_time.size());
	write(fd,"\n",2);
	mutex.unlock();
	printf("\033[31m%s%s %s\033[0m\n",prefix.c_str(),message.c_str(),log_time.c_str());
}
