#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../util/Util.h"
#include "NetServer.h"
using namespace std;

NetServer::NetServer(NetUnit *unit):netUnit(unit),runningState(true)
{
    this->serverIP = "127.0.0.1";
    this->serverPort = "8017";
    this->masterIP = "127.0.0.1";
    this->masterPort = "8010";
}

NetServer::~NetServer()
{
    close(listenfd);
}



void NetServer::initData(string serverIP ,string serverPort,string masterIP,string masterPort)
{
    this->serverIP = serverIP;
    this->serverPort = serverPort;
    this->masterIP = masterIP;
    this->masterPort = masterPort;
    pthread_mutex_init(&scbMap_pthreadMutex,NULL);
}

string NetServer::subscribeTopic(Address desAddress, string currentIP,string currentPort, string topicName)
{
    //subscribe ip port name1,name2,name3
    string tempStr = "subscribe ";
    tempStr += currentIP+" "+currentPort+" "+topicName;
    string recvData  = sendData(desAddress.ip, desAddress.port,tempStr);
    return recvData;
}

void NetServer::record(const vector<string>& topicNames, Address address, int type)
{

    map<string , list<Address>*>  *topicMap = NULL;
    if(type ==1)
        topicMap = &subscriber;
    pthread_mutex_lock(&scbMap_pthreadMutex);
    for(int i=0;i<topicNames.size();i++)
    {
      list<Address>* addressList= NULL;
      if(topicMap->count(topicNames[i])<1)
      {
          addressList =new list<Address>();
          (*topicMap)[topicNames[i]] = addressList;
      }
      else
      {
          addressList = (*topicMap)[topicNames[i]] ;
      }
      list<Address>::iterator listIter = addressList->begin();
      bool isRecroded =false;
      while(listIter !=addressList->end())
      {
          if((*listIter).ip==address.ip&&(*listIter).port == address.port)
          {
              isRecroded = true;
          }
          listIter++;
      }
      if(!isRecroded)
        addressList->push_back(address);

    }
     pthread_mutex_unlock(&scbMap_pthreadMutex);
}


void NetServer::removeRecord(const vector<string>& topicNames,Address address,  int type)
{
    map<string , list<Address>*>  *topicMap = NULL;
    if(type ==1)
        topicMap = &subscriber;
    pthread_mutex_lock(&scbMap_pthreadMutex);
    for(int i=0;i<topicNames.size();i++)
    {
      list<Address>* addressList= NULL;
      if(topicMap->count(topicNames[i])>0)
      {
          addressList = (*topicMap)[topicNames[i]] ;
          list<Address>::iterator iter = addressList->begin();
          while(iter!=addressList->end())
          {
              if(iter->ip == address.ip&&iter->port == address.port)
              {
                  iter =addressList->erase(iter) ;
              }
              else
              {
                  iter++;
              }
          }

      }

    }
     pthread_mutex_unlock(&scbMap_pthreadMutex);
}

string NetServer::buildCmd(string action)
{
    map<int, UnitPort> inputPorts = netUnit->getInputPorts();
    map<int, UnitPort> outputPorts = netUnit->getOutPorts();
    string cmd = action+" ";
    cmd+= serverIP+" "+serverPort+" ";
    cmd+="publish:";
    map<int, UnitPort>::iterator inputPortIter = inputPorts.begin();
    while(inputPortIter!=inputPorts.end())
    {
         cmd+= inputPortIter->second.portTag;
         inputPortIter++;
         if(inputPortIter!=inputPorts.end())
         {
             cmd+= ",";
         }
    }
    cmd+=" subscribe:";
    map<int, UnitPort>::iterator outputPortIter = outputPorts.begin();
    while(outputPortIter!=outputPorts.end())
    {
         cmd+= outputPortIter->second.portTag;
         outputPortIter++;
         if(outputPortIter!=outputPorts.end())
         {
             cmd+= ",";
         }
    }
    return cmd;
}

void NetServer::registerInfo(string ip,string port)
{
     initData(netUnit->getServerIp(),netUnit->getServerPort(),ip,port);
    cout<<"-----------------registerInfo-----------------"<<endl;
    cout<<buildCmd("register")<<endl;
    string data="register 127.0.0.1 8017 publish:r,t subscribe:a,b";
   // string responseData= sendData( ip, port , buildCmd("register"));
    string responseData= sendData( "127.0.0.1", "8010" , data);
      cout<<responseData <<endl;
      vector<string> tempVector=split(responseData ,"\n");
      if(tempVector.size()<2)
          return;
      vector<string> publisherInfos=split(tempVector[1],";");
      //name ip,port;name ip,port;
      cout<<publisherInfos.size()<<endl;
      for(int i=0;i<publisherInfos.size();i++)
      {
           cout<<publisherInfos[i]<<endl;
           vector<string>  publisherInfo = split(publisherInfos[i]," ");
           if(publisherInfo.size()!=2)
               break;
           vector<string>  address = split(publisherInfo[1],",");
           string recvData  = subscribeTopic(Address(address[0],address[1]),serverIP,serverPort ,publisherInfo[0]);
           cout<<publisherInfos[i]<<"    response   "<<recvData<<endl;
      }
    // Task *new_task=new Task();
    // //添加新的读任务
    // new_task->data="register 127.0.0.1 8015 publish:a,b,c subscribe:g,l";
    // new_task->action = "register";
    // new_task->address.ip= "127.0.0.1";
    // new_task->address.port ="8010";
    // new_task->owner = this;
    // threadPoll ->addTask(new_task,2);
}

void NetServer::sendHeartbeatInfo()
{
    while(runningState)
    {
         sendData( masterIP, masterPort, buildCmd("heartbeat"));
         sleep(10);
    }
}

void NetServer::putToBuffer(string portName,string data)
{
    map<int, UnitPort> outputPorts = netUnit->getOutPorts();
    map<int, UnitPort>::iterator iter = outputPorts.begin();
    while(iter!=outputPorts.end())
    {
        if(iter->second.portName ==portName)
        {
            netUnit->handleNetData(iter->second.portIndex,new int(1));
        }
    }
}



void NetServer::handleRequest(int fd)
{
         //从任务队列取出一个读任务
         char recvBuf[1024] = {0};
         int ret = 999;
         int rs = 1;
         while(rs)
         {
             ret = recv(fd,recvBuf,1024,0);// 接受客户端消息
             if(ret < 0)
             {
                 //由于是非阻塞的模式,所以当errno为EAGAIN时,表示当前缓冲区已无数据可//读在这里就当作是该次事件已处理过。
                 if(errno == EAGAIN)
                 {
                     printf("EAGAIN\n");
                     break;
                 }
                 else{
                     printf("recv error!\n");

                     close(fd);
                     break;
                 }
             }
             else if(ret == 0)
             {
                 // 这里表示对端的socket已正常关闭.
                 rs = 0;
             }
             if(ret == sizeof(recvBuf))
                 rs = 1; // 需要再次读取
             else
                 rs = 0;
         }
         if(ret>0){
               //------------------------------------------------------------------------------
              string recvData(recvBuf);
               //"subscribe ip port name1,name2,name3";
              cout<<recvData<<endl;
              vector<string> resultData=split(recvData," ");
              if(resultData[0] =="subscribe")
              {
                  string clientIp = resultData[1];
                  string clientPort = resultData[2];
                  Address address(clientIp, clientPort);
                  // address.ip = clientIp;
                 //  address.port = clientPort;
                  vector<string> topics = split(resultData[3],",");
                  record( topics , address,1);
                   //string resStr = makeResposeStr(subscribe,publisher)+"\n";
                  string resStr ="subscribe_ok";
                  char buf[1000] = {0};
                  strcpy(buf,resStr.c_str());
                  send(fd,buf,strlen(buf)+1,0);
                 // close(fd);
              }
              else if(resultData[0] =="newTopic")
              {
                    subscribeTopic(Address(resultData[2],resultData[3]),serverIP,serverPort,resultData[1]);
                    char buf[1000] = {0};
                    strcpy(buf,"newTopic_ok");
                    send(fd,buf,strlen(buf)+1,0);
              }
              else if(resultData[0] =="unsubscribe")
              {
                    string clientIp = resultData[1];
                    string clientPort = resultData[2];
                    Address address(clientIp, clientPort);
                    vector<string> topics = split(resultData[3],",");
                    removeRecord(topics,address,  1);
                    char buf[1000] = {0};
                    strcpy(buf,"unsubscribe");
                    send(fd,buf,strlen(buf)+1,0);
              }
              else if(resultData[0] =="publishMsg")
              {
                  if(resultData.size()<3)
                  {
                      cout<<"receive invalid data"<<endl;
                  }
                  putToBuffer(resultData[1],resultData[2]);
              }
              else if(resultData[0] =="heartbeat")
              {

              }
              else
              {
                    char buf[1000] = {0};
                    strcpy(buf,"other_ok");
                    send(fd,buf,strlen(buf)+1,0);
              }
        }
}


void NetServer::publishMsg(string dataToPublish)
{
    pthread_mutex_lock(&scbMap_pthreadMutex);
    map<string, list<Address>*>::iterator iter = subscriber.begin();
    while(iter !=subscriber.end() )
    {
        string name = iter->first;
        list<Address>* addressList = iter->second;
        list<Address>::iterator listIter = addressList->begin();
        while(listIter!=addressList->end())
        {
            string  data = "publishMsg ";
            data+= name+" ";
            data+=dataToPublish;
            cout<<serverIP<<" "<<serverPort<<" "<<data<<" to "<<listIter->ip<<" "<<listIter->port<<endl;
            string result = sendData( listIter->ip,listIter->port ,data);
            listIter++;
        }
        iter++;
    }
     pthread_mutex_unlock(&scbMap_pthreadMutex);

}


void NetServer::run()
{
    cout<<"epoll start"<<endl;
    struct epoll_event ev,events[20];
    int epfd;
    int i, maxi, connfd, sockfd,nfds;
    socklen_t clilen;
    //生成用于处理accept的epoll专用的文件描述符
    epfd=epoll_create(256);
    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    //把socket设置为非阻塞方式

    setnonblocking(listenfd);
    //设置与要处理的事件相关的文件描述符

    ev.data.fd=listenfd;
    //设置要处理的事件类型

    ev.events=EPOLLIN|EPOLLET;
    //注册epoll事件
    epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);

    memset(&serveraddr,0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port=htons(atoi(serverPort.c_str()));
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    bind(listenfd,(sockaddr *)&serveraddr, sizeof(serveraddr));
    listen(listenfd, 5);
    maxi = 0;

    while (runningState) {
         //等待epoll事件的发生
         nfds=epoll_wait(epfd,events,20,500);
         //处理所发生的所有事件

       for(i=0;i<nfds;++i)
       {
              if(events[i].data.fd==listenfd)
              {
                   connfd = accept(listenfd,(sockaddr *)&clientaddr, &clilen);
                   if(connfd<0){
                     perror("connfd<0");
                     exit(1);
                  }
                   setnonblocking(connfd);

                   char *str = inet_ntoa(clientaddr.sin_addr);
                   //std::cout<<"connec_ from >>"<
                   //设置用于读操作的文件描述符

                   ev.data.fd=connfd;
                   //设置用于注测的读操作事件

                ev.events=EPOLLIN|EPOLLET;
                   //注册ev

                epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
              }
           else if(events[i].events&EPOLLIN)
           {
                   if ( (sockfd = events[i].data.fd) < 0) 
                        continue;
                    handleRequest(sockfd);

             }
              else if(events[i].events&EPOLLOUT)
              {

              }

         }

    }
}
void NetServer::stop()
{
        runningState = false;
}






