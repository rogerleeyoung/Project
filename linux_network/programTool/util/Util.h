#ifndef _UTIL_H
#define _UTIL__H
#include <string>
#include<vector>
using namespace std;
string sendData( string host_name,string portStr,string str);
vector<std::string> split(const  string& s, const string& delim);
void setnonblocking(int sock);

#endif
