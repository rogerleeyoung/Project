#include <iostream>
#include <unistd.h>
#include "../netUnit/NetUnit.h"
#include "../engine/Engine.h"
using namespace std;


//void* runServer(void* arg)
//{
//  NetUnit* netUnit = (NetUnit*) arg;
//  netUnit->run();
//  return NULL;
//}


int main(int argc, char *argv[])
{

//    if(argc!=2)
//    {
//        cout<<"arg error"<<endl;
//        return 0;
//    }
//    cout<<argv[1]<<endl;
//    //配置文件文件的路径　　配置文件为src目录下的config.baos
//    char *path = argv[1];
   // char *path="/home/lfy/Project/new_baos/testProject/config.baos";
    NetUnit netUnit;
    netUnit.registerInfo();
    sleep(5);
     netUnit.run();
//    Engine engine(path,5);
//    engine.run();
    sleep(1000);
//    pthread_t tid;
//    ret = pthread_create(&tid, NULL, runServer, (void*) this);
//    if (ret != 0) {
//      cerr << "pthread_create() failed: " << ret << endl;
//      return -1;

  
    return 0;
}
