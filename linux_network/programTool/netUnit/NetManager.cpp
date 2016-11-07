#include "NetManager.h"
#include "NetServer.h"
class NetManagerImpl
{
public:
	NetManagerImpl(NetUnit *netUnit);
	void putToBuffer(string portName,string data);
	void publishMsg(string data);
    void registerInfo(string ip,string port);
    void sendHearbeatInfo();
    void run();
    void stop();
	~NetManagerImpl();
private: 
    NetServer *netServer;
};
NetManagerImpl::NetManagerImpl(NetUnit *netUnit)
{
    netServer = new NetServer(netUnit);
}
void NetManagerImpl::putToBuffer(string portName,string data)
{
    netServer->putToBuffer(portName,data);
}
void NetManagerImpl::publishMsg(string data)
{
    netServer->publishMsg(data);
}

void NetManagerImpl::registerInfo(string ip,string port)
{
    netServer->registerInfo(ip,port);
}

void NetManagerImpl::sendHearbeatInfo()
{
    netServer->sendHeartbeatInfo();
}

void NetManagerImpl::run()
{
    netServer->run();
}
void NetManagerImpl::stop()
{
     netServer->stop();
}


NetManagerImpl::~NetManagerImpl()
{
    delete netServer;
}

NetManager::NetManager(NetUnit *netUnit)
{
    netManagerImpl = new NetManagerImpl(netUnit);
}

NetManager::~NetManager()
{
	delete  netManagerImpl ;
}

void NetManager::putToBuffer(string portName,string data)
{
	netManagerImpl->putToBuffer(portName,data);
}

void NetManager::publishMsg(string data)
{
	netManagerImpl->publishMsg(data);
}

void NetManager::registerInfo(string ip,string port)
{
    netManagerImpl->registerInfo(ip,port);
}
 void NetManager::sendHearbeatInfo()
 {
     netManagerImpl->sendHearbeatInfo();
 }

void NetManager::run()
{
    netManagerImpl->run();
}
void NetManager::stop()
{
    netManagerImpl->stop();
}



