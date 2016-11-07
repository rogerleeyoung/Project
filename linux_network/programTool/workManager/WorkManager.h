#ifndef _WORKMANAGER_H
#define _WORKMANAGER_H
#include "../threadpool/Task.h"
class WorkManagerImpl;
class WorkManager
{
public:
	WorkManager(int size);
	~WorkManager();
	int addTask(Task* task);
	int init();
private:
	WorkManagerImpl *workManagerImpl;
};

#endif