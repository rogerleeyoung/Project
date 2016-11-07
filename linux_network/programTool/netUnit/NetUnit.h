#ifndef _NETUNIT_H
#define _NETUNIT_H
#include "../unit/UnitBase.h"
#include "NetManager.h"
#include<map>
#include<list>
#include "Address.h"
class NetManager;
class NetUnit:public UnitBase
{
public:
    NetUnit();
    void initAddreess(string localIP,string localPort,string masterIP,string masterPort);
    string getServerIp();
    string getServerPort();
    void putOutputBuffer(string portName,string data);
    void handleNetData(int port,void *data);
    void publishMsg(string data);
    void registerInfo();
    void sendHearbeatInfo();
    void* run();
    void stop();
    ~NetUnit();
    void onInput_start();
    void onStoped();
private:
    string localIP;
    string localPort;
    string masterIP;
    string masterPort;
    NetManager *netManager;

};

extern "C" {
    UnitBase* create();
    void destroy(UnitBase* u);
} 

#endif
