#ifndef _ADDRESS_H
#define _ADDRESS__H
#include<string>
using namespace std;
struct Address
{
    string ip;
    string port;
    Address();
    Address(string ip,string port);
};
#endif