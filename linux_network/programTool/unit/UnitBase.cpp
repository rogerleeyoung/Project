#include "UnitBase.h"
#include "UnitLink.h"
#include "ThreadSafeQueue.h"
#include "UnitLink.h"
#include<iostream>

using namespace std;
class UnitBaseImpl
{
public:
	~UnitBaseImpl();
	void addLinkUnit(int fromPort,int toPort,UnitBase *targetUnit,  void  *outputQueue);
	void initData(WorkManager *workManager );
	void addInput(const string inputName,const string type,const int index);
	void addOutput(const string outputName, const string type, const int index);
	void addFunc(unitFunc func);
	unitFunc  getFunc(int portIndex);
	void *getInput(int portIndex);
	void putOutput(int portIndex,void *data);
	 void * getInputQueue(int portIndex);
 private:
	WorkManager *workManager;
    vector<unitFunc> functions;
	map<int, ThreadSafeQueue<void*>*> iuputQueueMap;
	map<int,vector<UnitLink>*>linkUnitMap;

};

//void UnitBaseImpl::run(const int func_index, UnitBase *unit)
//{
//    if((func_index<0) || (func_index>=pAu->vFuncs.size())) {
//		printf("no func\n");
//	}
//	//run
//    UnitBase_func_t *f= pAu->vFuncs[func_index];
//    f(unit);
//}

void UnitBaseImpl::putOutput(int portIndex,void *data)
{
	if( linkUnitMap.count(portIndex)==0)
	{
        cout<<"outPort "<< portIndex<<"no linkUnit"<<endl;
	   return;
	}
    vector<UnitLink> * tempVector= linkUnitMap[portIndex];
	int count =tempVector->size();
	for (int i = 0; i < count;i++)
	{
		UnitLink linkUnit=(*tempVector)[i];
		ThreadSafeQueue<void*> *q=(ThreadSafeQueue<void*>*)linkUnit.getOutputQueue();
		q->push(data);
		int inputPort=linkUnit.getTargetInputPort();
		UnitBase *unit = linkUnit.getTargetUnit();
		Task *tempTask=new Task(unit->getFunc(inputPort),unit);
		workManager->addTask(tempTask);
	}
}

void* UnitBaseImpl::getInput(int portIndex)
{
	if(linkUnitMap.count(portIndex)==0)
	{
        cout<<"port"  <<portIndex<< "no  inputQuene"<<endl;
	    return NULL;
	}
    ThreadSafeQueue<void*> *q=iuputQueueMap[portIndex];
	void *tempdata = NULL;
	if(q->isEmpty())
	{
        cout<<"port  "<<portIndex<<"  InputQuene  is empty"<<endl;
	}
	else
	{
		tempdata = q->get_front();
	}
	return tempdata;
}

void UnitBaseImpl::addInput(const string inputName,const string type,const int index)
{
    ThreadSafeQueue<void*>*  q=new ThreadSafeQueue<void*> ();
    iuputQueueMap[index]=q;
}

void UnitBaseImpl::addOutput(const string inputName,const string type,const int index)
{
}
void UnitBaseImpl::addFunc(unitFunc func)
{
	functions.push_back(func);
}
unitFunc  UnitBaseImpl::getFunc(int portIndex)
{
	if(portIndex>=functions.size())
		return NULL;
	return functions[portIndex];
}
 void UnitBaseImpl::addLinkUnit(int fromPort,int toPort,UnitBase *targetUnit,  void  *outputQueue)
 {
    UnitLink linkUnit(targetUnit,toPort, outputQueue);
	if( linkUnitMap.count(fromPort)==0)
	{
    vector<UnitLink>  *temp=new vector<UnitLink>();
	linkUnitMap[fromPort]=temp;
	}
    vector<UnitLink>* tempVector= linkUnitMap[fromPort];
	tempVector->push_back(linkUnit);
 }

 void * UnitBaseImpl::getInputQueue(int portIndex)
 {
	return iuputQueueMap[portIndex];
 }

void UnitBaseImpl::initData(WorkManager *workManager )
{
    this->workManager = workManager;
}

UnitBaseImpl:: ~UnitBaseImpl(){

    map<int, ThreadSafeQueue<void*>*>::iterator iter_2= iuputQueueMap.begin();
	while(iter_2 != iuputQueueMap.end())
	{
		if(!iter_2->second)
		{
              ThreadSafeQueue<void*>* tempQueue= iter_2->second;
			  void *tempdata = tempQueue->get_front();
			  while(!tempQueue->isEmpty())
			  {
				tempdata = tempQueue->get_front();
				delete tempdata;
			  }

			  delete tempQueue;
		}
		iter_2->second = NULL;
        iuputQueueMap.erase(iter_2++); //#1
	}

}
/***************
UnitBase
****************/

UnitBase::UnitBase()
{
	unitImpl=new UnitBaseImpl();
}

UnitBase::~UnitBase()
{
	delete unitImpl;
}



void UnitBase::putOutput(int portIndex,void *data)
{
    unitImpl->putOutput(portIndex,data);
}

void* UnitBase::getInput(int portIndex)
{
    return unitImpl->getInput(portIndex);
}

unitFunc UnitBase::getFunc(const int func_index)
{
	 return unitImpl->getFunc(func_index);
}

void UnitBase::addInput(const string inputName,const string type,const int index,const string tag)
{
    UnitPort unitPort =  UnitPort(inputName,type,index,tag);
    inputPorts[index] = unitPort ;
    unitImpl->addInput(inputName, type, index);
}

void UnitBase::addOutput(const string inputName,const string type,const int index,const string tag)
{
    UnitPort unitPort =  UnitPort(inputName,type,index,tag);
    outputPorts[index] = unitPort ;
    unitImpl->addOutput(inputName,type, index);
}

void UnitBase::initAttribute(vector<UnitPort> inputPorts, vector<UnitPort> outputPorts)
{
    vector<UnitPort>::iterator inputIter = inputPorts.begin();
    while(inputIter!=inputPorts.end())
    {
        addInput((*inputIter).portName,(*inputIter).portType,(*inputIter).portIndex,(*inputIter).portTag);
        inputIter++;
    }
    vector<UnitPort>::iterator outputIter = outputPorts.begin();
    while(outputIter!=outputPorts.end())
    {
        addOutput((*outputIter).portName,(*outputIter).portType,(*outputIter).portIndex,(*outputIter).portTag);
        outputIter++;
    }
}


void UnitBase::addFunc(unitFunc func)
{
	unitImpl->addFunc(func);
}

void UnitBase::addLinkUnit(int fromPort,int toPort, UnitBase *targetUnit )
{
    unitImpl->addLinkUnit(fromPort,toPort, targetUnit, targetUnit->unitImpl->getInputQueue(toPort));
}

void UnitBase::initData(WorkManager *workManager)
{
    unitImpl->initData(workManager);
}

map<int, UnitPort> UnitBase::getInputPorts()
{
    return  inputPorts;
}

map<int, UnitPort> UnitBase::getOutPorts()
{
    return outputPorts;
}






