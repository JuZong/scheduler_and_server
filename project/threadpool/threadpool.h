#ifndef _THREADPOOL_                                                                                                                 
#define _THREADPOOL_
#include <list>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <pthread.h>
#include "../mutex/mutex.h"
#include"../log/log.h"
using namespace std;

class ThreadPool
{
   public:
     ThreadPool();
	 ~ThreadPool();  
	 
	 void add_work_thread();	 
     void append(int clifd);// add a task
	 static void *work(void *arg);
	 void run();
	 Mutex  mutex;
 //  private:
	 static Logger *log;
	 int m_reduce_flag;//告诉线程该减少了
	 int m_need_check_reduce;//进入reduce判断
	 int m_thread_num;	//当前线程数量
	 int m_max_thread_num; // 最大线程数
	 list <int> m_taskqueue; //任务队列
	 vector<pthread_t> m_thread_id;  //线程数组
	 pthread_cond_t queue_ready; //唤醒线程的条件变量
	 //Mutex mutex;	 
};
#endif

