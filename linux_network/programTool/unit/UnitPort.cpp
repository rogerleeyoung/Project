#include "UnitPort.h"
UnitPort::UnitPort()
{

}

UnitPort::UnitPort(string name,string type,int index,string tag)
{
	portIndex = index;
	portName = name;
	portType = type;
    portTag = tag;
}
