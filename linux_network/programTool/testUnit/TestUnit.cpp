#include "TestUnit.h"
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<unistd.h>
#include<iostream>
using namespace std;

UnitBase* create() {
    TestUnit *instance=new TestUnit();
    return (UnitBase*)(instance);
}
void destroy(UnitBase* unit) {
    if(unit)
        delete unit;
}

static void onInput_start_impl(void *unit)
{
    TestUnit* instance=(TestUnit*)unit;
    int *a = (int*)instance->getInput(0);
    if(a)
        instance->onInput_start(a);
    delete a;
}


TestUnit::TestUnit()
{
    addFunc(onInput_start_impl);
}

TestUnit::~TestUnit()
{

}


void TestUnit::onInput_start(int *a)
{
    int time1=0;
    while(time1!=20)
    {
        time_t timep;
        time (&timep);
        cout<<"-------------------------------------"<<endl;
        printf("%s\n",ctime(&timep));
        cout<<a<<endl;
        cout<<"-------------------------------------"<<endl;
        putOutput(0,new int(1));
        sleep(10);
        time1++;
    }
}






