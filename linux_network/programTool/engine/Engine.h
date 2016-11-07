#ifndef _ENGINE_H
#define _ENGINE_H
#include <map>
#include "../workManager/WorkManager.h"
#include "../unit/UnitBase.h"
#include <pthread.h>
#include <errno.h>
class Engine
{
public:
	Engine(char *path,int size);
	UnitBase* createUnit(const char* soPath);
    void  destroyUnit(const char* soPath , UnitBase *unitToBeDestory);
	bool loadUnit();
	void deleteUnit();
    void runNetServer(int index);
    int run();
	~Engine();
private:
	char* configPath;
    bool netOpen;
    int startUnitIndex;
    int startFuncIndex;
    int netUnitIndex;
	WorkManager workManager;
	map<int, UnitBase*> unitMap;
    pthread_t netServerTid;
    pthread_t hearBeatTid;

};



#endif
