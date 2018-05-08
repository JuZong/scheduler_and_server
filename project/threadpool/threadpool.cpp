#include"threadpool.h"
//构造
Logger* ThreadPool::log=NULL;
void RecvServerTmp(int fd);
ThreadPool::ThreadPool():m_thread_num(3),m_reduce_flag(0),m_need_check_reduce(0),m_max_thread_num(8)
{
	log=Logger::get_log("../../logfile/ServerLog.txt");
	m_thread_id.resize(3);
    pthread_cond_init(&queue_ready,NULL);
	for (int i = 0; i <3; ++i)
	{
		if (pthread_create(&m_thread_id[i],NULL,work,this)!=0)
		{
			exit(0);
		}
		if (pthread_detach(m_thread_id[i]))
		{
			exit(0);
		}
	}
	log->notice("Create ThreadPool OK");
}

ThreadPool::~ThreadPool()
{	
}

void ThreadPool::add_work_thread()
{
	mutex.lock();
    pthread_t t;
    if(pthread_create(&t,NULL,work,(void*)this)!=0)
	{
		exit(0);
	}
	if(pthread_detach(t)!=0)
	{
		exit(0);
	}	
    m_thread_id.push_back(t);
	m_thread_num = m_thread_num+1;
	m_need_check_reduce = 1;
	cout<<"current work_thread number :"<<m_thread_num<<endl;
	log->warn("add a extra work_thread");
	mutex.unlock();
}

void ThreadPool::append(int clifd)// add a task
{
    mutex.lock();
    m_taskqueue.push_back(clifd);
    log->notice("clifd push");
    mutex.unlock();
    pthread_cond_signal(&(queue_ready));
}
void* ThreadPool::work(void *arg)
{
	log->notice("work begin");
	ThreadPool *pool = (ThreadPool *)arg;
	pool->run();
	return pool;
}
void ThreadPool::run()
{
	while (1)
	{
	    mutex.lock();
		while(m_taskqueue.empty())
		{
			pthread_cond_wait(&(queue_ready),&mutex.mutex);
		}
		if(m_reduce_flag==1 && m_thread_num>3)
		{
			log->warn("reduce extra a work_thread");
			
			cout<<"current work_thread number: "<<m_thread_num<<endl;
			if(m_thread_num==3)
			{
					m_reduce_flag=0;
					m_need_check_reduce = 0;
			}
			pthread_t pid=pthread_self();
			vector<pthread_t>::iterator it=m_thread_id.begin();
			for(;it!=m_thread_id.end();it++)
			{
				if(*it==pid)
				{
					m_thread_id.erase(it);
                    break;
				}
			}
			m_thread_num--;
			mutex.unlock();
			pthread_exit(NULL);
		}
		
		int Serfd = m_taskqueue.front();
		log->notice("Serverfd pop");
		//处理函数
		RecvServerTmp(Serfd);
		m_taskqueue.pop_front();
		mutex.unlock();
		sleep(1);		
	}
}


