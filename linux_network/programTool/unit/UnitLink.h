#ifndef _UNITLINK_H
#define _UNITLINK_H
#include "UnitBase.h"
class UnitLink
 {
public:
    UnitLink(UnitBase *targetUnit,int port,void *outputQueue);
   UnitBase * getTargetUnit();
   int getTargetInputPort();
   void * getOutputQueue();
 private:
    UnitBase *targetUnit;
    void *outputQueue;
    int targetInputport;
};
#endif
