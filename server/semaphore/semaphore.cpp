#include "semaphore.h"
#include <iostream>
#include <assert.h>
#include <string.h>

Semaphore::Semaphore(key_t _key)
{
	sem_id = semget(_key,1,IPC_CREAT|0666);//sem_id对ctl和op函数有用。
	assert(sem_id != -1);
}
Semaphore::~Semaphore()
{
	union semun sem_union;
	int ret = semctl(sem_id,0,IPC_RMID,sem_union);//从内核中删除信号量集合
	if(ret == -1)
	{
		std::cerr<<"destory sem fail"<<std::endl;
	}
}

bool Semaphore::init_sem(int _init_value)
{
	union semun sem_union;
	sem_union.val = _init_value;
	int ret = semctl(sem_id,0,SETVAL,sem_union);//給信号量设置初值
	if(ret == -1)
	{
		return false;
	}
	else
		return true;
}

bool Semaphore::sem_p()
{
	struct sembuf sem;
	sem.sem_num = 0; //信号量集合中的信号量编号，0代表第1个信号量
	sem.sem_op = -1;//若val<0进行P操作信号量值减val，若(semval-val)<0（semval为该信号量值），则调用进程阻塞，直到资源可用；
	sem.sem_flg = 0;//*0 设置信号量的默认操作
	if(semop(sem_id,&sem,1)== -1)//最后一个参数：设置此值等于1，只完成对一个信号量的操作
	{
		return false;
	}
	return true;
}
bool Semaphore::sem_v()
{
	struct sembuf sem;
	sem.sem_num = 0;
	sem.sem_op = 1;
	sem.sem_flg = 0;
	if(semop(sem_id,&sem,1) == -1)
	{
		return false;
	}
	return true;
}

int Semaphore::get_sem_value()
{
	semun sem_union;
	memset(&sem_union,0,sizeof(sem_union));
	int ret = semctl(sem_id,0,GETVAL,sem_union);//返回信号量的val（sem_union.val）
	if(ret == -1)
	{
		return -1;
	}
	return ret;
}


