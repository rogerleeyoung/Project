#include "UnitLink.h"


 UnitLink::UnitLink(UnitBase *targetUnit,int port,void *outputQueue)
    {
        this->targetUnit =targetUnit;
        this->targetInputport =port;
        this->outputQueue=outputQueue;
    }
   UnitBase * UnitLink::getTargetUnit()
   {
       return targetUnit;
   }
   int UnitLink::getTargetInputPort()
   {
       return targetInputport;
   }
   void * UnitLink::getOutputQueue()
   {
       return outputQueue;
   }
