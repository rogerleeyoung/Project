#pragma once

#include <pthread.h>
#include <unistd.h>
#include <deque>
#include <iostream>
#include <vector>
#include <errno.h>
#include <string.h>

using namespace std;

//template<class TClass>
class Task
{
public:
//  Task(TCLass::* obj_fn_ptr); // pass an object method pointer
  Task(void (*func_ptr)(void*), void* arg); // pass a free function pointer
  ~Task();
  void run();
private:
//  TClass* _obj_fn_ptr;
  void (*m_func_ptr)(void*);
  void* m_arg;
};
