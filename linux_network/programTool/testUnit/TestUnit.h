#ifndef _TESTUNIT_H
#define _TESTUNIT_H
#include "../unit/UnitBase.h"
class TestUnit:public UnitBase
{
private:

    int data;
public:
    TestUnit();
    ~TestUnit();
    void onInput_start(int *a);
    void onStoped();

};

/******************/
// the class factories
extern "C" {
    UnitBase* create();
    void destroy(UnitBase* u);
} 

#endif
