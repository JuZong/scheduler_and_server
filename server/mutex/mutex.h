#ifndef MUTEX_H
#define MUTEX_H
#include<pthread.h>
//#include"../threadpool/threadpool.h"
class ThreadPool;
class Mutex
{
    private:
        int error;
        pthread_mutex_t mutex;
		friend  class ThreadPool;
    public:
        Mutex();
        void lock();
//        void trylock();
        void unlock();
        ~Mutex();
	
};
#endif
