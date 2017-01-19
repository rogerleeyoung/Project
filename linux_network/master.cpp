#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <cstdlib>
#include <errno.h>
#include <string.h>
#include <string>
#include <vector>
#include <sstream>
#define MAXLINE 50
#define OPEN_MAX 100
#define LISTENQ 20
#define SERV_PORT 8015
#define INFTIM 1000
#include <list>
#include <map>

using namespace std;

string sendData( string host_name,string portStr,string str)
{
    char buf[8192];
    int port = atoi(portStr.c_str());
    //char message[256];
    int socket_descriptor;
    struct sockaddr_in pin;
    bzero(&pin,sizeof(pin));
    pin.sin_family = AF_INET;
    inet_pton(AF_INET,host_name.c_str(),&pin.sin_addr);
    pin.sin_port = htons(port);
    if((socket_descriptor =  socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
            perror("error opening socket \n");
            exit(1);
    }
    if(connect(socket_descriptor,(struct sockaddr * )&pin,sizeof(pin)) == -1)
    {
       perror("error connecting to socket \n");
       exit(1);
     }

    printf("sending message %s to server ..\n",str.c_str());
    if( write(socket_descriptor,(const void *)str.c_str(),str.length()+1) == -1 )
    {
        printf("error ..in send\n");
      //  perror("error in send \n");
        string tempStr;
        return tempStr;
    }

    printf("..sent message ...wait for message..\n");
    if( read(socket_descriptor,buf,8192) == -1 )
    {
        printf("error ..in send\n");
       // perror("error in receiving response from server \n");
        string tempStr;
        return tempStr;
    }

    string recvData = string(buf);
    close(socket_descriptor);
    return recvData ;
}





vector<std::string> split(const  string& s, const string& delim)
{
    std::vector<std::string> elems;
    size_t pos = 0;
    size_t len = s.length();
    size_t delim_len = delim.length();
    if (delim_len == 0) return elems;
    while (pos < len)
    {
        int find_pos = s.find(delim, pos);
        if (find_pos < 0)
        {
            elems.push_back(s.substr(pos, len - pos));
            break;
        }
        elems.push_back(s.substr(pos, find_pos - pos));
        pos = find_pos + delim_len;
    }
    return elems;
}


void setnonblocking(int sock)
{
     int opts;
     opts=fcntl(sock,F_GETFL);
     if(opts<0)
     {
          perror("fcntl(sock,GETFL)");
          exit(1);
     }
    opts = opts|O_NONBLOCK;
     if(fcntl(sock,F_SETFL,opts)<0)
     {
          perror("fcntl(sock,SETFL,opts)");
          exit(1);
     }
}



struct Address
{
    string ip;
    string port;
    Address()
    {
    }
    Address(string ip,string port)
    {
        this->ip=ip;
        this->port=port;
    }
};

struct NodeInfo
{
    Address address;
    int heartbeatCount;
};
//线程池任务队列结构体
class M_threadPool;
class NetServer
{
    private:
        map<string, list<NodeInfo>*> subscriber;
        map<string , list<NodeInfo>*> publisher;
        pthread_mutex_t scbMap_pthreadMutex;
        M_threadPool *threadPoll;
        int serverPort;
        bool runningState;
        int listenfd;

    public:
        NetServer(int serverPort=8010);
        ~NetServer();
        bool getRunningState();
        void setThreadPoll(M_threadPool *threadPoll);
        void record(const vector<string>& topicNames,  Address address, int type);
        void updateRecord(const vector<string>& topicNames,  Address address, int type);
        void removeRecord(const vector<string>& topicNames,  Address address, int type);
        void clearOfflineNode(int type);
        string makeResposeStr(const vector<string>& topicNames, int type);
        void registerInfo();
        void run();
        void noticeSubscriber(const vector<string>& topicNames,Address address);
        void noticePublisher(const vector<string>& topicNames,Address address);
};

struct Task{
  int fd; //需要读写的文件描述符
  Address address;
  string data;
  NetServer *owner;
};





class M_threadPool
{
private:
    static vector<Task*> readTaskList;     /** 任务列表 */
    static vector<Task*> writeTaskList;
    static bool shutdown;                    /** 线程退出标志 */
    int  readThreadNum;                     /** 线程池中启动的线程数 */
    int  writeThreadNum;
    pthread_t   *pthread_id;
    int runingThreadNum;
    static pthread_mutex_t read_pthreadMutex;    /** 线程同步锁 */
    static pthread_cond_t read_pthreadCond;      /** 线程同步的条件变量 */
    static pthread_mutex_t write_pthreadMutex;
    static pthread_cond_t write_pthreadCond;

    static void* writeTask(void * threadData); /** 新线程的线程回调函数 */
    static void* readTask(void * threadData);
    static int MoveToIdle(pthread_t tid);       /** 线程执行结束后，把自己放入到空闲线程中 */
    static int MoveToBusy(pthread_t tid);       /** 移入到忙碌线程中去 */
    int create(int readThreadNum,int writeThreadNum)           /** 创建线程池中的线程 */
    {
        printf("create thread!\n");
        int threadNum = readThreadNum+writeThreadNum;
        pthread_id = (pthread_t*)malloc(sizeof(pthread_t) * threadNum);
        for(int i = 0; i < readThreadNum; i++)
        {
               pthread_create(&pthread_id[i], NULL, readTask, NULL);

        }
        for(int i = readThreadNum; i < readThreadNum+writeThreadNum; i++)
        {
               pthread_create(&pthread_id[i], NULL, writeTask, NULL);
        }
        return 0;
    }

public:
    M_threadPool(int readThreadNum = 2,int writeThreadNum =2)
    {
        create(readThreadNum,writeThreadNum);
//        pthread_mutex_init(&read_pthreadMutex,NULL);
//        pthread_cond_init(&read_pthreadCond,NULL);
//        pthread_mutex_init(&write_pthreadMutex,NULL);
//        pthread_cond_init(&write_pthreadCond,NULL);
    }

    int getRuningThreadNum();
    int addTask(Task *task, int flag)     /** 把任务添加到任务队列中 */
    {
        if(flag == 1)
        {
            pthread_mutex_lock(&read_pthreadMutex);
            this->readTaskList.push_back(task);
            pthread_mutex_unlock(&read_pthreadMutex);
            pthread_cond_signal(&read_pthreadCond);
        }
        else
        {
            pthread_mutex_lock(&write_pthreadMutex);
            this->writeTaskList.push_back(task);
            pthread_mutex_unlock(&write_pthreadMutex);
            pthread_cond_signal(&write_pthreadCond);
        }

    }

    int StopAll();                 /** 使线程池中的线程退出 */
    int getTaskSize();             /** 获取当前任务队列中的任务数 */

};

pthread_mutex_t M_threadPool::read_pthreadMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t M_threadPool::read_pthreadCond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t M_threadPool::write_pthreadMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t M_threadPool::write_pthreadCond = PTHREAD_COND_INITIALIZER;
vector<Task*>  M_threadPool::readTaskList;         /** 任务列表 */
vector<Task*> M_threadPool::writeTaskList;
bool M_threadPool::shutdown = false;                    /** 线程退出标志 */

void* M_threadPool::writeTask(void * threadData)
{
    while(1){
        pthread_mutex_lock(&write_pthreadMutex);
        while (writeTaskList.size() == 0 && !shutdown)
        {
            pthread_cond_wait(&write_pthreadCond, &write_pthreadMutex);
        }

        if (shutdown)
        {
            pthread_mutex_unlock(&write_pthreadMutex);
          //  printf("thread %lu will exit/n", pthread_self());
            pthread_exit(NULL);
        }

        vector<Task*>::iterator iter = writeTaskList.begin();

        /**
        * 取出一个任务并处理之
        */
        Task* task = *iter;
        if (iter != writeTaskList.end())
        {
            task = *iter;
            writeTaskList.erase(iter);
        }
        pthread_mutex_unlock(&write_pthreadMutex);

         string responseData = sendData( task->address.ip,task->address.port ,task->data);
         cout<<responseData <<endl;
         cout<<"publish success"<<endl;
         delete task;
    }
}

void* M_threadPool::readTask(void * threadData)
{
    int fd=-1;
    unsigned int n;
    //用于把读出来的数据传递出去
    bool isFirst=true;
    while (1)
    {
        pthread_mutex_lock(&read_pthreadMutex);
        while (readTaskList.size() == 0 && !shutdown)
        {
            pthread_cond_wait(&read_pthreadCond, &read_pthreadMutex);
        }

        if (shutdown)
        {
            pthread_mutex_unlock(&read_pthreadMutex);
          //  printf("thread %lu will exit/n", pthread_self());
            pthread_exit(NULL);
        }
        vector<Task*>::iterator iter = readTaskList.begin();

        /**
        * 取出一个任务并处理之
        */
        Task* task = *iter;
        if (iter != readTaskList.end())
        {
            task = *iter;
            readTaskList.erase(iter);
        }
        pthread_mutex_unlock(&read_pthreadMutex);
         fd= task->fd;
         NetServer *netServer = task->owner;
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
         delete task;
         if(ret>0){

        string recvData(recvBuf);
         //"register 192.168.0.1 8080 publish:a,b,c subscribe:d,e,f";
        cout<<"receive from client "<<recvData<<endl;
        vector<string> responseData=split(recvData," ");
        string responseStr = "ok\n";
        cout<<"clientData size "<<responseData.size()<<endl;
        if(responseData.size()==5)
        {
                 string clientIp = responseData[1];
                 string clientPort = responseData[2];
                  Address address(clientIp, clientPort);
                 vector<string> temp1 = split(responseData[3],":");
                 vector<string> temp2 = split(responseData[4],":");
                 cout<<"record publish"<<endl;
                 if(temp1.size()==2)
                 {
                      vector<string> publish =  split(temp1[1],",");

                      if(responseData[0]=="register")
                      {
                          
                          netServer->record( publish, address, 2);
                          netServer->noticeSubscriber(publish,address);
                      }
                      else if(responseData[0]=="unregister")
                      {
                          netServer->removeRecord(publish, address, 2);
                      }
                      else if(responseData[0]=="heartbeat")
                      {
                            netServer->updateRecord(publish, address, 2);
                      }

                 }
                 cout<<"record subsceibe"<<endl;
                 if (temp2.size()==2)
                 {
                     vector<string> subscribe =  split(temp2[1],",");
                     if(responseData[0]=="register")
                     {
                         netServer->record(subscribe, address, 1);
                         responseStr += netServer->makeResposeStr(subscribe,2)+"\n";
                     }
                     else if(responseData[0]=="unregister")
                     {
                          netServer->removeRecord(subscribe, address, 1);
                          netServer->noticePublisher(subscribe,address);
                     }
                     else if(responseData[0]=="heartbeat")
                     {
                         netServer->updateRecord(subscribe, address, 1);
                     }
                 }
         }
         else
         {
            responseStr +="bad request";
         }

        char buf[1000] = {0};
        cout<<responseStr<<endl;
        strcpy(buf,responseStr.c_str());
        send(fd,buf,strlen(buf)+1,0);


        }
    }
}




NetServer::NetServer(int serverPort)
{
    runningState = true;
     this->serverPort = serverPort;
     pthread_mutex_init(&scbMap_pthreadMutex,NULL);
}
void NetServer::setThreadPoll(M_threadPool *threadPoll)
{
    this->threadPoll = threadPoll;
}
bool NetServer::getRunningState()
{
    return runningState;
}

void NetServer::noticeSubscriber(const vector<string>& topicNames,Address address)
{
    for(int i=0;i<topicNames.size();i++)
    {
         if(subscriber.count(topicNames[i])<1)
            break;
        list<NodeInfo>* topicList = subscriber[topicNames[i]];
        list<NodeInfo>::iterator listIter = topicList->begin();
        //newTopic topicName ip port
        while(listIter !=topicList->end())
        {
            //添加新的读任务
            Task *new_task=new Task();
            new_task->data = "newTopic ";
            new_task->data +=topicNames[i]+" "+address.ip+" "+address.port;
            new_task->address = listIter->address;
            new_task->owner = this;
            threadPoll->addTask(new_task,2);
            listIter ++;
        }

    }
}
void NetServer::noticePublisher(const vector<string>& topicNames,Address address)
{
    for(int i=0;i<topicNames.size();i++)
    {
        if(publisher.count(topicNames[i])<1)
            break;
        list<NodeInfo>* topicList = publisher[topicNames[i]];
        list<NodeInfo>::iterator listIter = topicList->begin();
        //newTopic topicName ip port
        while(listIter !=topicList->end())
        {
            //添加新的读任务
            Task *new_task=new Task();
            new_task->data = "unsubscribe "; 
            new_task->data +=topicNames[i]+" "+address.ip+" "+address.port;
            new_task->address = listIter->address;
            new_task->owner = this;
            threadPoll->addTask(new_task,2);
            listIter ++;
        }

    }
}

void NetServer::record(const vector<string>& topicNames, Address address, int type)
{
    map<string , list<NodeInfo>*>  *topicMap = NULL;
    if(type ==1)
        topicMap = &subscriber;
    else
        topicMap = &publisher;
    pthread_mutex_lock(&scbMap_pthreadMutex);
    for(int i=0;i<topicNames.size();i++)
    {
          list<NodeInfo>* nodeInfoList= NULL;
          if(topicMap->count(topicNames[i])<1)
          {
              nodeInfoList =new list<NodeInfo>();
              (*topicMap)[topicNames[i]] = nodeInfoList;
          }
          else
          {
              nodeInfoList = (*topicMap)[topicNames[i]] ;
          }

          list<NodeInfo>::iterator listIter = nodeInfoList->begin();
          bool isRecroded =false;
          while(listIter !=nodeInfoList->end())
          {
              if((*listIter).address.ip==address.ip&&(*listIter).address.port == address.port)
              {
                  isRecroded = true;
              }
              listIter++;
          }
          if(!isRecroded)
          {
              NodeInfo nodeInfo;
              nodeInfo.heartbeatCount = 0;
              nodeInfo.address = address;
             nodeInfoList->push_back(nodeInfo);
          }
    }

     pthread_mutex_unlock(&scbMap_pthreadMutex);
}
void NetServer::removeRecord(const vector<string>& topicNames,Address address,  int type)
{
    map<string , list<NodeInfo>*>  *topicMap = NULL;
    if(type ==1)
        topicMap = &subscriber;
    else
        topicMap = &publisher;
    pthread_mutex_lock(&scbMap_pthreadMutex);
    for(int i=0;i<topicNames.size();i++)
    {
      list<NodeInfo>* nodeInfoList= NULL;
      if(topicMap->count(topicNames[i])>0)
      {
          nodeInfoList = (*topicMap)[topicNames[i]] ;
          list<NodeInfo>::iterator iter = nodeInfoList->begin();
          while(iter!=nodeInfoList->end())
          {
              if(iter->address.ip == address.ip&&iter->address.port == address.port)
              {
                  iter =nodeInfoList->erase(iter) ;
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

void NetServer::updateRecord(const vector<string>& topicNames,  Address address, int type)
{
    map<string , list<NodeInfo>*>  *topicMap = NULL;
    if(type ==1)
        topicMap = &subscriber;
    else
        topicMap = &publisher;
    pthread_mutex_lock(&scbMap_pthreadMutex);
    for(int i=0;i<topicNames.size();i++)
    {
      list<NodeInfo>* nodeInfoList= NULL;
      if(topicMap->count(topicNames[i])>0)
      {
          nodeInfoList = (*topicMap)[topicNames[i]] ;
          list<NodeInfo>::iterator iter = nodeInfoList->begin();
          while(iter!=nodeInfoList->end())
          {
              if(iter->address.ip == address.ip&&iter->address.port == address.port)
              {
                  iter->heartbeatCount =0;
              }

               iter++;

          }

      }

    }
     pthread_mutex_unlock(&scbMap_pthreadMutex);
}

void NetServer::clearOfflineNode(int type)
{
    map<string , list<NodeInfo>*>  *topicMap = NULL;
    if(type ==1)
        topicMap = &subscriber;
    else
        topicMap = &publisher;
    pthread_mutex_lock(&scbMap_pthreadMutex);
    map<string , list<NodeInfo>*>::iterator topicIter = topicMap->begin();
    list<NodeInfo>* nodeInfoList = NULL;
    while(topicIter!=topicMap->end())
    {
        nodeInfoList= topicIter->second;
        list<NodeInfo>::iterator iter = nodeInfoList->begin();
        while(iter!=nodeInfoList->end())
        {
            if(iter->heartbeatCount == 3)
            {
                iter = nodeInfoList->erase(iter) ;

            }
            else
            {
                 iter->heartbeatCount++;
                 iter++;
            }

        }
    }
     pthread_mutex_unlock(&scbMap_pthreadMutex);
}

string NetServer::makeResposeStr(const vector<string>& topicNames, int type)
{
    map<string , list<NodeInfo>*>  *topicMap = NULL;
    if(type ==1)
        topicMap = &subscriber;
    else
        topicMap = &publisher;
    string str = "";
    for(int i=0;i<topicNames.size();i++)
    {
      if(topicMap->count(topicNames[i])<1)
          continue;
      list<NodeInfo>* nodeInfoList= (*topicMap)[topicNames[i]];
      str = str+topicNames[i]+" ";
      list<NodeInfo>::iterator iter = nodeInfoList->begin();

      while(iter!=nodeInfoList->end())
      {

          str =str+(*iter).address.ip+","+(*iter).address.port+";";
          iter++;
      }
    }
    return str;
}

void NetServer::run()
{
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
    serveraddr.sin_port=htons(serverPort);
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    bind(listenfd,(sockaddr *)&serveraddr, sizeof(serveraddr));
    listen(listenfd, LISTENQ);
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
                   printf("reading!\n");
                   if ( (sockfd = events[i].data.fd) < 0) continue;
                   Task *new_task=new Task();
                   new_task->fd=sockfd;
                   //添加新的读任务
                   new_task->owner = this;
                   threadPoll->addTask(new_task,1);

             }
              else if(events[i].events&EPOLLOUT)
              {

              }

         }

    }
}

NetServer::~NetServer()
{
    runningState = false;
    close(listenfd);
}

void * handleHeart(void *arg)
{
    NetServer * netServer = (NetServer*)arg;
    while (netServer->getRunningState())
    {
        sleep(10);
        netServer->clearOfflineNode(1);
        netServer->clearOfflineNode(2);
    }
}





int main()
{
    M_threadPool threadPool(2,2);
    NetServer netServer(8010);
    netServer.setThreadPoll(&threadPool);
    pthread_t tid;
  //  pthread_create(&tid, NULL, handleHeart, &netServer);
    netServer.run();
}


