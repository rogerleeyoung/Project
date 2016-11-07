#include "Engine.h"
#include "yaml-cpp/yaml.h"
#include <stdio.h>
#include <dlfcn.h>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <map>
#include "../netUnit/NetUnit.h"
#include "../testUnit/TestUnit.h"
using namespace std;

Engine::Engine(char *path,int size):workManager(size),configPath(path)
{
    netOpen = false;
}
UnitBase* Engine::createUnit(const char* soPath)
{
	void *handle;
	handle=dlopen(soPath,RTLD_NOW);
	if(!handle)
	{
	    //printf("load so error\n");
	    printf("dlopen err:%s.\n",dlerror());
	    return NULL;
	}
	// load the symbols
	create_t create_unit = (create_t) dlsym(handle, "create");
	const char* dlsym_error = dlerror();
	if (dlsym_error) {
	    printf("Cannot load symbol create:%s\n",dlsym_error);
	    return NULL;
	}
	UnitBase* temp=create_unit();
	if(handle)
	{
	    printf("close handle\n");
	    //dlclose(handle);
	}
	// create an instance of the class
	return temp;
}
void  Engine::destroyUnit(const char* soPath , UnitBase *unitToBeDestory)
{
	 if(!unitToBeDestory)
	{
	    cout<<"destory NULL point"<<endl;
	    return;
	}
	void *handle;
	printf("delete start\n");
	handle=dlopen(soPath,RTLD_NOW);
	if(!handle)
	{
	    //printf("load so error");
	     printf("dlopen err:%s.\n",dlerror());
	    return ;
	}
	printf("delete ing\n");
	destroy_t destroy_unit = (destroy_t) dlsym(handle, "destroy");
	const char* dlsym_error = dlerror();
	if (dlsym_error) {
	    printf("Cannot load symbol destroy:%s\n",dlsym_error);
	    return;
	}
	printf("delete UnitBase\n");

	try
	{
	    destroy_unit(unitToBeDestory);
	}
	catch (exception& e)
	{
	       cout << "delete exception: " << e.what() << endl;
	}
	if(handle)
	     dlclose(handle);
}
bool Engine::loadUnit()
{
	YAML::Node doc = YAML::LoadFile(configPath);
	int index=0;
	string soPath;
    string portName;
    string portType;
    string portTag;
    int portIndex;
    YAML::Node runningNode = doc["running"];
    startUnitIndex =  runningNode["startUnitIndex"].as<int>();
    startFuncIndex =  runningNode["startFuncIndex"].as<int>();
    netUnitIndex =  runningNode["netUnitIndex"].as<int>();

    YAML::Node netNode = doc["netconfig"];
    string localIP = netNode["localIP"].as<string>();
    string localPort = netNode["localPort"].as<string>();
    string masterIP = netNode["masterIP"].as<string>();
    string masterPort =netNode["masterPort"].as<string>();
    string isOpen = netNode["open"].as<string>();

	YAML::Node soDoc = doc["modules"];
	for(YAML::const_iterator it=soDoc.begin();it!=soDoc.end();++it){
		index = it->first.as<int>();
        soPath = it->second["soPath"].as<string>();
        YAML::Node inputPortNode = it->second["inputPort"];
        vector<UnitPort> inputPorts;
        for(YAML::const_iterator it= inputPortNode.begin();it!= inputPortNode.end();++it){
            cout<<it->first.as<int>()<<endl;

            portName=it->second["name"].as<string>();
            cout<<"2"<<endl;
            portType = it->second["type"].as<string>();
            portTag = it->second["tag"].as<string>();
            portIndex = it->second["index"].as<int>();
             UnitPort unitPort(portName,portType,portIndex,portTag);
            inputPorts.push_back(unitPort);
            cout<<"input tag   "<<portTag <<endl;
        }

        YAML::Node outputPortNode = it->second["outputPort"];
       vector<UnitPort> outputPorts;
        for(YAML::const_iterator it= outputPortNode.begin();it!= outputPortNode.end();++it){
            portName=it->second["name"].as<string>();
            portType = it->second["type"].as<string>();
            portTag = it->second["tag"].as<string>();
            portIndex = it->second["index"].as<int>();
            UnitPort unitPort(portName,portType,portIndex,portTag);
            outputPorts.push_back(unitPort);
            cout<<"output tag   "<<portTag <<endl;
        }
		cout<<index<<"    "<<soPath<<"     "<<endl;
		const char *p=soPath.c_str();
		UnitBase *temp = createUnit(p);

		if(temp)
		{
             temp->initData(&workManager);
             temp->initAttribute(inputPorts,outputPorts);
             if(isOpen=="true"&&index ==1)
             {
                 NetUnit *netUnit = (NetUnit*)temp;
                 netUnit->initAddreess(localIP,localPort,masterIP,masterPort);
                 cout<<localIP<<"   "<<localPort<<endl;
                 netOpen = true;
             }
             unitMap[index]=temp;
		}
		else
		{
		 printf("NULL object\n");
		 return false;
		}
	 }
	YAML::Node relativeDoc = doc["relative"];
	for(YAML::const_iterator it=relativeDoc.begin();it!=relativeDoc.end();++it){
		int fromUnitIndex=it->second["fromUnitIndex"].as<int>();
		int fromPort =it->second["fromPort"].as<int>();
		int toUnitIndex =it->second["toUnitIndex"].as<int>();
		int toPort=it->second["toPort"].as<int>();
		UnitBase * fromUnit  =unitMap[fromUnitIndex];
		UnitBase * toUnit = unitMap[toUnitIndex];
		fromUnit->addLinkUnit(fromPort,toPort, toUnit);
	}

	return true;
}
void Engine::deleteUnit()
{
    try
    {
        YAML::Node doc = YAML::LoadFile(configPath);
          int index=0;
          string soPath;
          YAML::Node soDoc = doc["modules"];
          for(YAML::const_iterator it=soDoc.begin();it!=soDoc.end();++it){
                   index = it->first.as<int>();
                  soPath = it->second.as<string>();
                  cout<<index<<"    "<<soPath<<"     "<<endl;
                  const char *p=soPath.c_str();
                  UnitBase *temp = unitMap[index];
                  destroyUnit(p,temp);
                  cout<<"delete  "<<index<<endl;
                  unitMap[index]=NULL;
              }
    }
      catch (exception& e)
      {
             cout << "Standard exception: " << e.what() << endl;
      }
}

void* startNetThread(void* arg)
{
   NetUnit* netUnit = (NetUnit*)arg;
   netUnit->run();
   return 0;
}

void * startHearbeatThread(void* arg)
{
    NetUnit* netUnit = (NetUnit*)arg;
    netUnit->sendHearbeatInfo();
    return 0;
}

void Engine::runNetServer(int index)
{
    int ret = pthread_create(&netServerTid, NULL, startNetThread, (void*)unitMap[index]);
    if (ret == -1) {
        cerr << "Failed to initialize netServer!" << endl;
        return ;
    }
    ret = pthread_create(&hearBeatTid, NULL, startNetThread, (void*)unitMap[index]);
    if (ret == -1) {
        cerr << "Failed to initialize hearbeat thread!" << endl;
        return ;
    }
    NetUnit* netUnit = (NetUnit*) unitMap[index];
    netUnit->registerInfo();

}
int Engine::run()
{
	if(!loadUnit())
	{
		cout<<"load unit error"<<endl;
		return 0;
	}
    if(netOpen)
    {
        cout<<"runNetServer-----------------"<<endl;
        runNetServer(netUnitIndex);
        startUnitIndex++;
    }
	int ret = workManager.init();
	if (ret == -1) {
	    cerr << "Failed to initialize thread pool!" << endl;
	    return 0;
	}
	Task *tempTask=new Task(unitMap[startUnitIndex]->getFunc(startFuncIndex),unitMap[startUnitIndex]);
    workManager.addTask(tempTask);
}
Engine::~Engine()
{
    NetUnit* netUnit = (NetUnit*) unitMap[netUnitIndex];
    netUnit->stop();
    void* result;
    int ret = pthread_join(netServerTid, &result);
    cout << "pthread_join() returned " << ret << ": " << strerror(errno) << endl;
    ret = pthread_join(hearBeatTid, &result);
    cout << "pthread_join() returned " << ret << ": " << strerror(errno) << endl;
	deleteUnit();
}
