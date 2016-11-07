#ifndef _UNITBASE_H
#define _UNITBASE_H

#include<iostream>
#include <map>
#include <vector>
#include "UnitPort.h"
#include "../workManager/WorkManager.h"
using namespace std;
class UnitBaseImpl;
typedef void (*unitFunc)(void*);
class UnitBase
{
public:
	UnitBase();
	virtual ~UnitBase();
	unitFunc getFunc(const int func_index);
    void *getInput(int port);
	void addLinkUnit(int fromPort,int toPort, UnitBase *targetUnit);
    void initData(WorkManager *workManager);
    void initAttribute(vector<UnitPort> inputPorts, vector<UnitPort> outputPorts);
    map<int, UnitPort> getInputPorts();
    map<int, UnitPort> getOutPorts();
protected:
    void addInput(const string inputName,const string type,const int index,const string tag = "0");
    void addOutput(const string outputName, const string type, const int index,const string tag = "0");
	void addFunc(unitFunc func);
	void putOutput(int port,void *data);
private:
    map<int, UnitPort> inputPorts;
    map<int,UnitPort> outputPorts;
	UnitBaseImpl * unitImpl;

};

typedef UnitBase*  (*create_t)();
typedef void (*destroy_t)(UnitBase*);

#endif
