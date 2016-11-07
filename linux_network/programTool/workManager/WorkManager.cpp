#include "WorkManager.h"
#include "../threadpool/ThreadPool.h"
class WorkManagerImpl
{
public:
	WorkManagerImpl(int size);
	int addTask(Task* task);
	int init();
	~WorkManagerImpl();
private:	
    ThreadPool threadPool;
};

WorkManagerImpl::WorkManagerImpl(int size):threadPool(size)
{
}
int WorkManagerImpl::addTask(Task* task)
{
	threadPool.add_task(task);
	return 0;
}
int WorkManagerImpl::init()
{
	return threadPool.initialize_threadpool();
}

WorkManagerImpl::~WorkManagerImpl()
{

}


WorkManager::WorkManager(int size)
{
	workManagerImpl = new WorkManagerImpl(size);
}
WorkManager::~WorkManager()
{
	delete workManagerImpl;
}

int WorkManager::addTask(Task* task)
{
	return workManagerImpl->addTask(task);
}
int WorkManager::init()
{
	return workManagerImpl->init();
}

