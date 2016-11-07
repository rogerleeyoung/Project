#include <string>
#include<vector>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<stdio.h>
#include <netinet/in.h>
#include <fcntl.h>
#include<unistd.h>
#include<string.h>
using namespace std;

string sendData( string host_name,string portStr,string str)
{
    char buf[8192];
    //char message[256];
     int port = atoi(portStr.c_str());
    int socket_descriptor;
    struct sockaddr_in pin;
    bzero(&pin,sizeof(pin));
    pin.sin_family = AF_INET;
    inet_pton(AF_INET,host_name.c_str(),&pin.sin_addr);
    pin.sin_port = htons(port);
    if((socket_descriptor =  socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
            perror("error opening socket \n");
            exit(1);
    }
    if(connect(socket_descriptor,(struct sockaddr * )&pin,sizeof(pin)) == -1)
    {
       perror("error connecting to socket \n");
       exit(1);
     }

    printf("sending message %s to server ..\n",str.c_str());
    if( write(socket_descriptor,(const void *)str.c_str(),str.length()+1) == -1 )
    {
        printf("error ..in send\n");
      //  perror("error in send \n");
        string tempStr;
        return tempStr;
    }

    printf("..sent message ...wait for message..\n");
    if( read(socket_descriptor,buf,8192) == -1 )
    {
        printf("error ..in send\n");
       // perror("error in receiving response from server \n");
        string tempStr;
        return tempStr;
    }

    string recvData = string(buf);
    close(socket_descriptor);
    return recvData ;
}





vector<std::string> split(const  string& s, const string& delim)
{
    std::vector<std::string> elems;
    size_t pos = 0;
    size_t len = s.length();
    size_t delim_len = delim.length();
    if (delim_len == 0) return elems;
    while (pos < len)
    {
        int find_pos = s.find(delim, pos);
        if (find_pos < 0)
        {
            elems.push_back(s.substr(pos, len - pos));
            break;
        }
        elems.push_back(s.substr(pos, find_pos - pos));
        pos = find_pos + delim_len;
    }
    return elems;
}


void setnonblocking(int sock)
{
     int opts;
     opts=fcntl(sock,F_GETFL);
     if(opts<0)
     {
          perror("fcntl(sock,GETFL)");
          exit(1);
     }
    opts = opts|O_NONBLOCK;
     if(fcntl(sock,F_SETFL,opts)<0)
     {
          perror("fcntl(sock,SETFL,opts)");
          exit(1);
     }
}
