#include"mutex.h"
#include<pthread.h>
#include<assert.h>
Mutex::Mutex()
{
    error = pthread_mutex_init(&mutex,NULL);
    assert(error == 0);
}
Mutex::~Mutex()
{

}
void Mutex::lock()
{
    error = pthread_mutex_lock(&mutex);
    assert(error == 0);
}
/*void Mutex::trylock()
{
    error = pthread_mutex_trylock(&mutex);
    assert(error == 0);
}*/
void Mutex::unlock()
{
    error = pthread_mutex_unlock(&mutex);
    assert(error == 0);
}
