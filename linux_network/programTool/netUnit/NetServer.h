#ifndef _NETSERVER_H
#define _NETSERVER_H
#include "NetUnit.h"

class NetServer
{
public:
	NetServer(NetUnit *unit);
	~NetServer();
    void initData(string serverIP ,string serverPort,string masterIP,string masterPort);
	void record(const vector<string>& topicNames,  Address address, int type);
	void removeRecord(const vector<string>& topicNames,  Address address, int type);
    void registerInfo(string ip,string port);
    void sendHeartbeatInfo();
    string buildCmd(string action);
    void handleRequest(int fd);
    void putToBuffer(string portName,string data);
	string subscribeTopic(Address desAddress, string currentIP,string currentPort, string topicName);
	void run();
    void stop();
    void publishMsg(string dataToPublish);
private:
	map<string, list<Address>*> subscriber;
	pthread_mutex_t scbMap_pthreadMutex;
    string serverIP;
    string serverPort;
    string masterIP;
    string masterPort;
    int listenfd;
    bool runningState;
	NetUnit *netUnit;
};
#endif
