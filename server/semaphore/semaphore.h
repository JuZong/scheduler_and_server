/* Semaphore
 *1.create one semaphore (2-value)
 *2.destory the semapthore
 *3.P
 *4.V
*/
#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include <sys/sem.h>
union semun
{
	int val;
};


class Semaphore
{
public:
	Semaphore(key_t _key);
	~Semaphore();
	//set init_value
	bool init_sem(int _init_value);		
	//P,V for the semapthore
	bool sem_p();
	bool sem_v();
	int get_sem_value();
private:
	int sem_id;	
};
#endif

