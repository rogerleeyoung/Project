#ifndef  _UNITPORT_H
#define _UNITPORT_H
#include <string>
using namespace std;
struct UnitPort
{
    UnitPort(string name,string type,int index,string  tag="0");
    UnitPort();
	string  portName;
	string  portType;
    string portTag;
	int portIndex;
};

#endif
