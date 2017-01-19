#include "Task.h"

Task::Task(void (*func_ptr)(void*), void* arg) : m_func_ptr(func_ptr), m_arg(arg) {
}

Task::~Task() {
}

void Task::run() {
  (*m_func_ptr)(m_arg);
}
