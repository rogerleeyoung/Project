#include "NetUnit.h"
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../util/Util.h"
using namespace std;


UnitBase* create() {
    NetUnit *instance=new NetUnit();
    return (UnitBase*)(instance);
}
void destroy(UnitBase* unit) {
    if(unit)
        delete unit;
}

static void onInput_start_impl(void *unit)
{
    NetUnit* instance=(NetUnit*)unit;
    instance->onInput_start();
}

void NetUnit::putOutputBuffer(string portName,string data)
{
      netManager->putToBuffer(portName,data);
}
void NetUnit::publishMsg(string data)
{
      netManager->publishMsg(data);
}

void NetUnit::handleNetData(int port,void *data)
{
    putOutput(port,data);
}

void NetUnit::registerInfo()
{
    netManager->registerInfo(masterIP,masterPort);
}
void NetUnit::sendHearbeatInfo()
{
    netManager->sendHearbeatInfo();
}

void* NetUnit::run()
{
    netManager->run();
    return 0;
}

void NetUnit::stop()
{
    netManager->stop();
}

string NetUnit::getServerIp()
{
    return localIP;
}

string NetUnit::getServerPort()
{
    return  localPort;
}

NetUnit::NetUnit()
{
    netManager = new NetManager(this);
    addFunc(onInput_start_impl);
}
void NetUnit::initAddreess(string localIP,string localPort,string masterIP,string masterPort)
{
    this->localIP = localIP;
    this->localPort = localPort;
    this->masterIP =  masterIP;
    this->masterPort = masterPort;
}

NetUnit::~NetUnit()
{
    delete netManager;
}




void NetUnit::onInput_start()
{
    cout<<"hello"<<endl;
    publishMsg("hello world");
}






