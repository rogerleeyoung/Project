#ifndef _NETMANAGER_H
#define _NETMANAGER_H
#include "NetUnit.h"
class NetUnit;
class NetManagerImpl;
class NetManager
{
public:
    NetManager(NetUnit *netUnit);
    void putToBuffer(string portName, string data);
	void publishMsg(string data);
    void registerInfo(string ip,string port);
    void sendHearbeatInfo();
    void run();
    void stop();
    ~NetManager();
private:
	NetManagerImpl * netManagerImpl;
	
};
#endif
