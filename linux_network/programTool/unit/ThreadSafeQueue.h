#ifndef  _THREADSAFEQUEUE_H
#define  _THREADSAFEQUEUE_H

#include <queue>
#include <pthread.h>
using namespace std;
template <typename T>  class ThreadSafeQueue
{
private:
    std::queue<T> dataQueue;
    pthread_mutex_t m_pthreadMutex ;
public:
    ThreadSafeQueue();
    void push(const T &data);
    T get_front();
    bool isEmpty();
};

template <typename T>  ThreadSafeQueue<T>::ThreadSafeQueue()
{
        pthread_mutex_init(&m_pthreadMutex,NULL);
}

template <typename T>  void ThreadSafeQueue<T>::push(const T &data)
{
pthread_mutex_lock(&m_pthreadMutex);
dataQueue.push(data);
pthread_mutex_unlock(&m_pthreadMutex);
}

template <typename T>  T ThreadSafeQueue<T>::get_front()
{
pthread_mutex_lock(&m_pthreadMutex);
T tempdata = dataQueue.front();
dataQueue.pop();
pthread_mutex_unlock(&m_pthreadMutex);
return tempdata;
}
template <typename T>  bool ThreadSafeQueue<T>::isEmpty()
{
pthread_mutex_lock(&m_pthreadMutex);
bool  flag= dataQueue.empty();
pthread_mutex_unlock(&m_pthreadMutex);
return flag;
}

#endif
