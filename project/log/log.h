#ifndef LOG_H
#define LOG_H
#include<string>
#include<unistd.h>
#include<fcntl.h>
#include"../mutex/mutex.h"

using namespace std;
class Logger
{
	private:
		Logger(const string &filename);
		string get_current_time();//获取系统当前时间 
		int fd;
		static  Logger *log;
		static  Mutex mutex;
		
	public:
		static Logger *get_log(const string &filename);
		void notice(const string &message);
		void error(const string &message);
		void warn(const string &message);
		~Logger();
};
#endif
